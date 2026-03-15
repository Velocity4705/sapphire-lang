#include "graphql.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

namespace Sapphire {
namespace GraphQL {

// ===== Schema =====

Schema::Schema() {}
Schema::~Schema() {}

void Schema::add_type(const TypeDef& type) {
    types_[type.name] = type;
}

void Schema::add_scalar(const std::string& name, const std::string& description) {
    TypeDef t;
    t.name = name; t.kind = "scalar"; t.description = description;
    types_[name] = t;
}

void Schema::add_enum(const std::string& name, const std::vector<std::string>& values) {
    TypeDef t;
    t.name = name; t.kind = "enum"; t.enum_values = values;
    types_[name] = t;
}

void Schema::set_query_type(const std::string& n)        { query_type_ = n; }
void Schema::set_mutation_type(const std::string& n)     { mutation_type_ = n; }
void Schema::set_subscription_type(const std::string& n) { subscription_type_ = n; }

const TypeDef* Schema::get_type(const std::string& name) const {
    auto it = types_.find(name);
    return it != types_.end() ? &it->second : nullptr;
}

bool Schema::has_type(const std::string& name) const {
    return types_.find(name) != types_.end();
}

bool Schema::validate(std::string& error) const {
    if (query_type_.empty()) { error = "Schema must define a query type"; return false; }
    if (!has_type(query_type_)) { error = "Query type '" + query_type_ + "' not defined"; return false; }
    return true;
}

std::string Schema::field_type_sdl(const FieldDef& f) const {
    std::string t = f.type_name;
    if (f.is_list) t = "[" + t + "]";
    if (f.non_null) t = t + "!";
    return t;
}

std::string Schema::to_sdl() const {
    std::ostringstream ss;
    // Schema block
    if (!query_type_.empty()) {
        ss << "schema {\n";
        ss << "  query: " << query_type_ << "\n";
        if (!mutation_type_.empty())     ss << "  mutation: " << mutation_type_ << "\n";
        if (!subscription_type_.empty()) ss << "  subscription: " << subscription_type_ << "\n";
        ss << "}\n\n";
    }
    for (const auto& kv : types_) {
        const auto& t = kv.second;
        if (!t.description.empty()) ss << "\"\"\"" << t.description << "\"\"\"\n";
        if (t.kind == "scalar") {
            ss << "scalar " << t.name << "\n\n";
        } else if (t.kind == "enum") {
            ss << "enum " << t.name << " {\n";
            for (const auto& v : t.enum_values) ss << "  " << v << "\n";
            ss << "}\n\n";
        } else {
            ss << t.kind << " " << t.name;
            if (!t.implements.empty()) {
                ss << " implements ";
                for (size_t i = 0; i < t.implements.size(); i++) {
                    if (i > 0) ss << " & ";
                    ss << t.implements[i];
                }
            }
            ss << " {\n";
            for (const auto& f : t.fields) {
                if (!f.description.empty()) ss << "  \"\"\"" << f.description << "\"\"\"\n";
                ss << "  " << f.name << ": " << field_type_sdl(f);
                if (f.deprecated) ss << " @deprecated(reason: \"" << f.deprecation_reason << "\")";
                ss << "\n";
            }
            ss << "}\n\n";
        }
    }
    return ss.str();
}

std::string Schema::escape_json(const std::string& s) const {
    std::string r;
    for (char c : s) {
        if (c == '"') r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else r += c;
    }
    return r;
}

std::string Schema::introspect() const {
    std::ostringstream ss;
    ss << "{\"__schema\":{";
    ss << "\"queryType\":{\"name\":\"" << escape_json(query_type_) << "\"}";
    if (!mutation_type_.empty())
        ss << ",\"mutationType\":{\"name\":\"" << escape_json(mutation_type_) << "\"}";
    if (!subscription_type_.empty())
        ss << ",\"subscriptionType\":{\"name\":\"" << escape_json(subscription_type_) << "\"}";
    ss << ",\"types\":[";
    bool first = true;
    for (const auto& kv : types_) {
        if (!first) ss << ",";
        first = false;
        const auto& t = kv.second;
        ss << "{\"name\":\"" << escape_json(t.name) << "\"";
        ss << ",\"kind\":\"" << escape_json(t.kind) << "\"";
        if (!t.description.empty()) ss << ",\"description\":\"" << escape_json(t.description) << "\"";
        ss << ",\"fields\":[";
        bool ff = true;
        for (const auto& f : t.fields) {
            if (!ff) ss << ",";
            ff = false;
            ss << "{\"name\":\"" << escape_json(f.name) << "\"";
            ss << ",\"type\":{\"name\":\"" << escape_json(f.type_name) << "\"}}";
        }
        ss << "]}";
    }
    ss << "]}}";
    return ss.str();
}


// ===== QueryEngine =====

QueryEngine::QueryEngine(Schema* schema) : schema_(schema) {}
QueryEngine::~QueryEngine() {}

void QueryEngine::register_resolver(const std::string& type_name,
                                     const std::string& field_name,
                                     Resolver resolver) {
    resolvers_[type_name][field_name] = resolver;
}

void QueryEngine::register_default_resolver(const std::string& type_name) {
    // Default resolver: extract field from parent JSON
    // Registered as a sentinel - handled in resolve_field
    resolvers_[type_name]["__default__"] = [](const ResolverContext& ctx) {
        return ctx.parent_data;
    };
}

int QueryEngine::resolver_count() const {
    int count = 0;
    for (const auto& kv : resolvers_) count += static_cast<int>(kv.second.size());
    return count;
}

void QueryEngine::skip_whitespace(const std::string& text, size_t& pos) const {
    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\n' ||
           text[pos] == '\r' || text[pos] == '\t' || text[pos] == ',')) pos++;
}

