#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace Sapphire {
namespace Ecosystem {

// ===== Semver =====
struct Version {
    int major, minor, patch;
    std::string pre;   // pre-release label e.g. "alpha.1"
    std::string build; // build metadata
};

Version    semver_parse(const std::string& s);
std::string semver_to_string(const Version& v);
int        semver_compare(const Version& a, const Version& b); // -1 0 1
bool       semver_satisfies(const Version& v, const std::string& range); // "^1.2", "~1.2", ">=1.0 <2.0"
Version    semver_bump_major(const Version& v);
Version    semver_bump_minor(const Version& v);
Version    semver_bump_patch(const Version& v);
bool       semver_is_valid(const std::string& s);
bool       semver_is_prerelease(const Version& v);

// ===== Package Manifest =====
struct Dependency {
    std::string name;
    std::string version_range;
};

struct Manifest {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string entry;
    std::vector<Dependency> dependencies;
    std::vector<std::string> keywords;
};

Manifest manifest_create(const std::string& name, const std::string& version);
void     manifest_set_description(Manifest& m, const std::string& desc);
void     manifest_set_author(Manifest& m, const std::string& author);
void     manifest_set_license(Manifest& m, const std::string& lic);
void     manifest_add_dep(Manifest& m, const std::string& name, const std::string& range);
void     manifest_add_keyword(Manifest& m, const std::string& kw);
std::string manifest_to_toml(const Manifest& m);
Manifest manifest_from_toml(const std::string& toml);

// ===== Package Registry (in-memory simulation) =====
struct PackageInfo {
    std::string name;
    std::string latest_version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> versions;
    int downloads;
};

void        registry_reset();
void        registry_publish(const PackageInfo& pkg);
bool        registry_exists(const std::string& name);
PackageInfo registry_get(const std::string& name);
std::vector<PackageInfo> registry_search(const std::string& query);
std::vector<std::string> registry_list_all();
int         registry_package_count();

// ===== Package Manager =====
struct InstallResult {
    bool success;
    std::string message;
    std::vector<std::string> installed;
};

struct ResolveResult {
    bool success;
    std::string message;
    std::vector<Dependency> resolved; // flattened dep tree
};

void          pkgmgr_reset();
InstallResult pkgmgr_install(const std::string& name, const std::string& version_range);
InstallResult pkgmgr_install_manifest(const Manifest& m);
bool          pkgmgr_is_installed(const std::string& name);
std::string   pkgmgr_installed_version(const std::string& name);
std::vector<std::string> pkgmgr_list_installed();
bool          pkgmgr_uninstall(const std::string& name);
ResolveResult pkgmgr_resolve(const Manifest& m);
std::string   pkgmgr_lockfile(const Manifest& m);

// ===== Template Engine =====
// Simple {{variable}} substitution + {{#if var}}...{{/if}} + {{#each list}}...{{/each}}
std::string tmpl_render(const std::string& tmpl, const std::map<std::string, std::string>& vars);
std::string tmpl_render_list(const std::string& tmpl, const std::string& key,
                              const std::vector<std::string>& items,
                              const std::map<std::string, std::string>& vars);

// ===== Code Formatter =====
struct FormatOptions {
    int indent_size = 4;
    bool use_spaces = true;
    int max_line_length = 100;
    bool trailing_newline = true;
};

std::string fmt_indent(const std::string& code, int spaces);
std::string fmt_trim_trailing_whitespace(const std::string& code);
std::string fmt_normalize_newlines(const std::string& code);
std::string fmt_wrap_lines(const std::string& code, int max_len);
int         fmt_count_lines(const std::string& code);
int         fmt_count_chars(const std::string& code);
std::string fmt_apply(const std::string& code, const FormatOptions& opts);

} // namespace Ecosystem
} // namespace Sapphire

// ===== C API =====
extern "C" {

// Semver
void* eco_semver_parse(const char* s);
void  eco_semver_destroy(void* v);
int   eco_semver_major(void* v);
int   eco_semver_minor(void* v);
int   eco_semver_patch(void* v);
const char* eco_semver_pre(void* v);
const char* eco_semver_to_string(void* v);
int   eco_semver_compare(void* a, void* b);
int   eco_semver_satisfies(void* v, const char* range);
void* eco_semver_bump_major(void* v);
void* eco_semver_bump_minor(void* v);
void* eco_semver_bump_patch(void* v);
int   eco_semver_is_valid(const char* s);
int   eco_semver_is_prerelease(void* v);

// Manifest
void* eco_manifest_create(const char* name, const char* version);
void  eco_manifest_destroy(void* m);
void  eco_manifest_set_description(void* m, const char* desc);
void  eco_manifest_set_author(void* m, const char* author);
void  eco_manifest_set_license(void* m, const char* lic);
void  eco_manifest_add_dep(void* m, const char* name, const char* range);
void  eco_manifest_add_keyword(void* m, const char* kw);
const char* eco_manifest_name(void* m);
const char* eco_manifest_version(void* m);
const char* eco_manifest_description(void* m);
int   eco_manifest_dep_count(void* m);
const char* eco_manifest_dep_name(void* m, int i);
const char* eco_manifest_dep_range(void* m, int i);
const char* eco_manifest_to_toml(void* m);

// Registry
void  eco_registry_reset();
void* eco_registry_publish(const char* name, const char* version, const char* desc,
                            const char* author, const char* license, int downloads);
void  eco_registry_pkg_destroy(void* p);
int   eco_registry_exists(const char* name);
void* eco_registry_get(const char* name);
const char* eco_registry_pkg_name(void* p);
const char* eco_registry_pkg_latest(void* p);
const char* eco_registry_pkg_description(void* p);
const char* eco_registry_pkg_author(void* p);
int   eco_registry_pkg_downloads(void* p);
int   eco_registry_package_count();
void* eco_registry_search(const char* query); // returns StrVec
void* eco_registry_list_all();                // returns StrVec

// StrVec helpers
void  eco_strvec_destroy(void* v);
int   eco_strvec_count(void* v);
const char* eco_strvec_get(void* v, int i);

// Package manager
void  eco_pkgmgr_reset();
int   eco_pkgmgr_install(const char* name, const char* version_range);
const char* eco_pkgmgr_install_message();
int   eco_pkgmgr_install_manifest(void* manifest);
int   eco_pkgmgr_is_installed(const char* name);
const char* eco_pkgmgr_installed_version(const char* name);
void* eco_pkgmgr_list_installed();  // returns StrVec
int   eco_pkgmgr_uninstall(const char* name);
const char* eco_pkgmgr_lockfile(void* manifest);

// Template engine
const char* eco_tmpl_render(const char* tmpl, void* vars_map);
void* eco_tmpl_vars_create();
void  eco_tmpl_vars_destroy(void* m);
void  eco_tmpl_vars_set(void* m, const char* key, const char* value);

// Formatter
const char* eco_fmt_indent(const char* code, int spaces);
const char* eco_fmt_trim(const char* code);
const char* eco_fmt_normalize(const char* code);
int   eco_fmt_count_lines(const char* code);
int   eco_fmt_count_chars(const char* code);

} // extern "C"
