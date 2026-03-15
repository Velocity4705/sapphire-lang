#include "security.h"
#include <string>
#include <sstream>
#include <cstring>

using namespace Sapphire::Security;

// Thread-local string storage for returned const char*
static thread_local std::string tl_str;
static const char* ret_str(const std::string& s) { tl_str = s; return tl_str.c_str(); }

// ===== VulnScanner =====

void* sec_scanner_create() { return new VulnScanner(); }
void  sec_scanner_destroy(void* s) { delete static_cast<VulnScanner*>(s); }

void* sec_scanner_scan_host(void* s, const char* target, const char* ports_csv) {
    auto* scanner = static_cast<VulnScanner*>(s);
    std::vector<int> ports;
    if (ports_csv && ports_csv[0] != '\0') {
        std::string csv(ports_csv);
        std::istringstream ss(csv);
        std::string tok;
        while (std::getline(ss, tok, ',')) {
            try { ports.push_back(std::stoi(tok)); } catch (...) {}
        }
    }
    return new ScanResult(scanner->scan_host(target ? target : "", ports));
}

int sec_scanner_vuln_db_size(void* s) {
    return static_cast<VulnScanner*>(s)->vuln_db_size();
}

// ===== ScanResult =====

void  sec_scanresult_destroy(void* r) { delete static_cast<ScanResult*>(r); }
const char* sec_scanresult_target(void* r) { return ret_str(static_cast<ScanResult*>(r)->target); }
int   sec_scanresult_reachable(void* r) { return static_cast<ScanResult*>(r)->reachable ? 1 : 0; }
int   sec_scanresult_open_ports(void* r) { return static_cast<ScanResult*>(r)->open_port_count(); }
int   sec_scanresult_vuln_count(void* r) { return static_cast<ScanResult*>(r)->vuln_count(); }
const char* sec_scanresult_to_json(void* r) { return ret_str(static_cast<ScanResult*>(r)->to_json()); }

const char* sec_scanresult_port_service(void* r, int i) {
    auto* sr = static_cast<ScanResult*>(r);
    if (i < 0 || i >= (int)sr->ports.size()) return "";
    return ret_str(sr->ports[i].service);
}
int sec_scanresult_port_number(void* r, int i) {
    auto* sr = static_cast<ScanResult*>(r);
    if (i < 0 || i >= (int)sr->ports.size()) return 0;
    return sr->ports[i].port;
}
const char* sec_scanresult_vuln_name(void* r, int i) {
    auto* sr = static_cast<ScanResult*>(r);
    if (i < 0 || i >= (int)sr->vulnerabilities.size()) return "";
    return ret_str(sr->vulnerabilities[i].name);
}
const char* sec_scanresult_vuln_severity(void* r, int i) {
    auto* sr = static_cast<ScanResult*>(r);
    if (i < 0 || i >= (int)sr->vulnerabilities.size()) return "";
    VulnScanner tmp;
    return ret_str(tmp.severity_name(sr->vulnerabilities[i].severity));
}
const char* sec_scanresult_vuln_remediation(void* r, int i) {
    auto* sr = static_cast<ScanResult*>(r);
    if (i < 0 || i >= (int)sr->vulnerabilities.size()) return "";
    return ret_str(sr->vulnerabilities[i].remediation);
}

// ===== WebSecurityTester =====

void* sec_web_tester_create() { return new WebSecurityTester(); }
void  sec_web_tester_destroy(void* t) { delete static_cast<WebSecurityTester*>(t); }

void* sec_web_scan(void* t, const char* url) {
    return new WebScanResult(static_cast<WebSecurityTester*>(t)->scan(url ? url : ""));
}
int sec_web_test_sqli(void* t, const char* url, const char* param) {
    return static_cast<WebSecurityTester*>(t)->test_sql_injection(url?url:"", param?param:"") ? 1 : 0;
}
int sec_web_test_xss(void* t, const char* url, const char* param) {
    return static_cast<WebSecurityTester*>(t)->test_xss(url?url:"", param?param:"") ? 1 : 0;
}
int sec_web_test_csrf(void* t, const char* url) {
    return static_cast<WebSecurityTester*>(t)->test_csrf(url?url:"") ? 1 : 0;
}
int sec_web_test_auth_bypass(void* t, const char* url) {
    return static_cast<WebSecurityTester*>(t)->test_auth_bypass(url?url:"") ? 1 : 0;
}
int sec_web_test_path_traversal(void* t, const char* url, const char* param) {
    return static_cast<WebSecurityTester*>(t)->test_path_traversal(url?url:"", param?param:"") ? 1 : 0;
}
const char* sec_web_encode_payload(void* t, const char* payload, const char* encoding) {
    return ret_str(static_cast<WebSecurityTester*>(t)->encode_payload(payload?payload:"", encoding?encoding:""));
}
const char* sec_web_get_payloads(void* t, const char* vuln_type) {
    auto v = static_cast<WebSecurityTester*>(t)->get_payloads(vuln_type?vuln_type:"");
    std::string out;
    for (size_t i = 0; i < v.size(); i++) { if (i) out += "|"; out += v[i]; }
    return ret_str(out);
}

