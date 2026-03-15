#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace Sapphire {
namespace ORM {

// Forward declarations
class Model;
class QueryBuilder;
class Migration;
class DatabaseConnection;

// Field types for model definitions
enum class FieldType {
    INTEGER,
    STRING,
    DOUBLE,
    BOOLEAN,
    DATETIME,
    TEXT,
    JSON
};

// Field definition
struct Field {
    std::string name;
    FieldType type;
    bool primary_key = false;
    bool nullable = true;
    bool unique = false;
    std::string default_value;
    int max_length = 0;
    
    Field(const std::string& n, FieldType t) : name(n), type(t) {}
};

// Model record (row data)
class Record {
public:
    Record();
    ~Record();
    
    // Field access
    void set(const std::string& field, const std::string& value);
    void set(const std::string& field, int value);
    void set(const std::string& field, double value);
    void set(const std::string& field, bool value);
    
    std::string get_string(const std::string& field) const;
    int get_int(const std::string& field) const;
    double get_double(const std::string& field) const;
    bool get_bool(const std::string& field) const;
    
    bool has_field(const std::string& field) const;
    std::vector<std::string> get_field_names() const;
    
    // JSON serialization
    std::string to_json() const;
    void from_json(const std::string& json);

private:
    std::unordered_map<std::string, std::string> data_;
};

// Query builder for fluent database queries
class QueryBuilder {
public:
    QueryBuilder(const std::string& table_name);
    ~QueryBuilder();
    
    // Query building
    QueryBuilder& select(const std::vector<std::string>& fields = {});
    QueryBuilder& where(const std::string& field, const std::string& op, const std::string& value);
    QueryBuilder& where_eq(const std::string& field, const std::string& value);
    QueryBuilder& where_gt(const std::string& field, const std::string& value);
    QueryBuilder& where_lt(const std::string& field, const std::string& value);
    QueryBuilder& order_by(const std::string& field, bool ascending = true);
    QueryBuilder& limit(int count);
    QueryBuilder& offset(int count);
    
    // Joins
    QueryBuilder& join(const std::string& table, const std::string& on_condition);
    QueryBuilder& left_join(const std::string& table, const std::string& on_condition);
    
    // Execution
    std::vector<Record> get();
    Record first();
    int count();
    
    // Insert/Update/Delete
    bool insert(const Record& record);
    bool update(const Record& record);
    bool delete_where();
    
    // SQL generation
    std::string to_sql() const;

private:
    std::string table_name_;
    std::vector<std::string> select_fields_;
    std::vector<std::string> where_conditions_;
    std::vector<std::string> joins_;
    std::string order_field_;
    bool order_ascending_ = true;
    int limit_ = -1;
    int offset_ = -1;
    
    std::string build_select_sql() const;
    std::string build_insert_sql(const Record& record) const;
    std::string build_update_sql(const Record& record) const;
    std::string build_delete_sql() const;
};

// Model base class
class Model {
public:
    Model(const std::string& table_name);
    virtual ~Model();
    
    // Schema definition
    void add_field(const std::string& name, FieldType type);
    void add_primary_key(const std::string& name, FieldType type = FieldType::INTEGER);
    void add_string_field(const std::string& name, int max_length = 255);
    void add_text_field(const std::string& name);
    void add_int_field(const std::string& name);
    void add_double_field(const std::string& name);
    void add_bool_field(const std::string& name);
    void add_datetime_field(const std::string& name);
    void add_json_field(const std::string& name);
    
    // Relationships
    void has_many(const std::string& related_model, const std::string& foreign_key);
    void belongs_to(const std::string& related_model, const std::string& foreign_key);
    void has_one(const std::string& related_model, const std::string& foreign_key);
    
    // Query interface
    QueryBuilder query();
    QueryBuilder where(const std::string& field, const std::string& op, const std::string& value);
    QueryBuilder where_eq(const std::string& field, const std::string& value);
    
    // CRUD operations
    Record create(const Record& data);
    std::vector<Record> all();
    Record find(int id);
    Record find_by(const std::string& field, const std::string& value);
    bool update(int id, const Record& data);
    bool delete_record(int id);
    
    // Schema operations
    bool create_table();
    bool drop_table();
    std::string get_create_table_sql() const;
    
    // Getters
    const std::string& get_table_name() const { return table_name_; }
    const std::vector<Field>& get_fields() const { return fields_; }

protected:
    std::string table_name_;
    std::vector<Field> fields_;
    std::unordered_map<std::string, std::string> relationships_;
    
