#include "plot3d.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

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

// ── Tiny HTTP server (child process) ──────────────────────────────────────
// Serves two routes: GET / → html_content, GET /plotly.min.js → plotly_js
// Exits after serving `max_requests` requests or after `timeout_sec` seconds.
static void run_http_server(int port,
                             const std::string& html_content,
                             const std::string& plotly_js,
                             int max_requests = 8,
                             int timeout_sec  = 120) {
    // Create listening socket
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) _exit(1);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port        = htons((uint16_t)port);
    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) _exit(1);
    listen(srv, 4);

    // Set overall timeout via alarm
    signal(SIGALRM, [](int){ _exit(0); });
    alarm((unsigned)timeout_sec);

    int served = 0;
    while (served < max_requests) {
        int cli = accept(srv, nullptr, nullptr);
        if (cli < 0) continue;

        // Read request line
        char buf[2048] = {};
        int n = (int)recv(cli, buf, sizeof(buf)-1, 0);
        if (n <= 0) { close(cli); continue; }

        // Determine which resource was requested
        bool want_plotly = (strstr(buf, "GET /plotly") != nullptr);
        bool want_html   = (strstr(buf, "GET / ")      != nullptr ||
                            strstr(buf, "GET /index")  != nullptr);

        const std::string* body    = nullptr;
        const char*        ctype   = "text/plain";
        if (want_plotly && !plotly_js.empty()) {
            body  = &plotly_js;
            ctype = "application/javascript";
        } else if (want_html) {
            body  = &html_content;
            ctype = "text/html; charset=utf-8";
        }

        if (body) {
            std::ostringstream resp;
            resp << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: " << ctype << "\r\n"
                 << "Content-Length: " << body->size() << "\r\n"
                 << "Connection: close\r\n\r\n";
            std::string hdr = resp.str();
            send(cli, hdr.c_str(), hdr.size(), 0);
            // Send body in chunks
            const char* p = body->data();
            size_t left = body->size();
            while (left > 0) {
                ssize_t sent = send(cli, p, left, 0);
                if (sent <= 0) break;
                p += sent; left -= (size_t)sent;
            }
            served++;
        } else {
            const char* r404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            send(cli, r404, strlen(r404), 0);
        }
        close(cli);
    }
    close(srv);
    _exit(0);
}

// Pick a free port in range [base, base+100)
static int pick_free_port(int base = 19000) {
    for (int p = base; p < base + 100; p++) {
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0) continue;
        struct sockaddr_in a{};
        a.sin_family      = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        a.sin_port        = htons((uint16_t)p);
        int opt = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        bool ok = (bind(s, (struct sockaddr*)&a, sizeof(a)) == 0);
        close(s);
        if (ok) return p;
    }
    return base;
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
    // ── Build HTML ──────────────────────────────────────────────────────
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

    // HTML references plotly.min.js via the local HTTP server
    std::ostringstream html;
    html << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
         << "<meta charset=\"UTF-8\">\n"
         << "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n"
         << "<title>" << (title.empty() ? "3D Plot" : title) << "</title>\n"
         << "<style>\n"
         << "  * { margin:0; padding:0; box-sizing:border-box; }\n"
         << "  body { background:#f5f5f5; font-family:Arial,sans-serif; }\n"
         << "  #header { padding:12px 20px; background:#1a1a2e; color:#fff; font-size:14px; }\n"
         << "  #plot { width:100vw; height:calc(100vh - 44px); }\n"
         << "</style>\n</head>\n<body>\n"
         << "<div id=\"header\">Sapphire 3D Plot &mdash; drag to rotate &bull; scroll to zoom &bull; right-drag to pan</div>\n"
         << "<div id=\"plot\"></div>\n"
         << "<script src=\"/plotly.min.js\"></script>\n"
         << "<script>\n"
         << "var traces = " << traces.str() << ";\n"
         << "var layout = " << layout.str() << ";\n"
         << "var config = {responsive:true, displayModeBar:true, displaylogo:false};\n"
         << "Plotly.newPlot('plot', traces, layout, config);\n"
         << "</script>\n</body>\n</html>\n";

    std::string html_str = html.str();

    // Also write the HTML to disk so the user can keep it
    {
        std::ofstream f(filename);
        if (f.is_open()) f << html_str;
    }

    // ── Load Plotly JS ──────────────────────────────────────────────────
    std::string plotly_path = find_plotly_js_path();
    std::string plotly_js;
    if (!plotly_path.empty()) plotly_js = read_file_bytes(plotly_path);

    // ── Pick port & fork server ─────────────────────────────────────────
    int port = pick_free_port(19000);

    pid_t pid = fork();
    if (pid == 0) {
        // Child: close stdin/stdout/stderr so it's fully detached
        int devnull = open("/dev/null", O_RDWR);
        if (devnull >= 0) { dup2(devnull, 0); dup2(devnull, 1); dup2(devnull, 2); close(devnull); }
        // Serve for up to 120 seconds / 8 requests then exit
        run_http_server(port, html_str, plotly_js, 8, 120);
        _exit(0);
    }
    // Parent: detach child
    if (pid > 0) signal(SIGCHLD, SIG_IGN);

    // Give the server a moment to bind
    usleep(150000); // 150ms

    std::string url = "http://127.0.0.1:" + std::to_string(port) + "/";
    std::cout << "3D plot saved : " << filename << "\n";
    std::cout << "Serving at    : " << url << "\n";
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
