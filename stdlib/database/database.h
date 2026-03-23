#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <functional>
#include <cstdint>

namespace Sapphire {
namespace Database {

// ===== Core value type =====
using DbValue = std::variant<std::nullptr_t, int64_t, double, std::string, bool>;
using Row     = std::vector<DbValue>;
using Rows    = std::vector<Row>;

std::string dbval_to_string(const DbValue& v);
DbValue     dbval_null();
DbValue     dbval_int(int64_t v);
DbValue     dbval_real(double v);
DbValue     dbval_text(const std::string& v);
DbValue     dbval_bool(bool v);

// ===== ResultSet — unified result from any driver =====
struct ResultSet {
    std::vector<std::string> columns;
    Rows   rows;
    int64_t rows_affected = 0;
    int64_t last_insert_id = 0;
    std::string error;
    bool ok = true;

    int  row_count()  const { return (int)rows.size(); }
    int  col_count()  const { return (int)columns.size(); }
    bool has_error()  const { return !ok; }
    std::string get_string(int row, int col) const;
    int64_t     get_int   (int row, int col) const;
    double      get_real  (int row, int col) const;
    bool        get_bool  (int row, int col) const;
    bool        is_null   (int row, int col) const;
};

// ===== Connection config =====
struct ConnConfig {
    std::string driver;    // "sqlite", "mysql", "postgresql", "mssql", "oracle", "mongodb", "supabase"
    std::string host;
    int         port       = 0;
    std::string database;
    std::string username;
    std::string password;
    std::string url;       // full connection string / URI (alternative to above)
    bool        ssl        = false;
    int         timeout_ms = 5000;
    int         pool_size  = 5;
    std::map<std::string, std::string> options;
};

// ===== Abstract driver interface =====
class IDriver {
public:
    virtual ~IDriver() = default;
    virtual bool        connect(const ConnConfig& cfg)                                    = 0;
    virtual void        disconnect()                                                       = 0;
    virtual bool        is_connected() const                                               = 0;
    virtual ResultSet   query(const std::string& sql,
                              const std::vector<DbValue>& params = {})                    = 0;
    virtual ResultSet   exec(const std::string& sql,
                             const std::vector<DbValue>& params = {})                     = 0;
    virtual bool        begin_tx()                                                         = 0;
    virtual bool        commit_tx()                                                        = 0;
    virtual bool        rollback_tx()                                                      = 0;
    virtual std::string driver_name() const                                                = 0;
    virtual std::string server_version() const                                             = 0;
    virtual std::string last_error() const                                                 = 0;
};

// ===== Driver registry =====
IDriver* create_driver(const std::string& driver_name);

// ===== Connection (wraps a driver) =====
class Connection {
public:
    explicit Connection(IDriver* drv);
    ~Connection();

    bool        connect(const ConnConfig& cfg);
    void        disconnect();
    bool        is_connected() const;

    ResultSet   query(const std::string& sql, const std::vector<DbValue>& params = {});
    ResultSet   exec (const std::string& sql, const std::vector<DbValue>& params = {});

    // Prepared statement helpers
    ResultSet   query_prepared(const std::string& sql, const std::vector<DbValue>& params);
    ResultSet   exec_prepared (const std::string& sql, const std::vector<DbValue>& params);

    // Transactions
    bool begin();
    bool commit();
    bool rollback();

    // Convenience
    std::string driver_name()    const;
    std::string server_version() const;
    std::string last_error()     const;

private:
    IDriver* drv_;
    bool     in_tx_ = false;
};

// ===== Connection factory =====
Connection* db_connect(const ConnConfig& cfg);
Connection* db_connect_url(const std::string& url);   // "mysql://user:pass@host:3306/db"
void        db_disconnect(Connection* conn);

// ===== Connection Pool =====
class ConnectionPool {
public:
    ConnectionPool(const ConnConfig& cfg, int size = 5);
    ~ConnectionPool();

    Connection* acquire();
    void        release(Connection* conn);
    int         size()   const;
    int         active() const;
    int         idle()   const;

private:
    ConnConfig cfg_;
    std::vector<Connection*> pool_;
    std::vector<bool>        in_use_;
    int size_;
};

ConnectionPool* db_pool_create(const ConnConfig& cfg, int size);
void            db_pool_destroy(ConnectionPool* pool);

// ===== Query Builder =====
class QueryBuilder {
public:
    explicit QueryBuilder(Connection* conn, const std::string& table);

