#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Sapphire {
namespace REST {

// ===== Route Generator =====

struct RouteDefinition {
    std::string method;   // GET, POST, PUT, PATCH, DELETE
    std::string path;     // e.g. "/api/v1/users"
    std::string handler;  // auto-generated handler name
    std::string model;    // associated model name
    std::string action;   // list, create, show, update, destroy, custom
    bool auth_required;
};

class RouteGenerator {
public:
    RouteGenerator(const std::string& prefix = "/api/v1");
    ~RouteGenerator();

    // Generate standard REST routes for a model
    void generate_crud(const std::string& model_name);

    // Add a custom action route
    void add_custom(const std::string& method, const std::string& path,
                    const std::string& handler, bool auth_required = false);

    // Set auth requirement for all routes of a model
    void require_auth(const std::string& model_name, bool required = true);

    // Get all generated routes
    std::vector<RouteDefinition> get_routes() const;

    // Get routes for a specific model
    std::vector<RouteDefinition> get_model_routes(const std::string& model_name) const;

    // Print route table (for debugging)
    std::string route_table() const;

    // Count
    int route_count() const { return static_cast<int>(routes_.size()); }

    void set_prefix(const std::string& prefix) { prefix_ = prefix; }
    const std::string& prefix() const { return prefix_; }

private:
    std::string prefix_;
    std::vector<RouteDefinition> routes_;
    std::unordered_map<std::string, bool> model_auth_;

    std::string to_lower(const std::string& s) const;
    std::string pluralize(const std::string& s) const;
};

// ===== Request Validator =====

struct ValidationRule {
    std::string field;
    std::string type;       // string, int, float, bool, email, url
    bool required;
    int min_length;
    int max_length;
    std::string pattern;    // regex-like pattern hint
};

struct ValidationError {
    std::string field;
    std::string message;
};

class RequestValidator {
public:
    RequestValidator();
    ~RequestValidator();

    // Define a schema for a route/model
    void define_schema(const std::string& schema_name,
                       const std::vector<ValidationRule>& rules);

    // Add a single rule to an existing schema
    void add_rule(const std::string& schema_name, const ValidationRule& rule);

    // Validate a JSON-like key-value map against a schema
    // Returns true if valid; errors populated on failure
    bool validate(const std::string& schema_name,
                  const std::unordered_map<std::string, std::string>& data,
                  std::vector<ValidationError>& errors) const;

    // Validate a single field value
    bool validate_field(const ValidationRule& rule, const std::string& value,
                        ValidationError& error) const;

    // Format errors as JSON string
    std::string errors_to_json(const std::vector<ValidationError>& errors) const;

    bool has_schema(const std::string& name) const;
    int schema_count() const { return static_cast<int>(schemas_.size()); }

private:
    std::unordered_map<std::string, std::vector<ValidationRule>> schemas_;

    bool is_valid_email(const std::string& s) const;
    bool is_valid_url(const std::string& s) const;
    bool is_integer(const std::string& s) const;
    bool is_float(const std::string& s) const;
};

// ===== Response Formatter =====

struct PaginationMeta {
    int page;
    int per_page;
    int total;
    int total_pages;
};

class ResponseFormatter {
public:
    ResponseFormatter();
    ~ResponseFormatter();

    // Success responses
    std::string success(const std::string& data, int status_code = 200) const;
    std::string created(const std::string& data) const;
    std::string no_content() const;

    // Error responses
    std::string error(const std::string& message, int status_code = 400) const;
    std::string not_found(const std::string& resource = "Resource") const;
    std::string unauthorized(const std::string& message = "Unauthorized") const;
    std::string forbidden(const std::string& message = "Forbidden") const;
    std::string validation_error(const std::string& errors_json) const;
    std::string server_error(const std::string& message = "Internal server error") const;

    // Paginated list response
    std::string paginated(const std::string& data,
                          const PaginationMeta& meta) const;

    // Wrap raw data in standard envelope
    std::string wrap(const std::string& data, int status_code,
                     const std::string& message = "") const;

    // Pretty-print JSON (indent with 2 spaces)
    std::string pretty(const std::string& json) const;

