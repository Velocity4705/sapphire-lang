#include "rest.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

namespace Sapphire {
namespace REST {

// ===== RouteGenerator =====

RouteGenerator::RouteGenerator(const std::string& prefix) : prefix_(prefix) {}
RouteGenerator::~RouteGenerator() {}

std::string RouteGenerator::to_lower(const std::string& s) const {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

std::string RouteGenerator::pluralize(const std::string& s) const {
    if (s.empty()) return s;
    // Simple pluralization rules
    if (s.back() == 's' || s.back() == 'x' || s.back() == 'z') return s + "es";
    if (s.size() >= 2 && s.back() == 'y' && std::string("aeiou").find(s[s.size()-2]) == std::string::npos) {
        return s.substr(0, s.size()-1) + "ies";
    }
    return s + "s";
}

void RouteGenerator::generate_crud(const std::string& model_name) {
    std::string lower = to_lower(model_name);
    std::string plural = pluralize(lower);
    std::string base = prefix_ + "/" + plural;
    bool auth = model_auth_.count(model_name) ? model_auth_.at(model_name) : false;

    routes_.push_back({"GET",    base,              "list_"    + lower, model_name, "list",    auth});
    routes_.push_back({"POST",   base,              "create_"  + lower, model_name, "create",  auth});
    routes_.push_back({"GET",    base + "/:id",     "show_"    + lower, model_name, "show",    auth});
    routes_.push_back({"PUT",    base + "/:id",     "update_"  + lower, model_name, "update",  auth});
    routes_.push_back({"PATCH",  base + "/:id",     "patch_"   + lower, model_name, "patch",   auth});
    routes_.push_back({"DELETE", base + "/:id",     "destroy_" + lower, model_name, "destroy", auth});
}

void RouteGenerator::add_custom(const std::string& method, const std::string& path,
                                 const std::string& handler, bool auth_required) {
    routes_.push_back({method, path, handler, "", "custom", auth_required});
}

void RouteGenerator::require_auth(const std::string& model_name, bool required) {
    model_auth_[model_name] = required;
    // Update existing routes for this model
    for (auto& r : routes_) {
        if (r.model == model_name) r.auth_required = required;
    }
}

std::vector<RouteDefinition> RouteGenerator::get_routes() const { return routes_; }

std::vector<RouteDefinition> RouteGenerator::get_model_routes(const std::string& model_name) const {
    std::vector<RouteDefinition> result;
    for (const auto& r : routes_)
        if (r.model == model_name) result.push_back(r);
    return result;
}

std::string RouteGenerator::route_table() const {
    std::ostringstream ss;
    ss << "Routes (" << routes_.size() << " total):\n";
    ss << "  METHOD   PATH                              HANDLER                  AUTH\n";
    ss << "  -------- --------------------------------- ------------------------ ----\n";
    for (const auto& r : routes_) {
        ss << "  ";
        // pad method
        std::string m = r.method;
        while (m.size() < 8) m += ' ';
        ss << m;
        std::string p = r.path;
        while (p.size() < 33) p += ' ';
        ss << p << " ";
        std::string h = r.handler;
        while (h.size() < 24) h += ' ';
        ss << h << " ";
        ss << (r.auth_required ? "yes" : "no") << "\n";
    }
    return ss.str();
}


// ===== RequestValidator =====

RequestValidator::RequestValidator() {}
RequestValidator::~RequestValidator() {}

void RequestValidator::define_schema(const std::string& schema_name,
                                      const std::vector<ValidationRule>& rules) {
    schemas_[schema_name] = rules;
}

void RequestValidator::add_rule(const std::string& schema_name, const ValidationRule& rule) {
    schemas_[schema_name].push_back(rule);
}

bool RequestValidator::is_valid_email(const std::string& s) const {
    auto at = s.find('@');
    if (at == std::string::npos || at == 0 || at == s.size()-1) return false;
    auto dot = s.find('.', at);
    return dot != std::string::npos && dot < s.size()-1;
}

bool RequestValidator::is_valid_url(const std::string& s) const {
    return s.substr(0, 7) == "http://" || s.substr(0, 8) == "https://";
}

bool RequestValidator::is_integer(const std::string& s) const {
    if (s.empty()) return false;
    size_t start = (s[0] == '-') ? 1 : 0;
    if (start == s.size()) return false;
    for (size_t i = start; i < s.size(); i++)
        if (!std::isdigit(s[i])) return false;
    return true;
}

bool RequestValidator::is_float(const std::string& s) const {
    if (s.empty()) return false;
    bool dot = false;
    size_t start = (s[0] == '-') ? 1 : 0;
    for (size_t i = start; i < s.size(); i++) {
        if (s[i] == '.') { if (dot) return false; dot = true; }
        else if (!std::isdigit(s[i])) return false;
    }
    return true;
}

bool RequestValidator::validate_field(const ValidationRule& rule, const std::string& value,
                                       ValidationError& error) const {
    error.field = rule.field;
    if (value.empty()) {
        if (rule.required) { error.message = "Field '" + rule.field + "' is required"; return false; }
        return true;
    }
    if (rule.type == "int" && !is_integer(value)) {
        error.message = "Field '" + rule.field + "' must be an integer"; return false;
    }
    if (rule.type == "float" && !is_float(value)) {
        error.message = "Field '" + rule.field + "' must be a number"; return false;
    }
    if (rule.type == "email" && !is_valid_email(value)) {
        error.message = "Field '" + rule.field + "' must be a valid email"; return false;
    }
    if (rule.type == "url" && !is_valid_url(value)) {
        error.message = "Field '" + rule.field + "' must be a valid URL"; return false;
    }
    if (rule.min_length > 0 && (int)value.size() < rule.min_length) {
        error.message = "Field '" + rule.field + "' must be at least " +
                        std::to_string(rule.min_length) + " characters"; return false;
    }
    if (rule.max_length > 0 && (int)value.size() > rule.max_length) {
        error.message = "Field '" + rule.field + "' must be at most " +
                        std::to_string(rule.max_length) + " characters"; return false;
    }
    return true;
}

bool RequestValidator::validate(const std::string& schema_name,
                                 const std::unordered_map<std::string, std::string>& data,
                                 std::vector<ValidationError>& errors) const {
    auto it = schemas_.find(schema_name);
    if (it == schemas_.end()) return true; // no schema = no validation
    for (const auto& rule : it->second) {
        auto dit = data.find(rule.field);
        std::string val = (dit != data.end()) ? dit->second : "";
        ValidationError err;
        if (!validate_field(rule, val, err)) errors.push_back(err);
    }
    return errors.empty();
}

std::string RequestValidator::errors_to_json(const std::vector<ValidationError>& errors) const {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < errors.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"field\":\"" << errors[i].field << "\",\"message\":\"" << errors[i].message << "\"}";
    }
    ss << "]";
    return ss.str();
}

bool RequestValidator::has_schema(const std::string& name) const {
    return schemas_.find(name) != schemas_.end();
}


// ===== ResponseFormatter =====

ResponseFormatter::ResponseFormatter() : version_("1.0") {}
ResponseFormatter::~ResponseFormatter() {}

std::string ResponseFormatter::escape_json_string(const std::string& s) const {
    std::string r;
    for (char c : s) {
        if (c == '"') r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '\r') r += "\\r";
        else if (c == '\t') r += "\\t";
        else r += c;
    }
    return r;
}

std::string ResponseFormatter::build_envelope(int status_code, bool ok,
                                               const std::string& message,
                                               const std::string& data) const {
    std::ostringstream ss;
    ss << "{";
    ss << "\"status\":" << status_code;
    ss << ",\"success\":" << (ok ? "true" : "false");
    if (!version_.empty()) ss << ",\"version\":\"" << version_ << "\"";
    if (!message.empty()) ss << ",\"message\":\"" << escape_json_string(message) << "\"";
    if (!data.empty()) ss << ",\"data\":" << data;
    ss << "}";
    return ss.str();
}

std::string ResponseFormatter::success(const std::string& data, int status_code) const {
    return build_envelope(status_code, true, "", data.empty() ? "null" : data);
}

std::string ResponseFormatter::created(const std::string& data) const {
    return build_envelope(201, true, "Created", data.empty() ? "null" : data);
}

std::string ResponseFormatter::no_content() const {
    return build_envelope(204, true, "No Content", "null");
}

std::string ResponseFormatter::error(const std::string& message, int status_code) const {
    return build_envelope(status_code, false, message, "null");
}

std::string ResponseFormatter::not_found(const std::string& resource) const {
    return build_envelope(404, false, resource + " not found", "null");
}

std::string ResponseFormatter::unauthorized(const std::string& message) const {
    return build_envelope(401, false, message, "null");
}

std::string ResponseFormatter::forbidden(const std::string& message) const {
    return build_envelope(403, false, message, "null");
}

std::string ResponseFormatter::validation_error(const std::string& errors_json) const {
    std::ostringstream ss;
    ss << "{\"status\":422,\"success\":false,\"message\":\"Validation failed\",\"errors\":" << errors_json << "}";
    return ss.str();
}

std::string ResponseFormatter::server_error(const std::string& message) const {
    return build_envelope(500, false, message, "null");
}

std::string ResponseFormatter::paginated(const std::string& data, const PaginationMeta& meta) const {
    std::ostringstream ss;
    ss << "{\"status\":200,\"success\":true";
    if (!version_.empty()) ss << ",\"version\":\"" << version_ << "\"";
    ss << ",\"data\":" << (data.empty() ? "[]" : data);
    ss << ",\"meta\":{";
    ss << "\"page\":" << meta.page;
    ss << ",\"per_page\":" << meta.per_page;
    ss << ",\"total\":" << meta.total;
    ss << ",\"total_pages\":" << meta.total_pages;
    ss << "}}";
    return ss.str();
}

std::string ResponseFormatter::wrap(const std::string& data, int status_code,
                                     const std::string& message) const {
    return build_envelope(status_code, status_code < 400, message, data.empty() ? "null" : data);
}

std::string ResponseFormatter::pretty(const std::string& json) const {
    // Simple pretty-printer: add newlines and indentation
    std::string result;
    int indent = 0;
    bool in_string = false;
    for (size_t i = 0; i < json.size(); i++) {
        char c = json[i];
        if (c == '"' && (i == 0 || json[i-1] != '\\')) in_string = !in_string;
        if (in_string) { result += c; continue; }
        if (c == '{' || c == '[') {
            result += c; result += '\n';
            indent += 2;
            result += std::string(indent, ' ');
        } else if (c == '}' || c == ']') {
            result += '\n';
            indent -= 2;
            result += std::string(indent, ' ');
            result += c;
        } else if (c == ',') {
            result += c; result += '\n';
            result += std::string(indent, ' ');
        } else if (c == ':') {
            result += ": ";
        } else if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
            result += c;
        }
    }
    return result;
}


