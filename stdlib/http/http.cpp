#include "http.h"
#include <sstream>
#include <cstring>
#include <algorithm>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

namespace sapphire {
namespace stdlib {

// Static members
std::map<std::string, std::string> HttpClient::default_headers_;
int HttpClient::timeout_ = 30;

//=============================================================================
// HttpClient Implementation
//=============================================================================

HttpClient::HttpClient() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

HttpClient::~HttpClient() {
#ifdef _WIN32
    WSACleanup();
#endif
}

HttpResponse HttpClient::get(const std::string& url) {
    HttpRequest req;
    req.method = "GET";
    req.url = url;
    return perform_request(req);
}

HttpResponse HttpClient::post(const std::string& url, const std::string& body) {
    HttpRequest req;
    req.method = "POST";
    req.url = url;
    req.body = body;
    req.headers["Content-Length"] = std::to_string(body.length());
    return perform_request(req);
}

HttpResponse HttpClient::put(const std::string& url, const std::string& body) {
    HttpRequest req;
    req.method = "PUT";
    req.url = url;
    req.body = body;
    req.headers["Content-Length"] = std::to_string(body.length());
    return perform_request(req);
}

HttpResponse HttpClient::delete_request(const std::string& url) {
    HttpRequest req;
    req.method = "DELETE";
    req.url = url;
    return perform_request(req);
}

HttpResponse HttpClient::request(const HttpRequest& req) {
    return perform_request(req);
}

void HttpClient::set_header(const std::string& key, const std::string& value) {
    default_headers_[key] = value;
}

void HttpClient::set_timeout(int seconds) {
    timeout_ = seconds;
}

HttpResponse HttpClient::perform_request(const HttpRequest& req) {
    HttpResponse response;
    
    // Parse URL to extract host and path
    std::string url = req.url;
    std::string protocol, host, path;
    int port = 80;
    
    // Simple URL parsing
    if (url.find("http://") == 0) {
        url = url.substr(7);
    } else if (url.find("https://") == 0) {
        url = url.substr(8);
        port = 443;
    }
    
    size_t slash_pos = url.find('/');
    if (slash_pos != std::string::npos) {
        host = url.substr(0, slash_pos);
        path = url.substr(slash_pos);
    } else {
        host = url;
        path = "/";
    }

    // Check for port in host
    size_t colon_pos = host.find(':');
    if (colon_pos != std::string::npos) {
        port = std::stoi(host.substr(colon_pos + 1));
        host = host.substr(0, colon_pos);
    }
    
    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        response.status_code = 0;
        response.status_message = "Failed to create socket";
        return response;
    }
    
    // Resolve hostname
    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        closesocket(sock);
        response.status_code = 0;
        response.status_message = "Failed to resolve hostname";
        return response;
    }
    
    // Connect to server
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        closesocket(sock);
        response.status_code = 0;
        response.status_message = "Failed to connect";
        return response;
    }
    
    // Build request
    std::stringstream request_stream;
    request_stream << req.method << " " << path << " HTTP/1.1\r\n";
    request_stream << "Host: " << host << "\r\n";
    request_stream << "User-Agent: Sapphire-HTTP/1.0\r\n";
    request_stream << "Connection: close\r\n";
    
    // Add default headers
    for (const auto& header : default_headers_) {
        request_stream << header.first << ": " << header.second << "\r\n";
    }
    
    // Add request headers
    for (const auto& header : req.headers) {
        request_stream << header.first << ": " << header.second << "\r\n";
    }
    
    request_stream << "\r\n";
    
    // Add body if present
    if (!req.body.empty()) {
        request_stream << req.body;
    }
    
    std::string request_str = request_stream.str();

    // Send request
    if (send(sock, request_str.c_str(), request_str.length(), 0) < 0) {
        closesocket(sock);
        response.status_code = 0;
        response.status_message = "Failed to send request";
        return response;
    }
    
    // Receive response
    std::string response_str;
    char buffer[4096];
    int bytes_received;
    
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        response_str += buffer;
    }
    
    closesocket(sock);
    
    // Parse response
    return parse_response(response_str);
}