    // Set API version tag included in responses
    void set_version(const std::string& version) { version_ = version; }
    const std::string& version() const { return version_; }

private:
    std::string version_;

    std::string build_envelope(int status_code, bool success,
                               const std::string& message,
                               const std::string& data) const;
    std::string escape_json_string(const std::string& s) const;
};

// ===== API Documentation Generator =====

struct EndpointDoc {
    std::string method;
    std::string path;
    std::string summary;
    std::string description;
    bool auth_required;
    std::vector<std::string> parameters;   // param descriptions
    std::string request_body_schema;
    std::string response_schema;
    std::vector<int> status_codes;
};

class ApiDocGenerator {
public:
    ApiDocGenerator(const std::string& title = "API Documentation",
                    const std::string& version = "1.0.0");
    ~ApiDocGenerator();

    // Add endpoint documentation
    void add_endpoint(const EndpointDoc& doc);

    // Auto-generate docs from RouteGenerator routes
    void from_routes(const std::vector<RouteDefinition>& routes);

    // Generate OpenAPI 3.0 JSON spec
    std::string generate_openapi() const;

    // Generate simple Markdown documentation
    std::string generate_markdown() const;

    // Generate HTML documentation page
    std::string generate_html() const;

    // Set API metadata
    void set_title(const std::string& title) { title_ = title; }
    void set_version(const std::string& version) { version_ = version; }
    void set_description(const std::string& desc) { description_ = desc; }
    void set_base_url(const std::string& url) { base_url_ = url; }

    int endpoint_count() const { return static_cast<int>(endpoints_.size()); }

private:
    std::string title_;
    std::string version_;
    std::string description_;
    std::string base_url_;
    std::vector<EndpointDoc> endpoints_;

    std::string escape_json(const std::string& s) const;
    std::string escape_html(const std::string& s) const;
    std::string method_color(const std::string& method) const;
};

} // namespace REST
} // namespace Sapphire

// ===== C API =====
extern "C" {

// RouteGenerator
void* rest_router_create(const char* prefix);
void  rest_router_destroy(void* router);
void  rest_router_generate_crud(void* router, const char* model_name);
void  rest_router_add_custom(void* router, const char* method, const char* path,
                              const char* handler, int auth_required);
void  rest_router_require_auth(void* router, const char* model_name, int required);
int   rest_router_route_count(void* router);
const char* rest_router_route_table(void* router);
const char* rest_router_get_prefix(void* router);

// RequestValidator
void* rest_validator_create();
void  rest_validator_destroy(void* validator);
void  rest_validator_add_rule(void* validator, const char* schema,
                               const char* field, const char* type,
                               int required, int min_len, int max_len);
int   rest_validator_validate(void* validator, const char* schema,
                               const char* json_body,
                               char* error_buf, int buf_size);
int   rest_validator_has_schema(void* validator, const char* schema);
int   rest_validator_schema_count(void* validator);

// ResponseFormatter
void* rest_formatter_create(const char* version);
void  rest_formatter_destroy(void* formatter);
const char* rest_formatter_success(void* formatter, const char* data, int status_code);
const char* rest_formatter_created(void* formatter, const char* data);
const char* rest_formatter_error(void* formatter, const char* message, int status_code);
const char* rest_formatter_not_found(void* formatter, const char* resource);
const char* rest_formatter_unauthorized(void* formatter, const char* message);
const char* rest_formatter_forbidden(void* formatter, const char* message);
const char* rest_formatter_validation_error(void* formatter, const char* errors_json);
const char* rest_formatter_paginated(void* formatter, const char* data,
                                      int page, int per_page, int total);
const char* rest_formatter_pretty(void* formatter, const char* json);

// ApiDocGenerator
void* rest_docs_create(const char* title, const char* version);
void  rest_docs_destroy(void* docs);
void  rest_docs_add_endpoint(void* docs, const char* method, const char* path,
                              const char* summary, int auth_required);
void  rest_docs_from_router(void* docs, void* router);
const char* rest_docs_openapi(void* docs);
const char* rest_docs_markdown(void* docs);
const char* rest_docs_html(void* docs);
int   rest_docs_endpoint_count(void* docs);

} // extern "C"
