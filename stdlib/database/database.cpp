#include "database.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <random>

namespace Sapphire {
namespace Database {

// ─── DbValue helpers ────────────────────────────────────────────────────────

std::string dbval_to_string(const DbValue& v) {
    return std::visit([](auto&& a) -> std::string {
        using T = std::decay_t<decltype(a)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) return "NULL";
        else if constexpr (std::is_same_v<T, int64_t>)   return std::to_string(a);
        else if constexpr (std::is_same_v<T, double>)     return std::to_string(a);
        else if constexpr (std::is_same_v<T, bool>)       return a ? "true" : "false";
        else return a;
    }, v);
}

DbValue dbval_null()                    { return nullptr; }
DbValue dbval_int(int64_t v)            { return v; }
DbValue dbval_real(double v)            { return v; }
DbValue dbval_text(const std::string& v){ return v; }
DbValue dbval_bool(bool v)              { return v; }

// ─── ResultSet helpers ───────────────────────────────────────────────────────

std::string ResultSet::get_string(int r, int c) const {
    return dbval_to_string(rows[r][c]);
}
int64_t ResultSet::get_int(int r, int c) const {
    if (auto* p = std::get_if<int64_t>(&rows[r][c])) return *p;
    return 0;
}
double ResultSet::get_real(int r, int c) const {
    if (auto* p = std::get_if<double>(&rows[r][c])) return *p;
    return 0.0;
}
bool ResultSet::get_bool(int r, int c) const {
    if (auto* p = std::get_if<bool>(&rows[r][c])) return *p;
    return false;
}
bool ResultSet::is_null(int r, int c) const {
    return std::holds_alternative<std::nullptr_t>(rows[r][c]);
}

// ─── Driver stubs ────────────────────────────────────────────────────────────
// Each driver simulates a real connection. In production these would link
// against libmysqlclient, libpq, libsqlite3, mongoc, etc.

class SQLiteDriver : public IDriver {
    bool connected_ = false;
    std::string db_path_;
    std::string last_error_;
    // In production: sqlite3* handle_
public:
    bool connect(const ConnConfig& cfg) override {
        db_path_ = cfg.database.empty() ? ":memory:" : cfg.database;
        connected_ = true;
        return true;
    }
    void disconnect() override { connected_ = false; }
    bool is_connected() const override { return connected_; }
    std::string driver_name()    const override { return "sqlite"; }
    std::string server_version() const override { return "SQLite 3.45.0"; }
    std::string last_error()     const override { return last_error_; }

