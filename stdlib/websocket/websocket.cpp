#include "websocket.h"
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

namespace sapphire {
namespace stdlib {
namespace WebSocket {

// WebSocket GUID as per RFC 6455
static const std::string WEBSOCKET_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

// Utility Functions Implementation
std::string websocket_guid() {
    return WEBSOCKET_GUID;
}

std::string base64_encode(const std::vector<uint8_t>& data) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    
    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    
    return result;
}

std::vector<uint8_t> base64_decode(const std::string& encoded) {
    BIO *bio, *b64;
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(encoded.c_str(), encoded.length());
    bio = BIO_push(b64, bio);
    
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    
    std::vector<uint8_t> result(encoded.length());
    int decoded_size = BIO_read(bio, result.data(), encoded.length());
    result.resize(decoded_size);
    
    BIO_free_all(bio);
    return result;
}

std::string sha1_hash(const std::string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    
    std::vector<uint8_t> hash_vec(hash, hash + SHA_DIGEST_LENGTH);
    return base64_encode(hash_vec);
}

// WebSocketConnection Implementation
WebSocketConnection::WebSocketConnection(int socket_fd) 
    : socket_fd_(socket_fd), state_(ConnectionState::CONNECTING), running_(false) {
    connection_id_ = generate_connection_id();
    
    // Get remote address
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getpeername(socket_fd_, (struct sockaddr*)&addr, &addr_len) == 0) {
        remote_address_ = inet_ntoa(addr.sin_addr);
        remote_address_ += ":" + std::to_string(ntohs(addr.sin_port));
    }
}

WebSocketConnection::~WebSocketConnection() {
    if (running_) {
        running_ = false;
        if (read_thread_.joinable()) {
            read_thread_.join();
        }
    }
    if (socket_fd_ >= 0) {
        close(socket_fd_);
    }
}

std::string WebSocketConnection::generate_connection_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::string id;
    for (int i = 0; i < 16; ++i) {
        id += "0123456789abcdef"[dis(gen)];
    }
    return id;
}

std::string WebSocketConnection::calculate_websocket_accept(const std::string& key) {
    return sha1_hash(key + WEBSOCKET_GUID);
}

bool WebSocketConnection::send_text(const std::string& text) {
    Message msg(MessageType::TEXT, text);
    return send_message(msg);
}

bool WebSocketConnection::send_binary(const std::vector<uint8_t>& data) {
    std::string data_str(data.begin(), data.end());
    Message msg(MessageType::BINARY, data_str);
    return send_message(msg);
}

bool WebSocketConnection::send_ping(const std::string& data) {
    Message msg(MessageType::PING, data);
    return send_message(msg);
}

bool WebSocketConnection::send_pong(const std::string& data) {
    Message msg(MessageType::PONG, data);
    return send_message(msg);
}

bool WebSocketConnection::send_message(const Message& message) {
    if (state_ != ConnectionState::OPEN) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(send_mutex_);
    auto frame = create_frame(message);
    
    ssize_t sent = send(socket_fd_, frame.data(), frame.size(), 0);
    return sent == static_cast<ssize_t>(frame.size());
}

void WebSocketConnection::close(int code, const std::string& reason) {
    if (state_ == ConnectionState::CLOSED) {
        return;
    }
    
    state_ = ConnectionState::CLOSING;
    
    // Send close frame
    std::string close_data;
    close_data += static_cast<char>((code >> 8) & 0xFF);
    close_data += static_cast<char>(code & 0xFF);
    close_data += reason;
    
    Message close_msg(MessageType::CLOSE, close_data);
    send_message(close_msg);
    
    running_ = false;
    state_ = ConnectionState::CLOSED;
}