HttpResponse HttpClient::parse_response(const std::string& response_str) {
    HttpResponse response;
    
    std::istringstream stream(response_str);
    std::string line;
    
    // Parse status line
    if (std::getline(stream, line)) {
        std::istringstream status_stream(line);
        std::string http_version;
        status_stream >> http_version >> response.status_code;
        std::getline(status_stream, response.status_message);
        // Trim leading space
        if (!response.status_message.empty() && response.status_message[0] == ' ') {
            response.status_message = response.status_message.substr(1);
        }
        // Remove \r if present
        if (!response.status_message.empty() && response.status_message.back() == '\r') {
            response.status_message.pop_back();
        }
    }
    
    // Parse headers
    while (std::getline(stream, line) && line != "\r") {
        if (line.back() == '\r') line.pop_back();
        if (line.empty()) break;
        
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            // Trim leading space from value
            if (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            response.headers[key] = value;
        }
    }
    
    // Parse body
    std::string body;
    while (std::getline(stream, line)) {
        body += line + "\n";
    }
    if (!body.empty() && body.back() == '\n') {
        body.pop_back();
    }
    response.body = body;
    
    return response;
}

//=============================================================================
// HttpServer Implementation
//=============================================================================

HttpServer::HttpServer(int port) : port_(port), socket_fd_(-1), running_(false) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::get(const std::string& path, Handler handler) {
    routes_.push_back({"GET", path, handler});
}

void HttpServer::post(const std::string& path, Handler handler) {
    routes_.push_back({"POST", path, handler});
}

void HttpServer::put(const std::string& path, Handler handler) {
    routes_.push_back({"PUT", path, handler});
}

void HttpServer::delete_route(const std::string& path, Handler handler) {
    routes_.push_back({"DELETE", path, handler});
}

void HttpServer::serve_static(const std::string& path, const std::string& directory) {
    static_paths_[path] = directory;
}

void HttpServer::start() {
    if (running_) return;
    
    // Create socket
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    
    // Set socket options
    int opt = 1;
    setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    
    // Bind socket
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);
    
    if (bind(socket_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        closesocket(socket_fd_);
        throw std::runtime_error("Failed to bind socket");
    }
    
    // Listen
    if (listen(socket_fd_, 10) < 0) {
        closesocket(socket_fd_);
        throw std::runtime_error("Failed to listen on socket");
    }
    
    running_ = true;
    
    // Accept connections (simplified - single-threaded)
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(socket_fd_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            if (running_) continue;
            else break;
        }
        
        handle_connection(client_fd);
        closesocket(client_fd);
    }
}

void HttpServer::stop() {
    if (!running_) return;
    running_ = false;
    if (socket_fd_ >= 0) {
        closesocket(socket_fd_);
        socket_fd_ = -1;
    }
}

void HttpServer::handle_connection(int client_fd) {
    // Receive request
    char buffer[4096];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) return;
    
    buffer[bytes_received] = '\0';
    std::string request_str(buffer);
    
    // Parse request
    HttpRequest request = parse_request(request_str);
    
    // Find handler
    Handler handler = find_handler(request.method, request.path);
    
    // Create response
    HttpResponse response;
    
    if (handler) {
        handler(request, response);
    } else {
        response.status_code = 404;
        response.status_message = "Not Found";
        response.body = "404 Not Found";
    }
    
    // Send response
    std::string response_str = build_response_string(response);
    send(client_fd, response_str.c_str(), response_str.length(), 0);
}

HttpServer::Handler HttpServer::find_handler(const std::string& method, const std::string& path) {
    for (const auto& route : routes_) {
        if (route.method == method && route.path == path) {
            return route.handler;
        }
    }
    return nullptr;
}