// ===== WebScanResult =====

void  sec_webscan_destroy(void* r) { delete static_cast<WebScanResult*>(r); }
int   sec_webscan_finding_count(void* r) { return (int)static_cast<WebScanResult*>(r)->findings.size(); }
const char* sec_webscan_finding_name(void* r, int i) {
    auto* wr = static_cast<WebScanResult*>(r);
    if (i < 0 || i >= (int)wr->findings.size()) return "";
    return ret_str(wr->findings[i].name);
}
const char* sec_webscan_finding_severity(void* r, int i) {
    auto* wr = static_cast<WebScanResult*>(r);
    if (i < 0 || i >= (int)wr->findings.size()) return "";
    VulnScanner tmp;
    return ret_str(tmp.severity_name(wr->findings[i].severity));
}
const char* sec_webscan_finding_param(void* r, int i) {
    auto* wr = static_cast<WebScanResult*>(r);
    if (i < 0 || i >= (int)wr->findings.size()) return "";
    return ret_str(wr->findings[i].parameter);
}
const char* sec_webscan_finding_payload(void* r, int i) {
    auto* wr = static_cast<WebScanResult*>(r);
    if (i < 0 || i >= (int)wr->findings.size()) return "";
    return ret_str(wr->findings[i].payload);
}
const char* sec_webscan_to_json(void* r) { return ret_str(static_cast<WebScanResult*>(r)->to_json()); }

// ===== MalwareAnalyzer =====

void* sec_malware_analyzer_create() { return new MalwareAnalyzer(); }
void  sec_malware_analyzer_destroy(void* a) { delete static_cast<MalwareAnalyzer*>(a); }

void* sec_malware_static_analysis(void* a, const char* filepath, const char* content_hex) {
    return new StaticAnalysisResult(
        static_cast<MalwareAnalyzer*>(a)->static_analysis(
            filepath?filepath:"", content_hex?content_hex:""));
}
void* sec_malware_dynamic_analysis(void* a, const char* filepath, int timeout_ms) {
    return new DynamicAnalysisResult(
        static_cast<MalwareAnalyzer*>(a)->dynamic_analysis(filepath?filepath:"", timeout_ms));
}
int sec_malware_add_yara_rule(void* a, const char* name, const char* pattern) {
    return static_cast<MalwareAnalyzer*>(a)->add_yara_rule(name?name:"", pattern?pattern:"") ? 1 : 0;
}

// ===== StaticAnalysisResult =====

void  sec_static_result_destroy(void* r) { delete static_cast<StaticAnalysisResult*>(r); }
const char* sec_static_result_threat_level(void* r) {
    MalwareAnalyzer tmp;
    return ret_str(tmp.threat_level_name(static_cast<StaticAnalysisResult*>(r)->threat_level));
}
const char* sec_static_result_file_type(void* r) {
    MalwareAnalyzer tmp;
    return ret_str(tmp.file_type_name(static_cast<StaticAnalysisResult*>(r)->file_type));
}
const char* sec_static_result_sha256(void* r) { return ret_str(static_cast<StaticAnalysisResult*>(r)->sha256); }
const char* sec_static_result_md5(void* r) { return ret_str(static_cast<StaticAnalysisResult*>(r)->md5); }
double      sec_static_result_entropy(void* r) { return static_cast<StaticAnalysisResult*>(r)->entropy; }
int   sec_static_result_string_count(void* r) { return (int)static_cast<StaticAnalysisResult*>(r)->strings_found.size(); }
const char* sec_static_result_string_value(void* r, int i) {
    auto* sr = static_cast<StaticAnalysisResult*>(r);
    if (i < 0 || i >= (int)sr->strings_found.size()) return "";
    return ret_str(sr->strings_found[i].value);
}
const char* sec_static_result_string_category(void* r, int i) {
    auto* sr = static_cast<StaticAnalysisResult*>(r);
    if (i < 0 || i >= (int)sr->strings_found.size()) return "";
    return ret_str(sr->strings_found[i].category);
}
int   sec_static_result_suspicious_count(void* r) { return (int)static_cast<StaticAnalysisResult*>(r)->suspicious_patterns.size(); }
const char* sec_static_result_suspicious(void* r, int i) {
    auto* sr = static_cast<StaticAnalysisResult*>(r);
    if (i < 0 || i >= (int)sr->suspicious_patterns.size()) return "";
    return ret_str(sr->suspicious_patterns[i]);
}
const char* sec_static_result_to_json(void* r) { return ret_str(static_cast<StaticAnalysisResult*>(r)->to_json()); }

// ===== DynamicAnalysisResult =====

