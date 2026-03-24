// STB_IMAGE_IMPLEMENTATION is defined in gl3d.cpp
#include "stb_image.h"
#include "texture.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdint>
#include <cstring>

// ── Internal structs ───────────────────────────────────────────────────────

struct TextureHandle {
    int w = 0, h = 0;
    std::vector<uint8_t> pixels;
    int bound_unit = -1;
};

struct SpriteSheet {
    TextureHandle* tex = nullptr;
    int fw = 0, fh = 0, count = 0;
};

struct AtlasRegion { float u0 = 0, v0 = 0, u1 = 0, v1 = 0; };

struct TextureAtlas {
    TextureHandle* tex = nullptr;
    std::unordered_map<std::string, AtlasRegion> regions;
};

// ── Error state ────────────────────────────────────────────────────────────

static std::string s_texture_error;
static std::string s_atlas_error;

// ── Minimal JSON parser for atlas metadata ─────────────────────────────────
// Expects format: {"frames":{"name":{"frame":{"x":0,"y":0,"w":32,"h":32}},...}}
// or flat: {"name":{"x":0,"y":0,"w":32,"h":32},...}
// We do a simple string scan — no full JSON library needed.

static int json_int(const std::string& s, const std::string& key, size_t from) {
    size_t p = s.find("\"" + key + "\"", from);
    if (p == std::string::npos) return -1;
    p = s.find(':', p);
    if (p == std::string::npos) return -1;
    p++;
    while (p < s.size() && (s[p] == ' ' || s[p] == '\t')) p++;
    return std::stoi(s.substr(p));
}

static bool parse_atlas_json(const std::string& json, TextureHandle* tex,
                              std::unordered_map<std::string, AtlasRegion>& out) {
    // Find "frames" object if present, otherwise use root
    size_t start = 0;
    size_t fp = json.find("\"frames\"");
    if (fp != std::string::npos) {
        start = json.find('{', fp + 8);
        if (start == std::string::npos) return false;
        start++; // inside frames object
    } else {
        start = json.find('{');
        if (start == std::string::npos) return false;
        start++;
    }

    float tw = (tex && tex->w > 0) ? (float)tex->w : 1.0f;
    float th = (tex && tex->h > 0) ? (float)tex->h : 1.0f;

    size_t pos = start;
    while (pos < json.size()) {
        // Find next quoted key
        size_t q1 = json.find('"', pos);
        if (q1 == std::string::npos) break;
        size_t q2 = json.find('"', q1 + 1);
        if (q2 == std::string::npos) break;
        std::string name = json.substr(q1 + 1, q2 - q1 - 1);
        pos = q2 + 1;

        // Find the object for this name
        size_t ob = json.find('{', pos);
        if (ob == std::string::npos) break;
        size_t cb = json.find('}', ob);
        if (cb == std::string::npos) break;
        std::string obj = json.substr(ob, cb - ob + 1);

        // Look for x, y, w, h inside (possibly nested in "frame":{...})
        size_t inner_start = 0;
        size_t fp2 = obj.find("\"frame\"");
        if (fp2 != std::string::npos) {
            size_t ob2 = obj.find('{', fp2 + 7);
            if (ob2 != std::string::npos) inner_start = ob2;
        }

        int x = json_int(obj, "x", inner_start);
        int y = json_int(obj, "y", inner_start);
        int w = json_int(obj, "w", inner_start);
        int h = json_int(obj, "h", inner_start);

        if (x >= 0 && y >= 0 && w > 0 && h > 0 && !name.empty()) {
            AtlasRegion r;
            r.u0 = x / tw;
            r.v0 = y / th;
            r.u1 = (x + w) / tw;
            r.v1 = (y + h) / th;
            out[name] = r;
        }

        pos = cb + 1;
    }
    return !out.empty();
}

// ── Texture API ────────────────────────────────────────────────────────────

