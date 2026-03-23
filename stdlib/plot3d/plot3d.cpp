#include "plot3d.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

// ── Plotly path ────────────────────────────────────────────────────────────
static std::string find_plotly_js_path() {
    static const char* candidates[] = {
        "stdlib/plot3d/plotly.min.js",
        "../stdlib/plot3d/plotly.min.js",
        nullptr
    };
    for (int i = 0; candidates[i]; i++) {
        std::ifstream f(candidates[i]);
        if (f.good()) {
            char abs[4096] = {};
            if (realpath(candidates[i], abs)) return std::string(abs);
        }
    }
    return std::string();
}

static std::string read_file_bytes(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    return std::string((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());
}

namespace sapphire { namespace stdlib { namespace Plot3D {

Plot3D::Plot3D() {}

void Plot3D::set_title(const std::string& t)  { title  = t; }
void Plot3D::set_xlabel(const std::string& l) { xlabel = l; }
void Plot3D::set_ylabel(const std::string& l) { ylabel = l; }
void Plot3D::set_zlabel(const std::string& l) { zlabel = l; }

void Plot3D::add_surface(const std::vector<double>& z_flat, int rows, int cols,
                         const std::string& label, const std::string& colorscale) {
    Series3D s;
    s.type = Plot3DType::SURFACE;
    s.z = z_flat;
    s.rows = rows;
    s.cols = cols;
    s.label = label.empty() ? "Surface" : label;
    s.colorscale = colorscale.empty() ? "Viridis" : colorscale;
    series.push_back(s);
}

void Plot3D::add_scatter(const std::vector<double>& x, const std::vector<double>& y,
                         const std::vector<double>& z, const std::string& label) {
    Series3D s;
    s.type = Plot3DType::SCATTER;
    s.x = x; s.y = y; s.z = z;
    s.label = label.empty() ? "Scatter" : label;
    series.push_back(s);
}

void Plot3D::add_line(const std::vector<double>& x, const std::vector<double>& y,
                      const std::vector<double>& z, const std::string& label) {
    Series3D s;
    s.type = Plot3DType::LINE;
    s.x = x; s.y = y; s.z = z;
    s.label = label.empty() ? "Line" : label;
    series.push_back(s);
}

static std::string js_array(const std::vector<double>& v) {
    std::ostringstream o;
    o << "[";
    for (size_t i = 0; i < v.size(); i++) {
        if (i) o << ",";
        o << v[i];
    }
    o << "]";
    return o.str();
}

std::string Plot3D::build_trace(const Series3D& s, size_t /*idx*/) const {
    std::ostringstream o;
    if (s.type == Plot3DType::SURFACE) {
        o << "{type:'surface',name:'" << s.label << "',"
          << "colorscale:'" << s.colorscale << "',opacity:" << s.opacity << ",z:[";
        for (int r = 0; r < s.rows; r++) {
            o << "[";
            for (int c = 0; c < s.cols; c++) {
                if (c) o << ",";
                o << s.z[r * s.cols + c];
            }
            o << "]";
            if (r + 1 < s.rows) o << ",";
        }
        o << "]}";
    } else if (s.type == Plot3DType::SCATTER) {
        o << "{type:'scatter3d',mode:'markers',name:'" << s.label << "',"
          << "x:" << js_array(s.x) << ","
          << "y:" << js_array(s.y) << ","
          << "z:" << js_array(s.z) << ","
          << "marker:{size:4,opacity:0.8}}";
    } else {
        o << "{type:'scatter3d',mode:'lines',name:'" << s.label << "',"
          << "x:" << js_array(s.x) << ","
          << "y:" << js_array(s.y) << ","
          << "z:" << js_array(s.z) << ","
          << "line:{width:4}}";
    }
    return o.str();
}

void Plot3D::save_html(const std::string& filename) {
    // ── Build traces & layout ───────────────────────────────────────────
    std::ostringstream traces;
    traces << "[";
    for (size_t i = 0; i < series.size(); i++) {
        if (i) traces << ",";
        traces << build_trace(series[i], i);
    }
    traces << "]";

    std::ostringstream layout;
    layout << "{"
           << "title:{text:'" << title << "',font:{size:18}},"
           << "autosize:true,"
           << "scene:{"
           <<   "xaxis:{title:'" << xlabel << "'},"
           <<   "yaxis:{title:'" << ylabel << "'},"
           <<   "zaxis:{title:'" << zlabel << "'},"
           <<   "camera:{eye:{x:1.5,y:1.5,z:1.2}}"
           << "},"
           << "margin:{l:0,r:0,t:60,b:0},"
           << "paper_bgcolor:'#ffffff',"
           << "showlegend:true"
           << "}";

    // ── Load Plotly JS (inline it so the HTML is self-contained) ────────
    std::string plotly_path = find_plotly_js_path();
    std::string plotly_js;
    if (!plotly_path.empty()) plotly_js = read_file_bytes(plotly_path);

    // ── Build self-contained HTML ────────────────────────────────────────
    std::ostringstream html;
    html << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
         << "<meta charset=\"UTF-8\">\n"
         << "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n"
         << "<title>" << (title.empty() ? "3D Plot" : title) << "</title>\n"
         << "<style>\n"
         << "  * { margin:0; padding:0; box-sizing:border-box; }\n"
         << "  html, body { width:100%; height:100%; overflow:hidden; background:#f5f5f5; font-family:Arial,sans-serif; }\n"
         << "  #header { padding:12px 20px; background:#1a1a2e; color:#fff; font-size:14px; height:44px; }\n"
         << "  #plot { display:block; width:100%; height:calc(100vh - 44px); }\n"
         << "</style>\n</head>\n<body>\n"
         << "<div id=\"header\">Sapphire 3D Plot &mdash; drag to rotate &bull; scroll to zoom &bull; right-drag to pan</div>\n"
         << "<div id=\"plot\"></div>\n";

    if (!plotly_js.empty()) {
        // Inline Plotly so the file is fully self-contained — no server needed
        html << "<script>\n" << plotly_js << "\n</script>\n";
    } else {
        // Fallback: CDN
        html << "<script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script>\n";
    }

    html << "<script>\n"
         << "var traces = " << traces.str() << ";\n"
         << "var layout = " << layout.str() << ";\n"
         << "var config = {responsive:true, displayModeBar:true, displaylogo:false};\n"
         << "function renderPlot() {\n"
         << "  var el = document.getElementById('plot');\n"
         << "  el.style.width  = window.innerWidth  + 'px';\n"
         << "  el.style.height = (window.innerHeight - 44) + 'px';\n"
         << "  Plotly.newPlot('plot', traces, layout, config).then(function() {\n"
         << "    Plotly.relayout('plot', {autosize: true});\n"
         << "  });\n"
         << "}\n"
         << "if (document.readyState === 'complete') { renderPlot(); }\n"
         << "else { window.addEventListener('load', renderPlot); }\n"
         << "window.addEventListener('resize', function() {\n"
         << "  Plotly.relayout('plot', {width: window.innerWidth, height: window.innerHeight - 44});\n"
         << "});\n"
         << "</script>\n</body>\n</html>\n";

    std::string html_str = html.str();

    // Write self-contained HTML to disk
    {
        std::ofstream f(filename);
        if (f.is_open()) f << html_str;
    }

    std::cout << "3D plot saved : " << filename << "\n";

    // Open directly from disk — no server needed
    std::string abs_path;
    char buf[4096] = {};
    if (realpath(filename.c_str(), buf)) abs_path = buf;
    else abs_path = filename;

    std::string url = "file://" + abs_path;
    std::cout << "Opening in browser...\n";
    std::string cmd = "xdg-open \"" + url + "\" 2>/dev/null &";
    std::system(cmd.c_str());
}

}}} // namespace

// ── C API ──────────────────────────────────────────────────────────────────
extern "C" {
using P = sapphire::stdlib::Plot3D::Plot3D;

void* plot3d_create() { return new P(); }
void  plot3d_destroy(void* p) { delete static_cast<P*>(p); }

void plot3d_add_surface(void* p, double* z_flat, int rows, int cols,
                        const char* label, const char* colorscale) {
    std::vector<double> z(z_flat, z_flat + rows * cols);
    static_cast<P*>(p)->add_surface(z, rows, cols,
        label ? label : "", colorscale ? colorscale : "Viridis");
}

void plot3d_add_scatter(void* p, double* x, double* y, double* z, int n, const char* label) {
    static_cast<P*>(p)->add_scatter(
        std::vector<double>(x, x+n),
        std::vector<double>(y, y+n),
        std::vector<double>(z, z+n),
        label ? label : "");
}

void plot3d_add_line(void* p, double* x, double* y, double* z, int n, const char* label) {
    static_cast<P*>(p)->add_line(
        std::vector<double>(x, x+n),
        std::vector<double>(y, y+n),
        std::vector<double>(z, z+n),
        label ? label : "");
}

void plot3d_set_title(void* p, const char* t)  { static_cast<P*>(p)->set_title(t  ? t  : ""); }
void plot3d_set_xlabel(void* p, const char* l) { static_cast<P*>(p)->set_xlabel(l ? l : "X"); }
void plot3d_set_ylabel(void* p, const char* l) { static_cast<P*>(p)->set_ylabel(l ? l : "Y"); }
void plot3d_set_zlabel(void* p, const char* l) { static_cast<P*>(p)->set_zlabel(l ? l : "Z"); }
void plot3d_save_html(void* p, const char* fn) {
    static_cast<P*>(p)->save_html(fn ? fn : "plot3d.html");
}
} // extern "C"
