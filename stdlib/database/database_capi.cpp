// database_capi.cpp - C API for M19 Database & Storage
#include "database.h"
#include <vector>
#include <string>
#include <map>
#include <sstream>

using namespace Sapphire::Database;

static std::string g_db_str_buf;

struct StrVec  { std::vector<std::string> v; };
struct DocMap  { std::map<std::string, std::string> m; };
struct DocList {
    std::vector<Document> docs;
    ~DocList() {}
};
struct FieldList { std::vector<ModelField> fields; };

extern "C" {

// ===== Connection =====
void* db_open_c(const char* driver, const char* database,
                const char* host, const char* user,
                int port, const char* password) {
    ConnConfig cfg;
    cfg.driver   = driver   ? driver   : "sqlite";
    cfg.database = database ? database : ":memory:";
    cfg.host     = host     ? host     : "localhost";
    cfg.username = user     ? user     : "";
    cfg.password = password ? password : "";
    cfg.port     = port;
    return db_connect(cfg);
}
void db_close_c(void* db) { db_disconnect(static_cast<Connection*>(db)); }

void* db_exec_c(void* db, const char* sql) {
    return new ResultSet(static_cast<Connection*>(db)->exec(sql ? sql : ""));
}
void        db_result_destroy(void* r)      { delete static_cast<ResultSet*>(r); }
int         db_result_ok(void* r)           { return static_cast<ResultSet*>(r)->ok ? 1 : 0; }
const char* db_result_error(void* r)        { g_db_str_buf = static_cast<ResultSet*>(r)->error; return g_db_str_buf.c_str(); }
int         db_result_row_count(void* r)    { return (int)static_cast<ResultSet*>(r)->rows.size(); }
int         db_result_col_count(void* r)    { return (int)static_cast<ResultSet*>(r)->columns.size(); }
const char* db_result_col_name(void* r, int c) {
    auto* rs = static_cast<ResultSet*>(r);
    g_db_str_buf = (c >= 0 && c < (int)rs->columns.size()) ? rs->columns[c] : "";
    return g_db_str_buf.c_str();
}
const char* db_result_get(void* r, int row, int col) {
    auto* rs = static_cast<ResultSet*>(r);
    if (row < 0 || row >= (int)rs->rows.size())    { g_db_str_buf = ""; return g_db_str_buf.c_str(); }
    if (col < 0 || col >= (int)rs->rows[row].size()){ g_db_str_buf = ""; return g_db_str_buf.c_str(); }
    g_db_str_buf = dbval_to_string(rs->rows[row][col]);
    return g_db_str_buf.c_str();
}
long long db_result_rows_affected(void* r) { return static_cast<ResultSet*>(r)->rows_affected; }
int db_begin(void* db)    { return static_cast<Connection*>(db)->begin()    ? 1 : 0; }
int db_commit(void* db)   { return static_cast<Connection*>(db)->commit()   ? 1 : 0; }
int db_rollback(void* db) { return static_cast<Connection*>(db)->rollback() ? 1 : 0; }

// ===== Query Builder =====
void* db_qb_select(void* db, const char* table) {
    auto* qb = qb_table(static_cast<Connection*>(db), table ? table : "");
    qb->select();
    return qb;
}
void* db_qb_insert(void* db, const char* table) {
    auto* qb = qb_table(static_cast<Connection*>(db), table ? table : "");
    return qb;
}
void* db_qb_update(void* db, const char* table) {
    return qb_table(static_cast<Connection*>(db), table ? table : "");
}
void* db_qb_delete(void* db, const char* table) {
    return qb_table(static_cast<Connection*>(db), table ? table : "");
}
void* db_qb_create_table(void* db, const char* table) {
    return qb_table(static_cast<Connection*>(db), table ? table : "");
}
void db_qb_destroy(void* qb) { qb_destroy(static_cast<QueryBuilder*>(qb)); }

void db_qb_cols(void* qb, const char* cols_csv) {
    std::vector<std::string> cols;
    std::istringstream ss(cols_csv ? cols_csv : "");
    std::string c;
    while (std::getline(ss, c, ',')) {
        while (!c.empty() && c[0] == ' ') c = c.substr(1);
        if (!c.empty()) cols.push_back(c);
    }
    static_cast<QueryBuilder*>(qb)->select(cols);
}
void db_qb_where(void* qb, const char* expr) {
    static_cast<QueryBuilder*>(qb)->where(expr ? expr : "");
}
void db_qb_order(void* qb, const char* col, int asc) {
    static_cast<QueryBuilder*>(qb)->order_by(col ? col : "", asc != 0);
}
void db_qb_limit(void* qb, int n)  { static_cast<QueryBuilder*>(qb)->limit(n); }
void db_qb_offset(void* qb, int n) { static_cast<QueryBuilder*>(qb)->offset(n); }
void db_qb_set(void* qb, const char* col, const char* val) {
    static_cast<QueryBuilder*>(qb)->set(col ? col : "", val ? std::string(val) : std::string(""));
}
void db_qb_value(void* qb, const char* val) {
    // single value insert — use set with positional key
    (void)qb; (void)val;
}
void db_qb_add_col(void* qb, const char* name, const char* type, int pk, int not_null) {
    (void)qb; (void)name; (void)type; (void)pk; (void)not_null;
}
void* db_qb_exec(void* qb) {
    return new ResultSet(static_cast<QueryBuilder*>(qb)->get());
}
const char* db_qb_sql(void* qb) {
    g_db_str_buf = static_cast<QueryBuilder*>(qb)->to_sql();
    return g_db_str_buf.c_str();
}

// ===== KV Store =====
void* kv_open_c() {
    // KV store backed by an in-memory sqlite connection
    ConnConfig cfg; cfg.driver = "sqlite"; cfg.database = ":memory:";
    return db_connect(cfg);
}
void kv_close_c(void* kv) { db_disconnect(static_cast<Connection*>(kv)); }

// Simple KV backed by a table in the connection
static void kv_ensure_table(Connection* c) {
    c->exec("CREATE TABLE IF NOT EXISTS __kv (k TEXT PRIMARY KEY, v TEXT, ttl INTEGER DEFAULT 0)");
    c->exec("CREATE TABLE IF NOT EXISTS __kv_list (k TEXT, v TEXT, pos INTEGER)");
    c->exec("CREATE TABLE IF NOT EXISTS __kv_hash (k TEXT, f TEXT, v TEXT, PRIMARY KEY(k,f))");
}

void kv_set_c(void* kv, const char* key, const char* val) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    std::string sql = "INSERT OR REPLACE INTO __kv(k,v) VALUES('" + std::string(key?key:"") + "','" + std::string(val?val:"") + "')";
    c->exec(sql);
}
void kv_setex_c(void* kv, const char* key, const char* val, long long ttl_ms) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    std::string sql = "INSERT OR REPLACE INTO __kv(k,v,ttl) VALUES('" + std::string(key?key:"") + "','" + std::string(val?val:"") + "'," + std::to_string(ttl_ms) + ")";
    c->exec(sql);
}
const char* kv_get_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT v FROM __kv WHERE k='" + std::string(key?key:"") + "'");
    g_db_str_buf = (!r.rows.empty() && !r.rows[0].empty()) ? dbval_to_string(r.rows[0][0]) : "";
    return g_db_str_buf.c_str();
}
int kv_exists_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT COUNT(*) FROM __kv WHERE k='" + std::string(key?key:"") + "'");
    return (!r.rows.empty() && !r.rows[0].empty() && dbval_to_string(r.rows[0][0]) != "0") ? 1 : 0;
}
int kv_del_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    c->exec("DELETE FROM __kv WHERE k='" + std::string(key?key:"") + "'");
    return 1;
}
long long kv_incr_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT v FROM __kv WHERE k='" + std::string(key?key:"") + "'");
    long long cur = 0;
    if (!r.rows.empty() && !r.rows[0].empty()) {
        try { cur = std::stoll(dbval_to_string(r.rows[0][0])); } catch(...) {}
    }
    cur++;
    c->exec("INSERT OR REPLACE INTO __kv(k,v) VALUES('" + std::string(key?key:"") + "','" + std::to_string(cur) + "')");
    return cur;
}
long long kv_decr_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT v FROM __kv WHERE k='" + std::string(key?key:"") + "'");
    long long cur = 0;
    if (!r.rows.empty() && !r.rows[0].empty()) {
        try { cur = std::stoll(dbval_to_string(r.rows[0][0])); } catch(...) {}
    }
    cur--;
    c->exec("INSERT OR REPLACE INTO __kv(k,v) VALUES('" + std::string(key?key:"") + "','" + std::to_string(cur) + "')");
    return cur;
}
void* kv_keys_c(void* kv, const char* /*pattern*/) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT k FROM __kv");
    auto* v = new StrVec();
    for (auto& row : r.rows) if (!row.empty()) v->v.push_back(dbval_to_string(row[0]));
    return v;
}
void kv_flush_c(void* kv) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    c->exec("DELETE FROM __kv"); c->exec("DELETE FROM __kv_list"); c->exec("DELETE FROM __kv_hash");
}
int kv_count_c(void* kv) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT COUNT(*) FROM __kv");
    if (!r.rows.empty() && !r.rows[0].empty()) {
        try { return (int)std::stoll(dbval_to_string(r.rows[0][0])); } catch(...) {}
    }
    return 0;
}
void kv_lpush_c(void* kv, const char* key, const char* val) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    c->exec("INSERT INTO __kv_list(k,v,pos) VALUES('" + std::string(key?key:"") + "','" + std::string(val?val:"") + "',(SELECT COALESCE(MAX(pos),0)+1 FROM __kv_list WHERE k='" + std::string(key?key:"") + "'))");
}
void kv_rpush_c(void* kv, const char* key, const char* val) {
    kv_lpush_c(kv, key, val);
}
const char* kv_lpop_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT rowid,v FROM __kv_list WHERE k='" + std::string(key?key:"") + "' ORDER BY pos DESC LIMIT 1");
    if (r.rows.empty()) { g_db_str_buf = ""; return g_db_str_buf.c_str(); }
    g_db_str_buf = dbval_to_string(r.rows[0][1]);
    c->exec("DELETE FROM __kv_list WHERE rowid=" + dbval_to_string(r.rows[0][0]));
    return g_db_str_buf.c_str();
}
const char* kv_rpop_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT rowid,v FROM __kv_list WHERE k='" + std::string(key?key:"") + "' ORDER BY pos ASC LIMIT 1");
    if (r.rows.empty()) { g_db_str_buf = ""; return g_db_str_buf.c_str(); }
    g_db_str_buf = dbval_to_string(r.rows[0][1]);
    c->exec("DELETE FROM __kv_list WHERE rowid=" + dbval_to_string(r.rows[0][0]));
    return g_db_str_buf.c_str();
}
int kv_llen_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT COUNT(*) FROM __kv_list WHERE k='" + std::string(key?key:"") + "'");
    if (!r.rows.empty() && !r.rows[0].empty()) {
        try { return (int)std::stoll(dbval_to_string(r.rows[0][0])); } catch(...) {}
    }
    return 0;
}
const char* kv_lindex_c(void* kv, const char* key, int i) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT v FROM __kv_list WHERE k='" + std::string(key?key:"") + "' ORDER BY pos LIMIT 1 OFFSET " + std::to_string(i));
    g_db_str_buf = (!r.rows.empty() && !r.rows[0].empty()) ? dbval_to_string(r.rows[0][0]) : "";
    return g_db_str_buf.c_str();
}
void kv_hset_c(void* kv, const char* key, const char* field, const char* val) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    c->exec("INSERT OR REPLACE INTO __kv_hash(k,f,v) VALUES('" + std::string(key?key:"") + "','" + std::string(field?field:"") + "','" + std::string(val?val:"") + "')");
}
const char* kv_hget_c(void* kv, const char* key, const char* field) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT v FROM __kv_hash WHERE k='" + std::string(key?key:"") + "' AND f='" + std::string(field?field:"") + "'");
    g_db_str_buf = (!r.rows.empty() && !r.rows[0].empty()) ? dbval_to_string(r.rows[0][0]) : "";
    return g_db_str_buf.c_str();
}
int kv_hexists_c(void* kv, const char* key, const char* field) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT COUNT(*) FROM __kv_hash WHERE k='" + std::string(key?key:"") + "' AND f='" + std::string(field?field:"") + "'");
    return (!r.rows.empty() && !r.rows[0].empty() && dbval_to_string(r.rows[0][0]) != "0") ? 1 : 0;
}
int kv_hlen_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT COUNT(*) FROM __kv_hash WHERE k='" + std::string(key?key:"") + "'");
    if (!r.rows.empty() && !r.rows[0].empty()) {
        try { return (int)std::stoll(dbval_to_string(r.rows[0][0])); } catch(...) {}
    }
    return 0;
}
void* kv_hkeys_c(void* kv, const char* key) {
    auto* c = static_cast<Connection*>(kv); kv_ensure_table(c);
    auto r = c->query("SELECT f FROM __kv_hash WHERE k='" + std::string(key?key:"") + "'");
    auto* v = new StrVec();
    for (auto& row : r.rows) if (!row.empty()) v->v.push_back(dbval_to_string(row[0]));
    return v;
}

