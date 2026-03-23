// ecosystem.cpp - M18 Community & Ecosystem for Sapphire
#include "ecosystem.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <stdexcept>
#include <cctype>
#include <numeric>

namespace Sapphire {
namespace Ecosystem {

// ===== Semver =====

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

Version semver_parse(const std::string& raw) {
    Version v{0,0,0,"",""};
    std::string s = trim(raw);
    // strip leading 'v'
    if (!s.empty() && (s[0] == 'v' || s[0] == 'V')) s = s.substr(1);
    // split build metadata
    auto plus = s.find('+');
    if (plus != std::string::npos) { v.build = s.substr(plus+1); s = s.substr(0, plus); }
    // split pre-release
    auto dash = s.find('-');
    if (dash != std::string::npos) { v.pre = s.substr(dash+1); s = s.substr(0, dash); }
    // parse major.minor.patch
    std::istringstream ss(s);
    char dot;
    ss >> v.major >> dot >> v.minor >> dot >> v.patch;
    return v;
}

std::string semver_to_string(const Version& v) {
    std::string s = std::to_string(v.major) + "." + std::to_string(v.minor) + "." + std::to_string(v.patch);
    if (!v.pre.empty()) s += "-" + v.pre;
    if (!v.build.empty()) s += "+" + v.build;
    return s;
}

int semver_compare(const Version& a, const Version& b) {
    if (a.major != b.major) return a.major < b.major ? -1 : 1;
    if (a.minor != b.minor) return a.minor < b.minor ? -1 : 1;
    if (a.patch != b.patch) return a.patch < b.patch ? -1 : 1;
    // pre-release: no pre > has pre
    if (a.pre.empty() && !b.pre.empty()) return 1;
    if (!a.pre.empty() && b.pre.empty()) return -1;
    if (a.pre != b.pre) return a.pre < b.pre ? -1 : 1;
    return 0;
}

bool semver_satisfies(const Version& v, const std::string& range) {
    std::string r = trim(range);
    if (r.empty() || r == "*") return true;
    // Handle ^major.minor.patch — compatible with major
    if (r[0] == '^') {
        Version base = semver_parse(r.substr(1));
        Version upper{base.major + 1, 0, 0, "", ""};
        return semver_compare(v, base) >= 0 && semver_compare(v, upper) < 0;
    }
    // Handle ~major.minor.patch — compatible with minor
    if (r[0] == '~') {
        Version base = semver_parse(r.substr(1));
        Version upper{base.major, base.minor + 1, 0, "", ""};
        return semver_compare(v, base) >= 0 && semver_compare(v, upper) < 0;
    }
    // Handle >= <= > < =
    auto parse_op = [&](const std::string& expr) -> bool {
        std::string op, ver_str;
        size_t i = 0;
        while (i < expr.size() && (expr[i] == '>' || expr[i] == '<' || expr[i] == '=' || expr[i] == '!')) op += expr[i++];
        while (i < expr.size() && expr[i] == ' ') i++;
        ver_str = expr.substr(i);
        Version base = semver_parse(ver_str);
        int cmp = semver_compare(v, base);
        if (op == ">=" || op == "=>") return cmp >= 0;
        if (op == "<=" || op == "=<") return cmp <= 0;
        if (op == ">")  return cmp > 0;
        if (op == "<")  return cmp < 0;
        if (op == "=" || op == "==") return cmp == 0;
        if (op == "!=") return cmp != 0;
        return semver_compare(v, semver_parse(expr)) == 0;
    };
    // Handle space-separated AND ranges
    std::istringstream ss(r);
    std::string part;
    while (ss >> part) {
        if (!parse_op(part)) return false;
    }
    return true;
}

Version semver_bump_major(const Version& v) { return {v.major+1, 0, 0, "", ""}; }
Version semver_bump_minor(const Version& v) { return {v.major, v.minor+1, 0, "", ""}; }
Version semver_bump_patch(const Version& v) { return {v.major, v.minor, v.patch+1, "", ""}; }

bool semver_is_valid(const std::string& s) {
    try {
        std::string t = trim(s);
        if (!t.empty() && (t[0]=='v'||t[0]=='V')) t=t.substr(1);
        // strip pre/build
        auto p = t.find('+'); if (p!=std::string::npos) t=t.substr(0,p);
        p = t.find('-'); if (p!=std::string::npos) t=t.substr(0,p);
        int ma,mi,pa; char d1,d2;
        std::istringstream ss(t);
        if (!(ss >> ma >> d1 >> mi >> d2 >> pa)) return false;
        return d1=='.' && d2=='.' && ma>=0 && mi>=0 && pa>=0;
    } catch(...) { return false; }
}

bool semver_is_prerelease(const Version& v) { return !v.pre.empty(); }

// ===== Manifest =====

Manifest manifest_create(const std::string& name, const std::string& version) {
    Manifest m;
    m.name = name; m.version = version;
    m.license = "MIT";
    return m;
}

void manifest_set_description(Manifest& m, const std::string& desc) { m.description = desc; }
void manifest_set_author(Manifest& m, const std::string& author) { m.author = author; }
void manifest_set_license(Manifest& m, const std::string& lic) { m.license = lic; }
void manifest_add_dep(Manifest& m, const std::string& name, const std::string& range) {
    m.dependencies.push_back({name, range});
}
void manifest_add_keyword(Manifest& m, const std::string& kw) { m.keywords.push_back(kw); }

std::string manifest_to_toml(const Manifest& m) {
    std::ostringstream oss;
    oss << "[package]\n"
        << "name = \"" << m.name << "\"\n"
        << "version = \"" << m.version << "\"\n";
    if (!m.description.empty()) oss << "description = \"" << m.description << "\"\n";
    if (!m.author.empty())      oss << "author = \"" << m.author << "\"\n";
    if (!m.license.empty())     oss << "license = \"" << m.license << "\"\n";
    if (!m.entry.empty())       oss << "entry = \"" << m.entry << "\"\n";
    if (!m.keywords.empty()) {
        oss << "keywords = [";
        for (size_t i = 0; i < m.keywords.size(); i++) {
            if (i) oss << ", ";
            oss << "\"" << m.keywords[i] << "\"";
        }
        oss << "]\n";
    }
    if (!m.dependencies.empty()) {
        oss << "\n[dependencies]\n";
        for (const auto& d : m.dependencies)
            oss << d.name << " = \"" << d.version_range << "\"\n";
    }
    return oss.str();
}

Manifest manifest_from_toml(const std::string& toml) {
    Manifest m;
    std::istringstream ss(toml);
    std::string line;
    bool in_deps = false;
    while (std::getline(ss, line)) {
        line = trim(line);
        if (line == "[dependencies]") { in_deps = true; continue; }
        if (!line.empty() && line[0] == '[') { in_deps = false; continue; }
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq+1));
        // strip quotes
        if (val.size() >= 2 && val.front()=='"' && val.back()=='"')
            val = val.substr(1, val.size()-2);
        if (in_deps) { m.dependencies.push_back({key, val}); continue; }
        if (key == "name") m.name = val;
        else if (key == "version") m.version = val;
        else if (key == "description") m.description = val;
        else if (key == "author") m.author = val;
        else if (key == "license") m.license = val;
        else if (key == "entry") m.entry = val;
    }
    return m;
}

