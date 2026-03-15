#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace Sapphire {
namespace GraphQL {

// ===== Type System =====

enum class ScalarType { String, Int, Float, Boolean, ID, Custom };

struct FieldDef {
    std::string name;
    std::string type_name;   // "String", "Int", "User", "[Post]", etc.
    bool non_null = false;   // type!
    bool is_list = false;    // [type]
    std::string description;
    bool deprecated = false;
    std::string deprecation_reason;
};

struct TypeDef {
    std::string name;
    std::string kind;        // "type", "input", "enum", "scalar", "interface"
    std::string description;
    std::vector<FieldDef> fields;
    std::vector<std::string> enum_values;
    std::vector<std::string> implements; // interfaces
};

// ===== Schema =====

class Schema {
public:
    Schema();
    ~Schema();

    // Define types
    void add_type(const TypeDef& type);
    void add_scalar(const std::string& name, const std::string& description = "");
    void add_enum(const std::string& name, const std::vector<std::string>& values);

    // Set root operation types
    void set_query_type(const std::string& type_name);
    void set_mutation_type(const std::string& type_name);
    void set_subscription_type(const std::string& type_name);

    // Lookup
    const TypeDef* get_type(const std::string& name) const;
    bool has_type(const std::string& name) const;
    int type_count() const { return static_cast<int>(types_.size()); }

    // Validation
    bool validate(std::string& error) const;

    // SDL output (Schema Definition Language)
    std::string to_sdl() const;

    // Introspection JSON
    std::string introspect() const;

    const std::string& query_type() const { return query_type_; }
    const std::string& mutation_type() const { return mutation_type_; }
    const std::string& subscription_type() const { return subscription_type_; }

private:
    std::unordered_map<std::string, TypeDef> types_;
    std::string query_type_;
    std::string mutation_type_;
    std::string subscription_type_;

    std::string field_type_sdl(const FieldDef& f) const;
    std::string escape_json(const std::string& s) const;
};

// ===== Query Execution =====

struct ResolverContext {
    std::string field_name;
    std::string parent_type;
    std::unordered_map<std::string, std::string> args;
    std::string parent_data; // JSON of parent object
};

// Resolver: takes context, returns JSON string
using Resolver = std::function<std::string(const ResolverContext&)>;

struct QueryField {
    std::string name;
    std::string alias;
    std::unordered_map<std::string, std::string> args;
    std::vector<QueryField> selections; // sub-fields
};

class QueryEngine {
public:
    QueryEngine(Schema* schema);
    ~QueryEngine();

    // Register a field resolver
    void register_resolver(const std::string& type_name,
                           const std::string& field_name,
                           Resolver resolver);

    // Register a default data resolver (returns field from JSON object)
    void register_default_resolver(const std::string& type_name);

    // Execute a query string, returns JSON result
    std::string execute(const std::string& query,
                        const std::string& variables_json = "{}",
                        const std::string& operation_name = "") const;

    // Parse a query into fields (simplified)
    std::vector<QueryField> parse_query(const std::string& query) const;

    // Check query complexity
    int query_complexity(const std::string& query) const;

    int resolver_count() const;

private:
    Schema* schema_;
    std::unordered_map<std::string, std::unordered_map<std::string, Resolver>> resolvers_;

    std::string execute_fields(const std::vector<QueryField>& fields,
                               const std::string& type_name,
                               const std::string& parent_json) const;
    std::string resolve_field(const QueryField& field,
                              const std::string& type_name,
                              const std::string& parent_json) const;
    std::string extract_json_field(const std::string& json, const std::string& key) const;
    std::vector<QueryField> parse_selection_set(const std::string& text, size_t& pos) const;
    void skip_whitespace(const std::string& text, size_t& pos) const;
    std::string parse_name(const std::string& text, size_t& pos) const;
    std::unordered_map<std::string, std::string> parse_args(const std::string& text, size_t& pos) const;
};

// ===== Mutation Engine =====

class MutationEngine {
public:
    MutationEngine(Schema* schema);
    ~MutationEngine();