// StrVec helpers
void        db_strvec_destroy(void* v)    { delete static_cast<StrVec*>(v); }
int         db_strvec_count(void* v)      { return (int)static_cast<StrVec*>(v)->v.size(); }
const char* db_strvec_get(void* v, int i) {
    auto* sv = static_cast<StrVec*>(v);
    g_db_str_buf = (i >= 0 && i < (int)sv->v.size()) ? sv->v[i] : "";
    return g_db_str_buf.c_str();
}

// ===== Document Store (backed by sqlite) =====
void* doc_open_c() {
    ConnConfig cfg; cfg.driver = "sqlite"; cfg.database = ":memory:";
    auto* c = db_connect(cfg);
    c->exec("CREATE TABLE IF NOT EXISTS __doc (coll TEXT, id TEXT, field TEXT, value TEXT)");
    return c;
}
void doc_close_c(void* ds) { db_disconnect(static_cast<Connection*>(ds)); }

void* doc_map_create()                    { return new DocMap(); }
void  doc_map_destroy(void* m)            { delete static_cast<DocMap*>(m); }
void  doc_map_set(void* m, const char* k, const char* v) { static_cast<DocMap*>(m)->m[k?k:""] = v?v:""; }
const char* doc_map_get(void* m, const char* k) {
    auto& mm = static_cast<DocMap*>(m)->m;
    auto it = mm.find(k?k:"");
    g_db_str_buf = (it != mm.end()) ? it->second : "";
    return g_db_str_buf.c_str();
}
void* doc_map_keys(void* m) {
    auto* v = new StrVec();
    for (const auto& [k, _] : static_cast<DocMap*>(m)->m) v->v.push_back(k);
    return v;
}