// ===== Registry =====

static std::map<std::string, PackageInfo> g_registry;

void registry_reset() { g_registry.clear(); }

void registry_publish(const PackageInfo& pkg) {
    auto it = g_registry.find(pkg.name);
    if (it != g_registry.end()) {
        it->second.versions.push_back(pkg.latest_version);
        it->second.latest_version = pkg.latest_version;
        it->second.downloads += pkg.downloads;
    } else {
        PackageInfo p = pkg;
        if (p.versions.empty()) p.versions.push_back(p.latest_version);
        g_registry[pkg.name] = p;
    }
}

bool registry_exists(const std::string& name) { return g_registry.count(name) > 0; }

PackageInfo registry_get(const std::string& name) {
    auto it = g_registry.find(name);
    if (it == g_registry.end()) return {};
    return it->second;
}

std::vector<PackageInfo> registry_search(const std::string& query) {
    std::vector<PackageInfo> results;
    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);
    for (const auto& [name, pkg] : g_registry) {
        std::string n = name, d = pkg.description;
        std::transform(n.begin(), n.end(), n.begin(), ::tolower);
        std::transform(d.begin(), d.end(), d.begin(), ::tolower);
        if (n.find(q) != std::string::npos || d.find(q) != std::string::npos)
            results.push_back(pkg);
    }
    return results;
}