// ===== ApiDocGenerator =====

ApiDocGenerator::ApiDocGenerator(const std::string& title, const std::string& version)
    : title_(title), version_(version), base_url_("http://localhost:8080") {}
ApiDocGenerator::~ApiDocGenerator() {}

void ApiDocGenerator::add_endpoint(const EndpointDoc& doc) {
    endpoints_.push_back(doc);
}

void ApiDocGenerator::from_routes(const std::vector<RouteDefinition>& routes) {
    for (const auto& r : routes) {
        EndpointDoc doc;
        doc.method = r.method;
        doc.path   = r.path;
        doc.auth_required = r.auth_required;
        // Generate summary from action
        if (r.action == "list")    doc.summary = "List all " + r.model;
        else if (r.action == "create")  doc.summary = "Create a new " + r.model;
        else if (r.action == "show")    doc.summary = "Get a " + r.model + " by ID";
        else if (r.action == "update")  doc.summary = "Update a " + r.model;
        else if (r.action == "patch")   doc.summary = "Partially update a " + r.model;
        else if (r.action == "destroy") doc.summary = "Delete a " + r.model;
        else doc.summary = r.handler;
        doc.status_codes = {200, 400, 404, 500};
        if (r.auth_required) doc.status_codes.push_back(401);
        endpoints_.push_back(doc);
    }
}