std::vector<uint8_t> WebSocketConnection::create_frame(const Message& message) {
    std::vector<uint8_t> frame;
    
    // First byte: FIN + opcode
    uint8_t first_byte = 0x80; // FIN = 1
    switch (message.type) {
        case MessageType::TEXT:
            first_byte |= 0x01;
            break;
        case MessageType::BINARY:
            first_byte |= 0x02;
            break;
        case MessageType::CLOSE:
            first_byte |= 0x08;
            break;
        case MessageType::PING:
            first_byte |= 0x09;
            break;
        case MessageType::PONG:
            first_byte |= 0x0A;
            break;
    }
    frame.push_back(first_byte);
    
    // Payload length
    size_t payload_len = message.data.length();
    if (payload_len < 126) {
        frame.push_back(static_cast<uint8_t>(payload_len));
    } else if (payload_len < 65536) {
        frame.push_back(126);
        frame.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(payload_len & 0xFF));
    } else {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i) {
            frame.push_back(static_cast<uint8_t>((payload_len >> (i * 8)) & 0xFF));
        }
    }
    
    // Payload data
    frame.insert(frame.end(), message.data.begin(), message.data.end());
    
    return frame;
}

void WebSocketConnection::read_loop() {
    std::vector<uint8_t> buffer(4096);
    
    while (running_ && state_ == ConnectionState::OPEN) {
        ssize_t bytes_received = recv(socket_fd_, buffer.data(), buffer.size(), 0);
        
        if (bytes_received <= 0) {
            if (on_error_) {
                on_error_(shared_from_this(), "Connection lost");
            }
            break;
        }
        
        // Parse WebSocket frame (simplified)
        if (bytes_received >= 2) {
            uint8_t opcode = buffer[0] & 0x0F;
            bool masked = (buffer[1] & 0x80) != 0;
            uint64_t payload_len = buffer[1] & 0x7F;
            
            size_t header_size = 2;
            if (payload_len == 126) {
                payload_len = (buffer[2] << 8) | buffer[3];
                header_size = 4;
            } else if (payload_len == 127) {
                payload_len = 0;
                for (int i = 0; i < 8; ++i) {
                    payload_len = (payload_len << 8) | buffer[2 + i];
                }
                header_size = 10;
            }
            
            if (masked) {
                header_size += 4; // Mask key
            }
            
            if (bytes_received >= static_cast<ssize_t>(header_size + payload_len)) {
                std::string payload_data;
                
                if (masked) {
                    uint8_t mask[4] = {buffer[header_size-4], buffer[header_size-3], 
                                      buffer[header_size-2], buffer[header_size-1]};
                    
                    for (uint64_t i = 0; i < payload_len; ++i) {
                        payload_data += static_cast<char>(buffer[header_size + i] ^ mask[i % 4]);
                    }
                } else {
                    payload_data.assign(buffer.begin() + header_size, 
                                       buffer.begin() + header_size + payload_len);
                }
                
                Message msg;
                msg.data = payload_data;
                
                switch (opcode) {
                    case 0x1: // Text
                        msg.type = MessageType::TEXT;
                        if (on_message_) {
                            on_message_(shared_from_this(), msg);
                        }
                        break;
                    case 0x2: // Binary
                        msg.type = MessageType::BINARY;
                        if (on_message_) {
                            on_message_(shared_from_this(), msg);
                        }
                        break;
                    case 0x8: // Close
                        handle_close_frame(msg);
                        break;
                    case 0x9: // Ping
                        handle_ping_frame(msg);
                        break;
                    case 0xA: // Pong
                        handle_pong_frame(msg);
                        break;
                }
            }
        }
    }
}

void WebSocketConnection::handle_close_frame(const Message& message) {
    int code = 1000;
    std::string reason;
    
    if (message.data.length() >= 2) {
        code = (static_cast<uint8_t>(message.data[0]) << 8) | 
               static_cast<uint8_t>(message.data[1]);
        if (message.data.length() > 2) {
            reason = message.data.substr(2);
        }
    }
    
    if (on_close_) {
        on_close_(shared_from_this(), code, reason);
    }
    
    state_ = ConnectionState::CLOSED;
    running_ = false;
}

void WebSocketConnection::handle_ping_frame(const Message& message) {
    send_pong(message.data);
}

void WebSocketConnection::handle_pong_frame(const Message& message) {
    // Handle pong response (could be used for keep-alive)
    (void)message; // Suppress unused parameter warning
}

// WebSocketServer Implementation
WebSocketServer::WebSocketServer(int port) 
    : port_(port), server_fd_(-1), running_(false), max_connections_(1000) {
}

