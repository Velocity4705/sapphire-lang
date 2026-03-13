#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <variant>
#include <optional>
#include <chrono>

namespace Sapphire {
namespace Database {

// Forward declarations
class Connection;
class Model;
class QueryBuilder;
class Migration;
class Schema;

// Database value types
using Value = std::variant<
    std::nullptr_t,
    bool,
    int64_t,
    double,
    std::string,
    std::chrono::system_clock::time_point
>;

// Field types for schema definition
enum class FieldType {
    INTEGER,
    BIGINT,
    REAL,
    TEXT,
    BOOLEAN,
    DATETIME,
    BLOB
};

// Relationship types
enum class RelationType {
    HAS_ONE,
    HAS_MANY,
    BELONGS_TO,
    MANY_TO_MANY
};

// Database field definition
struct Field {
    std::string name;
    FieldType type;
    bool primary_key = false;
    bool auto_increment = false;
    bool nullable = true;
    bool unique = false;
    std::optional<Value> default_value;
    std::optional<size_t> max_length;
    
    Field(const std::string& name, FieldType type) : name(name), type(type) {}
    
    Field& primary() { primary_key = true; nullable = false; return *this; }
    Field& auto_inc() { auto_increment = true; return *this; }
    Field& not_null() { nullable = false; return *this; }
    Field& unique_constraint() { unique = true; return *this; }
    Field& default_val(const Value& val) { default_value = val; return *this; }
    Field& max_len(size_t len) { max_length = len; return *this; }
};

// Database relationship definition
struct Relationship {
    std::string name;
    RelationType type;
    std::string target_model;
    std::string foreign_key;
    std::string local_key = "id";
    std::optional<std::string> junction_table; // For many-to-many
    
    Relationship(const std::string& name, RelationType type, const std::string& target)
        : name(name), type(type), target_model(target) {}
};

// Query result row
using Row = std::unordered_map<std::string, Value>;

// Query results
class QueryResult {
public:
    QueryResult() = default;
    QueryResult(std::vector<Row> rows) : rows_(std::move(rows)) {}
    
    // Iterator support
    auto begin() { return rows_.begin(); }
    auto end() { return rows_.end(); }
    auto begin() const { return rows_.begin(); }
    auto end() const { return rows_.end(); }
    
    // Access methods
    size_t size() const { return rows_.size(); }
    bool empty() const { return rows_.empty(); }
    const Row& operator[](size_t index) const { return rows_[index]; }
    Row& operator[](size_t index) { return rows_[index]; }
    
    // Convenience methods
    std::optional<Row> first() const {
        return empty() ? std::nullopt : std::make_optional(rows_[0]);
    }
    
    std::vector<Value> pluck(const std::string& column) const;
    QueryResult where(const std::string& column, const Value& value) const;

private:
    std::vector<Row> rows_;
};

// Database connection interface
class Connection {
public:
    virtual ~Connection() = default;
    
    // Connection management
    virtual bool connect(const std::string& connection_string) = 0;
    virtual void disconnect() = 0;
    virtual bool is_connected() const = 0;
    
    // Query execution
    virtual QueryResult execute(const std::string& sql, const std::vector<Value>& params = {}) = 0;
    virtual int64_t execute_update(const std::string& sql, const std::vector<Value>& params = {}) = 0;
    virtual int64_t last_insert_id() = 0;
    
    // Transaction support
    virtual void begin_transaction() = 0;
    virtual void commit() = 0;
    virtual void rollback() = 0;
    
    // Schema operations
    virtual bool table_exists(const std::string& table_name) = 0;
    virtual std::vector<std::string> get_tables() = 0;
    virtual std::vector<Field> get_table_schema(const std::string& table_name) = 0;
};

// SQLite connection implementation
class SQLiteConnection : public Connection {
public:
    SQLiteConnection();
    ~SQLiteConnection() override;
    
    bool connect(const std::string& connection_string) override;
    void disconnect() override;
    bool is_connected() const override;
    
    QueryResult execute(const std::string& sql, const std::vector<Value>& params = {}) override;
    int64_t execute_update(const std::string& sql, const std::vector<Value>& params = {}) override;
    int64_t last_insert_id() override;
    
    void begin_transaction() override;
    void commit() override;
    void rollback() override;
    
