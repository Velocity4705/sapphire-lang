#include "graphql.h"
#include <sstream>
#include <cstring>

// C API implementation

extern "C" {

static std::string g_gql_last_string;

// ===== Schema =====

void* gql_schema_create() { return new Sapphire::GraphQL::Schema(); }
void  gql_schema_destroy(void* s) { delete static_cast<Sapphire::GraphQL::Schema*>(s); }

void gql_schema_add_type(void* s, const char* name, const char* kind, const char* description) {
    Sapphire::GraphQL::TypeDef t;
    t.name = name; t.kind = kind ? kind : "type";
    t.description = description ? description : "";
    static_cast<Sapphire::GraphQL::Schema*>(s)->add_type(t);
}

void gql_schema_add_field(void* s, const char* type_name, const char* field_name,
                           const char* field_type, int non_null, int is_list, const char* description) {
    auto* schema = static_cast<Sapphire::GraphQL::Schema*>(s);
    // Get or create the type
    std::string tn(type_name);
    const Sapphire::GraphQL::TypeDef* existing = schema->get_type(tn);
    Sapphire::GraphQL::TypeDef t;
    if (existing) t = *existing;
    else { t.name = tn; t.kind = "type"; }

    Sapphire::GraphQL::FieldDef f;
    f.name = field_name;
    f.type_name = field_type;
    f.non_null = non_null != 0;
    f.is_list = is_list != 0;
    f.description = description ? description : "";
    t.fields.push_back(f);
    schema->add_type(t);
}

void gql_schema_add_scalar(void* s, const char* name, const char* description) {
    static_cast<Sapphire::GraphQL::Schema*>(s)->add_scalar(name, description ? description : "");
}

void gql_schema_add_enum(void* s, const char* name, const char* values_csv) {
    std::vector<std::string> vals;
    std::string csv(values_csv ? values_csv : "");
    std::istringstream ss(csv);
    std::string v;
    while (std::getline(ss, v, ',')) {
        while (!v.empty() && v[0] == ' ') v = v.substr(1);
        while (!v.empty() && v.back() == ' ') v.pop_back();
        if (!v.empty()) vals.push_back(v);
    }
    static_cast<Sapphire::GraphQL::Schema*>(s)->add_enum(name, vals);
}

void gql_schema_set_query(void* s, const char* n)        { static_cast<Sapphire::GraphQL::Schema*>(s)->set_query_type(n); }
void gql_schema_set_mutation(void* s, const char* n)     { static_cast<Sapphire::GraphQL::Schema*>(s)->set_mutation_type(n); }
void gql_schema_set_subscription(void* s, const char* n) { static_cast<Sapphire::GraphQL::Schema*>(s)->set_subscription_type(n); }
int  gql_schema_has_type(void* s, const char* n)         { return static_cast<Sapphire::GraphQL::Schema*>(s)->has_type(n) ? 1 : 0; }
int  gql_schema_type_count(void* s)                      { return static_cast<Sapphire::GraphQL::Schema*>(s)->type_count(); }

int gql_schema_validate(void* s, char* error_buf, int buf_size) {
    std::string err;
    bool ok = static_cast<Sapphire::GraphQL::Schema*>(s)->validate(err);
    if (!ok && error_buf && buf_size > 0) {
        strncpy(error_buf, err.c_str(), buf_size - 1);
        error_buf[buf_size - 1] = '\0';
    }
    return ok ? 1 : 0;
}

const char* gql_schema_to_sdl(void* s) {
    g_gql_last_string = static_cast<Sapphire::GraphQL::Schema*>(s)->to_sdl();
    return g_gql_last_string.c_str();
}

const char* gql_schema_introspect(void* s) {
    g_gql_last_string = static_cast<Sapphire::GraphQL::Schema*>(s)->introspect();
    return g_gql_last_string.c_str();
}

// ===== Query Engine =====

// Store static data for resolvers (simple key->json map)
struct QueryEngineWrapper {
    Sapphire::GraphQL::QueryEngine* engine;
    std::unordered_map<std::string, std::string> static_data; // "Type.field" -> json
};

void* gql_query_engine_create(void* schema) {
    auto* w = new QueryEngineWrapper();
    w->engine = new Sapphire::GraphQL::QueryEngine(static_cast<Sapphire::GraphQL::Schema*>(schema));
    return w;
}

void gql_query_engine_destroy(void* e) {
    auto* w = static_cast<QueryEngineWrapper*>(e);
    delete w->engine;
    delete w;
}

void gql_query_engine_register_data(void* e, const char* type_name,
                                     const char* field_name, const char* json_data) {
    auto* w = static_cast<QueryEngineWrapper*>(e);
    std::string key = std::string(type_name) + "." + field_name;
    std::string data(json_data ? json_data : "null");
    w->static_data[key] = data;
    // Register a resolver that returns the static data
    w->engine->register_resolver(type_name, field_name,
        [data](const Sapphire::GraphQL::ResolverContext&) { return data; });
}

const char* gql_query_execute(void* e, const char* query, const char* variables) {
    auto* w = static_cast<QueryEngineWrapper*>(e);
    g_gql_last_string = w->engine->execute(query ? query : "", variables ? variables : "{}");
    return g_gql_last_string.c_str();
}

int gql_query_complexity(void* e, const char* query) {
    auto* w = static_cast<QueryEngineWrapper*>(e);
    return w->engine->query_complexity(query ? query : "");
}

int gql_query_resolver_count(void* e) {
    auto* w = static_cast<QueryEngineWrapper*>(e);
    return w->engine->resolver_count();
}

// ===== Mutation Engine =====

struct MutationEngineWrapper {
    Sapphire::GraphQL::MutationEngine* engine;
    std::unordered_map<std::string, std::string> mutation_results;
};

void* gql_mutation_engine_create(void* schema) {
    auto* w = new MutationEngineWrapper();
    w->engine = new Sapphire::GraphQL::MutationEngine(static_cast<Sapphire::GraphQL::Schema*>(schema));
    return w;
}

void gql_mutation_engine_destroy(void* e) {
    auto* w = static_cast<MutationEngineWrapper*>(e);
    delete w->engine;
    delete w;
}

void gql_mutation_register(void* e, const char* name, const char* result_json) {
    auto* w = static_cast<MutationEngineWrapper*>(e);
    std::string result(result_json ? result_json : "null");
    w->mutation_results[name] = result;
    w->engine->register_mutation(name,
        [result](const Sapphire::GraphQL::ResolverContext&) { return result; });
}

const char* gql_mutation_execute(void* e, const char* mutation, const char* variables) {
    auto* w = static_cast<MutationEngineWrapper*>(e);
    g_gql_last_string = w->engine->execute(mutation ? mutation : "", variables ? variables : "{}");
    return g_gql_last_string.c_str();
}

int gql_mutation_count(void* e) {
    auto* w = static_cast<MutationEngineWrapper*>(e);
    return w->engine->mutation_count();
}

// ===== Subscription Manager =====

void* gql_subscription_manager_create() { return new Sapphire::GraphQL::SubscriptionManager(); }
void  gql_subscription_manager_destroy(void* m) { delete static_cast<Sapphire::GraphQL::SubscriptionManager*>(m); }

const char* gql_subscribe(void* m, const char* client_id, const char* event_name, const char* query) {
    g_gql_last_string = static_cast<Sapphire::GraphQL::SubscriptionManager*>(m)->subscribe(
        client_id, event_name, query ? query : "");
    return g_gql_last_string.c_str();
}

int gql_unsubscribe(void* m, const char* subscription_id) {
    return static_cast<Sapphire::GraphQL::SubscriptionManager*>(m)->unsubscribe(subscription_id) ? 1 : 0;
}

void gql_unsubscribe_client(void* m, const char* client_id) {
    static_cast<Sapphire::GraphQL::SubscriptionManager*>(m)->unsubscribe_client(client_id);
}

const char* gql_publish(void* m, const char* event_name, const char* data_json) {
    auto notifications = static_cast<Sapphire::GraphQL::SubscriptionManager*>(m)->publish(
        event_name, data_json ? data_json : "null");
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < notifications.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"client\":\"" << notifications[i].first << "\",\"payload\":" << notifications[i].second << "}";
    }
    ss << "]";
    g_gql_last_string = ss.str();
    return g_gql_last_string.c_str();
}

int gql_subscription_count(void* m) {
    return static_cast<Sapphire::GraphQL::SubscriptionManager*>(m)->subscription_count();
}

int gql_client_subscription_count(void* m, const char* client_id) {
    return static_cast<Sapphire::GraphQL::SubscriptionManager*>(m)->client_subscription_count(client_id);
}

int gql_is_subscribed(void* m, const char* client_id, const char* event_name) {
    return static_cast<Sapphire::GraphQL::SubscriptionManager*>(m)->is_subscribed(client_id, event_name) ? 1 : 0;
}

} // extern "C"