std::vector<std::string> registry_list_all() {
    std::vector<std::string> names;
    for (const auto& [name, _] : g_registry) names.push_back(name);
    return names;
}

int registry_package_count() { return (int)g_registry.size(); }

// ===== Package Manager =====

static std::map<std::string, std::string> g_installed; // name -> version
static std::string g_last_install_msg;

void pkgmgr_reset() { g_installed.clear(); }

InstallResult pkgmgr_install(const std::string& name, const std::string& version_range) {
    InstallResult r;
    if (!registry_exists(name)) {
        r.success = false;
        r.message = "Package '" + name + "' not found in registry";
        g_last_install_msg = r.message;
        return r;
    }
    auto pkg = registry_get(name);
    // Find best matching version
    std::string best;
    for (const auto& ver : pkg.versions) {
        Version v = semver_parse(ver);
        if (semver_satisfies(v, version_range)) {
            if (best.empty() || semver_compare(v, semver_parse(best)) > 0)
                best = ver;
        }
    }
    if (best.empty()) best = pkg.latest_version;
    g_installed[name] = best;
    r.success = true;
    r.message = "Installed " + name + "@" + best;
    r.installed.push_back(name + "@" + best);
    g_last_install_msg = r.message;
    return r;
}

InstallResult pkgmgr_install_manifest(const Manifest& m) {
    InstallResult r;
    r.success = true;
    for (const auto& dep : m.dependencies) {
        auto sub = pkgmgr_install(dep.name, dep.version_range);
        if (!sub.success) { r.success = false; r.message += sub.message + "; "; }
        else for (const auto& s : sub.installed) r.installed.push_back(s);
    }
    if (r.success) r.message = "All dependencies installed";
    g_last_install_msg = r.message;
    return r;
}

bool pkgmgr_is_installed(const std::string& name) { return g_installed.count(name) > 0; }

std::string pkgmgr_installed_version(const std::string& name) {
    auto it = g_installed.find(name);
    return it != g_installed.end() ? it->second : "";
}

std::vector<std::string> pkgmgr_list_installed() {
    std::vector<std::string> list;
    for (const auto& [name, ver] : g_installed) list.push_back(name + "@" + ver);
    return list;
}

bool pkgmgr_uninstall(const std::string& name) {
    return g_installed.erase(name) > 0;
}

ResolveResult pkgmgr_resolve(const Manifest& m) {
    ResolveResult r;
    r.success = true;
    for (const auto& dep : m.dependencies) {
        if (!registry_exists(dep.name)) {
            r.success = false;
            r.message = "Cannot resolve: " + dep.name + " not in registry";
            return r;
        }
        r.resolved.push_back(dep);
    }
    r.message = "Resolved " + std::to_string(r.resolved.size()) + " dependencies";
    return r;
}

std::string pkgmgr_lockfile(const Manifest& m) {
    std::ostringstream oss;
    oss << "# Sapphire lockfile\n# Generated for " << m.name << "@" << m.version << "\n\n";
    for (const auto& dep : m.dependencies) {
        std::string ver = pkgmgr_installed_version(dep.name);
        if (ver.empty()) ver = dep.version_range;
        oss << "[[package]]\n"
            << "name = \"" << dep.name << "\"\n"
            << "version = \"" << ver << "\"\n"
            << "resolved = \"https://registry.sapphire-lang.org/" << dep.name << "/" << ver << "\"\n\n";
    }
    return oss.str();
}

// ===== Template Engine =====

