#include "auth.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

namespace Sapphire {
namespace Auth {

// ===== Base64URL helpers =====

static const std::string B64_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string JwtHandler::base64url_encode(const std::string& input) const {
    std::string result;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(B64_CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) result.push_back(B64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    // URL-safe: replace + with -, / with _, strip =
    for (char& c : result) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }
    return result;
}

std::string JwtHandler::base64url_decode(const std::string& input) const {
    std::string s = input;
    for (char& c : s) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }
    while (s.size() % 4) s += '=';

    std::string result;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[(unsigned char)B64_CHARS[i]] = i;
    int val = 0, valb = -8;
    for (unsigned char c : s) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            result.push_back((char)((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}

std::string JwtHandler::hmac_sha256(const std::string& data, const std::string& key) const {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    HMAC(EVP_sha256(),
         key.c_str(), static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char*>(data.c_str()), data.size(),
         digest, &digest_len);
    return std::string(reinterpret_cast<char*>(digest), digest_len);
}

// ===== JwtHandler =====

JwtHandler::JwtHandler(const std::string& secret, const std::string& algorithm)
    : secret_(secret), algorithm_(algorithm) {}
JwtHandler::~JwtHandler() {}

std::string JwtHandler::build_header() const {
    return "{\"alg\":\"" + algorithm_ + "\",\"typ\":\"JWT\"}";
}

std::string JwtHandler::claims_to_json(const JwtClaims& c) const {
    std::ostringstream ss;
    ss << "{";
    ss << "\"sub\":\"" << c.subject << "\"";
    if (!c.issuer.empty())   ss << ",\"iss\":\"" << c.issuer << "\"";
    if (!c.audience.empty()) ss << ",\"aud\":\"" << c.audience << "\"";
    ss << ",\"iat\":" << c.issued_at;
    ss << ",\"exp\":" << c.expires_at;
    for (auto& kv : c.custom)
        ss << ",\"" << kv.first << "\":\"" << kv.second << "\"";
    ss << "}";
    return ss.str();
}

JwtClaims JwtHandler::json_to_claims(const std::string& json) const {
    JwtClaims c;
    c.issued_at = 0;
    c.expires_at = 0;

    auto extract = [&](const std::string& key) -> std::string {
        std::string search = "\"" + key + "\":";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return "";
        pos += search.size();
        if (json[pos] == '"') {
            pos++;
            size_t end = json.find('"', pos);
            return end != std::string::npos ? json.substr(pos, end - pos) : "";
        } else {
            size_t end = json.find_first_of(",}", pos);
            return end != std::string::npos ? json.substr(pos, end - pos) : "";
        }
    };

    c.subject  = extract("sub");
    c.issuer   = extract("iss");
    c.audience = extract("aud");
    std::string iat = extract("iat");
    std::string exp = extract("exp");
    if (!iat.empty()) try { c.issued_at  = std::stol(iat); } catch (...) {}
    if (!exp.empty()) try { c.expires_at = std::stol(exp); } catch (...) {}
    return c;
}

std::string JwtHandler::generate(const JwtClaims& claims) const {
    std::string header  = base64url_encode(build_header());
    std::string payload = base64url_encode(claims_to_json(claims));
    std::string signing_input = header + "." + payload;
    std::string sig = base64url_encode(hmac_sha256(signing_input, secret_));
    return signing_input + "." + sig;
}

bool JwtHandler::validate(const std::string& token) const {
    if (is_blacklisted(token)) return false;

    size_t dot1 = token.find('.');
    size_t dot2 = token.find('.', dot1 + 1);
    if (dot1 == std::string::npos || dot2 == std::string::npos) return false;

    std::string signing_input = token.substr(0, dot2);
    std::string provided_sig  = token.substr(dot2 + 1);
    std::string expected_sig  = base64url_encode(hmac_sha256(signing_input, secret_));
    if (provided_sig != expected_sig) return false;

    // Check expiry
    std::string payload_json = base64url_decode(token.substr(dot1 + 1, dot2 - dot1 - 1));
    JwtClaims claims = json_to_claims(payload_json);
    if (claims.expires_at > 0 && std::time(nullptr) > claims.expires_at) return false;

    return true;
}

JwtClaims JwtHandler::decode(const std::string& token) const {
    size_t dot1 = token.find('.');
    size_t dot2 = token.find('.', dot1 + 1);
    if (dot1 == std::string::npos || dot2 == std::string::npos) return JwtClaims{};
    std::string payload_json = base64url_decode(token.substr(dot1 + 1, dot2 - dot1 - 1));
    return json_to_claims(payload_json);
}

std::string JwtHandler::refresh(const std::string& token, int extend_seconds) {
    if (!validate(token)) return "";
    JwtClaims claims = decode(token);
    claims.issued_at  = std::time(nullptr);
    claims.expires_at = claims.issued_at + extend_seconds;
    blacklist(token);
    return generate(claims);
}

void JwtHandler::blacklist(const std::string& token) {
    blacklist_.push_back(token);
}

bool JwtHandler::is_blacklisted(const std::string& token) const {
    return std::find(blacklist_.begin(), blacklist_.end(), token) != blacklist_.end();
}

// ===== SessionManager =====

SessionManager::SessionManager(int ttl_seconds) : ttl_seconds_(ttl_seconds) {}
SessionManager::~SessionManager() {}

std::string SessionManager::generate_id() const {
    unsigned char buf[16];
    RAND_bytes(buf, sizeof(buf));
    std::ostringstream ss;
    for (int i = 0; i < 16; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)buf[i];
    return ss.str();
}

bool SessionManager::is_expired(const Session& s) const {
    return std::time(nullptr) > s.expires_at;
}

std::string SessionManager::create(const std::string& user_id) {
    Session s;
    s.id         = generate_id();
    s.user_id    = user_id;
    s.created_at = std::time(nullptr);
    s.expires_at = s.created_at + ttl_seconds_;
    s.active     = true;
    sessions_[s.id] = s;
    return s.id;
}

Session* SessionManager::get(const std::string& session_id) {
    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) return nullptr;
    if (is_expired(it->second) || !it->second.active) return nullptr;
    return &it->second;
}

void SessionManager::set_data(const std::string& session_id, const std::string& key, const std::string& value) {
    auto* s = get(session_id);
    if (s) s->data[key] = value;
}

std::string SessionManager::get_data(const std::string& session_id, const std::string& key) {
    auto* s = get(session_id);
    if (!s) return "";
    auto it = s->data.find(key);
    return it != s->data.end() ? it->second : "";
}

bool SessionManager::destroy(const std::string& session_id) {
    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) return false;
    it->second.active = false;
    sessions_.erase(it);
    return true;
}

std::string SessionManager::regenerate(const std::string& old_id) {
    auto it = sessions_.find(old_id);
    if (it == sessions_.end()) return "";
    Session s = it->second;
    sessions_.erase(it);
    s.id = generate_id();
    sessions_[s.id] = s;
    return s.id;
}

int SessionManager::cleanup() {
    int count = 0;
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        if (is_expired(it->second) || !it->second.active) {
            it = sessions_.erase(it);
            count++;
        } else {
            ++it;
        }
    }
    return count;
}

bool SessionManager::is_valid(const std::string& session_id) {
    return get(session_id) != nullptr;
}

// ===== RoleManager =====

RoleManager::RoleManager() {}
RoleManager::~RoleManager() {}

void RoleManager::define_role(const std::string& name, const std::string& parent) {
    Role r;
    r.name   = name;
    r.parent = parent;
    roles_[name] = r;
}

void RoleManager::add_permission(const std::string& role, const std::string& resource, const std::string& action) {
    Permission p;
    p.resource = resource;
    p.action   = action;
    roles_[role].permissions.push_back(p);
}

void RoleManager::assign_role(const std::string& user_id, const std::string& role) {
    auto& roles = user_roles_[user_id];
    if (std::find(roles.begin(), roles.end(), role) == roles.end())
        roles.push_back(role);
}

void RoleManager::revoke_role(const std::string& user_id, const std::string& role) {
    auto& roles = user_roles_[user_id];
    roles.erase(std::remove(roles.begin(), roles.end(), role), roles.end());
}

bool RoleManager::matches(const std::string& pattern, const std::string& value) {
    return pattern == "*" || pattern == value;
}

bool RoleManager::check_role_permission(const std::string& role_name,
                                         const std::string& resource,
                                         const std::string& action,
                                         int depth) {
    if (depth > 10) return false; // prevent infinite inheritance loops
    auto it = roles_.find(role_name);
    if (it == roles_.end()) return false;

    for (auto& p : it->second.permissions)
        if (matches(p.resource, resource) && matches(p.action, action))
            return true;

    // Check parent role
    if (!it->second.parent.empty())
        return check_role_permission(it->second.parent, resource, action, depth + 1);

    return false;
}

bool RoleManager::has_permission(const std::string& user_id, const std::string& resource, const std::string& action) {
    auto it = user_roles_.find(user_id);
    if (it == user_roles_.end()) return false;
    for (auto& role : it->second)
        if (check_role_permission(role, resource, action))
            return true;
    return false;
}

bool RoleManager::has_role(const std::string& user_id, const std::string& role) {
    auto it = user_roles_.find(user_id);
    if (it == user_roles_.end()) return false;
    return std::find(it->second.begin(), it->second.end(), role) != it->second.end();
}

std::vector<std::string> RoleManager::get_user_roles(const std::string& user_id) {
    auto it = user_roles_.find(user_id);
    return it != user_roles_.end() ? it->second : std::vector<std::string>{};
}

std::vector<Permission> RoleManager::get_role_permissions(const std::string& role_name) {
    auto it = roles_.find(role_name);
    return it != roles_.end() ? it->second.permissions : std::vector<Permission>{};
}

// ===== OAuth2Provider =====

OAuth2Provider::OAuth2Provider() {}
OAuth2Provider::~OAuth2Provider() {}

void OAuth2Provider::register_provider(const OAuth2Config& config) {
    providers_[config.provider] = config;
}

std::string OAuth2Provider::url_encode(const std::string& value) const {
    std::ostringstream escaped;
    for (unsigned char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            escaped << c;
        else
            escaped << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return escaped.str();
}

std::string OAuth2Provider::join_scopes(const std::vector<std::string>& scopes) const {
    std::string result;
    for (size_t i = 0; i < scopes.size(); i++) {
        if (i > 0) result += "%20";
        result += scopes[i];
    }
    return result;
}

std::string OAuth2Provider::get_auth_url(const std::string& provider, const std::string& state) const {
    auto it = providers_.find(provider);
    if (it == providers_.end()) return "";
    const auto& cfg = it->second;
    std::string url = cfg.auth_url;
    url += "?client_id=" + url_encode(cfg.client_id);
    url += "&redirect_uri=" + url_encode(cfg.redirect_uri);
    url += "&response_type=code";
    url += "&scope=" + join_scopes(cfg.scopes);
    if (!state.empty()) url += "&state=" + url_encode(state);
    return url;
}

OAuth2Token OAuth2Provider::exchange_code(const std::string&, const std::string&) const {
    // Stub - real impl would make HTTP POST to token_url
    OAuth2Token t;
    t.access_token  = "stub_access_token";
    t.refresh_token = "stub_refresh_token";
    t.token_type    = "Bearer";
    t.expires_in    = 3600;
    return t;
}

OAuth2UserProfile OAuth2Provider::get_user_profile(const std::string& provider, const std::string&) const {
    OAuth2UserProfile p;
    p.provider = provider;
    p.id       = "stub_user_id";
    p.email    = "user@example.com";
    p.name     = "OAuth2 User";
    return p;
}

std::string OAuth2Provider::generate_state() const {
    unsigned char buf[16];
    RAND_bytes(buf, sizeof(buf));
    std::ostringstream ss;
    for (int i = 0; i < 16; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)buf[i];
    std::string state = ss.str();
    valid_states_.push_back(state);
    return state;
}

bool OAuth2Provider::validate_state(const std::string& state) const {
    auto it = std::find(valid_states_.begin(), valid_states_.end(), state);
    if (it == valid_states_.end()) return false;
    valid_states_.erase(it); // one-time use
    return true;
}

bool OAuth2Provider::has_provider(const std::string& name) const {
    return providers_.find(name) != providers_.end();
}

} // namespace Auth
} // namespace Sapphire