    // SELECT
    QueryBuilder& select(const std::vector<std::string>& cols = {"*"});
    QueryBuilder& where(const std::string& expr, const std::vector<DbValue>& params = {});
    QueryBuilder& and_where(const std::string& expr, const std::vector<DbValue>& params = {});
    QueryBuilder& or_where(const std::string& expr, const std::vector<DbValue>& params = {});
    QueryBuilder& order_by(const std::string& col, bool asc = true);
    QueryBuilder& limit(int n);
    QueryBuilder& offset(int n);
    QueryBuilder& join(const std::string& table, const std::string& on);
    QueryBuilder& left_join(const std::string& table, const std::string& on);
    QueryBuilder& group_by(const std::string& col);
    QueryBuilder& having(const std::string& expr);

    // INSERT / UPDATE / DELETE
    QueryBuilder& set(const std::string& col, const DbValue& val);
    QueryBuilder& values(const std::map<std::string, DbValue>& data);

    // Execute
    ResultSet     get();          // SELECT
    ResultSet     first();        // SELECT LIMIT 1
    ResultSet     insert();       // INSERT
    ResultSet     update();       // UPDATE
    ResultSet     remove();       // DELETE
    int64_t       count();        // SELECT COUNT(*)
    bool          exists();       // SELECT EXISTS

    std::string   to_sql() const;

private:
    Connection*  conn_;
    std::string  table_;
    std::string  op_;
    std::vector<std::string>              select_cols_;
    std::vector<std::string>              where_clauses_;
    std::vector<DbValue>                  where_params_;
    std::map<std::string, DbValue>        set_vals_;
    std::string  order_col_;
    bool         order_asc_ = true;
    int          limit_     = -1;
    int          offset_    = 0;
    std::string  join_clause_;
    std::string  group_col_;
    std::string  having_clause_;
};

QueryBuilder* qb_table(Connection* conn, const std::string& table);
void          qb_destroy(QueryBuilder* qb);

// ===== ORM =====
struct ModelField {
    std::string name;
    std::string type;       // "int", "string", "float", "bool", "datetime"
    bool primary_key  = false;
    bool auto_increment = false;
    bool nullable     = true;
    bool unique       = false;
    std::string default_val;
};

struct ModelDef {
    std::string name;
    std::string table;
    std::vector<ModelField> fields;
};

class ORM {
public:
    explicit ORM(Connection* conn);

    void      define(const ModelDef& model);
    bool      migrate();          // CREATE TABLE IF NOT EXISTS for all models
    bool      drop(const std::string& model);

    ResultSet find_all(const std::string& model);
    ResultSet find_by_id(const std::string& model, const DbValue& id);
    ResultSet find_where(const std::string& model,
                         const std::string& field, const DbValue& val);
    ResultSet find_raw(const std::string& model, const std::string& where_sql,
                       const std::vector<DbValue>& params = {});

    ResultSet create(const std::string& model, const std::map<std::string, DbValue>& data);
    ResultSet update_by_id(const std::string& model, const DbValue& id,
                           const std::map<std::string, DbValue>& data);
    ResultSet delete_by_id(const std::string& model, const DbValue& id);
    int64_t   count(const std::string& model);

    QueryBuilder* query(const std::string& model);

private:
    Connection* conn_;
    std::map<std::string, ModelDef> models_;
    std::string pk_field(const std::string& model) const;
    std::string build_create_table(const ModelDef& m) const;
};

ORM*  orm_create(Connection* conn);
void  orm_destroy(ORM* orm);

// ===== MongoDB-style Document Driver =====
// (used when driver == "mongodb")
struct Document {
    std::string id;
    std::map<std::string, std::string> fields;
};

class DocumentCollection {
public:
    explicit DocumentCollection(Connection* conn, const std::string& collection);

    std::string insert_one(const std::map<std::string, std::string>& doc);
    std::vector<Document> find(const std::string& field, const std::string& value);
    std::vector<Document> find_all();
    bool update_one(const std::string& id, const std::string& field, const std::string& value);
    bool delete_one(const std::string& id);
    int  count();

private:
    Connection* conn_;
    std::string collection_;
};

DocumentCollection* doc_collection(Connection* conn, const std::string& name);
void                doc_collection_destroy(DocumentCollection* coll);

} // namespace Database
} // namespace Sapphire
