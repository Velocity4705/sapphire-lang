#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace sapphire {
namespace stdlib {
namespace Database {

// Database result types
using DatabaseRow = std::unordered_map<std::string, std::string>;
using DatabaseResult = std::vector<DatabaseRow>;

// Database connection interface
class DatabaseConnection {
public:
    virtual ~DatabaseConnection() = default;
    
    virtual bool connect(const std::string& connection_string) = 0;
    virtual void disconnect() = 0;
    virtual bool is_connected() const = 0;
    
    virtual DatabaseResult execute_query(const std::string& query) = 0;
    virtual bool execute_command(const std::string& command) = 0;
    
    virtual std::string escape_string(const std::string& str) = 0;
    virtual int get_last_insert_id() = 0;
    virtual int get_affected_rows() = 0;
};

// PostgreSQL connection
class PostgreSQLConnection : public DatabaseConnection {
public:
    PostgreSQLConnection();
    ~PostgreSQLConnection() override;
    
    bool connect(const std::string& connection_string) override;
    void disconnect() override;
    bool is_connected() const override;
    
    DatabaseResult execute_query(const std::string& query) override;
    bool execute_command(const std::string& command) override;
    
    std::string escape_string(const std::string& str) override;
    int get_last_insert_id() override;
    int get_affected_rows() override;

private:
    void* connection_; // PGconn*
    bool connected_;
};

// MySQL connection
class MySQLConnection : public DatabaseConnection {
public:
    MySQLConnection();
    ~MySQLConnection() override;
    
    bool connect(const std::string& connection_string) override;
    void disconnect() override;
    bool is_connected() const override;
    
    DatabaseResult execute_query(const std::string& query) override;
    bool execute_command(const std::string& command) override;
    
    std::string escape_string(const std::string& str) override;
    int get_last_insert_id() override;
    int get_affected_rows() override;

private:
    void* connection_; // MYSQL*
    bool connected_;
};

// Connection pool
class ConnectionPool {
public:
    ConnectionPool(const std::string& connection_string, int pool_size = 10);
    ~ConnectionPool();
    
    std::shared_ptr<DatabaseConnection> get_connection();
    void return_connection(std::shared_ptr<DatabaseConnection> conn);
    
    int get_pool_size() const { return pool_size_; }
    int get_active_connections() const { return active_connections_; }

private:
    std::string connection_string_;
    int pool_size_;
    int active_connections_;
    std::vector<std::shared_ptr<DatabaseConnection>> available_connections_;
    std::vector<std::shared_ptr<DatabaseConnection>> active_connections_list_;
};

} // namespace Database
} // namespace stdlib
} // namespace sapphire

// C API for Sapphire integration
extern "C" {
    // PostgreSQL
    void* sapphire_postgresql_create();
    void sapphire_postgresql_destroy(void* conn);
    int sapphire_postgresql_connect(void* conn, const char* connection_string);
    void sapphire_postgresql_disconnect(void* conn);
    int sapphire_postgresql_is_connected(void* conn);
    char* sapphire_postgresql_query(void* conn, const char* query);
    int sapphire_postgresql_execute(void* conn, const char* command);
    
    // MySQL
    void* sapphire_mysql_create();
    void sapphire_mysql_destroy(void* conn);
    int sapphire_mysql_connect(void* conn, const char* connection_string);
    void sapphire_mysql_disconnect(void* conn);
    int sapphire_mysql_is_connected(void* conn);
    char* sapphire_mysql_query(void* conn, const char* query);
    int sapphire_mysql_execute(void* conn, const char* command);
    
    // Utility
    void sapphire_database_free_result(char* result);
}