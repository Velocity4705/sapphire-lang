#ifndef SAPPHIRE_STDLIB_HTTP_H
#define SAPPHIRE_STDLIB_HTTP_H

#include <string>
#include <map>
#include <vector>

namespace sapphire {
namespace stdlib {

/**
 * HTTP Request
 */
struct HttpRequest {
    std::string method;      // GET, POST, PUT, DELETE, etc.
    std::string url;
    std::string path;
    std::string query;
    std::map<std::string, std::string> headers;
    std::string body;
    
    HttpRequest() : method("GET") {}
};

/**
 * HTTP Response
 */
struct HttpResponse {
    int status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::string body;
    
    HttpResponse() : status_code(200), status_message("OK") {}
};

/**
 * HTTP Client
 * 
 * Simple HTTP client for making requests.
 */
class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    // Simple request methods
    static HttpResponse get(const std::string& url);
    static HttpResponse post(const std::string& url, const std::string& body);
    static HttpResponse put(const std::string& url, const std::string& body);
    static HttpResponse delete_request(const std::string& url);
    
    // Advanced request
    static HttpResponse request(const HttpRequest& req);
    
    // Set default headers
    static void set_header(const std::string& key, const std::string& value);
    static void set_timeout(int seconds);
    
private:
    static std::map<std::string, std::string> default_headers_;
    static int timeout_;
    
    static HttpResponse perform_request(const HttpRequest& req);
    static std::string build_request_string(const HttpRequest& req);
    static HttpResponse parse_response(const std::string& response);
};

/**
 * HTTP Server
 * 
 * Simple HTTP server for handling requests.
 */
class HttpServer {
public:
    using Handler = void(*)(const HttpRequest&, HttpResponse&);
    
    HttpServer(int port);
    ~HttpServer();
    
    // Route handlers
    void get(const std::string& path, Handler handler);
    void post(const std::string& path, Handler handler);
    void put(const std::string& path, Handler handler);
    void delete_route(const std::string& path, Handler handler);
    
    // Start/stop server
    void start();
    void stop();
    bool is_running() const { return running_; }
    
    // Static file serving
    void serve_static(const std::string& path, const std::string& directory);
    
private:
    int port_;
    int socket_fd_;
    bool running_;
    
    struct Route {
        std::string method;
        std::string path;
        Handler handler;
    };
    
    std::vector<Route> routes_;
    std::map<std::string, std::string> static_paths_;
    
    void handle_connection(int client_fd);
    Handler find_handler(const std::string& method, const std::string& path);
    HttpRequest parse_request(const std::string& request_str);
    std::string build_response_string(const HttpResponse& response);
};

/**
 * URL Utilities
 */
class URL {
public:
    static std::string encode(const std::string& str);
    static std::string decode(const std::string& str);
    static std::map<std::string, std::string> parse_query(const std::string& query);
    static std::string build_query(const std::map<std::string, std::string>& params);
};

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // HTTP Client
    void* sapphire_http_get(const char* url);
    void* sapphire_http_post(const char* url, const char* body);
    int sapphire_http_response_status(void* response);
    const char* sapphire_http_response_body(void* response);
    void sapphire_http_response_free(void* response);
    
    // HTTP Server
    void* sapphire_http_server_create(int port);
    void sapphire_http_server_start(void* server);
    void sapphire_http_server_stop(void* server);
    void sapphire_http_server_free(void* server);
}

#endif // SAPPHIRE_STDLIB_HTTP_H
