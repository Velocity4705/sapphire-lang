#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <ctime>

namespace Sapphire {
namespace Auth {

// ===== JWT Handler =====

struct JwtClaims {
    std::string subject;       // "sub"
    std::string issuer;        // "iss"
    std::string audience;      // "aud"
    long issued_at;            // "iat"
    long expires_at;           // "exp"
    std::unordered_map<std::string, std::string> custom;
};

class JwtHandler {
public:
    JwtHandler(const std::string& secret, const std::string& algorithm = "HS256");
    ~JwtHandler();

    // Generate a signed JWT token
    std::string generate(const JwtClaims& claims) const;

    // Validate and decode a token; returns false if invalid/expired
    bool validate(const std::string& token) const;
    JwtClaims decode(const std::string& token) const;

    // Refresh: validate old token and issue new one with extended expiry
    std::string refresh(const std::string& token, int extend_seconds = 3600);

    // Blacklist support (in-memory)
    void blacklist(const std::string& token);
    bool is_blacklisted(const std::string& token) const;

    const std::string& algorithm() const { return algorithm_; }

private:
    std::string secret_;
    std::string algorithm_;
    std::vector<std::string> blacklist_;

    std::string base64url_encode(const std::string& input) const;
    std::string base64url_decode(const std::string& input) const;
    std::string hmac_sha256(const std::string& data, const std::string& key) const;
    std::string build_header() const;
    std::string claims_to_json(const JwtClaims& claims) const;
    JwtClaims json_to_claims(const std::string& json) const;
};

// ===== Session Manager =====

struct Session {
    std::string id;
    std::string user_id;
    long created_at;
    long expires_at;
    std::unordered_map<std::string, std::string> data;
    bool active;
};

class SessionManager {
public:
    SessionManager(int ttl_seconds = 3600);
    ~SessionManager();

    // Create a new session for a user
    std::string create(const std::string& user_id);

    // Get session by ID; returns nullptr if not found/expired
    Session* get(const std::string& session_id);

    // Set/get session data
    void set_data(const std::string& session_id, const std::string& key, const std::string& value);
    std::string get_data(const std::string& session_id, const std::string& key);

    // Destroy a session (logout)
    bool destroy(const std::string& session_id);

    // Regenerate session ID (security)
    std::string regenerate(const std::string& old_id);

    // Cleanup expired sessions
    int cleanup();

    // Validate session is active and not expired
    bool is_valid(const std::string& session_id);

    int session_count() const { return static_cast<int>(sessions_.size()); }

private:
    int ttl_seconds_;
    std::unordered_map<std::string, Session> sessions_;

    std::string generate_id() const;
    bool is_expired(const Session& s) const;
};

// ===== Role-Based Access Control =====

struct Permission {
    std::string resource;   // e.g. "posts", "users", "*"
    std::string action;     // e.g. "read", "write", "delete", "*"
};

struct Role {
    std::string name;
    std::string parent;     // for inheritance
    std::vector<Permission> permissions;
};

class RoleManager {
public:
    RoleManager();
    ~RoleManager();

    // Define roles
    void define_role(const std::string& name, const std::string& parent = "");
    void add_permission(const std::string& role, const std::string& resource, const std::string& action);

    // Assign roles to users
    void assign_role(const std::string& user_id, const std::string& role);
    void revoke_role(const std::string& user_id, const std::string& role);

    // Check access
    bool has_permission(const std::string& user_id, const std::string& resource, const std::string& action);
    bool has_role(const std::string& user_id, const std::string& role);

    std::vector<std::string> get_user_roles(const std::string& user_id);
    std::vector<Permission> get_role_permissions(const std::string& role_name);

private:
    std::unordered_map<std::string, Role> roles_;
    std::unordered_map<std::string, std::vector<std::string>> user_roles_;

