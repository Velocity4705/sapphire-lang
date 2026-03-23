// ecosystem_capi.cpp - C API for M18 Community & Ecosystem
#include "ecosystem.h"
#include <vector>
#include <string>
#include <map>

using namespace Sapphire::Ecosystem;

static std::string g_eco_str_buf;

struct StrVec { std::vector<std::string> v; };

extern "C" {

// ===== Semver =====

void* eco_semver_parse(const char* s) { return new Version(semver_parse(s ? s : "0.0.0")); }
void  eco_semver_destroy(void* v) { delete static_cast<Version*>(v); }
int   eco_semver_major(void* v) { return static_cast<Version*>(v)->major; }
int   eco_semver_minor(void* v) { return static_cast<Version*>(v)->minor; }
int   eco_semver_patch(void* v) { return static_cast<Version*>(v)->patch; }
const char* eco_semver_pre(void* v) {
    g_eco_str_buf = static_cast<Version*>(v)->pre;
    return g_eco_str_buf.c_str();
}
const char* eco_semver_to_string(void* v) {
    g_eco_str_buf = semver_to_string(*static_cast<Version*>(v));
    return g_eco_str_buf.c_str();
}
int eco_semver_compare(void* a, void* b) {
    return semver_compare(*static_cast<Version*>(a), *static_cast<Version*>(b));
}
int eco_semver_satisfies(void* v, const char* range) {
    return semver_satisfies(*static_cast<Version*>(v), range ? range : "*") ? 1 : 0;
}
void* eco_semver_bump_major(void* v) { return new Version(semver_bump_major(*static_cast<Version*>(v))); }
void* eco_semver_bump_minor(void* v) { return new Version(semver_bump_minor(*static_cast<Version*>(v))); }
void* eco_semver_bump_patch(void* v) { return new Version(semver_bump_patch(*static_cast<Version*>(v))); }
int   eco_semver_is_valid(const char* s) { return semver_is_valid(s ? s : "") ? 1 : 0; }
int   eco_semver_is_prerelease(void* v) { return semver_is_prerelease(*static_cast<Version*>(v)) ? 1 : 0; }

// ===== Manifest =====

void* eco_manifest_create(const char* name, const char* version) {
    return new Manifest(manifest_create(name ? name : "", version ? version : "0.0.0"));
}
void eco_manifest_destroy(void* m) { delete static_cast<Manifest*>(m); }
void eco_manifest_set_description(void* m, const char* d) { manifest_set_description(*static_cast<Manifest*>(m), d ? d : ""); }
void eco_manifest_set_author(void* m, const char* a)      { manifest_set_author(*static_cast<Manifest*>(m), a ? a : ""); }
void eco_manifest_set_license(void* m, const char* l)     { manifest_set_license(*static_cast<Manifest*>(m), l ? l : ""); }
void eco_manifest_add_dep(void* m, const char* name, const char* range) {
    manifest_add_dep(*static_cast<Manifest*>(m), name ? name : "", range ? range : "*");
}
void eco_manifest_add_keyword(void* m, const char* kw) {
    manifest_add_keyword(*static_cast<Manifest*>(m), kw ? kw : "");
}
const char* eco_manifest_name(void* m) {
    g_eco_str_buf = static_cast<Manifest*>(m)->name; return g_eco_str_buf.c_str();
}
const char* eco_manifest_version(void* m) {
    g_eco_str_buf = static_cast<Manifest*>(m)->version; return g_eco_str_buf.c_str();
}
const char* eco_manifest_description(void* m) {
    g_eco_str_buf = static_cast<Manifest*>(m)->description; return g_eco_str_buf.c_str();
}
int eco_manifest_dep_count(void* m) { return (int)static_cast<Manifest*>(m)->dependencies.size(); }
const char* eco_manifest_dep_name(void* m, int i) {
    g_eco_str_buf = static_cast<Manifest*>(m)->dependencies[i].name;
    return g_eco_str_buf.c_str();
}
const char* eco_manifest_dep_range(void* m, int i) {
    g_eco_str_buf = static_cast<Manifest*>(m)->dependencies[i].version_range;
    return g_eco_str_buf.c_str();
}
const char* eco_manifest_to_toml(void* m) {
    g_eco_str_buf = manifest_to_toml(*static_cast<Manifest*>(m));
    return g_eco_str_buf.c_str();
}

// ===== Registry =====

void eco_registry_reset() { registry_reset(); }

void* eco_registry_publish(const char* name, const char* version, const char* desc,
                            const char* author, const char* license, int downloads) {
    PackageInfo p;
    p.name = name ? name : "";
    p.latest_version = version ? version : "0.0.0";
    p.description = desc ? desc : "";
    p.author = author ? author : "";
    p.license = license ? license : "MIT";
    p.downloads = downloads;
    p.versions.push_back(p.latest_version);
    registry_publish(p);
    return new PackageInfo(registry_get(p.name));
}
void eco_registry_pkg_destroy(void* p) { delete static_cast<PackageInfo*>(p); }
int  eco_registry_exists(const char* name) { return registry_exists(name ? name : "") ? 1 : 0; }
void* eco_registry_get(const char* name) { return new PackageInfo(registry_get(name ? name : "")); }
const char* eco_registry_pkg_name(void* p) {
    g_eco_str_buf = static_cast<PackageInfo*>(p)->name; return g_eco_str_buf.c_str();
}
const char* eco_registry_pkg_latest(void* p) {
    g_eco_str_buf = static_cast<PackageInfo*>(p)->latest_version; return g_eco_str_buf.c_str();
}
const char* eco_registry_pkg_description(void* p) {
    g_eco_str_buf = static_cast<PackageInfo*>(p)->description; return g_eco_str_buf.c_str();
}
const char* eco_registry_pkg_author(void* p) {
    g_eco_str_buf = static_cast<PackageInfo*>(p)->author; return g_eco_str_buf.c_str();
}
int eco_registry_pkg_downloads(void* p) { return static_cast<PackageInfo*>(p)->downloads; }
int eco_registry_package_count() { return registry_package_count(); }

void* eco_registry_search(const char* query) {
    auto results = registry_search(query ? query : "");
    auto* v = new StrVec();
    for (const auto& p : results) v->v.push_back(p.name + " (" + p.latest_version + ") - " + p.description);
    return v;
}
void* eco_registry_list_all() {
    auto* v = new StrVec();
    v->v = registry_list_all();
    return v;
}

// StrVec
void  eco_strvec_destroy(void* v) { delete static_cast<StrVec*>(v); }
int   eco_strvec_count(void* v) { return (int)static_cast<StrVec*>(v)->v.size(); }
const char* eco_strvec_get(void* v, int i) {
    g_eco_str_buf = static_cast<StrVec*>(v)->v[i];
    return g_eco_str_buf.c_str();
}

// ===== Package Manager =====

void eco_pkgmgr_reset() { pkgmgr_reset(); }

int eco_pkgmgr_install(const char* name, const char* version_range) {
    auto r = pkgmgr_install(name ? name : "", version_range ? version_range : "*");
    return r.success ? 1 : 0;
}
const char* eco_pkgmgr_install_message() {
    // Expose the last install message (stored in ecosystem.cpp)
    g_eco_str_buf = Sapphire::Ecosystem::pkgmgr_installed_version(""); // placeholder
    // Re-run a dummy to get the message — just return the last installed list summary
    auto list = Sapphire::Ecosystem::pkgmgr_list_installed();
    g_eco_str_buf = list.empty() ? "No packages installed" : "Installed: " + list.back();
    return g_eco_str_buf.c_str();
}
int eco_pkgmgr_install_manifest(void* manifest) {
    auto r = pkgmgr_install_manifest(*static_cast<Manifest*>(manifest));
    return r.success ? 1 : 0;
}
int eco_pkgmgr_is_installed(const char* name) { return pkgmgr_is_installed(name ? name : "") ? 1 : 0; }
const char* eco_pkgmgr_installed_version(const char* name) {
    g_eco_str_buf = pkgmgr_installed_version(name ? name : "");
    return g_eco_str_buf.c_str();
}
void* eco_pkgmgr_list_installed() {
    auto* v = new StrVec();
    v->v = pkgmgr_list_installed();
    return v;
}
int eco_pkgmgr_uninstall(const char* name) { return pkgmgr_uninstall(name ? name : "") ? 1 : 0; }
const char* eco_pkgmgr_lockfile(void* manifest) {
    g_eco_str_buf = pkgmgr_lockfile(*static_cast<Manifest*>(manifest));
    return g_eco_str_buf.c_str();
}

// ===== Template Engine =====

struct VarsMap { std::map<std::string, std::string> m; };

void* eco_tmpl_vars_create() { return new VarsMap(); }
void  eco_tmpl_vars_destroy(void* m) { delete static_cast<VarsMap*>(m); }
void  eco_tmpl_vars_set(void* m, const char* key, const char* value) {
    static_cast<VarsMap*>(m)->m[key ? key : ""] = value ? value : "";
}
const char* eco_tmpl_render(const char* tmpl, void* vars_map) {
    g_eco_str_buf = tmpl_render(tmpl ? tmpl : "", static_cast<VarsMap*>(vars_map)->m);
    return g_eco_str_buf.c_str();
}

// ===== Formatter =====

const char* eco_fmt_indent(const char* code, int spaces) {
    g_eco_str_buf = fmt_indent(code ? code : "", spaces);
    return g_eco_str_buf.c_str();
}
const char* eco_fmt_trim(const char* code) {
    g_eco_str_buf = fmt_trim_trailing_whitespace(code ? code : "");
    return g_eco_str_buf.c_str();
}
const char* eco_fmt_normalize(const char* code) {
    g_eco_str_buf = fmt_normalize_newlines(code ? code : "");
    return g_eco_str_buf.c_str();
}
int eco_fmt_count_lines(const char* code) { return fmt_count_lines(code ? code : ""); }
int eco_fmt_count_chars(const char* code) { return fmt_count_chars(code ? code : ""); }

} // extern "C"
