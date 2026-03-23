#include "model.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>

// ── Internal structs ───────────────────────────────────────────────────────

struct Vec3f { float x = 0, y = 0, z = 0; };
struct Vec2f { float u = 0, v = 0; };
struct Face  { int pos[3]={0,0,0}; int uv[3]={0,0,0}; int n[3]={0,0,0}; };

struct ModelHandle {
    std::vector<Vec3f> positions;
    std::vector<Vec2f> uvs;
    std::vector<Vec3f> normals;
    std::vector<Face>  faces;
};

static std::string s_model_error;

// ── OBJ parser ─────────────────────────────────────────────────────────────

// Parse a face token like "1", "1/2", "1/2/3", "1//3"
// OBJ indices are 1-based; we convert to 0-based.
static void parse_face_token(const std::string& tok, int& pi, int& ui, int& ni) {
    pi = ui = ni = -1;
    std::istringstream ss(tok);
    std::string part;
    int idx = 0;
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            int v = std::stoi(part) - 1; // 0-based
            if (idx == 0) pi = v;
            else if (idx == 1) ui = v;
            else if (idx == 2) ni = v;
        }
        idx++;
    }
}

extern "C" {

void* model_load(const char* obj_path) {
    if (!obj_path) { s_model_error = "model_load: null path"; return nullptr; }
    std::ifstream f(obj_path);
    if (!f.is_open()) {
        s_model_error = std::string("model_load: cannot open ") + obj_path;
        return nullptr;
    }

    auto* m = new ModelHandle();
    std::string line;
    bool has_data = false;

    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "v") {
            Vec3f v;
            ss >> v.x >> v.y >> v.z;
            m->positions.push_back(v);
            has_data = true;
        } else if (token == "vt") {
            Vec2f vt;
            ss >> vt.u >> vt.v;
            m->uvs.push_back(vt);
        } else if (token == "vn") {
            Vec3f vn;
            ss >> vn.x >> vn.y >> vn.z;
            m->normals.push_back(vn);
        } else if (token == "f") {
            // Triangulate: read all vertices, fan-triangulate
            std::vector<std::string> toks;
            std::string t;
            while (ss >> t) toks.push_back(t);
            if (toks.size() < 3) continue;
            for (size_t i = 1; i + 1 < toks.size(); i++) {
                Face face;
                int pi, ui, ni;
                parse_face_token(toks[0],   pi, ui, ni);
                face.pos[0]=pi; face.uv[0]=ui; face.n[0]=ni;
                parse_face_token(toks[i],   pi, ui, ni);
                face.pos[1]=pi; face.uv[1]=ui; face.n[1]=ni;
                parse_face_token(toks[i+1], pi, ui, ni);
                face.pos[2]=pi; face.uv[2]=ui; face.n[2]=ni;
                m->faces.push_back(face);
            }
        }
    }

    if (!has_data) {
        s_model_error = std::string("model_load: no vertex data in ") + obj_path;
        delete m;
        return nullptr;
    }
    return m;
}

void model_destroy(void* handle) {
    delete static_cast<ModelHandle*>(handle);
}

void model_draw(void* handle, void* shader) {
    (void)handle; (void)shader; // headless no-op
}

int model_vertex_count(void* handle) {
    if (!handle) return 0;
    return (int)static_cast<ModelHandle*>(handle)->positions.size();
}

int model_face_count(void* handle) {
    if (!handle) return 0;
    return (int)static_cast<ModelHandle*>(handle)->faces.size();
}

const char* model_error(void) { return s_model_error.c_str(); }

} // extern "C"