    std::string field_type_to_sql(FieldType type) const;
};

// Migration system
class Migration {
public:
    Migration(const std::string& name);
    virtual ~Migration();
    
    // Migration operations
    virtual void up() = 0;
    virtual void down() = 0;
    
    // Schema operations
    void create_table(const std::string& name, std::function<void(Model&)> schema);
    void drop_table(const std::string& name);
    void add_column(const std::string& table, const std::string& column, FieldType type);
    void drop_column(const std::string& table, const std::string& column);
    void add_index(const std::string& table, const std::string& column);
    void drop_index(const std::string& table, const std::string& column);
    
    // Getters
    const std::string& get_name() const { return name_; }

private:
    std::string name_;
    std::vector<std::string> operations_;
};

// Database connection manager
class DatabaseConnection {
public:
    DatabaseConnection();
    ~DatabaseConnection();
    
    // Connection management
    bool connect(const std::string& connection_string);
    bool connect_sqlite(const std::string& database_path);
    bool connect_postgres(const std::string& host, int port, const std::string& database,
                         const std::string& username, const std::string& password);
    void disconnect();
    bool is_connected() const { return connected_; }
    
    // Query execution
    std::vector<Record> execute_query(const std::string& sql);
    bool execute_command(const std::string& sql);
    int execute_scalar(const std::string& sql);
    
    // Transaction support
    bool begin_transaction();
    bool commit_transaction();
    bool rollback_transaction();
    
    // Schema operations
    bool table_exists(const std::string& table_name);
    std::vector<std::string> get_table_names();
    std::vector<Field> get_table_schema(const std::string& table_name);

private:
    bool connected_ = false;
    std::string connection_string_;
    void* connection_handle_ = nullptr; // Database-specific connection handle
    
    bool execute_sqlite_query(const std::string& sql, std::vector<Record>& results);
    bool execute_postgres_query(const std::string& sql, std::vector<Record>& results);
};

// ORM Manager - singleton for global configuration
class ORMManager {
public:
    static ORMManager& instance();
    
    // Configuration
    void set_connection(std::shared_ptr<DatabaseConnection> conn);
    std::shared_ptr<DatabaseConnection> get_connection();
    
    // Model registration
    void register_model(const std::string& name, std::shared_ptr<Model> model);
    std::shared_ptr<Model> get_model(const std::string& name);
    
    // Migration management
    void add_migration(std::shared_ptr<Migration> migration);
    bool run_migrations();
    bool rollback_migration(const std::string& name);
    
    // Global operations
    bool create_all_tables();
    bool drop_all_tables();

private:
    ORMManager() = default;
    std::shared_ptr<DatabaseConnection> connection_;
    std::unordered_map<std::string, std::shared_ptr<Model>> models_;
    std::vector<std::shared_ptr<Migration>> migrations_;
};

} // namespace ORM
} // namespace Sapphire

// C API for Sapphire integration
extern "C" {
    // Model operations
    void* orm_model_create(const char* table_name);
    void orm_model_destroy(void* model);
    void orm_model_add_string_field(void* model, const char* name, int max_length);
    void orm_model_add_int_field(void* model, const char* name);
    void orm_model_add_primary_key(void* model, const char* name);
    int orm_model_create_table(void* model);
    const char* orm_model_get_create_sql(void* model);
    
    // Record operations
    void* orm_record_create();
    void orm_record_destroy(void* record);
    void orm_record_set_string(void* record, const char* field, const char* value);
    void orm_record_set_int(void* record, const char* field, int value);
    const char* orm_record_get_string(void* record, const char* field);
    int orm_record_get_int(void* record, const char* field);
    const char* orm_record_to_json(void* record);
    
    // Query operations
    void* orm_query_create(const char* table_name);
    void orm_query_destroy(void* query);
    void orm_query_where_eq(void* query, const char* field, const char* value);
    void orm_query_limit(void* query, int count);
    const char* orm_query_to_sql(void* query);
    void* orm_query_get_all(void* query); // Returns array of records
    void* orm_query_first(void* query);   // Returns single record
    
    // Database operations
    void* orm_db_create();
    void orm_db_destroy(void* db);
    int orm_db_connect_sqlite(void* db, const char* path);
    int orm_db_execute_command(void* db, const char* sql);
    
    // Utility functions
    void orm_free_string(char* str);
}