// ===== C API =====

extern "C" {

static std::string g_last_string; // thread-unsafe but fine for single-threaded Sapphire

void* jwt_create(const char* secret, const char* algorithm) {
    return new Sapphire::Auth::JwtHandler(secret, algorithm ? algorithm : "HS256");
}
void jwt_destroy(void* jwt) {
    delete static_cast<Sapphire::Auth::JwtHandler*>(jwt);
}
const char* jwt_generate(void* jwt, const char* subject, int expires_in_seconds) {
    auto* h = static_cast<Sapphire::Auth::JwtHandler*>(jwt);
    Sapphire::Auth::JwtClaims c;
    c.subject    = subject;
    c.issued_at  = std::time(nullptr);
    c.expires_at = c.issued_at + expires_in_seconds;
    g_last_string = h->generate(c);
    return g_last_string.c_str();
}
int jwt_validate(void* jwt, const char* token) {
    return static_cast<Sapphire::Auth::JwtHandler*>(jwt)->validate(token) ? 1 : 0;
}
const char* jwt_get_subject(void* jwt, const char* token) {
    g_last_string = static_cast<Sapphire::Auth::JwtHandler*>(jwt)->decode(token).subject;
    return g_last_string.c_str();
}
const char* jwt_get_claim(void* jwt, const char* token, const char* claim) {
    auto claims = static_cast<Sapphire::Auth::JwtHandler*>(jwt)->decode(token);
    std::string key(claim);
    if (key == "sub") g_last_string = claims.subject;
    else if (key == "iss") g_last_string = claims.issuer;
    else if (key == "aud") g_last_string = claims.audience;
    else {
        auto it = claims.custom.find(key);
        g_last_string = it != claims.custom.end() ? it->second : "";
    }
    return g_last_string.c_str();
}
const char* jwt_refresh(void* jwt, const char* token, int extend_seconds) {
    g_last_string = static_cast<Sapphire::Auth::JwtHandler*>(jwt)->refresh(token, extend_seconds);
    return g_last_string.c_str();
}
void jwt_blacklist(void* jwt, const char* token) {
    static_cast<Sapphire::Auth::JwtHandler*>(jwt)->blacklist(token);
}
int jwt_is_blacklisted(void* jwt, const char* token) {
    return static_cast<Sapphire::Auth::JwtHandler*>(jwt)->is_blacklisted(token) ? 1 : 0;
}

// Session
void* session_manager_create(int ttl_seconds) {
    return new Sapphire::Auth::SessionManager(ttl_seconds);
}
void session_manager_destroy(void* sm) {
    delete static_cast<Sapphire::Auth::SessionManager*>(sm);
}
const char* session_create(void* sm, const char* user_id) {
    g_last_string = static_cast<Sapphire::Auth::SessionManager*>(sm)->create(user_id);
    return g_last_string.c_str();
}
int session_is_valid(void* sm, const char* session_id) {
    return static_cast<Sapphire::Auth::SessionManager*>(sm)->is_valid(session_id) ? 1 : 0;
}
void session_set_data(void* sm, const char* session_id, const char* key, const char* value) {
    static_cast<Sapphire::Auth::SessionManager*>(sm)->set_data(session_id, key, value);
}
const char* session_get_data(void* sm, const char* session_id, const char* key) {
    g_last_string = static_cast<Sapphire::Auth::SessionManager*>(sm)->get_data(session_id, key);
    return g_last_string.c_str();
}
int session_destroy(void* sm, const char* session_id) {
    return static_cast<Sapphire::Auth::SessionManager*>(sm)->destroy(session_id) ? 1 : 0;
}
const char* session_regenerate(void* sm, const char* session_id) {
    g_last_string = static_cast<Sapphire::Auth::SessionManager*>(sm)->regenerate(session_id);
    return g_last_string.c_str();
}
int session_cleanup(void* sm) {
    return static_cast<Sapphire::Auth::SessionManager*>(sm)->cleanup();
}
const char* session_get_user_id(void* sm, const char* session_id) {
    auto* s = static_cast<Sapphire::Auth::SessionManager*>(sm)->get(session_id);
    g_last_string = s ? s->user_id : "";
    return g_last_string.c_str();
}

// RBAC
void* rbac_create() {
    return new Sapphire::Auth::RoleManager();
}
void rbac_destroy(void* rbac) {
    delete static_cast<Sapphire::Auth::RoleManager*>(rbac);
}
void rbac_define_role(void* rbac, const char* name, const char* parent) {
    static_cast<Sapphire::Auth::RoleManager*>(rbac)->define_role(name, parent ? parent : "");
}
void rbac_add_permission(void* rbac, const char* role, const char* resource, const char* action) {
    static_cast<Sapphire::Auth::RoleManager*>(rbac)->add_permission(role, resource, action);
}
void rbac_assign_role(void* rbac, const char* user_id, const char* role) {
    static_cast<Sapphire::Auth::RoleManager*>(rbac)->assign_role(user_id, role);
}
void rbac_revoke_role(void* rbac, const char* user_id, const char* role) {
    static_cast<Sapphire::Auth::RoleManager*>(rbac)->revoke_role(user_id, role);
}
int rbac_has_permission(void* rbac, const char* user_id, const char* resource, const char* action) {
    return static_cast<Sapphire::Auth::RoleManager*>(rbac)->has_permission(user_id, resource, action) ? 1 : 0;
}
int rbac_has_role(void* rbac, const char* user_id, const char* role) {
    return static_cast<Sapphire::Auth::RoleManager*>(rbac)->has_role(user_id, role) ? 1 : 0;
}

// OAuth2
void* oauth2_create() {
    return new Sapphire::Auth::OAuth2Provider();
}
void oauth2_destroy(void* oauth2) {
    delete static_cast<Sapphire::Auth::OAuth2Provider*>(oauth2);
}
void oauth2_register_provider(void* oauth2, const char* provider, const char* client_id,
                               const char* client_secret, const char* redirect_uri,
                               const char* scopes, const char* auth_url, const char* token_url) {
    Sapphire::Auth::OAuth2Config cfg;
    cfg.provider      = provider;
    cfg.client_id     = client_id;
    cfg.client_secret = client_secret;
    cfg.redirect_uri  = redirect_uri;
    cfg.auth_url      = auth_url;
    cfg.token_url     = token_url;
    // Parse space-separated scopes
    std::istringstream ss(scopes);
    std::string scope;
    while (ss >> scope) cfg.scopes.push_back(scope);
    static_cast<Sapphire::Auth::OAuth2Provider*>(oauth2)->register_provider(cfg);
}
const char* oauth2_get_auth_url(void* oauth2, const char* provider, const char* state) {
    g_last_string = static_cast<Sapphire::Auth::OAuth2Provider*>(oauth2)->get_auth_url(provider, state ? state : "");
    return g_last_string.c_str();
}
const char* oauth2_generate_state(void* oauth2) {
    g_last_string = static_cast<Sapphire::Auth::OAuth2Provider*>(oauth2)->generate_state();
    return g_last_string.c_str();
}
int oauth2_validate_state(void* oauth2, const char* state) {
    return static_cast<Sapphire::Auth::OAuth2Provider*>(oauth2)->validate_state(state) ? 1 : 0;
}
int oauth2_has_provider(void* oauth2, const char* provider) {
    return static_cast<Sapphire::Auth::OAuth2Provider*>(oauth2)->has_provider(provider) ? 1 : 0;
}

} // extern "C"
