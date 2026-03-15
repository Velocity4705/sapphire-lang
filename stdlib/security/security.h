#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace Sapphire {
namespace Security {

// ===== Phase 1: Vulnerability Scanner =====

enum class PortState { OPEN, CLOSED, FILTERED };
enum class Severity  { INFO, LOW, MEDIUM, HIGH, CRITICAL };

struct PortResult {
    int         port;
    PortState   state;
    std::string service;   // "http", "ssh", "ftp", etc.
    std::string banner;
};

struct Vulnerability {
    std::string id;        // CVE-style: "CVE-2024-XXXX" or custom
    std::string name;
    Severity    severity;
    std::string description;
    std::string affected;  // service/port
    std::string remediation;
};

struct ScanResult {
    std::string target;
    std::string ip;
    bool        reachable;
    std::vector<PortResult>    ports;
    std::vector<Vulnerability> vulnerabilities;
    uint64_t    scan_duration_ms;

    int open_port_count() const;
    int vuln_count() const { return static_cast<int>(vulnerabilities.size()); }
    int vuln_count_by_severity(Severity s) const;
    std::string to_string() const;
    std::string to_json() const;
};

class VulnScanner {
public:
    VulnScanner();
    ScanResult scan_host(const std::string& target,
                         const std::vector<int>& ports = {});
    ScanResult scan_range(const std::string& cidr);  // simulated
    void load_vuln_db(const std::string& json);
    int  vuln_db_size() const { return static_cast<int>(vuln_db_.size()); }
    std::string severity_name(Severity s) const;

private:
    std::vector<Vulnerability> vuln_db_;
    void init_default_vuln_db();
    std::vector<PortResult> simulate_port_scan(const std::string& target,
                                                const std::vector<int>& ports);
    std::vector<Vulnerability> check_vulns(const std::vector<PortResult>& ports);
};

// ===== Phase 2: Web Security Testing =====

enum class WebVulnType {
    SQL_INJECTION, XSS, CSRF, AUTH_BYPASS,
    OPEN_REDIRECT, PATH_TRAVERSAL, SSRF, XXE,
    INSECURE_DESERIALIZATION, SENSITIVE_DATA_EXPOSURE
};

struct WebVuln {
    WebVulnType type;
    std::string name;
    Severity    severity;
    std::string url;
    std::string parameter;
    std::string payload;
    std::string evidence;
    std::string remediation;
    std::string to_string() const;
};

struct WebScanResult {
    std::string target_url;
    std::vector<WebVuln> findings;
    int total_requests;
    uint64_t duration_ms;
    std::string to_json() const;
};

class WebSecurityTester {
public:
    WebSecurityTester();
    WebScanResult scan(const std::string& url);
    bool test_sql_injection(const std::string& url, const std::string& param);
    bool test_xss(const std::string& url, const std::string& param);
    bool test_csrf(const std::string& url);
    bool test_auth_bypass(const std::string& url);
    bool test_path_traversal(const std::string& url, const std::string& param);
    bool test_open_redirect(const std::string& url, const std::string& param);
    std::string encode_payload(const std::string& payload, const std::string& encoding);
    std::string decode_payload(const std::string& payload, const std::string& encoding);
    std::vector<std::string> get_payloads(const std::string& vuln_type) const;

private:
    std::unordered_map<std::string, std::vector<std::string>> payloads_;
    void init_payloads();
    WebVuln make_finding(WebVulnType t, const std::string& url,
                         const std::string& param, const std::string& payload);
};

// ===== Phase 3: Malware Analysis =====

enum class FileType { ELF, PE, MACHO, SCRIPT, ARCHIVE, UNKNOWN };
enum class ThreatLevel { CLEAN, SUSPICIOUS, MALICIOUS };

struct StringMatch {
    std::string value;
    std::string category; // "url", "ip", "registry", "api_call", "crypto"
    size_t      offset;
};

struct StaticAnalysisResult {
    std::string filename;
    FileType    file_type;
    std::string md5;
    std::string sha256;
    size_t      file_size;
    ThreatLevel threat_level;
    std::vector<StringMatch>   strings_found;
    std::vector<std::string>   imports;
    std::vector<std::string>   suspicious_patterns;
    std::vector<std::string>   yara_matches;
    double      entropy;
    std::string to_json() const;
};

struct BehaviorEvent {
    std::string type;    // "file_write", "network_connect", "registry_write", etc.
    std::string target;
    std::string details;
    uint64_t    timestamp_ms;
};

struct DynamicAnalysisResult {
    std::string filename;
    ThreatLevel threat_level;
    std::vector<BehaviorEvent> events;
    std::vector<std::string>   network_iocs;  // IPs/domains contacted
    std::vector<std::string>   file_iocs;     // files created/modified
    std::string to_json() const;
};

class MalwareAnalyzer {
public:
    MalwareAnalyzer();
    StaticAnalysisResult  static_analysis(const std::string& filepath,
                                           const std::string& content_hex = "");
    DynamicAnalysisResult dynamic_analysis(const std::string& filepath,
                                            int timeout_ms = 5000);
    bool add_yara_rule(const std::string& rule_name, const std::string& pattern);
    std::string threat_level_name(ThreatLevel t) const;
    std::string file_type_name(FileType t) const;

private:
    std::unordered_map<std::string, std::string> yara_rules_;
    void init_default_rules();
    double compute_entropy(const std::string& data) const;
    FileType detect_file_type(const std::string& content_hex) const;
    std::vector<StringMatch> extract_strings(const std::string& content) const;
};

// ===== Phase 4: Educational Platform =====

enum class ChallengeCategory {
    WEB, CRYPTO, FORENSICS, REVERSE_ENGINEERING, PWN, MISC
};
enum class ChallengeStatus { LOCKED, AVAILABLE, SOLVED };

struct CTFChallenge {
    std::string id;
    std::string name;
    ChallengeCategory category;
    int         points;
    std::string description;
    std::string flag;        // hashed
    std::string hint;
    ChallengeStatus status;
    std::string to_string() const;
};

struct CTFSession {
    std::string player;
    int         score;
    std::vector<std::string> solved_ids;
    std::vector<std::string> hints_used;
    bool submit_flag(CTFChallenge& ch, const std::string& flag_attempt);
    bool is_solved(const std::string& challenge_id) const;
    std::string scoreboard_entry() const;
};

class EducationalPlatform {
public:
    EducationalPlatform();
    void add_challenge(const CTFChallenge& ch);
    CTFChallenge* get_challenge(const std::string& id);
    std::vector<CTFChallenge*> list_challenges(ChallengeCategory cat);
    std::vector<CTFChallenge*> all_challenges();
    CTFSession create_session(const std::string& player);
    std::string leaderboard() const;
    int challenge_count() const { return static_cast<int>(challenges_.size()); }
    std::string get_tutorial(const std::string& topic) const;

private:
    std::vector<CTFChallenge> challenges_;
    std::vector<CTFSession>   sessions_;
    std::unordered_map<std::string, std::string> tutorials_;
    void init_default_challenges();
    void init_tutorials();
};

} // namespace Security
} // namespace Sapphire