std::string QueryEngine::parse_name(const std::string& text, size_t& pos) const {
    skip_whitespace(text, pos);
    std::string name;
    while (pos < text.size() && (std::isalnum(text[pos]) || text[pos] == '_')) {
        name += text[pos++];
    }
    return name;
}

std::unordered_map<std::string, std::string>
QueryEngine::parse_args(const std::string& text, size_t& pos) const {
    std::unordered_map<std::string, std::string> args;
    skip_whitespace(text, pos);
    if (pos >= text.size() || text[pos] != '(') return args;
    pos++; // consume '('
    while (pos < text.size() && text[pos] != ')') {
        skip_whitespace(text, pos);
        std::string key = parse_name(text, pos);
        skip_whitespace(text, pos);
        if (pos < text.size() && text[pos] == ':') pos++;
        skip_whitespace(text, pos);
        std::string val;
        if (pos < text.size() && text[pos] == '"') {
            pos++;
            while (pos < text.size() && text[pos] != '"') val += text[pos++];
            if (pos < text.size()) pos++;
        } else {
            while (pos < text.size() && text[pos] != ')' && text[pos] != ',' &&
                   text[pos] != ' ' && text[pos] != '\n') val += text[pos++];
        }
        if (!key.empty()) args[key] = val;
        skip_whitespace(text, pos);
    }
    if (pos < text.size()) pos++; // consume ')'
    return args;
}

std::vector<QueryField>
QueryEngine::parse_selection_set(const std::string& text, size_t& pos) const {
    std::vector<QueryField> fields;
    skip_whitespace(text, pos);
    if (pos >= text.size() || text[pos] != '{') return fields;
    pos++; // consume '{'
    while (pos < text.size() && text[pos] != '}') {
        skip_whitespace(text, pos);
        if (pos >= text.size() || text[pos] == '}') break;
        std::string name = parse_name(text, pos);
        if (name.empty()) { pos++; continue; }
        QueryField field;
        // Check for alias: "alias: fieldName"
        skip_whitespace(text, pos);
        if (pos < text.size() && text[pos] == ':') {
            pos++;
            field.alias = name;
            field.name = parse_name(text, pos);
        } else {
            field.name = name;
        }
        field.args = parse_args(text, pos);
        skip_whitespace(text, pos);
        if (pos < text.size() && text[pos] == '{') {
            field.selections = parse_selection_set(text, pos);
        }
        fields.push_back(field);
        skip_whitespace(text, pos);
    }
    if (pos < text.size()) pos++; // consume '}'
    return fields;
}

std::vector<QueryField> QueryEngine::parse_query(const std::string& query) const {
    size_t pos = 0;
    skip_whitespace(query, pos);
    // Skip "query", "mutation", "subscription" keyword and optional name
    std::string keyword = parse_name(query, pos);
    if (keyword == "query" || keyword == "mutation" || keyword == "subscription") {
        skip_whitespace(query, pos);
        // Optional operation name
        if (pos < query.size() && std::isalpha(query[pos])) parse_name(query, pos);
        // Optional variables
        skip_whitespace(query, pos);
        if (pos < query.size() && query[pos] == '(') {
            int depth = 1; pos++;
            while (pos < query.size() && depth > 0) {
                if (query[pos] == '(') depth++;
                else if (query[pos] == ')') depth--;
                pos++;
            }
        }
    } else {
        // Bare selection set - rewind
        pos = 0;
    }
    return parse_selection_set(query, pos);
}