std::string tmpl_render(const std::string& tmpl, const std::map<std::string, std::string>& vars) {
    std::string result = tmpl;
    // Replace {{var}}
    for (const auto& [key, val] : vars) {
        std::string token = "{{" + key + "}}";
        size_t pos = 0;
        while ((pos = result.find(token, pos)) != std::string::npos) {
            result.replace(pos, token.size(), val);
            pos += val.size();
        }
    }
    // Handle {{#if var}}...{{/if}}
    size_t pos = 0;
    while ((pos = result.find("{{#if ")) != std::string::npos) {
        size_t end_tag = result.find("}}", pos);
        if (end_tag == std::string::npos) break;
        std::string var_name = result.substr(pos + 6, end_tag - pos - 6);
        std::string close = "{{/if}}";
        size_t close_pos = result.find(close, end_tag);
        if (close_pos == std::string::npos) break;
        std::string inner = result.substr(end_tag + 2, close_pos - end_tag - 2);
        bool cond = vars.count(var_name) && !vars.at(var_name).empty() && vars.at(var_name) != "false" && vars.at(var_name) != "0";
        result.replace(pos, close_pos + close.size() - pos, cond ? inner : "");
    }
    return result;
}

std::string tmpl_render_list(const std::string& tmpl, const std::string& key,
                              const std::vector<std::string>& items,
                              const std::map<std::string, std::string>& vars) {
    std::string open = "{{#each " + key + "}}";
    std::string close = "{{/each}}";
    std::string result = tmpl;
    size_t pos = result.find(open);
    if (pos == std::string::npos) return tmpl_render(result, vars);
    size_t close_pos = result.find(close, pos);
    if (close_pos == std::string::npos) return tmpl_render(result, vars);
    std::string inner = result.substr(pos + open.size(), close_pos - pos - open.size());
    std::string expanded;
    for (const auto& item : items) {
        std::string part = inner;
        // Replace {{this}} with item
        size_t p = 0;
        while ((p = part.find("{{this}}", p)) != std::string::npos)
            part.replace(p, 8, item), p += item.size();
        expanded += part;
    }
    result.replace(pos, close_pos + close.size() - pos, expanded);
    return tmpl_render(result, vars);
}

// ===== Formatter =====

std::string fmt_indent(const std::string& code, int spaces) {
    std::string pad(spaces, ' ');
    std::istringstream ss(code);
    std::ostringstream out;
    std::string line;
    while (std::getline(ss, line)) out << pad << line << "\n";
    return out.str();
}

std::string fmt_trim_trailing_whitespace(const std::string& code) {
    std::istringstream ss(code);
    std::ostringstream out;
    std::string line;
    while (std::getline(ss, line)) {
        size_t end = line.find_last_not_of(" \t");
        out << (end == std::string::npos ? "" : line.substr(0, end+1)) << "\n";
    }
    return out.str();
}

std::string fmt_normalize_newlines(const std::string& code) {
    std::string r;
    for (size_t i = 0; i < code.size(); i++) {
        if (code[i] == '\r') { r += '\n'; if (i+1 < code.size() && code[i+1]=='\n') i++; }
        else r += code[i];
    }
    return r;
}

std::string fmt_wrap_lines(const std::string& code, int max_len) {
    std::istringstream ss(code);
    std::ostringstream out;
    std::string line;
    while (std::getline(ss, line)) {
        if ((int)line.size() <= max_len) { out << line << "\n"; continue; }
        // Simple word-wrap
        size_t pos = 0;
        while (pos < line.size()) {
            size_t end = std::min(pos + (size_t)max_len, line.size());
            if (end < line.size()) {
                size_t space = line.rfind(' ', end);
                if (space != std::string::npos && space > pos) end = space;
            }
            out << line.substr(pos, end - pos) << "\n";
            pos = end;
            while (pos < line.size() && line[pos] == ' ') pos++;
        }
    }
    return out.str();
}

int fmt_count_lines(const std::string& code) {
    return (int)std::count(code.begin(), code.end(), '\n') + (code.empty() || code.back()=='\n' ? 0 : 1);
}

int fmt_count_chars(const std::string& code) { return (int)code.size(); }

std::string fmt_apply(const std::string& code, const FormatOptions& opts) {
    std::string r = fmt_normalize_newlines(code);
    r = fmt_trim_trailing_whitespace(r);
    if (opts.max_line_length > 0) r = fmt_wrap_lines(r, opts.max_line_length);
    if (opts.trailing_newline && !r.empty() && r.back() != '\n') r += '\n';
    return r;
}

} // namespace Ecosystem
} // namespace Sapphire