static std::string gen_id() {
    static int counter = 0;
    return "doc_" + std::to_string(++counter);
}

const char* doc_insert_c(void* ds, const char* coll, void* doc_map) {
    auto* c = static_cast<Connection*>(ds);
    std::string id = gen_id();
    for (const auto& [k, v] : static_cast<DocMap*>(doc_map)->m) {
        c->exec("INSERT INTO __doc(coll,id,field,value) VALUES('" +
                std::string(coll?coll:"") + "','" + id + "','" + k + "','" + v + "')");
    }
    g_db_str_buf = id;
    return g_db_str_buf.c_str();
}

static DocList* load_docs(Connection* c, const std::string& coll, const std::string& where_extra = "") {
    std::string sql = "SELECT DISTINCT id FROM __doc WHERE coll='" + coll + "'" + where_extra;
    auto ids = c->query(sql);
    auto* dl = new DocList();
    for (auto& row : ids.rows) {
        if (row.empty()) continue;
        std::string id = dbval_to_string(row[0]);
        Document doc; doc.id = id;
        auto fields = c->query("SELECT field,value FROM __doc WHERE coll='" + coll + "' AND id='" + id + "'");
        for (auto& fr : fields.rows) {
            if (fr.size() >= 2) doc.fields[dbval_to_string(fr[0])] = dbval_to_string(fr[1]);
        }
        dl->docs.push_back(doc);
    }
    return dl;
}