std::string ApiDocGenerator::escape_json(const std::string& s) const {
    std::string r;
    for (char c : s) {
        if (c == '"') r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else r += c;
    }
    return r;
}

std::string ApiDocGenerator::escape_html(const std::string& s) const {
    std::string r;
    for (char c : s) {
        if (c == '<') r += "&lt;";
        else if (c == '>') r += "&gt;";
        else if (c == '&') r += "&amp;";
        else if (c == '"') r += "&quot;";
        else r += c;
    }
    return r;
}

std::string ApiDocGenerator::method_color(const std::string& method) const {
    if (method == "GET")    return "#61affe";
    if (method == "POST")   return "#49cc90";
    if (method == "PUT")    return "#fca130";
    if (method == "PATCH")  return "#50e3c2";
    if (method == "DELETE") return "#f93e3e";
    return "#999";
}

std::string ApiDocGenerator::generate_openapi() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"openapi\": \"3.0.0\",\n";
    ss << "  \"info\": {\n";
    ss << "    \"title\": \"" << escape_json(title_) << "\",\n";
    ss << "    \"version\": \"" << escape_json(version_) << "\"";
    if (!description_.empty())
        ss << ",\n    \"description\": \"" << escape_json(description_) << "\"";
    ss << "\n  },\n";
    ss << "  \"servers\": [{\"url\": \"" << escape_json(base_url_) << "\"}],\n";
    ss << "  \"paths\": {\n";
    // Group by path
    std::unordered_map<std::string, std::vector<const EndpointDoc*>> by_path;
    for (const auto& e : endpoints_) by_path[e.path].push_back(&e);
    bool first_path = true;
    for (const auto& kv : by_path) {
        if (!first_path) ss << ",\n";
        first_path = false;
        ss << "    \"" << escape_json(kv.first) << "\": {\n";
        bool first_method = true;
        for (const auto* e : kv.second) {
            if (!first_method) ss << ",\n";
            first_method = false;
            std::string m = e->method;
            std::transform(m.begin(), m.end(), m.begin(), ::tolower);
            ss << "      \"" << m << "\": {\n";
            ss << "        \"summary\": \"" << escape_json(e->summary) << "\",\n";
            ss << "        \"responses\": {\"200\": {\"description\": \"Success\"}}";
            if (e->auth_required)
                ss << ",\n        \"security\": [{\"bearerAuth\": []}]";
            ss << "\n      }";
        }
        ss << "\n    }";
    }
    ss << "\n  }\n}";
    return ss.str();
}