    // Register a mutation resolver
    void register_mutation(const std::string& name, Resolver resolver);

    // Execute a mutation
    std::string execute(const std::string& mutation,
                        const std::string& variables_json = "{}") const;

    int mutation_count() const { return static_cast<int>(mutations_.size()); }

private:
    Schema* schema_;
    std::unordered_map<std::string, Resolver> mutations_;

    std::string parse_mutation_name(const std::string& mutation) const;
    std::unordered_map<std::string, std::string> parse_mutation_args(const std::string& mutation) const;
};

// ===== Subscription Manager =====

struct Subscription {
    std::string id;
    std::string event_name;
    std::string client_id;
    std::string query;
    bool active;
};

class SubscriptionManager {
public:
    SubscriptionManager();
    ~SubscriptionManager();

    // Subscribe a client to an event
    std::string subscribe(const std::string& client_id,
                          const std::string& event_name,
                          const std::string& query);

    // Unsubscribe
    bool unsubscribe(const std::string& subscription_id);
    void unsubscribe_client(const std::string& client_id);

    // Publish an event - returns list of (client_id, payload) pairs
    std::vector<std::pair<std::string, std::string>>
    publish(const std::string& event_name, const std::string& data_json);

    // Get active subscriptions
    int subscription_count() const;
    int client_subscription_count(const std::string& client_id) const;
    bool is_subscribed(const std::string& client_id, const std::string& event_name) const;

private:
    std::vector<Subscription> subscriptions_;
    int next_id_;

    std::string generate_id();
};

} // namespace GraphQL
} // namespace Sapphire

// ===== C API =====
extern "C" {

// Schema
void* gql_schema_create();
void  gql_schema_destroy(void* schema);
void  gql_schema_add_type(void* schema, const char* name, const char* kind, const char* description);
void  gql_schema_add_field(void* schema, const char* type_name, const char* field_name,
                            const char* field_type, int non_null, int is_list, const char* description);
void  gql_schema_add_scalar(void* schema, const char* name, const char* description);
void  gql_schema_add_enum(void* schema, const char* name, const char* values_csv);
void  gql_schema_set_query(void* schema, const char* type_name);
void  gql_schema_set_mutation(void* schema, const char* type_name);
void  gql_schema_set_subscription(void* schema, const char* type_name);
int   gql_schema_has_type(void* schema, const char* name);
int   gql_schema_type_count(void* schema);
int   gql_schema_validate(void* schema, char* error_buf, int buf_size);
const char* gql_schema_to_sdl(void* schema);
const char* gql_schema_introspect(void* schema);

// Query Engine
void* gql_query_engine_create(void* schema);
void  gql_query_engine_destroy(void* engine);
void  gql_query_engine_register_data(void* engine, const char* type_name,
                                      const char* field_name, const char* json_data);
const char* gql_query_execute(void* engine, const char* query, const char* variables);
int   gql_query_complexity(void* engine, const char* query);
int   gql_query_resolver_count(void* engine);

// Mutation Engine
void* gql_mutation_engine_create(void* schema);
void  gql_mutation_engine_destroy(void* engine);
void  gql_mutation_register(void* engine, const char* name, const char* result_json);
const char* gql_mutation_execute(void* engine, const char* mutation, const char* variables);
int   gql_mutation_count(void* engine);

// Subscription Manager
void* gql_subscription_manager_create();
void  gql_subscription_manager_destroy(void* mgr);
const char* gql_subscribe(void* mgr, const char* client_id, const char* event_name, const char* query);
int   gql_unsubscribe(void* mgr, const char* subscription_id);
void  gql_unsubscribe_client(void* mgr, const char* client_id);
const char* gql_publish(void* mgr, const char* event_name, const char* data_json);
int   gql_subscription_count(void* mgr);
int   gql_client_subscription_count(void* mgr, const char* client_id);
int   gql_is_subscribed(void* mgr, const char* client_id, const char* event_name);

} // extern "C"