HttpRequest HttpServer::parse_request(const std::string& request_str) {
    HttpRequest request;
    
    std::istringstream stream(request_str);
    std::string line;
    
    // Parse request line
    if (std::getline(stream, line)) {
        std::istringstream request_stream(line);
        request_stream >> request.method >> request.path;
        
        // Parse query string
        size_t query_pos = request.path.find('?');
        if (query_pos != std::string::npos) {
            request.query = request.path.substr(query_pos + 1);
            request.path = request.path.substr(0, query_pos);
        }
    }
    
    // Parse headers
    while (std::getline(stream, line) && line != "\r") {
        if (line.back() == '\r') line.pop_back();
        if (line.empty()) break;
        
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            if (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            request.headers[key] = value;
        }
    }
    
    // Parse body
    std::string body;
    while (std::getline(stream, line)) {
        body += line + "\n";
    }
    if (!body.empty() && body.back() == '\n') {
        body.pop_back();
    }
    request.body = body;
    
    return request;
}

std::string HttpServer::build_response_string(const HttpResponse& response) {
    std::stringstream stream;
    
    stream << "HTTP/1.1 " << response.status_code << " " << response.status_message << "\r\n";
    
    // Add headers
    for (const auto& header : response.headers) {
        stream << header.first << ": " << header.second << "\r\n";
    }
    
    // Add Content-Length if not present
    if (response.headers.find("Content-Length") == response.headers.end()) {
        stream << "Content-Length: " << response.body.length() << "\r\n";
    }
    
    stream << "\r\n";
    stream << response.body;
    
    return stream.str();
}

//=============================================================================
// URL Utilities
//=============================================================================

std::string URL::encode(const std::string& str) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    
    for (char c : str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << int((unsigned char)c);
        }
    }
    
    return escaped.str();
}

std::string URL::decode(const std::string& str) {
    std::string result;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            std::istringstream is(str.substr(i + 1, 2));
            if (is >> std::hex >> value) {
                result += static_cast<char>(value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::map<std::string, std::string> URL::parse_query(const std::string& query) {
    std::map<std::string, std::string> params;
    
    std::istringstream stream(query);
    std::string pair;
    
    while (std::getline(stream, pair, '&')) {
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = decode(pair.substr(0, eq_pos));
            std::string value = decode(pair.substr(eq_pos + 1));
            params[key] = value;
        }
    }
    
    return params;
}

std::string URL::build_query(const std::map<std::string, std::string>& params) {
    std::ostringstream query;
    bool first = true;
    
    for (const auto& param : params) {
        if (!first) query << '&';
        query << encode(param.first) << '=' << encode(param.second);
        first = false;
    }
    
    return query.str();
}

} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

using namespace sapphire::stdlib;

extern "C" {

void* sapphire_http_get(const char* url) {
    try {
        HttpResponse* response = new HttpResponse(HttpClient::get(url));
        return response;
    } catch (...) {
        return nullptr;
    }
}

void* sapphire_http_post(const char* url, const char* body) {
    try {
        HttpResponse* response = new HttpResponse(HttpClient::post(url, body));
        return response;
    } catch (...) {
        return nullptr;
    }
}

int sapphire_http_response_status(void* response) {
    if (!response) return 0;
    return static_cast<HttpResponse*>(response)->status_code;
}

const char* sapphire_http_response_body(void* response) {
    if (!response) return "";
    return static_cast<HttpResponse*>(response)->body.c_str();
}

void sapphire_http_response_free(void* response) {
    if (response) {
        delete static_cast<HttpResponse*>(response);
    }
}

void* sapphire_http_server_create(int port) {
    try {
        return new HttpServer(port);
    } catch (...) {
        return nullptr;
    }
}

void sapphire_http_server_start(void* server) {
    if (server) {
        static_cast<HttpServer*>(server)->start();
    }
}

void sapphire_http_server_stop(void* server) {
    if (server) {
        static_cast<HttpServer*>(server)->stop();
    }
}

void sapphire_http_server_free(void* server) {
    if (server) {
        delete static_cast<HttpServer*>(server);
    }
}

} // extern "C"