    bool table_exists(const std::string& table_name) override;
    std::vector<std::string> get_tables() override;
    std::vector<Field> get_table_schema(const std::string& table_name) override;

private:
    void* db_; // sqlite3* handle
    bool connected_;
    
    std::string value_to_sql(const Value& value);
    Value sql_to_value(const std::string& sql_value, FieldType type);
};

// Query builder for fluent SQL generation
class QueryBuilder {
public:
    QueryBuilder(std::shared_ptr<Connection> conn, const std::string& table);
    
    // SELECT operations
    QueryBuilder& select(const std::vector<std::string>& columns = {"*"});
    QueryBuilder& where(const std::string& column, const std::string& op, const Value& value);
    QueryBuilder& where(const std::string& column, const Value& value) { return where(column, "=", value); }
    QueryBuilder& where_in(const std::string& column, const std::vector<Value>& values);
    QueryBuilder& where_null(const std::string& column);
    QueryBuilder& where_not_null(const std::string& column);
    QueryBuilder& order_by(const std::string& column, const std::string& direction = "ASC");
    QueryBuilder& group_by(const std::string& column);
    QueryBuilder& having(const std::string& condition);
    QueryBuilder& limit(int count);
    QueryBuilder& offset(int count);
    
    // JOIN operations
    QueryBuilder& join(const std::string& table, const std::string& condition);
    QueryBuilder& left_join(const std::string& table, const std::string& condition);
    QueryBuilder& right_join(const std::string& table, const std::string& condition);
    
    // Execution
    QueryResult get();
    std::optional<Row> first();
    Value scalar(); // Get single value
    int64_t count();
    bool exists();
    
    // INSERT/UPDATE/DELETE
    int64_t insert(const Row& data);
    int64_t update(const Row& data);
    int64_t delete_records();
    
    // SQL generation
    std::string to_sql() const;
    std::vector<Value> get_bindings() const { return bindings_; }

private:
    std::shared_ptr<Connection> connection_;
    std::string table_;
    std::vector<std::string> select_columns_;
    std::vector<std::string> where_clauses_;
    std::vector<std::string> join_clauses_;
    std::vector<std::string> order_clauses_;
    std::vector<std::string> group_clauses_;
    std::string having_clause_;
    std::optional<int> limit_count_;
    std::optional<int> offset_count_;
    std::vector<Value> bindings_;
    
    void add_binding(const Value& value) { bindings_.push_back(value); }
};

// Base model class for ORM
class Model {
public:
    Model() = default;
    virtual ~Model() = default;
    
    // Table configuration
    virtual std::string table_name() const = 0;
    virtual std::vector<Field> fields() const = 0;
    virtual std::vector<Relationship> relationships() const { return {}; }
    virtual std::string primary_key() const { return "id"; }
    
    // CRUD operations
    bool save();
    bool delete_model();
    void reload();
    
    // Attribute access
    void set_attribute(const std::string& name, const Value& value);
    Value get_attribute(const std::string& name) const;
    bool has_attribute(const std::string& name) const;
    
    // Dirty tracking
    bool is_dirty() const { return !dirty_attributes_.empty(); }
    bool is_dirty(const std::string& attribute) const;
    std::vector<std::string> get_dirty_attributes() const;
    void mark_clean();
    
    // Timestamps
    virtual bool use_timestamps() const { return true; }
    virtual std::string created_at_column() const { return "created_at"; }
    virtual std::string updated_at_column() const { return "updated_at"; }
    
    // Static query methods
    static QueryBuilder query();
    static QueryResult all();
    static std::optional<Row> find(const Value& id);
    static QueryResult where(const std::string& column, const Value& value);
    static int64_t count();
    
    // Relationships
    QueryResult has_one(const std::string& related_model, const std::string& foreign_key = "");
    QueryResult has_many(const std::string& related_model, const std::string& foreign_key = "");
    std::optional<Row> belongs_to(const std::string& related_model, const std::string& foreign_key = "");
    QueryResult many_to_many(const std::string& related_model, const std::string& junction_table = "");

protected:
    Row attributes_;
    std::unordered_map<std::string, Value> original_attributes_;
    std::unordered_map<std::string, bool> dirty_attributes_;
    bool exists_ = false;
    
    static std::shared_ptr<Connection> connection_;
    