// ===== C API =====
extern "C" {

// VulnScanner
void* sec_scanner_create();
void  sec_scanner_destroy(void* s);
void* sec_scanner_scan_host(void* s, const char* target, const char* ports_csv);
int   sec_scanner_vuln_db_size(void* s);

// ScanResult
void  sec_scanresult_destroy(void* r);
const char* sec_scanresult_target(void* r);
int   sec_scanresult_reachable(void* r);
int   sec_scanresult_open_ports(void* r);
int   sec_scanresult_vuln_count(void* r);
const char* sec_scanresult_to_json(void* r);
const char* sec_scanresult_port_service(void* r, int index);
int   sec_scanresult_port_number(void* r, int index);
const char* sec_scanresult_vuln_name(void* r, int index);
const char* sec_scanresult_vuln_severity(void* r, int index);
const char* sec_scanresult_vuln_remediation(void* r, int index);

// WebSecurityTester
void* sec_web_tester_create();
void  sec_web_tester_destroy(void* t);
void* sec_web_scan(void* t, const char* url);
int   sec_web_test_sqli(void* t, const char* url, const char* param);
int   sec_web_test_xss(void* t, const char* url, const char* param);
int   sec_web_test_csrf(void* t, const char* url);
int   sec_web_test_auth_bypass(void* t, const char* url);
int   sec_web_test_path_traversal(void* t, const char* url, const char* param);
const char* sec_web_encode_payload(void* t, const char* payload, const char* encoding);
const char* sec_web_get_payloads(void* t, const char* vuln_type);

// WebScanResult
void  sec_webscan_destroy(void* r);
int   sec_webscan_finding_count(void* r);
const char* sec_webscan_finding_name(void* r, int i);
const char* sec_webscan_finding_severity(void* r, int i);
const char* sec_webscan_finding_param(void* r, int i);
const char* sec_webscan_finding_payload(void* r, int i);
const char* sec_webscan_to_json(void* r);

// MalwareAnalyzer
void* sec_malware_analyzer_create();
void  sec_malware_analyzer_destroy(void* a);
void* sec_malware_static_analysis(void* a, const char* filepath, const char* content_hex);
void* sec_malware_dynamic_analysis(void* a, const char* filepath, int timeout_ms);
int   sec_malware_add_yara_rule(void* a, const char* name, const char* pattern);

// StaticAnalysisResult
void  sec_static_result_destroy(void* r);
const char* sec_static_result_threat_level(void* r);
const char* sec_static_result_file_type(void* r);
const char* sec_static_result_sha256(void* r);
const char* sec_static_result_md5(void* r);
double      sec_static_result_entropy(void* r);
int   sec_static_result_string_count(void* r);
const char* sec_static_result_string_value(void* r, int i);
const char* sec_static_result_string_category(void* r, int i);
int   sec_static_result_suspicious_count(void* r);
const char* sec_static_result_suspicious(void* r, int i);
const char* sec_static_result_to_json(void* r);

// DynamicAnalysisResult
void  sec_dynamic_result_destroy(void* r);
const char* sec_dynamic_result_threat_level(void* r);
int   sec_dynamic_result_event_count(void* r);
const char* sec_dynamic_result_event_type(void* r, int i);
const char* sec_dynamic_result_event_target(void* r, int i);
int   sec_dynamic_result_ioc_count(void* r);
const char* sec_dynamic_result_ioc(void* r, int i);
const char* sec_dynamic_result_to_json(void* r);

// EducationalPlatform
void* sec_edu_platform_create();
void  sec_edu_platform_destroy(void* p);
int   sec_edu_challenge_count(void* p);
const char* sec_edu_challenge_id(void* p, int i);
const char* sec_edu_challenge_name(void* p, int i);
const char* sec_edu_challenge_category(void* p, int i);
int   sec_edu_challenge_points(void* p, int i);
const char* sec_edu_challenge_description(void* p, int i);
const char* sec_edu_challenge_hint(void* p, int i);
void* sec_edu_create_session(void* p, const char* player);
void  sec_edu_session_destroy(void* session);
int   sec_edu_session_submit(void* p, void* session, const char* challenge_id, const char* flag);
int   sec_edu_session_score(void* session);
const char* sec_edu_session_player(void* session);
const char* sec_edu_leaderboard(void* p);
const char* sec_edu_get_tutorial(void* p, const char* topic);

} // extern "C"