    bool check_role_permission(const std::string& role_name,
                               const std::string& resource,
                               const std::string& action,
                               int depth = 0);
    bool matches(const std::string& pattern, const std::string& value);
};

// ===== OAuth2 Provider =====

struct OAuth2Config {
    std::string provider;           // "google", "github", "facebook"
    std::string client_id;
    std::string client_secret;
    std::string redirect_uri;
    std::vector<std::string> scopes;
    std::string auth_url;
    std::string token_url;
    std::string userinfo_url;
};

struct OAuth2Token {
    std::string access_token;
    std::string refresh_token;
    std::string token_type;
    int expires_in;
    std::string scope;
};

struct OAuth2UserProfile {
    std::string id;
    std::string email;
    std::string name;
    std::string avatar_url;
    std::string provider;
    std::unordered_map<std::string, std::string> raw;
};

class OAuth2Provider {
public:
    OAuth2Provider();
    ~OAuth2Provider();

    // Register a provider configuration
    void register_provider(const OAuth2Config& config);

    // Build the authorization URL (redirect user here)
    std::string get_auth_url(const std::string& provider, const std::string& state = "") const;

    // Exchange authorization code for tokens
    OAuth2Token exchange_code(const std::string& provider, const std::string& code) const;

    // Get user profile using access token
    OAuth2UserProfile get_user_profile(const std::string& provider, const std::string& access_token) const;

    // Validate state parameter (CSRF protection)
    std::string generate_state() const;
    bool validate_state(const std::string& state) const;

    bool has_provider(const std::string& name) const;

private:
    std::unordered_map<std::string, OAuth2Config> providers_;
    mutable std::vector<std::string> valid_states_;

    std::string url_encode(const std::string& value) const;
    std::string join_scopes(const std::vector<std::string>& scopes) const;
};

} // namespace Auth
} // namespace Sapphire

// ===== C API =====
extern "C" {

// JWT
void* jwt_create(const char* secret, const char* algorithm);
void  jwt_destroy(void* jwt);
const char* jwt_generate(void* jwt, const char* subject, int expires_in_seconds);
int   jwt_validate(void* jwt, const char* token);
const char* jwt_get_subject(void* jwt, const char* token);
const char* jwt_get_claim(void* jwt, const char* token, const char* claim);
const char* jwt_refresh(void* jwt, const char* token, int extend_seconds);
void  jwt_blacklist(void* jwt, const char* token);
int   jwt_is_blacklisted(void* jwt, const char* token);

// Session
void* session_manager_create(int ttl_seconds);
void  session_manager_destroy(void* sm);
const char* session_create(void* sm, const char* user_id);
int   session_is_valid(void* sm, const char* session_id);
void  session_set_data(void* sm, const char* session_id, const char* key, const char* value);
const char* session_get_data(void* sm, const char* session_id, const char* key);
int   session_destroy(void* sm, const char* session_id);
const char* session_regenerate(void* sm, const char* session_id);
int   session_cleanup(void* sm);
const char* session_get_user_id(void* sm, const char* session_id);

// RBAC
void* rbac_create();
void  rbac_destroy(void* rbac);
void  rbac_define_role(void* rbac, const char* name, const char* parent);
void  rbac_add_permission(void* rbac, const char* role, const char* resource, const char* action);
void  rbac_assign_role(void* rbac, const char* user_id, const char* role);
void  rbac_revoke_role(void* rbac, const char* user_id, const char* role);
int   rbac_has_permission(void* rbac, const char* user_id, const char* resource, const char* action);
int   rbac_has_role(void* rbac, const char* user_id, const char* role);

// OAuth2
void* oauth2_create();
void  oauth2_destroy(void* oauth2);
void  oauth2_register_provider(void* oauth2, const char* provider, const char* client_id,
                                const char* client_secret, const char* redirect_uri,
                                const char* scopes, const char* auth_url, const char* token_url);
const char* oauth2_get_auth_url(void* oauth2, const char* provider, const char* state);
const char* oauth2_generate_state(void* oauth2);
int   oauth2_validate_state(void* oauth2, const char* state);
int   oauth2_has_provider(void* oauth2, const char* provider);

} // extern "C"