    ResultSet query(const std::string& sql, const std::vector<DbValue>& params) override {
        ResultSet r; r.ok = connected_;
        if (!connected_) { r.error = "Not connected"; return r; }
        // Production: sqlite3_prepare_v2 / sqlite3_step
        r.columns = {"result"}; r.rows = {{dbval_text("SQLite: " + sql)}};
        return r;
    }
    ResultSet exec(const std::string& sql, const std::vector<DbValue>& params) override {
        ResultSet r; r.ok = connected_;
        if (!connected_) { r.error = "Not connected"; return r; }
        r.rows_affected = 1;
        return r;
    }
    bool begin_tx()    override { return connected_; }
    bool commit_tx()   override { return connected_; }
    bool rollback_tx() override { return connected_; }
};

class MySQLDriver : public IDriver {
    bool connected_ = false; std::string last_error_;
    // In production: MYSQL* handle_
public:
    bool connect(const ConnConfig& cfg) override {
        // Production: mysql_real_connect(handle_, host, user, pass, db, port, ...)
        connected_ = true; return true;
    }
    void disconnect() override { connected_ = false; }
    bool is_connected() const override { return connected_; }
    std::string driver_name()    const override { return "mysql"; }
    std::string server_version() const override { return "MySQL 8.0.36"; }
    std::string last_error()     const override { return last_error_; }
    ResultSet query(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_;
        if (!connected_) { r.error = "Not connected"; return r; }
        r.columns = {"result"}; r.rows = {{dbval_text("MySQL: " + sql)}};
        return r;
    }
    ResultSet exec(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_; r.rows_affected = 1; return r;
    }
    bool begin_tx()    override { return connected_; }
    bool commit_tx()   override { return connected_; }
    bool rollback_tx() override { return connected_; }
};

class PostgreSQLDriver : public IDriver {
    bool connected_ = false; std::string last_error_;
    // In production: PGconn* handle_
public:
    bool connect(const ConnConfig& cfg) override {
        // Production: PQconnectdb(conninfo)
        connected_ = true; return true;
    }
    void disconnect() override { connected_ = false; }
    bool is_connected() const override { return connected_; }
    std::string driver_name()    const override { return "postgresql"; }
    std::string server_version() const override { return "PostgreSQL 16.2"; }
    std::string last_error()     const override { return last_error_; }
    ResultSet query(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_;
        if (!connected_) { r.error = "Not connected"; return r; }
        r.columns = {"result"}; r.rows = {{dbval_text("PostgreSQL: " + sql)}};
        return r;
    }
    ResultSet exec(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_; r.rows_affected = 1; return r;
    }
    bool begin_tx()    override { return connected_; }
    bool commit_tx()   override { return connected_; }
    bool rollback_tx() override { return connected_; }
};

class MSSQLDriver : public IDriver {
    bool connected_ = false; std::string last_error_;
    // In production: SQLHENV/SQLHDBC via ODBC or FreeTDS
public:
    bool connect(const ConnConfig& cfg) override { connected_ = true; return true; }
    void disconnect() override { connected_ = false; }
    bool is_connected() const override { return connected_; }
    std::string driver_name()    const override { return "mssql"; }
    std::string server_version() const override { return "Microsoft SQL Server 2022"; }
    std::string last_error()     const override { return last_error_; }
    ResultSet query(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_;
        r.columns = {"result"}; r.rows = {{dbval_text("MSSQL: " + sql)}};
        return r;
    }
    ResultSet exec(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_; r.rows_affected = 1; return r;
    }
    bool begin_tx()    override { return connected_; }
    bool commit_tx()   override { return connected_; }
    bool rollback_tx() override { return connected_; }
};

class OracleDriver : public IDriver {
    bool connected_ = false; std::string last_error_;
    // In production: OCIEnv/OCISvcCtx via Oracle OCI
public:
    bool connect(const ConnConfig& cfg) override { connected_ = true; return true; }
    void disconnect() override { connected_ = false; }
    bool is_connected() const override { return connected_; }
    std::string driver_name()    const override { return "oracle"; }
    std::string server_version() const override { return "Oracle Database 21c"; }
    std::string last_error()     const override { return last_error_; }
    ResultSet query(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_;
        r.columns = {"result"}; r.rows = {{dbval_text("Oracle: " + sql)}};
        return r;
    }
    ResultSet exec(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_; r.rows_affected = 1; return r;
    }
    bool begin_tx()    override { return connected_; }
    bool commit_tx()   override { return connected_; }
    bool rollback_tx() override { return connected_; }
};

class MongoDBDriver : public IDriver {
    bool connected_ = false; std::string last_error_;
    // In production: mongoc_client_t* via mongo-c-driver
public:
    bool connect(const ConnConfig& cfg) override { connected_ = true; return true; }
    void disconnect() override { connected_ = false; }
    bool is_connected() const override { return connected_; }
    std::string driver_name()    const override { return "mongodb"; }
    std::string server_version() const override { return "MongoDB 7.0"; }
    std::string last_error()     const override { return last_error_; }
    ResultSet query(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_;
        r.columns = {"_id","data"}; r.rows = {{dbval_text("mongo_id_1"), dbval_text(sql)}};
        return r;
    }
    ResultSet exec(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_; r.rows_affected = 1; return r;
    }
    bool begin_tx()    override { return connected_; } // MongoDB 4.0+ sessions
    bool commit_tx()   override { return connected_; }
    bool rollback_tx() override { return connected_; }
};

class SupabaseDriver : public IDriver {
    bool connected_ = false; std::string last_error_;
    // Supabase is PostgreSQL under the hood — uses PostgREST HTTP API or libpq
public:
    bool connect(const ConnConfig& cfg) override { connected_ = true; return true; }
    void disconnect() override { connected_ = false; }
    bool is_connected() const override { return connected_; }
    std::string driver_name()    const override { return "supabase"; }
    std::string server_version() const override { return "Supabase (PostgreSQL 15)"; }
    std::string last_error()     const override { return last_error_; }
    ResultSet query(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_;
        r.columns = {"result"}; r.rows = {{dbval_text("Supabase: " + sql)}};
        return r;
    }
    ResultSet exec(const std::string& sql, const std::vector<DbValue>&) override {
        ResultSet r; r.ok = connected_; r.rows_affected = 1; return r;
    }
    bool begin_tx()    override { return connected_; }
    bool commit_tx()   override { return connected_; }
    bool rollback_tx() override { return connected_; }
};

// ─── Driver registry ─────────────────────────────────────────────────────────

IDriver* create_driver(const std::string& name) {
    if (name == "sqlite")     return new SQLiteDriver();
    if (name == "mysql")      return new MySQLDriver();
    if (name == "postgresql" || name == "postgres") return new PostgreSQLDriver();
    if (name == "mssql" || name == "sqlserver")     return new MSSQLDriver();
    if (name == "oracle")     return new OracleDriver();
    if (name == "mongodb")    return new MongoDBDriver();
    if (name == "supabase")   return new SupabaseDriver();
    throw std::runtime_error("Unknown database driver: " + name);
}

// ─── Connection ──────────────────────────────────────────────────────────────

Connection::Connection(IDriver* drv) : drv_(drv) {}
Connection::~Connection() { if (drv_) { drv_->disconnect(); delete drv_; } }

bool      Connection::connect(const ConnConfig& cfg) { return drv_->connect(cfg); }
void      Connection::disconnect()                   { drv_->disconnect(); }
bool      Connection::is_connected() const           { return drv_->is_connected(); }
ResultSet Connection::query(const std::string& sql, const std::vector<DbValue>& p) { return drv_->query(sql, p); }
ResultSet Connection::exec (const std::string& sql, const std::vector<DbValue>& p) { return drv_->exec(sql, p); }
ResultSet Connection::query_prepared(const std::string& sql, const std::vector<DbValue>& p) { return drv_->query(sql, p); }
ResultSet Connection::exec_prepared (const std::string& sql, const std::vector<DbValue>& p) { return drv_->exec(sql, p); }
bool      Connection::begin()    { in_tx_ = true;  return drv_->begin_tx(); }
bool      Connection::commit()   { in_tx_ = false; return drv_->commit_tx(); }
bool      Connection::rollback() { in_tx_ = false; return drv_->rollback_tx(); }
std::string Connection::driver_name()    const { return drv_->driver_name(); }
std::string Connection::server_version() const { return drv_->server_version(); }
std::string Connection::last_error()     const { return drv_->last_error(); }

// ─── URL parser helper ───────────────────────────────────────────────────────
// Parses: driver://user:pass@host:port/database?ssl=true

static ConnConfig parse_url(const std::string& url) {
    ConnConfig cfg;
    auto sep = url.find("://");
    if (sep == std::string::npos) { cfg.url = url; return cfg; }
    cfg.driver = url.substr(0, sep);
    std::string rest = url.substr(sep + 3);

    // user:pass@
    auto at = rest.find('@');
    if (at != std::string::npos) {
        std::string creds = rest.substr(0, at);
        rest = rest.substr(at + 1);
        auto colon = creds.find(':');
        if (colon != std::string::npos) {
            cfg.username = creds.substr(0, colon);
            cfg.password = creds.substr(colon + 1);
        } else { cfg.username = creds; }
    }

    // host:port/database
    auto slash = rest.find('/');
    std::string hostport = (slash != std::string::npos) ? rest.substr(0, slash) : rest;
    if (slash != std::string::npos) cfg.database = rest.substr(slash + 1);

    auto colon = hostport.find(':');
    if (colon != std::string::npos) {
        cfg.host = hostport.substr(0, colon);
        cfg.port = std::stoi(hostport.substr(colon + 1));
    } else { cfg.host = hostport; }

    return cfg;
}

Connection* db_connect(const ConnConfig& cfg) {
    IDriver* drv = create_driver(cfg.driver);
    auto* conn = new Connection(drv);
    if (!conn->connect(cfg)) {
        delete conn;
        return nullptr;
    }
    return conn;
}

Connection* db_connect_url(const std::string& url) {
    return db_connect(parse_url(url));
}

void db_disconnect(Connection* conn) {
    if (conn) { conn->disconnect(); delete conn; }
}

// ─── Connection Pool ─────────────────────────────────────────────────────────

ConnectionPool::ConnectionPool(const ConnConfig& cfg, int size)
    : cfg_(cfg), size_(size), pool_(size, nullptr), in_use_(size, false) {
    for (int i = 0; i < size; i++) {
        pool_[i] = db_connect(cfg);
    }
}
ConnectionPool::~ConnectionPool() {
    for (auto* c : pool_) delete c;
}
Connection* ConnectionPool::acquire() {
    for (int i = 0; i < size_; i++) {
        if (!in_use_[i] && pool_[i] && pool_[i]->is_connected()) {
            in_use_[i] = true;
            return pool_[i];
        }
    }
    return nullptr; // pool exhausted
}
void ConnectionPool::release(Connection* conn) {
    for (int i = 0; i < size_; i++) {
        if (pool_[i] == conn) { in_use_[i] = false; return; }
    }
}
int ConnectionPool::size()   const { return size_; }
int ConnectionPool::active() const { int n=0; for (auto b:in_use_) if(b) n++; return n; }
int ConnectionPool::idle()   const { return size_ - active(); }

ConnectionPool* db_pool_create(const ConnConfig& cfg, int size) { return new ConnectionPool(cfg, size); }
void            db_pool_destroy(ConnectionPool* p) { delete p; }

// ─── Query Builder ───────────────────────────────────────────────────────────

QueryBuilder::QueryBuilder(Connection* conn, const std::string& table)
    : conn_(conn), table_(table), op_("SELECT") {}

QueryBuilder& QueryBuilder::select(const std::vector<std::string>& cols) { select_cols_=cols; op_="SELECT"; return *this; }
QueryBuilder& QueryBuilder::where(const std::string& e, const std::vector<DbValue>& p) { where_clauses_={e}; where_params_=p; return *this; }
QueryBuilder& QueryBuilder::and_where(const std::string& e, const std::vector<DbValue>& p) { where_clauses_.push_back("AND "+e); for(auto&v:p) where_params_.push_back(v); return *this; }
QueryBuilder& QueryBuilder::or_where(const std::string& e, const std::vector<DbValue>& p)  { where_clauses_.push_back("OR "+e);  for(auto&v:p) where_params_.push_back(v); return *this; }
QueryBuilder& QueryBuilder::order_by(const std::string& col, bool asc) { order_col_=col; order_asc_=asc; return *this; }
QueryBuilder& QueryBuilder::limit(int n)  { limit_=n;  return *this; }
QueryBuilder& QueryBuilder::offset(int n) { offset_=n; return *this; }
QueryBuilder& QueryBuilder::join(const std::string& t, const std::string& on) { join_clause_="JOIN "+t+" ON "+on; return *this; }
QueryBuilder& QueryBuilder::left_join(const std::string& t, const std::string& on) { join_clause_="LEFT JOIN "+t+" ON "+on; return *this; }
QueryBuilder& QueryBuilder::group_by(const std::string& col) { group_col_=col; return *this; }
QueryBuilder& QueryBuilder::having(const std::string& e) { having_clause_=e; return *this; }
QueryBuilder& QueryBuilder::set(const std::string& col, const DbValue& val) { set_vals_[col]=val; return *this; }
QueryBuilder& QueryBuilder::values(const std::map<std::string,DbValue>& data) { set_vals_=data; return *this; }

std::string QueryBuilder::to_sql() const {
    std::ostringstream ss;
    if (op_ == "SELECT") {
        ss << "SELECT ";
        if (select_cols_.empty()) ss << "*";
        else for (size_t i=0;i<select_cols_.size();i++) { if(i) ss<<","; ss<<select_cols_[i]; }
        ss << " FROM " << table_;
        if (!join_clause_.empty()) ss << " " << join_clause_;
        if (!where_clauses_.empty()) { ss << " WHERE "; for(auto&w:where_clauses_) ss<<w<<" "; }
        if (!group_col_.empty()) ss << " GROUP BY " << group_col_;
        if (!having_clause_.empty()) ss << " HAVING " << having_clause_;
        if (!order_col_.empty()) ss << " ORDER BY " << order_col_ << (order_asc_?" ASC":" DESC");
        if (limit_  >= 0) ss << " LIMIT "  << limit_;
        if (offset_ >  0) ss << " OFFSET " << offset_;
    } else if (op_ == "INSERT") {
        ss << "INSERT INTO " << table_ << " (";
        bool first=true; for(auto&[k,v]:set_vals_){if(!first)ss<<",";ss<<k;first=false;}
        ss << ") VALUES (";
        first=true; for(auto&[k,v]:set_vals_){if(!first)ss<<",";ss<<"?";first=false;}
        ss << ")";
    } else if (op_ == "UPDATE") {
        ss << "UPDATE " << table_ << " SET ";
        bool first=true; for(auto&[k,v]:set_vals_){if(!first)ss<<",";ss<<k<<"=?";first=false;}
        if (!where_clauses_.empty()) { ss << " WHERE "; for(auto&w:where_clauses_) ss<<w<<" "; }
    } else if (op_ == "DELETE") {
        ss << "DELETE FROM " << table_;
        if (!where_clauses_.empty()) { ss << " WHERE "; for(auto&w:where_clauses_) ss<<w<<" "; }
    }
    return ss.str();
}

ResultSet QueryBuilder::get()    { op_="SELECT"; return conn_->query(to_sql(), where_params_); }
ResultSet QueryBuilder::first()  { op_="SELECT"; limit_=1; return conn_->query(to_sql(), where_params_); }
ResultSet QueryBuilder::insert() { op_="INSERT"; std::vector<DbValue> p; for(auto&[k,v]:set_vals_) p.push_back(v); return conn_->exec(to_sql(), p); }
ResultSet QueryBuilder::update() { op_="UPDATE"; std::vector<DbValue> p; for(auto&[k,v]:set_vals_) p.push_back(v); for(auto&v:where_params_) p.push_back(v); return conn_->exec(to_sql(), p); }
ResultSet QueryBuilder::remove() { op_="DELETE"; return conn_->exec(to_sql(), where_params_); }
int64_t   QueryBuilder::count()  { op_="SELECT"; select_cols_={"COUNT(*) as count"}; auto r=conn_->query(to_sql(),where_params_); return r.row_count()>0?r.get_int(0,0):0; }
bool      QueryBuilder::exists() { return count() > 0; }

QueryBuilder* qb_table(Connection* conn, const std::string& table) { return new QueryBuilder(conn, table); }
void          qb_destroy(QueryBuilder* qb) { delete qb; }

// ─── ORM ─────────────────────────────────────────────────────────────────────

ORM::ORM(Connection* conn) : conn_(conn) {}

void ORM::define(const ModelDef& m) { models_[m.name] = m; }

std::string ORM::pk_field(const std::string& model) const {
    auto it = models_.find(model);
    if (it == models_.end()) return "id";
    for (auto& f : it->second.fields) if (f.primary_key) return f.name;
    return "id";
}

std::string ORM::build_create_table(const ModelDef& m) const {
    std::ostringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS " << m.table << " (";
    bool first = true;
    for (auto& f : m.fields) {
        if (!first) ss << ", ";
        ss << f.name << " ";
        if      (f.type == "int")      ss << "INTEGER";
        else if (f.type == "float")    ss << "REAL";
        else if (f.type == "bool")     ss << "BOOLEAN";
        else if (f.type == "datetime") ss << "DATETIME";
        else                           ss << "TEXT";
        if (f.primary_key)    ss << " PRIMARY KEY";
        if (f.auto_increment) ss << " AUTOINCREMENT";
        if (!f.nullable)      ss << " NOT NULL";
        if (f.unique)         ss << " UNIQUE";
        if (!f.default_val.empty()) ss << " DEFAULT " << f.default_val;
        first = false;
    }
    ss << ")";
    return ss.str();
}

bool ORM::migrate() {
    for (auto& [name, model] : models_) {
        auto r = conn_->exec(build_create_table(model));
        if (!r.ok) return false;
    }
    return true;
}

bool ORM::drop(const std::string& model) {
    auto it = models_.find(model);
    if (it == models_.end()) return false;
    return conn_->exec("DROP TABLE IF EXISTS " + it->second.table).ok;
}

ResultSet ORM::find_all(const std::string& model) {
    auto it = models_.find(model);
    if (it == models_.end()) { ResultSet r; r.ok=false; r.error="Unknown model"; return r; }
    return conn_->query("SELECT * FROM " + it->second.table);
}

ResultSet ORM::find_by_id(const std::string& model, const DbValue& id) {
    auto it = models_.find(model);
    if (it == models_.end()) { ResultSet r; r.ok=false; r.error="Unknown model"; return r; }
    return conn_->query("SELECT * FROM " + it->second.table + " WHERE " + pk_field(model) + " = ?", {id});
}

ResultSet ORM::find_where(const std::string& model, const std::string& field, const DbValue& val) {
    auto it = models_.find(model);
    if (it == models_.end()) { ResultSet r; r.ok=false; r.error="Unknown model"; return r; }
    return conn_->query("SELECT * FROM " + it->second.table + " WHERE " + field + " = ?", {val});
}

ResultSet ORM::find_raw(const std::string& model, const std::string& where_sql, const std::vector<DbValue>& params) {
    auto it = models_.find(model);
    if (it == models_.end()) { ResultSet r; r.ok=false; r.error="Unknown model"; return r; }
    return conn_->query("SELECT * FROM " + it->second.table + " WHERE " + where_sql, params);
}

ResultSet ORM::create(const std::string& model, const std::map<std::string,DbValue>& data) {
    auto it = models_.find(model);
    if (it == models_.end()) { ResultSet r; r.ok=false; r.error="Unknown model"; return r; }
    std::ostringstream cols, vals;
    std::vector<DbValue> params;
    bool first = true;
    for (auto& [k,v] : data) {
        if (!first) { cols<<","; vals<<","; }
        cols << k; vals << "?"; params.push_back(v); first = false;
    }
    return conn_->exec("INSERT INTO " + it->second.table + " (" + cols.str() + ") VALUES (" + vals.str() + ")", params);
}

ResultSet ORM::update_by_id(const std::string& model, const DbValue& id, const std::map<std::string,DbValue>& data) {
    auto it = models_.find(model);
    if (it == models_.end()) { ResultSet r; r.ok=false; r.error="Unknown model"; return r; }
    std::ostringstream ss;
    std::vector<DbValue> params;
    bool first = true;
    for (auto& [k,v] : data) {
        if (!first) ss << ", ";
        ss << k << " = ?"; params.push_back(v); first = false;
    }
    params.push_back(id);
    return conn_->exec("UPDATE " + it->second.table + " SET " + ss.str() + " WHERE " + pk_field(model) + " = ?", params);
}

ResultSet ORM::delete_by_id(const std::string& model, const DbValue& id) {
    auto it = models_.find(model);
    if (it == models_.end()) { ResultSet r; r.ok=false; r.error="Unknown model"; return r; }
    return conn_->exec("DELETE FROM " + it->second.table + " WHERE " + pk_field(model) + " = ?", {id});
}

int64_t ORM::count(const std::string& model) {
    auto it = models_.find(model);
    if (it == models_.end()) return 0;
    auto r = conn_->query("SELECT COUNT(*) FROM " + it->second.table);
    return r.row_count() > 0 ? r.get_int(0,0) : 0;
}

QueryBuilder* ORM::query(const std::string& model) {
    auto it = models_.find(model);
    std::string table = (it != models_.end()) ? it->second.table : model;
    return new QueryBuilder(conn_, table);
}

ORM*  orm_create(Connection* conn) { return new ORM(conn); }
void  orm_destroy(ORM* orm)        { delete orm; }

// ─── Document Collection ─────────────────────────────────────────────────────

static std::string gen_id() {
    static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    return std::to_string(rng());
}

DocumentCollection::DocumentCollection(Connection* conn, const std::string& coll)
    : conn_(conn), collection_(coll) {}

std::string DocumentCollection::insert_one(const std::map<std::string,std::string>& doc) {
    std::string id = gen_id();
    std::ostringstream sql;
    sql << "INSERT INTO " << collection_ << " (_id";
    std::vector<DbValue> params = {dbval_text(id)};
    for (auto& [k,v] : doc) { sql << "," << k; params.push_back(dbval_text(v)); }
    sql << ") VALUES (?";
    for (size_t i=1;i<params.size();i++) sql << ",?";
    sql << ")";
    conn_->exec(sql.str(), params);
    return id;
}

std::vector<Document> DocumentCollection::find(const std::string& field, const std::string& value) {
    auto r = conn_->query("SELECT * FROM " + collection_ + " WHERE " + field + " = ?", {dbval_text(value)});
    std::vector<Document> docs;
    for (int i=0;i<r.row_count();i++) {
        Document d;
        d.id = r.row_count()>0 ? r.get_string(i,0) : "";
        for (int c=0;c<r.col_count();c++) d.fields[r.columns[c]] = r.get_string(i,c);
        docs.push_back(d);
    }
    return docs;
}

std::vector<Document> DocumentCollection::find_all() {
    auto r = conn_->query("SELECT * FROM " + collection_);
    std::vector<Document> docs;
    for (int i=0;i<r.row_count();i++) {
        Document d;
        for (int c=0;c<r.col_count();c++) d.fields[r.columns[c]] = r.get_string(i,c);
        docs.push_back(d);
    }
    return docs;
}

bool DocumentCollection::update_one(const std::string& id, const std::string& field, const std::string& value) {
    return conn_->exec("UPDATE " + collection_ + " SET " + field + " = ? WHERE _id = ?",
                       {dbval_text(value), dbval_text(id)}).ok;
}

bool DocumentCollection::delete_one(const std::string& id) {
    return conn_->exec("DELETE FROM " + collection_ + " WHERE _id = ?", {dbval_text(id)}).ok;
}

int DocumentCollection::count() {
    auto r = conn_->query("SELECT COUNT(*) FROM " + collection_);
    return r.row_count() > 0 ? (int)r.get_int(0,0) : 0;
}

DocumentCollection* doc_collection(Connection* conn, const std::string& name) {
    return new DocumentCollection(conn, name);
}
void doc_collection_destroy(DocumentCollection* c) { delete c; }

} // namespace Database
} // namespace Sapphire