std::string ApiDocGenerator::generate_markdown() const {
    std::ostringstream ss;
    ss << "# " << title_ << "\n\n";
    ss << "**Version:** " << version_ << "\n";
    if (!description_.empty()) ss << "\n" << description_ << "\n";
    ss << "\n**Base URL:** `" << base_url_ << "`\n\n";
    ss << "## Endpoints\n\n";
    for (const auto& e : endpoints_) {
        ss << "### `" << e.method << " " << e.path << "`\n\n";
        ss << "**Summary:** " << e.summary << "\n\n";
        if (e.auth_required) ss << "> Requires authentication\n\n";
        ss << "---\n\n";
    }
    return ss.str();
}

std::string ApiDocGenerator::generate_html() const {
    std::ostringstream ss;
    ss << "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    ss << "<title>" << escape_html(title_) << "</title>";
    ss << "<style>body{font-family:sans-serif;max-width:900px;margin:40px auto;padding:0 20px;}";
    ss << "h1{color:#333;}";
    ss << ".endpoint{border:1px solid #ddd;border-radius:4px;margin:12px 0;padding:12px;}";
    ss << ".method{display:inline-block;padding:4px 10px;border-radius:3px;color:#fff;font-weight:bold;font-size:13px;min-width:60px;text-align:center;}";
    ss << ".path{font-family:monospace;font-size:15px;margin-left:10px;}";
    ss << ".summary{color:#555;margin-top:6px;}";
    ss << ".auth-badge{background:#f0ad4e;color:#fff;padding:2px 8px;border-radius:3px;font-size:12px;margin-left:8px;}";
    ss << "</style></head><body>";
    ss << "<h1>" << escape_html(title_) << "</h1>";
    ss << "<p><strong>Version:</strong> " << escape_html(version_) << " &nbsp;";
    ss << "<strong>Base URL:</strong> <code>" << escape_html(base_url_) << "</code></p>";
    if (!description_.empty()) ss << "<p>" << escape_html(description_) << "</p>";
    ss << "<h2>Endpoints (" << endpoints_.size() << ")</h2>";
    for (const auto& e : endpoints_) {
        ss << "<div class='endpoint'>";
        ss << "<span class='method' style='background:" << method_color(e.method) << "'>" << escape_html(e.method) << "</span>";
        ss << "<span class='path'>" << escape_html(e.path) << "</span>";
        if (e.auth_required) ss << "<span class='auth-badge'>Auth</span>";
        ss << "<div class='summary'>" << escape_html(e.summary) << "</div>";
        ss << "</div>";
    }
    ss << "</body></html>";
    return ss.str();
}

} // namespace REST
} // namespace Sapphire


