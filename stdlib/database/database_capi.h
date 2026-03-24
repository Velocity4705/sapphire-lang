#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// ===== Connection =====
void*       db_open_c(const char* conn_string);
void        db_close_c(void* db);
void*       db_exec_c(void* db, const char* sql);
void        db_result_destroy(void* r);
int         db_result_ok(void* r);
const char* db_result_error(void* r);
int         db_result_row_count(void* r);
int         db_result_col_count(void* r);
const char* db_result_col_name(void* r, int c);
const char* db_result_get(void* r, int row, int col);
long long   db_result_rows_affected(void* r);
int         db_begin(void* db);
int         db_commit(void* db);
int         db_rollback(void* db);

// ===== Query Builder =====
void*       db_qb_select(void* db, const char* table);
void*       db_qb_insert(void* db, const char* table);
void*       db_qb_update(void* db, const char* table);
void*       db_qb_delete(void* db, const char* table);
void*       db_qb_create_table(void* db, const char* table);
void        db_qb_destroy(void* qb);
void        db_qb_cols(void* qb, const char* cols_csv);
void        db_qb_where(void* qb, const char* expr);
void        db_qb_order(void* qb, const char* col, int asc);
void        db_qb_limit(void* qb, int n);
void        db_qb_offset(void* qb, int n);
void        db_qb_set(void* qb, const char* col, const char* val);
void        db_qb_value(void* qb, const char* val);
void        db_qb_add_col(void* qb, const char* name, const char* type, int pk, int not_null);
void*       db_qb_exec(void* qb);
const char* db_qb_sql(void* qb);

// ===== KV Store =====
void*       kv_open_c(void);
void        kv_close_c(void* kv);
void        kv_set_c(void* kv, const char* key, const char* val);
void        kv_setex_c(void* kv, const char* key, const char* val, long long ttl_ms);
const char* kv_get_c(void* kv, const char* key);
int         kv_exists_c(void* kv, const char* key);
int         kv_del_c(void* kv, const char* key);
long long   kv_incr_c(void* kv, const char* key);
long long   kv_decr_c(void* kv, const char* key);
void*       kv_keys_c(void* kv, const char* pattern);
void        kv_flush_c(void* kv);
int         kv_count_c(void* kv);
void        kv_lpush_c(void* kv, const char* key, const char* val);
void        kv_rpush_c(void* kv, const char* key, const char* val);
const char* kv_lpop_c(void* kv, const char* key);
const char* kv_rpop_c(void* kv, const char* key);
int         kv_llen_c(void* kv, const char* key);
const char* kv_lindex_c(void* kv, const char* key, int i);
void        kv_hset_c(void* kv, const char* key, const char* field, const char* val);
const char* kv_hget_c(void* kv, const char* key, const char* field);
int         kv_hexists_c(void* kv, const char* key, const char* field);
int         kv_hlen_c(void* kv, const char* key);
void*       kv_hkeys_c(void* kv, const char* key);

// ===== StrVec helpers =====
void        db_strvec_destroy(void* sv);
int         db_strvec_count(void* sv);
const char* db_strvec_get(void* sv, int i);

// ===== Document Store =====
void*       doc_open_c(void);
void        doc_close_c(void* col);
void*       doc_map_create(void);
void        doc_map_destroy(void* m);
void        doc_map_set(void* m, const char* key, const char* val);
const char* doc_map_get(void* m, const char* key);
void*       doc_map_keys(void* m);
const char* doc_insert_c(void* col, const char* collection, void* doc_map);
void*       doc_find_c(void* col, const char* collection, const char* field, const char* value);
void*       doc_find_all_c(void* col, const char* collection);
int         doc_update_c(void* col, const char* collection, const char* id, const char* field, const char* value);
int         doc_delete_c(void* col, const char* collection, const char* id);
int         doc_count_c(void* col, const char* collection);
void        doc_list_destroy(void* list);
int         doc_list_count(void* list);
void*       doc_list_get_map(void* list, int idx);

// ===== ORM =====
void*       orm_create_c(void* db);
void        orm_destroy_c(void* orm);
void*       orm_fields_create(void);
void        orm_fields_destroy(void* fields);
void        orm_fields_add(void* fields, const char* name, const char* type, int pk, int not_null);
void        orm_register_c(void* orm, const char* model_name, void* fields);
int         orm_migrate_c(void* orm);
void*       orm_find_all_c(void* orm, const char* model_name);
void*       orm_find_by_c(void* orm, const char* model_name, const char* field, const char* value);
void*       orm_insert_c(void* orm, const char* model_name, void* doc_map);
void*       orm_update_c(void* orm, const char* model_name, const char* id_field, const char* id, void* doc_map);
void*       orm_delete_c(void* orm, const char* model_name, const char* id_field, const char* id);

#ifdef __cplusplus
}
#endif