WebSocketServer::~WebSocketServer() {
    stop();
}

bool WebSocketServer::start() {
    if (running_) {
        return true;
    }
    
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        return false;
    }
    
    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    
    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd_);
        return false;
    }
    
    if (listen(server_fd_, 10) < 0) {
        close(server_fd_);
        return false;
    }
    
    running_ = true;
    accept_thread_ = std::thread(&WebSocketServer::accept_loop, this);
    
    return true;
}

void WebSocketServer::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
    
    if (accept_thread_.joinable()) {
        accept_thread_.join();
    }
    
    std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.clear();
}

void WebSocketServer::accept_loop() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (running_) {
                std::cerr << "Accept failed" << std::endl;
            }
            continue;
        }
        
        handle_new_connection(client_fd);
    }
}

void WebSocketServer::handle_new_connection(int client_fd) {
    // Read HTTP request
    char buffer[4096];
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        close(client_fd);
        return;
    }
    
    buffer[bytes_read] = '\0';
    std::string request(buffer);
    
    if (!validate_websocket_request(request)) {
        close(client_fd);
        return;
    }
    
    // Extract WebSocket key
    std::string key;
    size_t key_pos = request.find("Sec-WebSocket-Key: ");
    if (key_pos != std::string::npos) {
        key_pos += 19; // Length of "Sec-WebSocket-Key: "
        size_t key_end = request.find("\r\n", key_pos);
        if (key_end != std::string::npos) {
            key = request.substr(key_pos, key_end - key_pos);
        }
    }
    
    if (key.empty()) {
        close(client_fd);
        return;
    }
    
    // Send WebSocket handshake response
    std::string response = create_websocket_response(key);
    send(client_fd, response.c_str(), response.length(), 0);
    
    // Create connection
    auto connection = std::make_shared<WebSocketConnection>(client_fd);
    connection->state_ = ConnectionState::OPEN;
    connection->running_ = true;
    
    // Set up event handlers
    connection->set_on_message(on_message_);
    connection->set_on_close([this](std::shared_ptr<WebSocketConnection> conn, int code, const std::string& reason) {
        remove_connection(conn->get_id());
        if (on_close_) {
            on_close_(conn, code, reason);
        }
    });
    connection->set_on_error(on_error_);
    
    // Start reading thread
    connection->read_thread_ = std::thread(&WebSocketConnection::read_loop, connection.get());
    
    // Add to connections
    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_[connection->get_id()] = connection;
    }
    
    if (on_open_) {
        on_open_(connection);
    }
}

bool WebSocketServer::validate_websocket_request(const std::string& request) {
    return request.find("GET ") == 0 &&
           request.find("Upgrade: websocket") != std::string::npos &&
           request.find("Connection: Upgrade") != std::string::npos &&
           request.find("Sec-WebSocket-Key: ") != std::string::npos;
}

std::string WebSocketServer::create_websocket_response(const std::string& key) {
    std::string accept_key = sha1_hash(key + WEBSOCKET_GUID);
    
    std::ostringstream response;
    response << "HTTP/1.1 101 Switching Protocols\r\n";
    response << "Upgrade: websocket\r\n";
    response << "Connection: Upgrade\r\n";
    response << "Sec-WebSocket-Accept: " << accept_key << "\r\n";
    response << "\r\n";
    
    return response.str();
}

void WebSocketServer::remove_connection(const std::string& connection_id) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.erase(connection_id);
}

std::vector<std::shared_ptr<WebSocketConnection>> WebSocketServer::get_connections() const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    std::vector<std::shared_ptr<WebSocketConnection>> result;
    for (const auto& pair : connections_) {
        result.push_back(pair.second);
    }
    return result;
}

std::shared_ptr<WebSocketConnection> WebSocketServer::get_connection(const std::string& id) const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto it = connections_.find(id);
    return (it != connections_.end()) ? it->second : nullptr;
}

void WebSocketServer::broadcast_text(const std::string& text) {
    Message msg(MessageType::TEXT, text);
    broadcast_message(msg);
}