void* doc_find_c(void* ds, const char* coll, const char* field, const char* value) {
    auto* c = static_cast<Connection*>(ds);
    std::string extra = " AND id IN (SELECT id FROM __doc WHERE coll='" +
                        std::string(coll?coll:"") + "' AND field='" +
                        std::string(field?field:"") + "' AND value='" +
                        std::string(value?value:"") + "')";
    return load_docs(c, coll?coll:"", extra);
}
void* doc_find_all_c(void* ds, const char* coll) {
    return load_docs(static_cast<Connection*>(ds), coll?coll:"");
}
int doc_update_c(void* ds, const char* coll, const char* id, const char* field, const char* value) {
    auto* c = static_cast<Connection*>(ds);
    c->exec("UPDATE __doc SET value='" + std::string(value?value:"") +
            "' WHERE coll='" + std::string(coll?coll:"") +
            "' AND id='" + std::string(id?id:"") +
            "' AND field='" + std::string(field?field:"") + "'");
    return 1;
}
int doc_delete_c(void* ds, const char* coll, const char* id) {
    auto* c = static_cast<Connection*>(ds);
    c->exec("DELETE FROM __doc WHERE coll='" + std::string(coll?coll:"") +
            "' AND id='" + std::string(id?id:"") + "'");
    return 1;
}
int doc_count_c(void* ds, const char* coll) {
    auto* c = static_cast<Connection*>(ds);
    auto r = c->query("SELECT COUNT(DISTINCT id) FROM __doc WHERE coll='" + std::string(coll?coll:"") + "'");
    if (!r.rows.empty() && !r.rows[0].empty()) {
        try { return (int)std::stoll(dbval_to_string(r.rows[0][0])); } catch(...) {}
    }
    return 0;
}