std::string QueryEngine::extract_json_field(const std::string& json, const std::string& key) const {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "null";
    pos += search.size();
    while (pos < json.size() && json[pos] == ' ') pos++;
    if (pos >= json.size()) return "null";
    if (json[pos] == '"') {
        pos++;
        std::string val;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\') pos++;
            if (pos < json.size()) val += json[pos++];
        }
        return "\"" + val + "\"";
    } else if (json[pos] == '{' || json[pos] == '[') {
        char open = json[pos], close = (open == '{') ? '}' : ']';
        int depth = 1; pos++;
        std::string val(1, open);
        while (pos < json.size() && depth > 0) {
            if (json[pos] == open) depth++;
            else if (json[pos] == close) depth--;
            val += json[pos++];
        }
        return val;
    } else {
        std::string val;
        while (pos < json.size() && json[pos] != ',' && json[pos] != '}' && json[pos] != ']')
            val += json[pos++];
        return val;
    }
}

std::string QueryEngine::resolve_field(const QueryField& field,
                                        const std::string& type_name,
                                        const std::string& parent_json) const {
    // Check for explicit resolver
    auto type_it = resolvers_.find(type_name);
    if (type_it != resolvers_.end()) {
        auto field_it = type_it->second.find(field.name);
        if (field_it != type_it->second.end()) {
            ResolverContext ctx;
            ctx.field_name   = field.name;
            ctx.parent_type  = type_name;
            ctx.args         = field.args;
            ctx.parent_data  = parent_json;
            std::string result = field_it->second(ctx);
            // If has sub-selections, recurse
            if (!field.selections.empty()) {
                // Determine child type from schema
                const TypeDef* t = schema_->get_type(type_name);
                std::string child_type = type_name;
                if (t) {
                    for (const auto& f : t->fields)
                        if (f.name == field.name) { child_type = f.type_name; break; }
                }
                return execute_fields(field.selections, child_type, result);
            }
            return result;
        }
    }
    // Default: extract from parent JSON
    std::string val = extract_json_field(parent_json, field.name);
    if (!field.selections.empty()) {
        const TypeDef* t = schema_->get_type(type_name);
        std::string child_type = type_name;
        if (t) {
            for (const auto& f : t->fields)
                if (f.name == field.name) { child_type = f.type_name; break; }
        }
        return execute_fields(field.selections, child_type, val);
    }
    return val;
}

std::string QueryEngine::execute_fields(const std::vector<QueryField>& fields,
                                         const std::string& type_name,
                                         const std::string& parent_json) const {
    std::ostringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& field : fields) {
        if (!first) ss << ",";
        first = false;
        std::string key = field.alias.empty() ? field.name : field.alias;
        ss << "\"" << key << "\":" << resolve_field(field, type_name, parent_json);
    }
    ss << "}";
    return ss.str();
}

std::string QueryEngine::execute(const std::string& query,
                                  const std::string& /*variables_json*/,
                                  const std::string& /*operation_name*/) const {
    auto fields = parse_query(query);
    if (fields.empty()) {
        return "{\"errors\":[{\"message\":\"Empty or invalid query\"}]}";
    }
    std::string query_type = schema_->query_type();
    std::string data = execute_fields(fields, query_type, "{}");
    return "{\"data\":" + data + "}";
}

int QueryEngine::query_complexity(const std::string& query) const {
    // Simple complexity: count field selections
    int count = 0;
    for (char c : query) if (c == '{') count++;
    return count;
}


// ===== MutationEngine =====

MutationEngine::MutationEngine(Schema* schema) : schema_(schema) {}
MutationEngine::~MutationEngine() {}

void MutationEngine::register_mutation(const std::string& name, Resolver resolver) {
    mutations_[name] = resolver;
}

std::string MutationEngine::parse_mutation_name(const std::string& mutation) const {
    size_t pos = 0;
    // Skip "mutation" keyword
    while (pos < mutation.size() && std::isspace(mutation[pos])) pos++;
    std::string kw;
    while (pos < mutation.size() && std::isalpha(mutation[pos])) kw += mutation[pos++];
    if (kw == "mutation") {
        while (pos < mutation.size() && std::isspace(mutation[pos])) pos++;
        // Optional operation name
        if (pos < mutation.size() && std::isalpha(mutation[pos])) {
            while (pos < mutation.size() && std::isalnum(mutation[pos])) pos++;
        }
    } else {
        pos = 0; // bare selection
    }
    // Find opening brace
    while (pos < mutation.size() && mutation[pos] != '{') pos++;
    if (pos < mutation.size()) pos++;
    while (pos < mutation.size() && std::isspace(mutation[pos])) pos++;
    std::string name;
    while (pos < mutation.size() && (std::isalnum(mutation[pos]) || mutation[pos] == '_'))
        name += mutation[pos++];
    return name;
}