// ===== C API =====

extern "C" {

static std::string g_rest_last_string;

// RouteGenerator
void* rest_router_create(const char* prefix) {
    return new Sapphire::REST::RouteGenerator(prefix ? prefix : "/api/v1");
}
void rest_router_destroy(void* router) {
    delete static_cast<Sapphire::REST::RouteGenerator*>(router);
}
void rest_router_generate_crud(void* router, const char* model_name) {
    static_cast<Sapphire::REST::RouteGenerator*>(router)->generate_crud(model_name);
}
void rest_router_add_custom(void* router, const char* method, const char* path,
                             const char* handler, int auth_required) {
    static_cast<Sapphire::REST::RouteGenerator*>(router)->add_custom(
        method, path, handler, auth_required != 0);
}
void rest_router_require_auth(void* router, const char* model_name, int required) {
    static_cast<Sapphire::REST::RouteGenerator*>(router)->require_auth(model_name, required != 0);
}
int rest_router_route_count(void* router) {
    return static_cast<Sapphire::REST::RouteGenerator*>(router)->route_count();
}
const char* rest_router_route_table(void* router) {
    g_rest_last_string = static_cast<Sapphire::REST::RouteGenerator*>(router)->route_table();
    return g_rest_last_string.c_str();
}
const char* rest_router_get_prefix(void* router) {
    g_rest_last_string = static_cast<Sapphire::REST::RouteGenerator*>(router)->prefix();
    return g_rest_last_string.c_str();
}

// RequestValidator
void* rest_validator_create() {
    return new Sapphire::REST::RequestValidator();
}
void rest_validator_destroy(void* validator) {
    delete static_cast<Sapphire::REST::RequestValidator*>(validator);
}
void rest_validator_add_rule(void* validator, const char* schema,
                              const char* field, const char* type,
                              int required, int min_len, int max_len) {
    Sapphire::REST::ValidationRule rule;
    rule.field      = field;
    rule.type       = type ? type : "string";
    rule.required   = required != 0;
    rule.min_length = min_len;
    rule.max_length = max_len;
    static_cast<Sapphire::REST::RequestValidator*>(validator)->add_rule(schema, rule);
}
int rest_validator_validate(void* validator, const char* schema,
                             const char* json_body, char* error_buf, int buf_size) {
    // Parse simple key=value pairs from json_body (format: "field1=val1,field2=val2")
    std::unordered_map<std::string, std::string> data;
    std::string body(json_body ? json_body : "");
    std::istringstream ss(body);
    std::string token;
    while (std::getline(ss, token, ',')) {
        auto eq = token.find('=');
        if (eq != std::string::npos)
            data[token.substr(0, eq)] = token.substr(eq + 1);
    }
    std::vector<Sapphire::REST::ValidationError> errors;
    bool valid = static_cast<Sapphire::REST::RequestValidator*>(validator)->validate(schema, data, errors);
    if (!valid && error_buf && buf_size > 0) {
        auto* v = static_cast<Sapphire::REST::RequestValidator*>(validator);
        std::string err_json = v->errors_to_json(errors);
        strncpy(error_buf, err_json.c_str(), buf_size - 1);
        error_buf[buf_size - 1] = '\0';
    }
    return valid ? 1 : 0;
}
int rest_validator_has_schema(void* validator, const char* schema) {
    return static_cast<Sapphire::REST::RequestValidator*>(validator)->has_schema(schema) ? 1 : 0;
}
int rest_validator_schema_count(void* validator) {
    return static_cast<Sapphire::REST::RequestValidator*>(validator)->schema_count();
}

// ResponseFormatter
void* rest_formatter_create(const char* version) {
    auto* f = new Sapphire::REST::ResponseFormatter();
    if (version) f->set_version(version);
    return f;
}
void rest_formatter_destroy(void* formatter) {
    delete static_cast<Sapphire::REST::ResponseFormatter*>(formatter);
}
const char* rest_formatter_success(void* formatter, const char* data, int status_code) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->success(data ? data : "", status_code);
    return g_rest_last_string.c_str();
}
const char* rest_formatter_created(void* formatter, const char* data) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->created(data ? data : "");
    return g_rest_last_string.c_str();
}
const char* rest_formatter_error(void* formatter, const char* message, int status_code) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->error(message ? message : "", status_code);
    return g_rest_last_string.c_str();
}
const char* rest_formatter_not_found(void* formatter, const char* resource) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->not_found(resource ? resource : "Resource");
    return g_rest_last_string.c_str();
}
const char* rest_formatter_unauthorized(void* formatter, const char* message) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->unauthorized(message ? message : "Unauthorized");
    return g_rest_last_string.c_str();
}
const char* rest_formatter_forbidden(void* formatter, const char* message) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->forbidden(message ? message : "Forbidden");
    return g_rest_last_string.c_str();
}
const char* rest_formatter_validation_error(void* formatter, const char* errors_json) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->validation_error(errors_json ? errors_json : "[]");
    return g_rest_last_string.c_str();
}
const char* rest_formatter_paginated(void* formatter, const char* data,
                                      int page, int per_page, int total) {
    Sapphire::REST::PaginationMeta meta;
    meta.page        = page;
    meta.per_page    = per_page;
    meta.total       = total;
    meta.total_pages = (per_page > 0) ? (total + per_page - 1) / per_page : 0;
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->paginated(data ? data : "[]", meta);
    return g_rest_last_string.c_str();
}
const char* rest_formatter_pretty(void* formatter, const char* json) {
    g_rest_last_string = static_cast<Sapphire::REST::ResponseFormatter*>(formatter)->pretty(json ? json : "");
    return g_rest_last_string.c_str();
}

