#include "orm.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

namespace Sapphire {
namespace ORM {

// ===== Record =====

Record::Record() {}
Record::~Record() {}

void Record::set(const std::string& field, const std::string& value) {
    data_[field] = value;
}
void Record::set(const std::string& field, int value) {
    data_[field] = std::to_string(value);
}
void Record::set(const std::string& field, double value) {
    data_[field] = std::to_string(value);
}
void Record::set(const std::string& field, bool value) {
    data_[field] = value ? "true" : "false";
}

std::string Record::get_string(const std::string& field) const {
    auto it = data_.find(field);
    return it != data_.end() ? it->second : "";
}
int Record::get_int(const std::string& field) const {
    auto it = data_.find(field);
    if (it == data_.end()) return 0;
    try { return std::stoi(it->second); } catch (...) { return 0; }
}
double Record::get_double(const std::string& field) const {
    auto it = data_.find(field);
    if (it == data_.end()) return 0.0;
    try { return std::stod(it->second); } catch (...) { return 0.0; }
}
bool Record::get_bool(const std::string& field) const {
    auto it = data_.find(field);
    if (it == data_.end()) return false;
    return it->second == "true" || it->second == "1";
}
bool Record::has_field(const std::string& field) const {
    return data_.find(field) != data_.end();
}
std::vector<std::string> Record::get_field_names() const {
    std::vector<std::string> names;
    for (auto& kv : data_) names.push_back(kv.first);
    return names;
}

std::string Record::to_json() const {
    std::ostringstream ss;
    ss << "{";
    bool first = true;
    for (auto& kv : data_) {
        if (!first) ss << ",";
        ss << "\"" << kv.first << "\":\"" << kv.second << "\"";
        first = false;
    }
    ss << "}";
    return ss.str();
}

void Record::from_json(const std::string& json) {
    // Simple JSON parser for key-value pairs
    std::string s = json;
    size_t pos = 0;
    while ((pos = s.find('"', pos)) != std::string::npos) {
        size_t key_start = pos + 1;
        size_t key_end = s.find('"', key_start);
        if (key_end == std::string::npos) break;
        std::string key = s.substr(key_start, key_end - key_start);
        size_t colon = s.find(':', key_end);
        if (colon == std::string::npos) break;
        size_t val_start = s.find('"', colon);
        if (val_start == std::string::npos) break;
        val_start++;
        size_t val_end = s.find('"', val_start);
        if (val_end == std::string::npos) break;
        std::string val = s.substr(val_start, val_end - val_start);
        data_[key] = val;
        pos = val_end + 1;
    }
}

// ===== QueryBuilder =====

QueryBuilder::QueryBuilder(const std::string& table_name) : table_name_(table_name) {}
QueryBuilder::~QueryBuilder() {}

QueryBuilder& QueryBuilder::select(const std::vector<std::string>& fields) {
    select_fields_ = fields;
    return *this;
}
QueryBuilder& QueryBuilder::where(const std::string& field, const std::string& op, const std::string& value) {
    where_conditions_.push_back(field + " " + op + " '" + value + "'");
    return *this;
}
QueryBuilder& QueryBuilder::where_eq(const std::string& field, const std::string& value) {
    return where(field, "=", value);
}
QueryBuilder& QueryBuilder::where_gt(const std::string& field, const std::string& value) {
    return where(field, ">", value);
}
QueryBuilder& QueryBuilder::where_lt(const std::string& field, const std::string& value) {
    return where(field, "<", value);
}
QueryBuilder& QueryBuilder::order_by(const std::string& field, bool ascending) {
    order_field_ = field;
    order_ascending_ = ascending;
    return *this;
}
QueryBuilder& QueryBuilder::limit(int count) {
    limit_ = count;
    return *this;
}
QueryBuilder& QueryBuilder::offset(int count) {
    offset_ = count;
    return *this;
}
QueryBuilder& QueryBuilder::join(const std::string& table, const std::string& on_condition) {
    joins_.push_back("INNER JOIN " + table + " ON " + on_condition);
    return *this;
}
QueryBuilder& QueryBuilder::left_join(const std::string& table, const std::string& on_condition) {
    joins_.push_back("LEFT JOIN " + table + " ON " + on_condition);
    return *this;
}

std::string QueryBuilder::build_select_sql() const {
    std::ostringstream ss;
    ss << "SELECT ";
    if (select_fields_.empty()) {
        ss << "*";
    } else {
        for (size_t i = 0; i < select_fields_.size(); i++) {
            if (i > 0) ss << ", ";
            ss << select_fields_[i];
        }
    }
    ss << " FROM " << table_name_;
    for (auto& j : joins_) ss << " " << j;
    if (!where_conditions_.empty()) {
        ss << " WHERE ";
        for (size_t i = 0; i < where_conditions_.size(); i++) {
            if (i > 0) ss << " AND ";
            ss << where_conditions_[i];
        }
    }
    if (!order_field_.empty()) {
        ss << " ORDER BY " << order_field_ << (order_ascending_ ? " ASC" : " DESC");
    }
    if (limit_ >= 0) ss << " LIMIT " << limit_;
    if (offset_ >= 0) ss << " OFFSET " << offset_;
    return ss.str();
}

std::string QueryBuilder::to_sql() const {
    return build_select_sql();
}

// Stub implementations (real execution requires a live DB connection)
std::vector<Record> QueryBuilder::get() { return {}; }
Record QueryBuilder::first() { return Record(); }
int QueryBuilder::count() { return 0; }
bool QueryBuilder::insert(const Record&) { return true; }
bool QueryBuilder::update(const Record&) { return true; }
bool QueryBuilder::delete_where() { return true; }

// ===== Model =====

Model::Model(const std::string& table_name) : table_name_(table_name) {}
Model::~Model() {}

void Model::add_field(const std::string& name, FieldType type) {
    fields_.push_back(Field(name, type));
}
void Model::add_primary_key(const std::string& name, FieldType type) {
    Field f(name, type);
    f.primary_key = true;
    f.nullable = false;
    fields_.push_back(f);
}
void Model::add_string_field(const std::string& name, int max_length) {
    Field f(name, FieldType::STRING);
    f.max_length = max_length;
    fields_.push_back(f);
}
void Model::add_text_field(const std::string& name) { add_field(name, FieldType::TEXT); }
void Model::add_int_field(const std::string& name) { add_field(name, FieldType::INTEGER); }
void Model::add_double_field(const std::string& name) { add_field(name, FieldType::DOUBLE); }
void Model::add_bool_field(const std::string& name) { add_field(name, FieldType::BOOLEAN); }
void Model::add_datetime_field(const std::string& name) { add_field(name, FieldType::DATETIME); }
void Model::add_json_field(const std::string& name) { add_field(name, FieldType::JSON); }

void Model::has_many(const std::string& related_model, const std::string& foreign_key) {
    relationships_["has_many:" + related_model] = foreign_key;
}
void Model::belongs_to(const std::string& related_model, const std::string& foreign_key) {
    relationships_["belongs_to:" + related_model] = foreign_key;
}
void Model::has_one(const std::string& related_model, const std::string& foreign_key) {
    relationships_["has_one:" + related_model] = foreign_key;
}

QueryBuilder Model::query() { return QueryBuilder(table_name_); }
QueryBuilder Model::where(const std::string& field, const std::string& op, const std::string& value) {
    return QueryBuilder(table_name_).where(field, op, value);
}
QueryBuilder Model::where_eq(const std::string& field, const std::string& value) {
    return QueryBuilder(table_name_).where_eq(field, value);
}

std::string Model::field_type_to_sql(FieldType type) const {
    switch (type) {
        case FieldType::INTEGER:  return "INTEGER";
        case FieldType::STRING:   return "VARCHAR";
        case FieldType::DOUBLE:   return "DOUBLE PRECISION";
        case FieldType::BOOLEAN:  return "BOOLEAN";
        case FieldType::DATETIME: return "TIMESTAMP";
        case FieldType::TEXT:     return "TEXT";
        case FieldType::JSON:     return "JSON";
        default:                  return "TEXT";
    }
}

std::string Model::get_create_table_sql() const {
    std::ostringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS " << table_name_ << " (";
    for (size_t i = 0; i < fields_.size(); i++) {
        if (i > 0) ss << ", ";
        const Field& f = fields_[i];
        ss << f.name << " " << field_type_to_sql(f.type);
        if (f.type == FieldType::STRING && f.max_length > 0)
            ss << "(" << f.max_length << ")";
        if (f.primary_key) ss << " PRIMARY KEY";
        if (!f.nullable && !f.primary_key) ss << " NOT NULL";
        if (f.unique) ss << " UNIQUE";
        if (!f.default_value.empty()) ss << " DEFAULT '" << f.default_value << "'";
    }
    ss << ")";
    return ss.str();
}

// Stub CRUD - real impl needs DB connection
Record Model::create(const Record& data) { return data; }
std::vector<Record> Model::all() { return {}; }
Record Model::find(int) { return Record(); }
Record Model::find_by(const std::string&, const std::string&) { return Record(); }
bool Model::update(int, const Record&) { return true; }
bool Model::delete_record(int) { return true; }
bool Model::create_table() { return true; }
bool Model::drop_table() { return true; }

// ===== Migration =====

Migration::Migration(const std::string& name) : name_(name) {}
Migration::~Migration() {}

void Migration::create_table(const std::string& name, std::function<void(Model&)> schema) {
    Model m(name);
    schema(m);
    operations_.push_back(m.get_create_table_sql());
}
void Migration::drop_table(const std::string& name) {
    operations_.push_back("DROP TABLE IF EXISTS " + name);
}
void Migration::add_column(const std::string& table, const std::string& column, FieldType type) {
    // Map type to SQL directly
    std::string type_sql;
    switch (type) {
        case FieldType::INTEGER:  type_sql = "INTEGER"; break;
        case FieldType::STRING:   type_sql = "VARCHAR(255)"; break;
        case FieldType::DOUBLE:   type_sql = "DOUBLE PRECISION"; break;
        case FieldType::BOOLEAN:  type_sql = "BOOLEAN"; break;
        case FieldType::DATETIME: type_sql = "TIMESTAMP"; break;
        case FieldType::TEXT:     type_sql = "TEXT"; break;
        case FieldType::JSON:     type_sql = "JSON"; break;
        default:                  type_sql = "TEXT"; break;
    }
    std::string sql = "ALTER TABLE " + table + " ADD COLUMN " + column + " " + type_sql;
    operations_.push_back(sql);
}
void Migration::drop_column(const std::string& table, const std::string& column) {
    operations_.push_back("ALTER TABLE " + table + " DROP COLUMN " + column);
}
void Migration::add_index(const std::string& table, const std::string& column) {
    operations_.push_back("CREATE INDEX IF NOT EXISTS idx_" + table + "_" + column + " ON " + table + "(" + column + ")");
}
void Migration::drop_index(const std::string& table, const std::string& column) {
    operations_.push_back("DROP INDEX IF EXISTS idx_" + table + "_" + column);
}

// ===== DatabaseConnection =====

DatabaseConnection::DatabaseConnection() {}
DatabaseConnection::~DatabaseConnection() { disconnect(); }

bool DatabaseConnection::connect(const std::string& connection_string) {
    connection_string_ = connection_string;
    connected_ = true; // Stub - real impl would open connection
    return true;
}
bool DatabaseConnection::connect_sqlite(const std::string& database_path) {
    return connect("sqlite:" + database_path);
}
bool DatabaseConnection::connect_postgres(const std::string& host, int port, const std::string& database,
                                          const std::string& username, const std::string& password) {
    std::ostringstream ss;
    ss << "host=" << host << " port=" << port << " dbname=" << database
       << " user=" << username << " password=" << password;
    return connect(ss.str());
}
void DatabaseConnection::disconnect() {
    connected_ = false;
    connection_handle_ = nullptr;
}

std::vector<Record> DatabaseConnection::execute_query(const std::string&) { return {}; }
bool DatabaseConnection::execute_command(const std::string&) { return connected_; }
int DatabaseConnection::execute_scalar(const std::string&) { return 0; }
bool DatabaseConnection::begin_transaction() { return connected_; }
bool DatabaseConnection::commit_transaction() { return connected_; }
bool DatabaseConnection::rollback_transaction() { return connected_; }
bool DatabaseConnection::table_exists(const std::string&) { return false; }
std::vector<std::string> DatabaseConnection::get_table_names() { return {}; }
std::vector<Field> DatabaseConnection::get_table_schema(const std::string&) { return {}; }

bool DatabaseConnection::execute_sqlite_query(const std::string&, std::vector<Record>&) { return false; }
bool DatabaseConnection::execute_postgres_query(const std::string&, std::vector<Record>&) { return false; }

// ===== ORMManager =====

ORMManager& ORMManager::instance() {
    static ORMManager inst;
    return inst;
}
void ORMManager::set_connection(std::shared_ptr<DatabaseConnection> conn) { connection_ = conn; }
std::shared_ptr<DatabaseConnection> ORMManager::get_connection() { return connection_; }
void ORMManager::register_model(const std::string& name, std::shared_ptr<Model> model) {
    models_[name] = model;
}
std::shared_ptr<Model> ORMManager::get_model(const std::string& name) {
    auto it = models_.find(name);
    return it != models_.end() ? it->second : nullptr;
}
void ORMManager::add_migration(std::shared_ptr<Migration> migration) {
    migrations_.push_back(migration);
}
bool ORMManager::run_migrations() { return true; }
bool ORMManager::rollback_migration(const std::string&) { return true; }
bool ORMManager::create_all_tables() { return true; }
bool ORMManager::drop_all_tables() { return true; }

} // namespace ORM
} // namespace Sapphire

// ===== C API =====

extern "C" {

void* orm_model_create(const char* table_name) {
    return new Sapphire::ORM::Model(table_name);
}
void orm_model_destroy(void* model) {
    delete static_cast<Sapphire::ORM::Model*>(model);
}
void orm_model_add_string_field(void* model, const char* name, int max_length) {
    static_cast<Sapphire::ORM::Model*>(model)->add_string_field(name, max_length);
}
void orm_model_add_int_field(void* model, const char* name) {
    static_cast<Sapphire::ORM::Model*>(model)->add_int_field(name);
}
void orm_model_add_primary_key(void* model, const char* name) {
    static_cast<Sapphire::ORM::Model*>(model)->add_primary_key(name);
}
int orm_model_create_table(void* model) {
    return static_cast<Sapphire::ORM::Model*>(model)->create_table() ? 1 : 0;
}
const char* orm_model_get_create_sql(void* model) {
    static std::string result;
    result = static_cast<Sapphire::ORM::Model*>(model)->get_create_table_sql();
    return result.c_str();
}

void* orm_record_create() {
    return new Sapphire::ORM::Record();
}
void orm_record_destroy(void* record) {
    delete static_cast<Sapphire::ORM::Record*>(record);
}
void orm_record_set_string(void* record, const char* field, const char* value) {
    static_cast<Sapphire::ORM::Record*>(record)->set(field, std::string(value));
}
void orm_record_set_int(void* record, const char* field, int value) {
    static_cast<Sapphire::ORM::Record*>(record)->set(field, value);
}
const char* orm_record_get_string(void* record, const char* field) {
    static std::string result;
    result = static_cast<Sapphire::ORM::Record*>(record)->get_string(field);
    return result.c_str();
}
int orm_record_get_int(void* record, const char* field) {
    return static_cast<Sapphire::ORM::Record*>(record)->get_int(field);
}
const char* orm_record_to_json(void* record) {
    static std::string result;
    result = static_cast<Sapphire::ORM::Record*>(record)->to_json();
    return result.c_str();
}

void* orm_query_create(const char* table_name) {
    return new Sapphire::ORM::QueryBuilder(table_name);
}
void orm_query_destroy(void* query) {
    delete static_cast<Sapphire::ORM::QueryBuilder*>(query);
}
void orm_query_where_eq(void* query, const char* field, const char* value) {
    static_cast<Sapphire::ORM::QueryBuilder*>(query)->where_eq(field, value);
}
void orm_query_limit(void* query, int count) {
    static_cast<Sapphire::ORM::QueryBuilder*>(query)->limit(count);
}
const char* orm_query_to_sql(void* query) {
    static std::string result;
    result = static_cast<Sapphire::ORM::QueryBuilder*>(query)->to_sql();
    return result.c_str();
}
void* orm_query_get_all(void* query) {
    (void)query;
    return nullptr; // Stub
}
void* orm_query_first(void* query) {
    (void)query;
    return nullptr; // Stub
}

void* orm_db_create() {
    return new Sapphire::ORM::DatabaseConnection();
}
void orm_db_destroy(void* db) {
    delete static_cast<Sapphire::ORM::DatabaseConnection*>(db);
}
int orm_db_connect_sqlite(void* db, const char* path) {
    return static_cast<Sapphire::ORM::DatabaseConnection*>(db)->connect_sqlite(path) ? 1 : 0;
}
int orm_db_execute_command(void* db, const char* sql) {
    return static_cast<Sapphire::ORM::DatabaseConnection*>(db)->execute_command(sql) ? 1 : 0;
}

void orm_free_string(char* str) {
    free(str);
}

} // extern "C"