    void update_timestamps();
    std::string get_foreign_key_name(const std::string& model_name) const;
};

// Schema builder for migrations
class Schema {
public:
    static void create_table(const std::string& table_name, 
                           std::function<void(Schema&)> callback);
    static void alter_table(const std::string& table_name, 
                          std::function<void(Schema&)> callback);
    static void drop_table(const std::string& table_name);
    static bool table_exists(const std::string& table_name);
    
    // Column definitions
    Schema& id(const std::string& name = "id");
    Schema& integer(const std::string& name);
    Schema& bigint(const std::string& name);
    Schema& real(const std::string& name);
    Schema& text(const std::string& name, size_t max_length = 0);
    Schema& boolean(const std::string& name);
    Schema& datetime(const std::string& name);
    Schema& timestamps(); // created_at, updated_at
    
    // Column modifiers
    Schema& nullable();
    Schema& not_null();
    Schema& unique();
    Schema& default_value(const Value& value);
    Schema& primary_key();
    Schema& auto_increment();
    
    // Indexes
    Schema& index(const std::vector<std::string>& columns, const std::string& name = "");
    Schema& unique_index(const std::vector<std::string>& columns, const std::string& name = "");
    
    // Foreign keys
    Schema& foreign_key(const std::string& column, const std::string& references_table, 
                       const std::string& references_column = "id");

private:
    std::string table_name_;
    std::vector<Field> fields_;
    std::vector<std::string> indexes_;
    std::vector<std::string> foreign_keys_;
    Field* current_field_ = nullptr;
    
    static std::shared_ptr<Connection> connection_;
    
    std::string generate_create_sql() const;
    std::string field_type_to_sql(FieldType type) const;
};

// Migration base class
class Migration {
public:
    virtual ~Migration() = default;
    virtual void up() = 0;
    virtual void down() = 0;
    virtual std::string name() const = 0;
    virtual std::string timestamp() const = 0;
};

// Migration runner
class Migrator {
public:
    static void run_migrations();
    static void rollback(int steps = 1);
    static void reset();
    static void refresh();
    static std::vector<std::string> get_pending_migrations();
    static std::vector<std::string> get_ran_migrations();
    
    static void register_migration(std::unique_ptr<Migration> migration);

private:
    static std::vector<std::unique_ptr<Migration>> migrations_;
    static std::shared_ptr<Connection> connection_;
    
    static void create_migrations_table();
    static bool migration_exists(const std::string& name);
    static void record_migration(const std::string& name);
    static void remove_migration_record(const std::string& name);
};

// Database configuration and connection management
class Database {
public:
    static void configure(const std::string& driver, const std::string& connection_string);
    static std::shared_ptr<Connection> connection();
    static void set_connection(std::shared_ptr<Connection> conn);
    
    // Transaction helpers
    static void transaction(std::function<void()> callback);
    static void begin();
    static void commit();
    static void rollback();

private:
    static std::shared_ptr<Connection> connection_;
    static std::string driver_;
    static std::string connection_string_;
};

} // namespace Database
} // namespace Sapphire

// C API for compiler integration
extern "C" {
    // Connection management
    void* db_connect(const char* connection_string);
    void db_disconnect(void* connection);
    int db_is_connected(void* connection);
    
    // Query execution
    void* db_execute(void* connection, const char* sql);
    int64_t db_execute_update(void* connection, const char* sql);
    int64_t db_last_insert_id(void* connection);
    
    // Transaction support
    void db_begin_transaction(void* connection);
    void db_commit(void* connection);
    void db_rollback(void* connection);
    
    // Query builder
    void* db_query_builder(void* connection, const char* table);
    void* db_query_select(void* builder, const char** columns, int count);
    void* db_query_where(void* builder, const char* column, const char* value);
    void* db_query_get(void* builder);
    void db_query_builder_destroy(void* builder);
    
    // Results
    int db_result_count(void* result);
    const char* db_result_get_string(void* result, int row, const char* column);
    int64_t db_result_get_int(void* result, int row, const char* column);
    double db_result_get_real(void* result, int row, const char* column);
    void db_result_destroy(void* result);
    
    // Schema operations
    void db_create_table(void* connection, const char* table_name);
    void db_add_column(void* connection, const char* table_name, const char* column_name, const char* type);
    void db_drop_table(void* connection, const char* table_name);
    
    // Utility functions
    char* db_escape_string(const char* str);
    void db_free_string(char* str);
}