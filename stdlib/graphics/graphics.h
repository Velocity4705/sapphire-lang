#ifndef SAPPHIRE_STDLIB_GRAPHICS_H
#define SAPPHIRE_STDLIB_GRAPHICS_H

#include <cstdint>
#include <string>
#include <vector>

namespace sapphire {
namespace stdlib {

/**
 * Graphics - 2D Graphics and Drawing
 * 
 * Provides basic 2D graphics primitives and canvas operations.
 */

struct Color {
    uint8_t r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t red, uint8_t green, uint8_t blue) 
        : r(red), g(green), b(blue), a(255) {}
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
        : r(red), g(green), b(blue), a(alpha) {}
    
    static Color white() { return Color(255, 255, 255); }
    static Color black() { return Color(0, 0, 0); }
    static Color red() { return Color(255, 0, 0); }
    static Color green() { return Color(0, 255, 0); }
    static Color blue() { return Color(0, 0, 255); }
    static Color yellow() { return Color(255, 255, 0); }
    static Color cyan() { return Color(0, 255, 255); }
    static Color magenta() { return Color(255, 0, 255); }
};

struct Point {
    double x, y;
    
    Point() : x(0), y(0) {}
    Point(double px, double py) : x(px), y(py) {}
};

struct Rect {
    double x, y, width, height;
    
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(double px, double py, double w, double h) 
        : x(px), y(py), width(w), height(h) {}
};

class Canvas {
private:
    uint32_t width_;
    uint32_t height_;
    std::vector<uint32_t> pixels_;
    Color fill_color_;
    Color stroke_color_;
    double stroke_width_;
    
public:
    Canvas(uint32_t w, uint32_t h);
    ~Canvas();
    
    // Properties
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }
    
    // Drawing state
    void set_fill_color(const Color& c) { fill_color_ = c; }
    void set_stroke_color(const Color& c) { stroke_color_ = c; }
    void set_stroke_width(double w) { stroke_width_ = w; }
    
    // Drawing primitives
    void clear(const Color& c = Color::white());
    void draw_pixel(int x, int y, const Color& c);
    void draw_line(const Point& p1, const Point& p2);
    void draw_rect(const Rect& r);
    void fill_rect(const Rect& r);
    void draw_circle(const Point& center, double radius);
    void fill_circle(const Point& center, double radius);
    void draw_triangle(const Point& p1, const Point& p2, const Point& p3);
    void fill_triangle(const Point& p1, const Point& p2, const Point& p3);
    
    // Export
    bool save_ppm(const std::string& filename) const;
    bool save_png(const std::string& filename) const;
};

} // namespace stdlib
} // namespace sapphire

// C API for code generation
extern "C" {
    void* sapphire_canvas_create(uint32_t width, uint32_t height);
    void sapphire_canvas_destroy(void* canvas);
    void sapphire_canvas_clear(void* canvas, uint8_t r, uint8_t g, uint8_t b);
    void sapphire_canvas_draw_pixel(void* canvas, int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void sapphire_canvas_draw_line(void* canvas, double x1, double y1, double x2, double y2);
    void sapphire_canvas_draw_rect(void* canvas, double x, double y, double w, double h);
    void sapphire_canvas_fill_rect(void* canvas, double x, double y, double w, double h);
    void sapphire_canvas_draw_circle(void* canvas, double x, double y, double r);
    void sapphire_canvas_fill_circle(void* canvas, double x, double y, double r);
    bool sapphire_canvas_save_ppm(void* canvas, const char* filename);
}

#endif // SAPPHIRE_STDLIB_GRAPHICS_H