void WebSocketServer::broadcast_binary(const std::vector<uint8_t>& data) {
    std::string data_str(data.begin(), data.end());
    Message msg(MessageType::BINARY, data_str);
    broadcast_message(msg);
}

void WebSocketServer::broadcast_message(const Message& message) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    for (const auto& pair : connections_) {
        if (pair.second->is_open()) {
            pair.second->send_message(message);
        }
    }
}

// WebSocketClient Implementation
WebSocketClient::WebSocketClient() {
}

WebSocketClient::~WebSocketClient() {
    disconnect();
}

bool WebSocketClient::connect(const std::string& url) {
    // Parse URL (simplified)
    std::string host = "localhost";
    int port = 80;
    std::string path = "/";
    
    // Extract host and port from URL
    size_t protocol_end = url.find("://");
    if (protocol_end != std::string::npos) {
        size_t host_start = protocol_end + 3;
        size_t path_start = url.find("/", host_start);
        if (path_start == std::string::npos) {
            path_start = url.length();
        }
        
        std::string host_port = url.substr(host_start, path_start - host_start);
        size_t colon_pos = host_port.find(":");
        if (colon_pos != std::string::npos) {
            host = host_port.substr(0, colon_pos);
            port = std::stoi(host_port.substr(colon_pos + 1));
        } else {
            host = host_port;
            port = (url.find("wss://") == 0) ? 443 : 80;
        }
        
        if (path_start < url.length()) {
            path = url.substr(path_start);
        }
    }
    
    // Create socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        return false;
    }
    
    // Connect to server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        close(socket_fd);
        return false;
    }
    
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    if (::connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(socket_fd);
        return false;
    }
    
    // Perform WebSocket handshake
    if (!perform_client_handshake(socket_fd, host, path)) {
        close(socket_fd);
        return false;
    }
    
    // Create connection
    connection_ = std::make_shared<WebSocketConnection>(socket_fd);
    connection_->state_ = ConnectionState::OPEN;
    connection_->running_ = true;
    
    // Set up event handlers
    connection_->set_on_message(on_message_);
    connection_->set_on_close(on_close_);
    connection_->set_on_error(on_error_);
    
    // Start reading thread
    connection_->read_thread_ = std::thread(&WebSocketConnection::read_loop, connection_.get());
    
    if (on_open_) {
        on_open_(connection_);
    }
    
    return true;
}

void WebSocketClient::disconnect() {
    if (connection_) {
        connection_->close();
        connection_.reset();
    }
}

bool WebSocketClient::is_connected() const {
    return connection_ && connection_->is_open();
}

bool WebSocketClient::send_text(const std::string& text) {
    return connection_ ? connection_->send_text(text) : false;
}

bool WebSocketClient::send_binary(const std::vector<uint8_t>& data) {
    return connection_ ? connection_->send_binary(data) : false;
}

bool WebSocketClient::send_message(const Message& message) {
    return connection_ ? connection_->send_message(message) : false;
}

bool WebSocketClient::perform_client_handshake(int socket_fd, const std::string& host, const std::string& path) {
    std::string key = generate_websocket_key();
    
    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Sec-WebSocket-Key: " << key << "\r\n";
    request << "Sec-WebSocket-Version: 13\r\n";
    request << "\r\n";
    
    std::string request_str = request.str();
    if (send(socket_fd, request_str.c_str(), request_str.length(), 0) < 0) {
        return false;
    }
    
    // Read response
    char buffer[4096];
    ssize_t bytes_read = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        return false;
    }
    
    buffer[bytes_read] = '\0';
    std::string response(buffer);
    
    // Validate response
    return response.find("HTTP/1.1 101") == 0 &&
           response.find("Upgrade: websocket") != std::string::npos &&
           response.find("Connection: Upgrade") != std::string::npos;
}

std::string WebSocketClient::generate_websocket_key() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 255);
    
    std::vector<uint8_t> key_bytes(16);
    for (int i = 0; i < 16; ++i) {
        key_bytes[i] = dis(gen);
    }
    
    return base64_encode(key_bytes);
}

} // namespace WebSocket
} // namespace stdlib
} // namespace sapphire

