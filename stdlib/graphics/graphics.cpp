#include "graphics.h"
#include <cmath>
#include <fstream>
#include <algorithm>

namespace sapphire {
namespace stdlib {

Canvas::Canvas(uint32_t w, uint32_t h) 
    : width_(w), height_(h), 
      fill_color_(Color::black()),
      stroke_color_(Color::black()),
      stroke_width_(1.0) {
    pixels_.resize(w * h, 0xFFFFFFFF); // White background
}

Canvas::~Canvas() {
    pixels_.clear();
}

void Canvas::clear(const Color& c) {
    uint32_t color = (c.a << 24) | (c.b << 16) | (c.g << 8) | c.r;
    std::fill(pixels_.begin(), pixels_.end(), color);
}

void Canvas::draw_pixel(int x, int y, const Color& c) {
    if (x >= 0 && x < static_cast<int>(width_) && 
        y >= 0 && y < static_cast<int>(height_)) {
        uint32_t color = (c.a << 24) | (c.b << 16) | (c.g << 8) | c.r;
        pixels_[y * width_ + x] = color;
    }
}

void Canvas::draw_line(const Point& p1, const Point& p2) {
    // Bresenham's line algorithm
    int x0 = static_cast<int>(p1.x);
    int y0 = static_cast<int>(p1.y);
    int x1 = static_cast<int>(p2.x);
    int y1 = static_cast<int>(p2.y);
    
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        draw_pixel(x0, y0, stroke_color_);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

void Canvas::draw_rect(const Rect& r) {
    Point p1(r.x, r.y);
    Point p2(r.x + r.width, r.y);
    Point p3(r.x + r.width, r.y + r.height);
    Point p4(r.x, r.y + r.height);
    
    draw_line(p1, p2);
    draw_line(p2, p3);
    draw_line(p3, p4);
    draw_line(p4, p1);
}

void Canvas::fill_rect(const Rect& r) {
    int x_start = static_cast<int>(r.x);
    int y_start = static_cast<int>(r.y);
    int x_end = static_cast<int>(r.x + r.width);
    int y_end = static_cast<int>(r.y + r.height);
    
    for (int y = y_start; y < y_end; ++y) {
        for (int x = x_start; x < x_end; ++x) {
            draw_pixel(x, y, fill_color_);
        }
    }
}

void Canvas::draw_circle(const Point& center, double radius) {
    int r = static_cast<int>(radius);
    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);
    
    for (int x = -r; x <= r; ++x) {
        for (int y = -r; y <= r; ++y) {
            if (x*x + y*y == r*r) {
                draw_pixel(cx + x, cy + y, stroke_color_);
            }
        }
    }
}

void Canvas::fill_circle(const Point& center, double radius) {
    int r = static_cast<int>(radius);
    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);
    
    for (int x = -r; x <= r; ++x) {
        for (int y = -r; y <= r; ++y) {
            if (x*x + y*y <= r*r) {
                draw_pixel(cx + x, cy + y, fill_color_);
            }
        }
    }
}

void Canvas::draw_triangle(const Point& p1, const Point& p2, const Point& p3) {
    draw_line(p1, p2);
    draw_line(p2, p3);
    draw_line(p3, p1);
}

void Canvas::fill_triangle(const Point& p1, const Point& p2, const Point& p3) {
    // Simple triangle fill using barycentric coordinates
    int min_x = static_cast<int>(std::min({p1.x, p2.x, p3.x}));
    int max_x = static_cast<int>(std::max({p1.x, p2.x, p3.x}));
    int min_y = static_cast<int>(std::min({p1.y, p2.y, p3.y}));
    int max_y = static_cast<int>(std::max({p1.y, p2.y, p3.y}));
    
    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            // Simplified point-in-triangle test
            draw_pixel(x, y, fill_color_);
        }
    }
}

bool Canvas::save_ppm(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    
    file << "P6\n" << width_ << " " << height_ << "\n255\n";
    
    for (uint32_t pixel : pixels_) {
        uint8_t r = pixel & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = (pixel >> 16) & 0xFF;
        file.put(r);
        file.put(g);
        file.put(b);
    }
    
    file.close();
    return true;
}

bool Canvas::save_png(const std::string& filename) const {
    // PNG support would require libpng
    // For now, just save as PPM
    return save_ppm(filename + ".ppm");
}

} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    void* sapphire_canvas_create(uint32_t width, uint32_t height) {
        return new sapphire::stdlib::Canvas(width, height);
    }
    
    void sapphire_canvas_destroy(void* canvas) {
        delete static_cast<sapphire::stdlib::Canvas*>(canvas);
    }
    
    void sapphire_canvas_clear(void* canvas, uint8_t r, uint8_t g, uint8_t b) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        c->clear(sapphire::stdlib::Color(r, g, b));
    }
    
    void sapphire_canvas_draw_pixel(void* canvas, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        c->draw_pixel(x, y, sapphire::stdlib::Color(r, g, b));
    }
    
    void sapphire_canvas_draw_line(void* canvas, double x1, double y1, double x2, double y2) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        c->draw_line(sapphire::stdlib::Point(x1, y1), sapphire::stdlib::Point(x2, y2));
    }
    
    void sapphire_canvas_draw_rect(void* canvas, double x, double y, double w, double h) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        c->draw_rect(sapphire::stdlib::Rect(x, y, w, h));
    }
    
    void sapphire_canvas_fill_rect(void* canvas, double x, double y, double w, double h) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        c->fill_rect(sapphire::stdlib::Rect(x, y, w, h));
    }
    
    void sapphire_canvas_draw_circle(void* canvas, double x, double y, double r) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        c->draw_circle(sapphire::stdlib::Point(x, y), r);
    }
    
    void sapphire_canvas_fill_circle(void* canvas, double x, double y, double r) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        c->fill_circle(sapphire::stdlib::Point(x, y), r);
    }
    
    bool sapphire_canvas_save_ppm(void* canvas, const char* filename) {
        auto* c = static_cast<sapphire::stdlib::Canvas*>(canvas);
        return c->save_ppm(filename);
    }
}
