#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <queue>

namespace sapphire {
namespace stdlib {
namespace WebSocket {

// Forward declarations
class WebSocketConnection;
class WebSocketServer;
class WebSocketClient;

// WebSocket message types
enum class MessageType {
    TEXT,
    BINARY,
    PING,
    PONG,
    CLOSE
};

// WebSocket message
struct Message {
    MessageType type;
    std::string data;
    
    Message(MessageType t = MessageType::TEXT, const std::string& d = "")
        : type(t), data(d) {}
};

// WebSocket connection state
enum class ConnectionState {
    CONNECTING,
    OPEN,
    CLOSING,
    CLOSED
};

// Event handlers
using OnOpenHandler = std::function<void(std::shared_ptr<WebSocketConnection>)>;
using OnMessageHandler = std::function<void(std::shared_ptr<WebSocketConnection>, const Message&)>;
using OnCloseHandler = std::function<void(std::shared_ptr<WebSocketConnection>, int code, const std::string& reason)>;
using OnErrorHandler = std::function<void(std::shared_ptr<WebSocketConnection>, const std::string& error)>;

// WebSocket Connection
class WebSocketConnection : public std::enable_shared_from_this<WebSocketConnection> {
    friend class WebSocketServer;
    friend class WebSocketClient;
public:
    WebSocketConnection(int socket_fd);
    ~WebSocketConnection();
    
    // Connection info
    std::string get_id() const { return connection_id_; }
    ConnectionState get_state() const { return state_; }
    std::string get_remote_address() const { return remote_address_; }
    
    // Message sending
    bool send_text(const std::string& text);
    bool send_binary(const std::vector<uint8_t>& data);
    bool send_ping(const std::string& data = "");
    bool send_pong(const std::string& data = "");
    bool send_message(const Message& message);
    
    // Connection control
    void close(int code = 1000, const std::string& reason = "");
    bool is_open() const { return state_ == ConnectionState::OPEN; }
    
    // Event handlers
    void set_on_message(OnMessageHandler handler) { on_message_ = handler; }
    void set_on_close(OnCloseHandler handler) { on_close_ = handler; }
    void set_on_error(OnErrorHandler handler) { on_error_ = handler; }

private:
    std::string connection_id_;
    int socket_fd_;
    ConnectionState state_;
    std::string remote_address_;
    std::thread read_thread_;
    std::atomic<bool> running_;
    std::mutex send_mutex_;
    
    // Event handlers
    OnMessageHandler on_message_;
    OnCloseHandler on_close_;
    OnErrorHandler on_error_;
    
    void read_loop();
    bool perform_handshake();
    Message parse_frame(const std::vector<uint8_t>& frame);
    std::vector<uint8_t> create_frame(const Message& message);
    void handle_close_frame(const Message& message);
    void handle_ping_frame(const Message& message);
    void handle_pong_frame(const Message& message);
    
    std::string generate_connection_id();
    std::string calculate_websocket_accept(const std::string& key);
};

// WebSocket Server
class WebSocketServer {
public:
    WebSocketServer(int port = 8080);
    ~WebSocketServer();
    
    // Server control
    bool start();
    void stop();
    bool is_running() const { return running_; }
    
    // Event handlers
    void set_on_open(OnOpenHandler handler) { on_open_ = handler; }
    void set_on_message(OnMessageHandler handler) { on_message_ = handler; }
    void set_on_close(OnCloseHandler handler) { on_close_ = handler; }
    void set_on_error(OnErrorHandler handler) { on_error_ = handler; }
    
    // Connection management
    std::vector<std::shared_ptr<WebSocketConnection>> get_connections() const;
    std::shared_ptr<WebSocketConnection> get_connection(const std::string& id) const;
    void broadcast_text(const std::string& text);
    void broadcast_binary(const std::vector<uint8_t>& data);
    void broadcast_message(const Message& message);
    
    // Configuration
    void set_port(int port) { port_ = port; }
    void set_max_connections(int max_conn) { max_connections_ = max_conn; }

private:
    int port_;
    int server_fd_;
    std::atomic<bool> running_;
    std::thread accept_thread_;
    std::unordered_map<std::string, std::shared_ptr<WebSocketConnection>> connections_;
    mutable std::mutex connections_mutex_;
    
    // Configuration
    int max_connections_;
    
    // Event handlers
    OnOpenHandler on_open_;
    OnMessageHandler on_message_;
    OnCloseHandler on_close_;
    OnErrorHandler on_error_;
    
    void accept_loop();
    void handle_new_connection(int client_fd);
    void remove_connection(const std::string& connection_id);
    bool validate_websocket_request(const std::string& request);
    std::string create_websocket_response(const std::string& key);
};

// WebSocket Client
class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();
    
    // Connection
    bool connect(const std::string& url);
    void disconnect();
    bool is_connected() const;
    
    // Message sending
    bool send_text(const std::string& text);
    bool send_binary(const std::vector<uint8_t>& data);
    bool send_message(const Message& message);
    
    // Event handlers
    void set_on_open(OnOpenHandler handler) { on_open_ = handler; }
    void set_on_message(OnMessageHandler handler) { on_message_ = handler; }
    void set_on_close(OnCloseHandler handler) { on_close_ = handler; }
    void set_on_error(OnErrorHandler handler) { on_error_ = handler; }

private:
    std::shared_ptr<WebSocketConnection> connection_;
    OnOpenHandler on_open_;
    OnMessageHandler on_message_;
    OnCloseHandler on_close_;
    OnErrorHandler on_error_;
    
    bool perform_client_handshake(int socket_fd, const std::string& host, const std::string& path);
    std::string generate_websocket_key();
};

// Utility functions
std::string websocket_guid();
std::string base64_encode(const std::vector<uint8_t>& data);
std::vector<uint8_t> base64_decode(const std::string& encoded);
std::string sha1_hash(const std::string& input);

} // namespace WebSocket
} // namespace stdlib
} // namespace sapphire

// C API for Sapphire integration
extern "C" {
    // WebSocket Server
    void* sapphire_websocket_server_create(int port);
    void sapphire_websocket_server_destroy(void* server);
    int sapphire_websocket_server_start(void* server);
    void sapphire_websocket_server_stop(void* server);
    void sapphire_websocket_server_broadcast_text(void* server, const char* text);
    void sapphire_websocket_server_broadcast_binary(void* server, const uint8_t* data, int size);
    int sapphire_websocket_server_connection_count(void* server);
    
    // WebSocket Client
    void* sapphire_websocket_client_create();
    void sapphire_websocket_client_destroy(void* client);
    int sapphire_websocket_client_connect(void* client, const char* url);
    void sapphire_websocket_client_disconnect(void* client);
    int sapphire_websocket_client_send_text(void* client, const char* text);
    int sapphire_websocket_client_send_binary(void* client, const uint8_t* data, int size);
    int sapphire_websocket_client_is_connected(void* client);
    
    // WebSocket Connection
    int sapphire_websocket_connection_send_text(void* connection, const char* text);
    int sapphire_websocket_connection_send_binary(void* connection, const uint8_t* data, int size);
    void sapphire_websocket_connection_close(void* connection, int code, const char* reason);
    int sapphire_websocket_connection_is_open(void* connection);
    const char* sapphire_websocket_connection_get_id(void* connection);
    
    // Utility functions
    char* sapphire_websocket_base64_encode(const uint8_t* data, int size);
    uint8_t* sapphire_websocket_base64_decode(const char* encoded, int* out_size);
    void sapphire_websocket_free_string(char* str);
    void sapphire_websocket_free_data(uint8_t* data);
}