// C API Implementation
extern "C" {
    using namespace sapphire::stdlib::WebSocket;
    
    // WebSocket Server
    void* sapphire_websocket_server_create(int port) {
        return new WebSocketServer(port);
    }
    
    void sapphire_websocket_server_destroy(void* server) {
        delete static_cast<WebSocketServer*>(server);
    }
    
    int sapphire_websocket_server_start(void* server) {
        return static_cast<WebSocketServer*>(server)->start() ? 1 : 0;
    }
    
    void sapphire_websocket_server_stop(void* server) {
        static_cast<WebSocketServer*>(server)->stop();
    }
    
    void sapphire_websocket_server_broadcast_text(void* server, const char* text) {
        static_cast<WebSocketServer*>(server)->broadcast_text(std::string(text));
    }
    
    void sapphire_websocket_server_broadcast_binary(void* server, const uint8_t* data, int size) {
        std::vector<uint8_t> data_vec(data, data + size);
        static_cast<WebSocketServer*>(server)->broadcast_binary(data_vec);
    }
    
    int sapphire_websocket_server_connection_count(void* server) {
        return static_cast<WebSocketServer*>(server)->get_connections().size();
    }
    
    // WebSocket Client
    void* sapphire_websocket_client_create() {
        return new WebSocketClient();
    }
    
    void sapphire_websocket_client_destroy(void* client) {
        delete static_cast<WebSocketClient*>(client);
    }
    
    int sapphire_websocket_client_connect(void* client, const char* url) {
        return static_cast<WebSocketClient*>(client)->connect(std::string(url)) ? 1 : 0;
    }
    
    void sapphire_websocket_client_disconnect(void* client) {
        static_cast<WebSocketClient*>(client)->disconnect();
    }
    
    int sapphire_websocket_client_send_text(void* client, const char* text) {
        return static_cast<WebSocketClient*>(client)->send_text(std::string(text)) ? 1 : 0;
    }
    
    int sapphire_websocket_client_send_binary(void* client, const uint8_t* data, int size) {
        std::vector<uint8_t> data_vec(data, data + size);
        return static_cast<WebSocketClient*>(client)->send_binary(data_vec) ? 1 : 0;
    }
    
    int sapphire_websocket_client_is_connected(void* client) {
        return static_cast<WebSocketClient*>(client)->is_connected() ? 1 : 0;
    }
    
    // WebSocket Connection
    int sapphire_websocket_connection_send_text(void* connection, const char* text) {
        return static_cast<WebSocketConnection*>(connection)->send_text(std::string(text)) ? 1 : 0;
    }
    
    int sapphire_websocket_connection_send_binary(void* connection, const uint8_t* data, int size) {
        std::vector<uint8_t> data_vec(data, data + size);
        return static_cast<WebSocketConnection*>(connection)->send_binary(data_vec) ? 1 : 0;
    }
    
    void sapphire_websocket_connection_close(void* connection, int code, const char* reason) {
        static_cast<WebSocketConnection*>(connection)->close(code, std::string(reason));
    }
    
    int sapphire_websocket_connection_is_open(void* connection) {
        return static_cast<WebSocketConnection*>(connection)->is_open() ? 1 : 0;
    }
    
    const char* sapphire_websocket_connection_get_id(void* connection) {
        static std::string id = static_cast<WebSocketConnection*>(connection)->get_id();
        return id.c_str();
    }
    
    // Utility functions
    char* sapphire_websocket_base64_encode(const uint8_t* data, int size) {
        std::vector<uint8_t> data_vec(data, data + size);
        std::string encoded = base64_encode(data_vec);
        char* result = new char[encoded.length() + 1];
        strcpy(result, encoded.c_str());
        return result;
    }
    
    uint8_t* sapphire_websocket_base64_decode(const char* encoded, int* out_size) {
        std::vector<uint8_t> decoded = base64_decode(std::string(encoded));
        *out_size = decoded.size();
        uint8_t* result = new uint8_t[decoded.size()];
        memcpy(result, decoded.data(), decoded.size());
        return result;
    }
    
    void sapphire_websocket_free_string(char* str) {
        delete[] str;
    }
    
    void sapphire_websocket_free_data(uint8_t* data) {
        delete[] data;
    }
}