extern "C" {

void* texture_load(const char* path) {
    if (!path) { s_texture_error = "texture_load: null path"; return nullptr; }
    int w = 0, h = 0, ch = 0;
    stbi_uc* data = stbi_load(path, &w, &h, &ch, 4);
    if (!data) {
        s_texture_error = std::string("texture_load: ") + stbi_failure_reason()
                          + " (" + path + ")";
        return nullptr;
    }
    auto* t = new TextureHandle();
    t->w = w; t->h = h;
    t->pixels.assign(data, data + (size_t)(w * h * 4));
    stbi_image_free(data);
    return t;
}

void texture_destroy(void* handle) {
    delete static_cast<TextureHandle*>(handle);
}

int texture_width(void* handle) {
    if (!handle) return 0;
    return static_cast<TextureHandle*>(handle)->w;
}

int texture_height(void* handle) {
    if (!handle) return 0;
    return static_cast<TextureHandle*>(handle)->h;
}

void texture_bind(void* handle, int unit) {
    if (!handle) return;
    static_cast<TextureHandle*>(handle)->bound_unit = unit; // headless no-op
}

const char* texture_error(void) { return s_texture_error.c_str(); }

// ── Sprite Sheet ───────────────────────────────────────────────────────────

void* spritesheet_create(void* tex, int fw, int fh, int count) {
    if (!tex || fw <= 0 || fh <= 0 || count <= 0) {
        s_texture_error = "spritesheet_create: invalid arguments";
        return nullptr;
    }
    auto* ss = new SpriteSheet();
    ss->tex = static_cast<TextureHandle*>(tex);
    ss->fw = fw; ss->fh = fh; ss->count = count;
    return ss;
}

void spritesheet_destroy(void* ss) {
    delete static_cast<SpriteSheet*>(ss);
}

void spritesheet_frame_uv(void* ss, int frame,
                           float* u0, float* v0, float* u1, float* v1) {
    auto zero = [&]{ if(u0)*u0=0; if(v0)*v0=0; if(u1)*u1=0; if(v1)*v1=0; };
    if (!ss) { zero(); return; }
    auto* s = static_cast<SpriteSheet*>(ss);
    if (frame < 0 || frame >= s->count) {
        s_texture_error = "spritesheet_frame_uv: frame index out of range";
        zero(); return;
    }
    int tw = s->tex ? s->tex->w : s->fw;
    int th = s->tex ? s->tex->h : s->fh;
    int cols = tw / s->fw;
    if (cols <= 0) cols = 1;
    int col = frame % cols;
    int row = frame / cols;
    if (u0) *u0 = (float)(col * s->fw) / tw;
    if (v0) *v0 = (float)(row * s->fh) / th;
    if (u1) *u1 = (float)((col + 1) * s->fw) / tw;
    if (v1) *v1 = (float)((row + 1) * s->fh) / th;
}

// ── Texture Atlas ──────────────────────────────────────────────────────────

void* atlas_load(void* tex, const char* json_path) {
    if (!json_path) { s_atlas_error = "atlas_load: null path"; return nullptr; }
    std::ifstream f(json_path);
    if (!f.is_open()) {
        s_atlas_error = std::string("atlas_load: cannot open ") + json_path;
        return nullptr;
    }
    std::string json((std::istreambuf_iterator<char>(f)),
                      std::istreambuf_iterator<char>());
    auto* atlas = new TextureAtlas();
    atlas->tex = static_cast<TextureHandle*>(tex);
    if (!parse_atlas_json(json, atlas->tex, atlas->regions)) {
        s_atlas_error = "atlas_load: no valid regions found in " + std::string(json_path);
        delete atlas;
        return nullptr;
    }
    return atlas;
}

void atlas_destroy(void* atlas) {
    delete static_cast<TextureAtlas*>(atlas);
}

void atlas_region_uv(void* atlas, const char* name,
                     float* u0, float* v0, float* u1, float* v1) {
    auto zero = [&]{ if(u0)*u0=0; if(v0)*v0=0; if(u1)*u1=0; if(v1)*v1=0; };
    if (!atlas || !name) { zero(); return; }
    auto* a = static_cast<TextureAtlas*>(atlas);
    auto it = a->regions.find(name);
    if (it == a->regions.end()) {
        s_atlas_error = std::string("atlas_region_uv: region '") + name + "' not found";
        zero(); return;
    }
    if (u0) *u0 = it->second.u0;
    if (v0) *v0 = it->second.v0;
    if (u1) *u1 = it->second.u1;
    if (v1) *v1 = it->second.v1;
}

const char* atlas_error(void) { return s_atlas_error.c_str(); }

} // extern "C"
