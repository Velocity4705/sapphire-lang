#include "security.h"
#include "../crypto/crypto.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace Sapphire {
namespace Security {

static uint64_t now_ms() {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
}

static std::string sev_name(Severity s) {
    switch (s) {
        case Severity::INFO:     return "INFO";
        case Severity::LOW:      return "LOW";
        case Severity::MEDIUM:   return "MEDIUM";
        case Severity::HIGH:     return "HIGH";
        case Severity::CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";
}

// ===== ScanResult =====

int ScanResult::open_port_count() const {
    int n = 0;
    for (auto& p : ports) if (p.state == PortState::OPEN) n++;
    return n;
}

int ScanResult::vuln_count_by_severity(Severity s) const {
    int n = 0;
    for (auto& v : vulnerabilities) if (v.severity == s) n++;
    return n;
}

std::string ScanResult::to_string() const { return to_json(); }

std::string ScanResult::to_json() const {
    std::ostringstream ss;
    ss << "{\"target\":\"" << target << "\",\"reachable\":" << (reachable?"true":"false")
       << ",\"open_ports\":" << open_port_count()
       << ",\"vulnerabilities\":" << vuln_count() << ",\"ports\":[";
    for (size_t i = 0; i < ports.size(); i++) {
        if (i) ss << ",";
        ss << "{\"port\":" << ports[i].port
           << ",\"state\":\"" << (ports[i].state==PortState::OPEN?"open":"closed") << "\""
           << ",\"service\":\"" << ports[i].service << "\"}";
    }
    ss << "],\"vulns\":[";
    for (size_t i = 0; i < vulnerabilities.size(); i++) {
        if (i) ss << ",";
        ss << "{\"id\":\"" << vulnerabilities[i].id
           << "\",\"name\":\"" << vulnerabilities[i].name
           << "\",\"severity\":\"" << sev_name(vulnerabilities[i].severity) << "\"}";
    }
    ss << "]}";
    return ss.str();
}

// ===== VulnScanner =====

static const std::unordered_map<std::string, std::string> SERVICE_MAP = {
    {"21","ftp"},{"22","ssh"},{"23","telnet"},{"25","smtp"},{"53","dns"},
    {"80","http"},{"110","pop3"},{"143","imap"},{"443","https"},{"445","smb"},
    {"3306","mysql"},{"5432","postgres"},{"6379","redis"},{"8080","http-alt"},
    {"27017","mongodb"},{"3389","rdp"},{"8443","https-alt"}
};

VulnScanner::VulnScanner() { init_default_vuln_db(); }

void VulnScanner::init_default_vuln_db() {
    vuln_db_ = {
        {"CVE-2021-44228","Log4Shell RCE",Severity::CRITICAL,
         "Remote code execution via JNDI injection in Log4j","http/https","Update Log4j >= 2.17.1"},
        {"CVE-2017-0144","EternalBlue SMB RCE",Severity::CRITICAL,
         "Remote code execution via SMBv1 vulnerability","smb","Disable SMBv1, apply MS17-010"},
        {"CVE-2019-0708","BlueKeep RDP RCE",Severity::CRITICAL,
         "Pre-auth RCE in Windows Remote Desktop Services","rdp","Apply KB4499175 patch"},
        {"CVE-2021-21985","vSphere RCE",Severity::CRITICAL,
         "RCE in VMware vCenter Server","https","Update vCenter Server"},
        {"CVE-2020-1472","Zerologon",Severity::CRITICAL,
         "Privilege escalation via Netlogon","smb","Apply August 2020 security update"},
        {"CVE-2022-22965","Spring4Shell",Severity::HIGH,
         "RCE in Spring Framework","http/https","Upgrade Spring Framework >= 5.3.18"},
        {"CVE-2021-26855","ProxyLogon Exchange SSRF",Severity::HIGH,
         "SSRF leading to RCE in Microsoft Exchange","https","Apply March 2021 Exchange patches"},
        {"VULN-TELNET-001","Telnet Cleartext Protocol",Severity::HIGH,
         "Telnet transmits credentials in cleartext","telnet","Replace with SSH"},
        {"VULN-FTP-001","Anonymous FTP Access",Severity::MEDIUM,
         "FTP server allows anonymous login","ftp","Disable anonymous FTP"},
        {"VULN-REDIS-001","Redis No Authentication",Severity::HIGH,
         "Redis instance accessible without authentication","redis","Set requirepass in redis.conf"},
        {"VULN-MONGO-001","MongoDB No Auth",Severity::HIGH,
         "MongoDB accessible without authentication","mongodb","Enable MongoDB authentication"},
        {"VULN-MYSQL-001","MySQL Default Credentials",Severity::MEDIUM,
         "MySQL using default or weak credentials","mysql","Change default credentials"},
        {"VULN-HTTP-001","HTTP (Unencrypted)",Severity::LOW,
         "Service running over unencrypted HTTP","http","Migrate to HTTPS with TLS 1.2+"},
        {"VULN-SMB-001","SMB Exposed to Internet",Severity::HIGH,
         "SMB port 445 exposed to internet","smb","Block SMB at perimeter firewall"},
    };
}

std::vector<PortResult> VulnScanner::simulate_port_scan(
        const std::string& target, const std::vector<int>& ports) {
    std::vector<PortResult> results;
    // Deterministic simulation based on target hash
    std::string h = Crypto::sha256(target);
    int seed = 0;
    for (char c : h.substr(0,8)) seed += c;

    for (int port : ports) {
        PortResult pr;
        pr.port = port;
        // Open if (seed + port) % 3 != 0 for common ports
        bool is_common = (port==80||port==443||port==22||port==8080);
        pr.state = (is_common || (seed + port) % 4 != 0)
                   ? PortState::OPEN : PortState::CLOSED;
        std::string ps = std::to_string(port);
        auto it = SERVICE_MAP.find(ps);
        pr.service = it != SERVICE_MAP.end() ? it->second : "unknown";
        pr.banner  = pr.state == PortState::OPEN
                   ? pr.service + "/1.0 (simulated)" : "";
        results.push_back(pr);
    }
    return results;
}

std::vector<Vulnerability> VulnScanner::check_vulns(
        const std::vector<PortResult>& ports) {
    std::vector<Vulnerability> found;
    for (auto& pr : ports) {
        if (pr.state != PortState::OPEN) continue;
        for (auto& v : vuln_db_) {
            if (v.affected.find(pr.service) != std::string::npos)
                found.push_back(v);
        }
    }
    return found;
}

ScanResult VulnScanner::scan_host(const std::string& target,
                                   const std::vector<int>& ports) {
    uint64_t t0 = now_ms();
    ScanResult r;
    r.target    = target;
    r.ip        = "192.168." + std::to_string(Crypto::sha256(target)[0] % 255)
                + "." + std::to_string(Crypto::sha256(target)[1] % 255);
    r.reachable = true;

    std::vector<int> scan_ports = ports.empty()
        ? std::vector<int>{21,22,23,25,53,80,110,143,443,445,
                           3306,5432,6379,8080,27017,3389,8443}
        : ports;

    r.ports           = simulate_port_scan(target, scan_ports);
    r.vulnerabilities = check_vulns(r.ports);
    r.scan_duration_ms = now_ms() - t0 + 1;
    return r;
}

ScanResult VulnScanner::scan_range(const std::string& cidr) {
    // Simulate scanning first host in range
    std::string host = cidr.substr(0, cidr.find('/'));
    return scan_host(host);
}

std::string VulnScanner::severity_name(Severity s) const { return sev_name(s); }

// ===== WebVuln =====

static std::string web_vuln_type_name(WebVulnType t) {
    switch (t) {
        case WebVulnType::SQL_INJECTION:           return "SQL Injection";
        case WebVulnType::XSS:                     return "Cross-Site Scripting (XSS)";
        case WebVulnType::CSRF:                    return "Cross-Site Request Forgery (CSRF)";
        case WebVulnType::AUTH_BYPASS:             return "Authentication Bypass";
        case WebVulnType::OPEN_REDIRECT:           return "Open Redirect";
        case WebVulnType::PATH_TRAVERSAL:          return "Path Traversal";
        case WebVulnType::SSRF:                    return "Server-Side Request Forgery (SSRF)";
        case WebVulnType::XXE:                     return "XML External Entity (XXE)";
        case WebVulnType::INSECURE_DESERIALIZATION:return "Insecure Deserialization";
        case WebVulnType::SENSITIVE_DATA_EXPOSURE: return "Sensitive Data Exposure";
    }
    return "Unknown";
}

std::string WebVuln::to_string() const {
    return "{\"type\":\"" + name + "\",\"severity\":\"" + sev_name(severity)
         + "\",\"url\":\"" + url + "\",\"param\":\"" + parameter
         + "\",\"payload\":\"" + payload + "\"}";
}

std::string WebScanResult::to_json() const {
    std::ostringstream ss;
    ss << "{\"target\":\"" << target_url << "\",\"findings\":" << findings.size()
       << ",\"requests\":" << total_requests << ",\"results\":[";
    for (size_t i = 0; i < findings.size(); i++) {
        if (i) ss << ",";
        ss << findings[i].to_string();
    }
    ss << "]}";
    return ss.str();
}

// ===== WebSecurityTester =====

WebSecurityTester::WebSecurityTester() { init_payloads(); }

void WebSecurityTester::init_payloads() {
    payloads_["sqli"] = {
        "' OR '1'='1", "' OR 1=1--", "'; DROP TABLE users--",
        "' UNION SELECT NULL,NULL--", "1' AND SLEEP(5)--",
        "' OR 'x'='x", "admin'--", "' OR 1=1#"
    };
    payloads_["xss"] = {
        "<script>alert(1)</script>",
        "<img src=x onerror=alert(1)>",
        "javascript:alert(1)",
        "<svg onload=alert(1)>",
        "'\"><script>alert(document.cookie)</script>",
        "<body onload=alert(1)>"
    };
    payloads_["path_traversal"] = {
        "../../../etc/passwd", "..\\..\\..\\windows\\system32\\drivers\\etc\\hosts",
        "%2e%2e%2f%2e%2e%2f%2e%2e%2fetc%2fpasswd",
        "....//....//....//etc/passwd"
    };
    payloads_["open_redirect"] = {
        "//evil.com", "https://evil.com", "//evil.com/%2F..",
        "javascript:alert(1)", "/\\evil.com"
    };
}

WebVuln WebSecurityTester::make_finding(WebVulnType t, const std::string& url,
                                         const std::string& param,
                                         const std::string& payload) {
    WebVuln v;
    v.type      = t;
    v.name      = web_vuln_type_name(t);
    v.url       = url;
    v.parameter = param;
    v.payload   = payload;
    v.evidence  = "Response contains indicator for " + v.name;
    switch (t) {
        case WebVulnType::SQL_INJECTION:
            v.severity = Severity::CRITICAL;
            v.remediation = "Use parameterized queries / prepared statements";
            break;
        case WebVulnType::XSS:
            v.severity = Severity::HIGH;
            v.remediation = "Encode output, use Content-Security-Policy header";
            break;
        case WebVulnType::CSRF:
            v.severity = Severity::MEDIUM;
            v.remediation = "Implement CSRF tokens, SameSite cookie attribute";
            break;
        case WebVulnType::AUTH_BYPASS:
            v.severity = Severity::CRITICAL;
            v.remediation = "Fix authentication logic, use secure session management";
            break;
        case WebVulnType::PATH_TRAVERSAL:
            v.severity = Severity::HIGH;
            v.remediation = "Validate and sanitize file paths, use allowlists";
            break;
        default:
            v.severity = Severity::MEDIUM;
            v.remediation = "Review and fix the identified vulnerability";
    }
    return v;
}

bool WebSecurityTester::test_sql_injection(const std::string& url,
                                            const std::string& param) {
    // Simulate: vulnerable if param contains "id", "user", "search"
    return param.find("id") != std::string::npos
        || param.find("user") != std::string::npos
        || param.find("search") != std::string::npos
        || param.find("q") != std::string::npos;
}

bool WebSecurityTester::test_xss(const std::string& url, const std::string& param) {
    return param.find("name") != std::string::npos
        || param.find("msg") != std::string::npos
        || param.find("comment") != std::string::npos
        || param.find("q") != std::string::npos;
}

bool WebSecurityTester::test_csrf(const std::string& url) {
    // Simulate: vulnerable if URL contains form-like paths
    return url.find("login") != std::string::npos
        || url.find("transfer") != std::string::npos
        || url.find("update") != std::string::npos
        || url.find("delete") != std::string::npos;
}

bool WebSecurityTester::test_auth_bypass(const std::string& url) {
    return url.find("admin") != std::string::npos
        || url.find("dashboard") != std::string::npos;
}

bool WebSecurityTester::test_path_traversal(const std::string& url,
                                             const std::string& param) {
    return param.find("file") != std::string::npos
        || param.find("path") != std::string::npos
        || param.find("dir") != std::string::npos;
}

bool WebSecurityTester::test_open_redirect(const std::string& url,
                                            const std::string& param) {
    return param.find("redirect") != std::string::npos
        || param.find("url") != std::string::npos
        || param.find("next") != std::string::npos;
}

std::string WebSecurityTester::encode_payload(const std::string& payload,
                                               const std::string& encoding) {
    if (encoding == "url") {
        std::ostringstream ss;
        for (unsigned char c : payload) {
            if (isalnum(c) || c=='-'||c=='_'||c=='.'||c=='~') ss << c;
            else ss << '%' << std::uppercase << std::hex << std::setw(2)
                    << std::setfill('0') << (int)c;
        }
        return ss.str();
    }
    if (encoding == "html") {
        std::string r;
        for (char c : payload) {
            if (c=='<') r += "&lt;";
            else if (c=='>') r += "&gt;";
            else if (c=='&') r += "&amp;";
            else if (c=='"') r += "&quot;";
            else r += c;
        }
        return r;
    }
    return payload;
}

std::string WebSecurityTester::decode_payload(const std::string& payload,
                                               const std::string& encoding) {
    (void)encoding;
    return payload; // simplified
}

std::vector<std::string> WebSecurityTester::get_payloads(
        const std::string& vuln_type) const {
    auto it = payloads_.find(vuln_type);
    return it != payloads_.end() ? it->second : std::vector<std::string>{};
}

WebScanResult WebSecurityTester::scan(const std::string& url) {
    WebScanResult r;
    r.target_url     = url;
    r.total_requests = 0;

    // Test common parameters
    std::vector<std::string> params = {"id","user","search","q","name","file","redirect","url"};
    for (auto& param : params) {
        r.total_requests++;
        if (test_sql_injection(url, param)) {
            auto& pl = payloads_["sqli"];
            r.findings.push_back(make_finding(WebVulnType::SQL_INJECTION, url, param,
                                               pl.empty() ? "" : pl[0]));
        }
        r.total_requests++;
        if (test_xss(url, param)) {
            auto& pl = payloads_["xss"];
            r.findings.push_back(make_finding(WebVulnType::XSS, url, param,
                                               pl.empty() ? "" : pl[0]));
        }
        r.total_requests++;
        if (test_path_traversal(url, param)) {
            auto& pl = payloads_["path_traversal"];
            r.findings.push_back(make_finding(WebVulnType::PATH_TRAVERSAL, url, param,
                                               pl.empty() ? "" : pl[0]));
        }
        r.total_requests++;
        if (test_open_redirect(url, param)) {
            auto& pl = payloads_["open_redirect"];
            r.findings.push_back(make_finding(WebVulnType::OPEN_REDIRECT, url, param,
                                               pl.empty() ? "" : pl[0]));
        }
    }
    r.total_requests++;
    if (test_csrf(url))
        r.findings.push_back(make_finding(WebVulnType::CSRF, url, "", ""));
    r.total_requests++;
    if (test_auth_bypass(url))
        r.findings.push_back(make_finding(WebVulnType::AUTH_BYPASS, url, "", ""));

    // Deduplicate by type+param
    std::vector<WebVuln> deduped;
    for (auto& f : r.findings) {
        bool dup = false;
        for (auto& d : deduped)
            if (d.type == f.type && d.parameter == f.parameter) { dup = true; break; }
        if (!dup) deduped.push_back(f);
    }
    r.findings = deduped;
    return r;
}

// ===== MalwareAnalyzer =====

MalwareAnalyzer::MalwareAnalyzer() { init_default_rules(); }

void MalwareAnalyzer::init_default_rules() {
    yara_rules_["ransomware_ext"]  = "\\.encrypted|\\.locked|\\.crypto|\\.ransom";
    yara_rules_["crypto_miner"]    = "stratum\\+tcp|xmrig|minerd|cryptonight";
    yara_rules_["reverse_shell"]   = "bash -i|/dev/tcp|nc -e|ncat.*-e";
    yara_rules_["credential_dump"] = "mimikatz|sekurlsa|lsadump|hashdump";
    yara_rules_["persistence"]     = "HKLM\\\\Run|crontab|systemctl enable|rc\\.local";
    yara_rules_["packer"]          = "UPX!|MPRESS|Themida|ASPack";
}

double MalwareAnalyzer::compute_entropy(const std::string& data) const {
    if (data.empty()) return 0.0;
    std::unordered_map<unsigned char, int> freq;
    for (unsigned char c : data) freq[c]++;
    double entropy = 0.0;
    double len = static_cast<double>(data.size());
    for (auto& [c, cnt] : freq) {
        double p = cnt / len;
        entropy -= p * std::log2(p);
    }
    return entropy;
}

FileType MalwareAnalyzer::detect_file_type(const std::string& hex) const {
    if (hex.size() >= 8) {
        std::string magic = hex.substr(0, 8);
        if (magic == "7f454c46") return FileType::ELF;
        if (magic.substr(0,4) == "4d5a") return FileType::PE;
        if (magic == "cafebabe" || magic == "cefaedfe") return FileType::MACHO;
        if (magic.substr(0,4) == "504b") return FileType::ARCHIVE;
    }
    return FileType::UNKNOWN;
}

std::vector<StringMatch> MalwareAnalyzer::extract_strings(
        const std::string& content) const {
    std::vector<StringMatch> matches;
    // Check for suspicious patterns
    struct Pattern { std::string pat; std::string cat; };
    std::vector<Pattern> patterns = {
        {"http://",  "url"}, {"https://", "url"},
        {"192.168.", "ip"},  {"10.0.",    "ip"},
        {"HKLM\\",  "registry"}, {"HKCU\\", "registry"},
        {"CreateProcess", "api_call"}, {"VirtualAlloc", "api_call"},
        {"WriteProcessMemory", "api_call"}, {"WinExec", "api_call"},
        {"base64",  "crypto"}, {"AES",    "crypto"}, {"RSA", "crypto"},
        {"/etc/passwd", "sensitive"}, {"/etc/shadow", "sensitive"},
        {"password", "credential"}, {"passwd", "credential"},
    };
    size_t offset = 0;
    for (auto& p : patterns) {
        size_t pos = content.find(p.pat);
        if (pos != std::string::npos) {
            StringMatch m;
            m.value    = p.pat;
            m.category = p.cat;
            m.offset   = pos;
            matches.push_back(m);
        }
        offset++;
    }
    return matches;
}

std::string MalwareAnalyzer::threat_level_name(ThreatLevel t) const {
    switch (t) {
        case ThreatLevel::CLEAN:      return "CLEAN";
        case ThreatLevel::SUSPICIOUS: return "SUSPICIOUS";
        case ThreatLevel::MALICIOUS:  return "MALICIOUS";
    }
    return "UNKNOWN";
}

std::string MalwareAnalyzer::file_type_name(FileType t) const {
    switch (t) {
        case FileType::ELF:     return "ELF";
        case FileType::PE:      return "PE";
        case FileType::MACHO:   return "MACHO";
        case FileType::SCRIPT:  return "SCRIPT";
        case FileType::ARCHIVE: return "ARCHIVE";
        default:                return "UNKNOWN";
    }
}

bool MalwareAnalyzer::add_yara_rule(const std::string& name,
                                     const std::string& pattern) {
    yara_rules_[name] = pattern;
    return true;
}

std::string StaticAnalysisResult::to_json() const {
    std::ostringstream ss;
    ss << "{\"filename\":\"" << filename
       << "\",\"file_type\":\"" << (file_type==FileType::ELF?"ELF":
                                    file_type==FileType::PE?"PE":"UNKNOWN")
       << "\",\"sha256\":\"" << sha256
       << "\",\"md5\":\"" << md5
       << "\",\"entropy\":" << entropy
       << ",\"threat_level\":\"" << (threat_level==ThreatLevel::MALICIOUS?"MALICIOUS":
                                     threat_level==ThreatLevel::SUSPICIOUS?"SUSPICIOUS":"CLEAN")
       << "\",\"strings\":" << strings_found.size()
       << ",\"suspicious_patterns\":" << suspicious_patterns.size()
       << ",\"yara_matches\":" << yara_matches.size() << "}";
    return ss.str();
}

std::string DynamicAnalysisResult::to_json() const {
    std::ostringstream ss;
    ss << "{\"filename\":\"" << filename
       << "\",\"threat_level\":\"" << (threat_level==ThreatLevel::MALICIOUS?"MALICIOUS":
                                       threat_level==ThreatLevel::SUSPICIOUS?"SUSPICIOUS":"CLEAN")
       << "\",\"events\":" << events.size()
       << ",\"network_iocs\":" << network_iocs.size()
       << ",\"file_iocs\":" << file_iocs.size() << "}";
    return ss.str();
}

StaticAnalysisResult MalwareAnalyzer::static_analysis(
        const std::string& filepath, const std::string& content_hex) {
    StaticAnalysisResult r;
    r.filename  = filepath;
    r.file_size = content_hex.size() / 2;
    r.sha256    = Crypto::sha256(content_hex.empty() ? filepath : content_hex);
    r.md5       = Crypto::md5(content_hex.empty() ? filepath : content_hex);
    r.file_type = detect_file_type(content_hex);
    r.entropy   = compute_entropy(content_hex.empty() ? filepath : content_hex);

    // Extract strings
    r.strings_found = extract_strings(content_hex.empty() ? filepath : content_hex);

    // Check YARA rules
    for (auto& [name, pattern] : yara_rules_) {
        if ((content_hex + filepath).find(pattern.substr(0, pattern.find('|')))
                != std::string::npos) {
            r.yara_matches.push_back(name);
            r.suspicious_patterns.push_back("YARA:" + name);
        }
    }

    // Determine threat level
    if (!r.yara_matches.empty() || r.entropy > 7.2)
        r.threat_level = ThreatLevel::MALICIOUS;
    else if (r.strings_found.size() > 3 || r.entropy > 6.5)
        r.threat_level = ThreatLevel::SUSPICIOUS;
    else
        r.threat_level = ThreatLevel::CLEAN;

    return r;
}

DynamicAnalysisResult MalwareAnalyzer::dynamic_analysis(
        const std::string& filepath, int timeout_ms) {
    (void)timeout_ms;
    DynamicAnalysisResult r;
    r.filename = filepath;

    // Simulate sandbox behavior based on filename
    bool is_suspicious = filepath.find("mal") != std::string::npos
                      || filepath.find("virus") != std::string::npos
                      || filepath.find("trojan") != std::string::npos;

    if (is_suspicious) {
        r.events = {
            {"file_write",      "/tmp/.hidden_payload",    "Dropped hidden file",       100},
            {"network_connect", "185.220.101.45:4444",     "C2 connection attempt",     250},
            {"process_spawn",   "/bin/sh -c 'id'",         "Shell execution",           300},
            {"registry_write",  "HKCU\\Run\\Updater",      "Persistence mechanism",     400},
            {"file_read",       "/etc/passwd",             "Credential file access",    500},
        };
        r.network_iocs = {"185.220.101.45", "evil-c2.example.com"};
        r.file_iocs    = {"/tmp/.hidden_payload", "/tmp/.backdoor"};
        r.threat_level = ThreatLevel::MALICIOUS;
    } else {
        r.events = {
            {"file_read",  filepath,    "Normal file read",  50},
            {"file_write", "/tmp/log",  "Log write",        100},
        };
        r.threat_level = ThreatLevel::CLEAN;
    }
    return r;
}

// ===== CTFChallenge =====

std::string CTFChallenge::to_string() const {
    return "{\"id\":\"" + id + "\",\"name\":\"" + name
         + "\",\"points\":" + std::to_string(points)
         + ",\"status\":\"" + (status==ChallengeStatus::SOLVED?"solved":"available") + "\"}";
}

bool CTFSession::submit_flag(CTFChallenge& ch, const std::string& flag_attempt) {
    if (ch.status == ChallengeStatus::SOLVED) return false;
    // Compare against hashed flag
    std::string attempt_hash = Crypto::sha256(flag_attempt);
    if (attempt_hash == ch.flag || flag_attempt == ch.flag) {
        ch.status = ChallengeStatus::SOLVED;
        solved_ids.push_back(ch.id);
        score += ch.points;
        return true;
    }
    return false;
}

bool CTFSession::is_solved(const std::string& challenge_id) const {
    for (auto& id : solved_ids) if (id == challenge_id) return true;
    return false;
}

std::string CTFSession::scoreboard_entry() const {
    return "{\"player\":\"" + player + "\",\"score\":" + std::to_string(score)
         + ",\"solved\":" + std::to_string(solved_ids.size()) + "}";
}

// ===== EducationalPlatform =====

EducationalPlatform::EducationalPlatform() {
    init_default_challenges();
    init_tutorials();
}

void EducationalPlatform::init_default_challenges() {
    challenges_ = {
        {"web-001","SQL Injection Basics",ChallengeCategory::WEB,100,
         "Find the SQL injection vulnerability in the login form. Payload: ' OR '1'='1",
         Crypto::sha256("FLAG{sql_injection_master}"),
         "Try single quote in the username field",
         ChallengeStatus::AVAILABLE},
        {"web-002","XSS Reflected",ChallengeCategory::WEB,150,
         "Exploit the reflected XSS in the search parameter",
         Crypto::sha256("FLAG{xss_reflected_pwned}"),
         "The search parameter is reflected without encoding",
         ChallengeStatus::AVAILABLE},
        {"web-003","CSRF Token Bypass",ChallengeCategory::WEB,200,
         "Bypass the CSRF protection on the transfer endpoint",
         Crypto::sha256("FLAG{csrf_bypass_success}"),
         "Check if the token is validated server-side",
         ChallengeStatus::AVAILABLE},
        {"crypto-001","Caesar Cipher",ChallengeCategory::CRYPTO,50,
         "Decrypt: IODJ{fdhvdu_flskhu_hdvb}",
         Crypto::sha256("FLAG{caesar_cipher_easy}"),
         "ROT-3 cipher",
         ChallengeStatus::AVAILABLE},
        {"crypto-002","Base64 Decode",ChallengeCategory::CRYPTO,75,
         "Decode: RkxBR3tiYXNlNjRfaXNfbm90X2VuY3J5cHRpb259",
         Crypto::sha256("FLAG{base64_is_not_encryption}"),
         "Standard Base64 encoding",
         ChallengeStatus::AVAILABLE},
        {"crypto-003","Hash Cracking",ChallengeCategory::CRYPTO,200,
         "Crack this MD5: 5f4dcc3b5aa765d61d8327deb882cf99",
         Crypto::sha256("FLAG{password_cracked}"),
         "Common password list",
         ChallengeStatus::AVAILABLE},
        {"forensics-001","File Carving",ChallengeCategory::FORENSICS,150,
         "Extract the hidden file from the disk image",
         Crypto::sha256("FLAG{file_carving_expert}"),
         "Look for magic bytes",
         ChallengeStatus::AVAILABLE},
        {"re-001","Reverse Me",ChallengeCategory::REVERSE_ENGINEERING,250,
         "Reverse engineer the binary to find the password",
         Crypto::sha256("FLAG{reverse_engineering_pro}"),
         "Use strings or ltrace",
         ChallengeStatus::AVAILABLE},
        {"pwn-001","Buffer Overflow",ChallengeCategory::PWN,300,
         "Exploit the stack buffer overflow to get a shell",
         Crypto::sha256("FLAG{buffer_overflow_king}"),
         "Check the vulnerable function with GDB",
         ChallengeStatus::AVAILABLE},
        {"misc-001","Steganography",ChallengeCategory::MISC,100,
         "Find the hidden message in the image",
         Crypto::sha256("FLAG{stego_hidden_message}"),
         "Try LSB steganography tools",
         ChallengeStatus::AVAILABLE},
    };
}

void EducationalPlatform::init_tutorials() {
    tutorials_["sql_injection"] =
        "SQL Injection Tutorial:\n"
        "1. SQL injection occurs when user input is concatenated into SQL queries\n"
        "2. Basic test: add ' to any input field\n"
        "3. Classic payload: ' OR '1'='1\n"
        "4. Prevention: Use parameterized queries/prepared statements\n"
        "5. Tools: sqlmap, manual testing\n"
        "OWASP Reference: A03:2021 - Injection";

    tutorials_["xss"] =
        "Cross-Site Scripting (XSS) Tutorial:\n"
        "1. XSS injects malicious scripts into web pages\n"
        "2. Types: Reflected, Stored, DOM-based\n"
        "3. Basic test: <script>alert(1)</script>\n"
        "4. Prevention: Output encoding, CSP headers\n"
        "5. Tools: XSStrike, Burp Suite\n"
        "OWASP Reference: A03:2021 - Injection";

    tutorials_["buffer_overflow"] =
        "Buffer Overflow Tutorial:\n"
        "1. Occurs when program writes beyond allocated buffer\n"
        "2. Can overwrite return address to control execution\n"
        "3. Tools: GDB, pwndbg, pwntools\n"
        "4. Find offset: pattern_create/pattern_offset\n"
        "5. Prevention: Stack canaries, ASLR, NX bit\n"
        "Technique: ret2libc, ROP chains";

    tutorials_["cryptography"] =
        "Cryptography Basics Tutorial:\n"
        "1. Symmetric: AES, DES (same key encrypt/decrypt)\n"
        "2. Asymmetric: RSA, ECDSA (public/private key pair)\n"
        "3. Hashing: SHA-256, MD5 (one-way)\n"
        "4. Common mistakes: ECB mode, weak keys, IV reuse\n"
        "5. Tools: CyberChef, hashcat, john the ripper";

    tutorials_["network_scanning"] =
        "Network Scanning Tutorial:\n"
        "1. Port scanning identifies open services\n"
        "2. TCP SYN scan: stealthy, most common\n"
        "3. Service detection: banner grabbing\n"
        "4. Tools: nmap, masscan, zmap\n"
        "5. Common ports: 22(SSH), 80(HTTP), 443(HTTPS), 3306(MySQL)\n"
        "Legal: Only scan systems you own or have permission to test";
}

void EducationalPlatform::add_challenge(const CTFChallenge& ch) {
    challenges_.push_back(ch);
}

CTFChallenge* EducationalPlatform::get_challenge(const std::string& id) {
    for (auto& ch : challenges_) if (ch.id == id) return &ch;
    return nullptr;
}

std::vector<CTFChallenge*> EducationalPlatform::list_challenges(ChallengeCategory cat) {
    std::vector<CTFChallenge*> result;
    for (auto& ch : challenges_) if (ch.category == cat) result.push_back(&ch);
    return result;
}

std::vector<CTFChallenge*> EducationalPlatform::all_challenges() {
    std::vector<CTFChallenge*> result;
    for (auto& ch : challenges_) result.push_back(&ch);
    return result;
}

CTFSession EducationalPlatform::create_session(const std::string& player) {
    CTFSession s;
    s.player = player;
    s.score  = 0;
    sessions_.push_back(s);
    return s;
}

std::string EducationalPlatform::leaderboard() const {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < sessions_.size(); i++) {
        if (i) ss << ",";
        ss << sessions_[i].scoreboard_entry();
    }
    ss << "]";
    return ss.str();
}

std::string EducationalPlatform::get_tutorial(const std::string& topic) const {
    auto it = tutorials_.find(topic);
    return it != tutorials_.end() ? it->second : "Tutorial not found: " + topic;
}

} // namespace Security
} // namespace Sapphire
