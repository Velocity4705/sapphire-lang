#include "../graphics/graphics.h"
#include <iostream>
#include <cassert>

using namespace sapphire::stdlib;

void test_color() {
    std::cout << "Testing Color..." << std::endl;
    
    Color red = Color::red();
    assert(red.r == 255 && red.g == 0 && red.b == 0);
    
    Color custom(128, 64, 32);
    assert(custom.r == 128 && custom.g == 64 && custom.b == 32);
    
    std::cout << "  ✓ Color creation and predefined colors" << std::endl;
}

void test_canvas() {
    std::cout << "Testing Canvas..." << std::endl;
    
    Canvas canvas(100, 100);
    assert(canvas.width() == 100);
    assert(canvas.height() == 100);
    
    canvas.clear(Color::white());
    canvas.set_fill_color(Color::blue());
    canvas.fill_rect(Rect(10, 10, 20, 20));
    
    canvas.set_stroke_color(Color::red());
    canvas.draw_line(Point(0, 0), Point(50, 50));
    
    canvas.draw_circle(Point(50, 50), 10);
    canvas.fill_circle(Point(75, 75), 5);
    
    std::cout << "  ✓ Canvas creation and drawing operations" << std::endl;
}

void test_ppm_export() {
    std::cout << "Testing PPM export..." << std::endl;
    
    Canvas canvas(200, 200);
    canvas.clear(Color::white());
    
    canvas.set_fill_color(Color::red());
    canvas.fill_rect(Rect(50, 50, 100, 100));
    
    canvas.set_stroke_color(Color::blue());
    canvas.draw_circle(Point(100, 100), 40);
    
    bool saved = canvas.save_ppm("test_output.ppm");
    assert(saved);
    
    std::cout << "  ✓ PPM file export (test_output.ppm created)" << std::endl;
}

void test_c_api() {
    std::cout << "Testing C API..." << std::endl;
    
    void* canvas = sapphire_canvas_create(100, 100);
    assert(canvas != nullptr);
    
    sapphire_canvas_clear(canvas, 255, 255, 255);
    sapphire_canvas_fill_rect(canvas, 10, 10, 30, 30);
    sapphire_canvas_draw_line(canvas, 0, 0, 100, 100);
    sapphire_canvas_draw_circle(canvas, 50, 50, 20);
    
    bool saved = sapphire_canvas_save_ppm(canvas, "test_c_api.ppm");
    assert(saved);
    
    sapphire_canvas_destroy(canvas);
    
    std::cout << "  ✓ C API functions (test_c_api.ppm created)" << std::endl;
}

int main() {
    std::cout << "=== Graphics Library Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_color();
    test_canvas();
    test_ppm_export();
    test_c_api();
    
    std::cout << std::endl;
    std::cout << "================================================================================";
    std::cout << std::endl;
    std::cout << "All Graphics tests passed! 🎨✅" << std::endl;
    std::cout << "================================================================================";
    std::cout << std::endl;
    
    return 0;
}