// ApiDocGenerator
void* rest_docs_create(const char* title, const char* version) {
    return new Sapphire::REST::ApiDocGenerator(title ? title : "API", version ? version : "1.0.0");
}
void rest_docs_destroy(void* docs) {
    delete static_cast<Sapphire::REST::ApiDocGenerator*>(docs);
}
void rest_docs_add_endpoint(void* docs, const char* method, const char* path,
                             const char* summary, int auth_required) {
    Sapphire::REST::EndpointDoc doc;
    doc.method       = method ? method : "GET";
    doc.path         = path ? path : "/";
    doc.summary      = summary ? summary : "";
    doc.auth_required = auth_required != 0;
    static_cast<Sapphire::REST::ApiDocGenerator*>(docs)->add_endpoint(doc);
}
void rest_docs_from_router(void* docs, void* router) {
    auto routes = static_cast<Sapphire::REST::RouteGenerator*>(router)->get_routes();
    static_cast<Sapphire::REST::ApiDocGenerator*>(docs)->from_routes(routes);
}
const char* rest_docs_openapi(void* docs) {
    g_rest_last_string = static_cast<Sapphire::REST::ApiDocGenerator*>(docs)->generate_openapi();
    return g_rest_last_string.c_str();
}
const char* rest_docs_markdown(void* docs) {
    g_rest_last_string = static_cast<Sapphire::REST::ApiDocGenerator*>(docs)->generate_markdown();
    return g_rest_last_string.c_str();
}
const char* rest_docs_html(void* docs) {
    g_rest_last_string = static_cast<Sapphire::REST::ApiDocGenerator*>(docs)->generate_html();
    return g_rest_last_string.c_str();
}
int rest_docs_endpoint_count(void* docs) {
    return static_cast<Sapphire::REST::ApiDocGenerator*>(docs)->endpoint_count();
}

} // extern "C"