void  sec_dynamic_result_destroy(void* r) { delete static_cast<DynamicAnalysisResult*>(r); }
const char* sec_dynamic_result_threat_level(void* r) {
    MalwareAnalyzer tmp;
    return ret_str(tmp.threat_level_name(static_cast<DynamicAnalysisResult*>(r)->threat_level));
}
int   sec_dynamic_result_event_count(void* r) { return (int)static_cast<DynamicAnalysisResult*>(r)->events.size(); }
const char* sec_dynamic_result_event_type(void* r, int i) {
    auto* dr = static_cast<DynamicAnalysisResult*>(r);
    if (i < 0 || i >= (int)dr->events.size()) return "";
    return ret_str(dr->events[i].type);
}
const char* sec_dynamic_result_event_target(void* r, int i) {
    auto* dr = static_cast<DynamicAnalysisResult*>(r);
    if (i < 0 || i >= (int)dr->events.size()) return "";
    return ret_str(dr->events[i].target);
}
int   sec_dynamic_result_ioc_count(void* r) { return (int)static_cast<DynamicAnalysisResult*>(r)->network_iocs.size(); }
const char* sec_dynamic_result_ioc(void* r, int i) {
    auto* dr = static_cast<DynamicAnalysisResult*>(r);
    if (i < 0 || i >= (int)dr->network_iocs.size()) return "";
    return ret_str(dr->network_iocs[i]);
}
const char* sec_dynamic_result_to_json(void* r) { return ret_str(static_cast<DynamicAnalysisResult*>(r)->to_json()); }

// ===== EducationalPlatform =====

void* sec_edu_platform_create() { return new EducationalPlatform(); }
void  sec_edu_platform_destroy(void* p) { delete static_cast<EducationalPlatform*>(p); }

int sec_edu_challenge_count(void* p) {
    return static_cast<EducationalPlatform*>(p)->challenge_count();
}
const char* sec_edu_challenge_id(void* p, int i) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto all = ep->all_challenges();
    if (i < 0 || i >= (int)all.size()) return "";
    return ret_str(all[i]->id);
}
const char* sec_edu_challenge_name(void* p, int i) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto all = ep->all_challenges();
    if (i < 0 || i >= (int)all.size()) return "";
    return ret_str(all[i]->name);
}
const char* sec_edu_challenge_category(void* p, int i) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto all = ep->all_challenges();
    if (i < 0 || i >= (int)all.size()) return "";
    switch (all[i]->category) {
        case ChallengeCategory::WEB:                return "WEB";
        case ChallengeCategory::CRYPTO:             return "CRYPTO";
        case ChallengeCategory::FORENSICS:          return "FORENSICS";
        case ChallengeCategory::REVERSE_ENGINEERING:return "RE";
        case ChallengeCategory::PWN:                return "PWN";
        case ChallengeCategory::MISC:               return "MISC";
    }
    return "UNKNOWN";
}
int sec_edu_challenge_points(void* p, int i) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto all = ep->all_challenges();
    if (i < 0 || i >= (int)all.size()) return 0;
    return all[i]->points;
}
const char* sec_edu_challenge_description(void* p, int i) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto all = ep->all_challenges();
    if (i < 0 || i >= (int)all.size()) return "";
    return ret_str(all[i]->description);
}
const char* sec_edu_challenge_hint(void* p, int i) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto all = ep->all_challenges();
    if (i < 0 || i >= (int)all.size()) return "";
    return ret_str(all[i]->hint);
}

// CTFSession wrapper
struct SessionWrapper {
    EducationalPlatform* platform;
    CTFSession session;
};

void* sec_edu_create_session(void* p, const char* player) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto* sw = new SessionWrapper();
    sw->platform = ep;
    sw->session  = ep->create_session(player ? player : "");
    return sw;
}
void sec_edu_session_destroy(void* session) { delete static_cast<SessionWrapper*>(session); }

int sec_edu_session_submit(void* p, void* session, const char* challenge_id, const char* flag) {
    auto* ep = static_cast<EducationalPlatform*>(p);
    auto* sw = static_cast<SessionWrapper*>(session);
    CTFChallenge* ch = ep->get_challenge(challenge_id ? challenge_id : "");
    if (!ch) return 0;
    return sw->session.submit_flag(*ch, flag ? flag : "") ? 1 : 0;
}
int sec_edu_session_score(void* session) {
    return static_cast<SessionWrapper*>(session)->session.score;
}
const char* sec_edu_session_player(void* session) {
    return ret_str(static_cast<SessionWrapper*>(session)->session.player);
}
const char* sec_edu_leaderboard(void* p) {
    return ret_str(static_cast<EducationalPlatform*>(p)->leaderboard());
}
const char* sec_edu_get_tutorial(void* p, const char* topic) {
    return ret_str(static_cast<EducationalPlatform*>(p)->get_tutorial(topic ? topic : ""));
}