std::unordered_map<std::string, std::string>
MutationEngine::parse_mutation_args(const std::string& mutation) const {
    std::unordered_map<std::string, std::string> args;
    size_t paren = mutation.find('(');
    if (paren == std::string::npos) return args;
    size_t end = mutation.find(')', paren);
    if (end == std::string::npos) return args;
    std::string arg_str = mutation.substr(paren + 1, end - paren - 1);
    // Parse "key: value, key: value"
    size_t pos = 0;
    while (pos < arg_str.size()) {
        while (pos < arg_str.size() && std::isspace(arg_str[pos])) pos++;
        std::string key;
        while (pos < arg_str.size() && arg_str[pos] != ':' && !std::isspace(arg_str[pos]))
            key += arg_str[pos++];
        while (pos < arg_str.size() && (std::isspace(arg_str[pos]) || arg_str[pos] == ':')) pos++;
        std::string val;
        if (pos < arg_str.size() && arg_str[pos] == '"') {
            pos++;
            while (pos < arg_str.size() && arg_str[pos] != '"') val += arg_str[pos++];
            if (pos < arg_str.size()) pos++;
        } else {
            while (pos < arg_str.size() && arg_str[pos] != ',' && !std::isspace(arg_str[pos]))
                val += arg_str[pos++];
        }
        if (!key.empty()) args[key] = val;
        while (pos < arg_str.size() && (std::isspace(arg_str[pos]) || arg_str[pos] == ',')) pos++;
    }
    return args;
}

std::string MutationEngine::execute(const std::string& mutation,
                                     const std::string& /*variables_json*/) const {
    std::string name = parse_mutation_name(mutation);
    auto it = mutations_.find(name);
    if (it == mutations_.end()) {
        return "{\"errors\":[{\"message\":\"Unknown mutation: " + name + "\"}]}";
    }
    ResolverContext ctx;
    ctx.field_name  = name;
    ctx.parent_type = schema_->mutation_type();
    ctx.args        = parse_mutation_args(mutation);
    std::string result = it->second(ctx);
    return "{\"data\":{\"" + name + "\":" + result + "}}";
}

// ===== SubscriptionManager =====

SubscriptionManager::SubscriptionManager() : next_id_(1) {}
SubscriptionManager::~SubscriptionManager() {}

std::string SubscriptionManager::generate_id() {
    return "sub_" + std::to_string(next_id_++);
}

std::string SubscriptionManager::subscribe(const std::string& client_id,
                                             const std::string& event_name,
                                             const std::string& query) {
    Subscription s;
    s.id         = generate_id();
    s.event_name = event_name;
    s.client_id  = client_id;
    s.query      = query;
    s.active     = true;
    subscriptions_.push_back(s);
    return s.id;
}

bool SubscriptionManager::unsubscribe(const std::string& subscription_id) {
    for (auto& s : subscriptions_) {
        if (s.id == subscription_id && s.active) {
            s.active = false;
            return true;
        }
    }
    return false;
}

void SubscriptionManager::unsubscribe_client(const std::string& client_id) {
    for (auto& s : subscriptions_)
        if (s.client_id == client_id) s.active = false;
}

std::vector<std::pair<std::string, std::string>>
SubscriptionManager::publish(const std::string& event_name, const std::string& data_json) {
    std::vector<std::pair<std::string, std::string>> notifications;
    for (const auto& s : subscriptions_) {
        if (s.active && s.event_name == event_name) {
            std::string payload = "{\"type\":\"data\",\"id\":\"" + s.id +
                                  "\",\"payload\":{\"data\":" + data_json + "}}";
            notifications.push_back({s.client_id, payload});
        }
    }
    return notifications;
}

int SubscriptionManager::subscription_count() const {
    int count = 0;
    for (const auto& s : subscriptions_) if (s.active) count++;
    return count;
}

int SubscriptionManager::client_subscription_count(const std::string& client_id) const {
    int count = 0;
    for (const auto& s : subscriptions_)
        if (s.active && s.client_id == client_id) count++;
    return count;
}

bool SubscriptionManager::is_subscribed(const std::string& client_id,
                                          const std::string& event_name) const {
    for (const auto& s : subscriptions_)
        if (s.active && s.client_id == client_id && s.event_name == event_name) return true;
    return false;
}

} // namespace GraphQL
} // namespace Sapphire