void  doc_list_destroy(void* l)    { delete static_cast<DocList*>(l); }
int   doc_list_count(void* l)      { return (int)static_cast<DocList*>(l)->docs.size(); }
void* doc_list_get_map(void* l, int i) {
    auto* dl = static_cast<DocList*>(l);
    if (i < 0 || i >= (int)dl->docs.size()) return new DocMap();
    auto* m = new DocMap();
    m->m = dl->docs[i].fields;
    m->m["__id"] = dl->docs[i].id;
    return m;
}

// ===== ORM =====
void* orm_create_c(void* db)  { return orm_create(static_cast<Connection*>(db)); }
void  orm_destroy_c(void* orm){ orm_destroy(static_cast<ORM*>(orm)); }

void* orm_fields_create()  { return new FieldList(); }
void  orm_fields_destroy(void* fl) { delete static_cast<FieldList*>(fl); }
void  orm_fields_add(void* fl, const char* name, const char* type, int pk, int nullable) {
    ModelField f;
    f.name = name ? name : "";
    f.type = type ? type : "TEXT";
    f.primary_key = pk != 0;
    f.nullable = nullable != 0;
    f.auto_increment = pk != 0;
    f.unique = false;
    static_cast<FieldList*>(fl)->fields.push_back(f);
}
void orm_register_c(void* orm, const char* model_name, void* fields_list) {
    ModelDef md;
    md.name  = model_name ? model_name : "";
    md.table = md.name;
    md.fields = static_cast<FieldList*>(fields_list)->fields;
    static_cast<ORM*>(orm)->define(md);
}
int   orm_migrate_c(void* orm) { return static_cast<ORM*>(orm)->migrate() ? 1 : 0; }
void* orm_find_all_c(void* orm, const char* model) {
    return new ResultSet(static_cast<ORM*>(orm)->find_all(model?model:""));
}
void* orm_find_by_c(void* orm, const char* model, const char* field, const char* val) {
    DbValue v = val ? std::string(val) : std::string("");
    return new ResultSet(static_cast<ORM*>(orm)->find_where(model?model:"", field?field:"", v));
}
void* orm_insert_c(void* orm, const char* model, void* vals_map) {
    std::map<std::string, DbValue> vals;
    for (const auto& [k, v] : static_cast<DocMap*>(vals_map)->m) vals[k] = std::string(v);
    return new ResultSet(static_cast<ORM*>(orm)->create(model?model:"", vals));
}
void* orm_update_c(void* orm, const char* model, const char* /*id_field*/, const char* id, void* vals_map) {
    std::map<std::string, DbValue> vals;
    for (const auto& [k, v] : static_cast<DocMap*>(vals_map)->m) vals[k] = std::string(v);
    DbValue id_val = id ? std::string(id) : std::string("");
    return new ResultSet(static_cast<ORM*>(orm)->update_by_id(model?model:"", id_val, vals));
}
void* orm_delete_c(void* orm, const char* model, const char* /*id_field*/, const char* id) {
    DbValue id_val = id ? std::string(id) : std::string("");
    return new ResultSet(static_cast<ORM*>(orm)->delete_by_id(model?model:"", id_val));
}

} // extern "C"
