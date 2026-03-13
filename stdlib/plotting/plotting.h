#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace sapphire {
namespace stdlib {
namespace Plotting {

// Forward declarations
struct PlotData;
struct PlotStyle;
struct PlotAxis;
struct PlotLegend;

// Color structure
struct Color {
    double r, g, b, a;
    
    Color(double red = 0.0, double green = 0.0, double blue = 0.0, double alpha = 1.0)
        : r(red), g(green), b(blue), a(alpha) {}
    
    // Predefined colors
    static Color Black() { return Color(0.0, 0.0, 0.0); }
    static Color White() { return Color(1.0, 1.0, 1.0); }
    static Color Red() { return Color(1.0, 0.0, 0.0); }
    static Color Green() { return Color(0.0, 1.0, 0.0); }
    static Color Blue() { return Color(0.0, 0.0, 1.0); }
    static Color Orange() { return Color(1.0, 0.5, 0.0); }
    static Color Purple() { return Color(0.5, 0.0, 1.0); }
    static Color Cyan() { return Color(0.0, 1.0, 1.0); }
    static Color Yellow() { return Color(1.0, 1.0, 0.0); }
    static Color Gray() { return Color(0.5, 0.5, 0.5); }
};

// Chart types
enum class ChartType {
    LINE,
    SCATTER,
    BAR,
    HISTOGRAM
};

// Marker styles
enum class MarkerStyle {
    NONE,
    CIRCLE,
    SQUARE,
    TRIANGLE,
    DIAMOND,
    CROSS,
    PLUS
};

// Line styles
enum class LineStyle {
    SOLID,
    DASHED,
    DOTTED,
    DASH_DOT
};

// Plot data series
struct PlotSeries {
    std::vector<double> x_data;
    std::vector<double> y_data;
    std::string label;
    Color color;
    MarkerStyle marker;
    LineStyle line_style;
    double line_width;
    double marker_size;
    
    PlotSeries(const std::string& name = "")
        : label(name), color(Color::Blue()), marker(MarkerStyle::NONE),
          line_style(LineStyle::SOLID), line_width(1.0), marker_size(3.0) {}
};

// Axis configuration
struct PlotAxis {
    std::string label;
    double min_value;
    double max_value;
    bool auto_scale;
    bool show_grid;
    Color grid_color;
    std::vector<double> tick_positions;
    std::vector<std::string> tick_labels;
    
    PlotAxis(const std::string& axis_label = "")
        : label(axis_label), min_value(0.0), max_value(1.0), auto_scale(true),
          show_grid(true), grid_color(Color::Gray()) {}
};

// Legend configuration
struct PlotLegend {
    bool show;
    std::string position; // "top-right", "top-left", "bottom-right", "bottom-left"
    Color background_color;
    Color text_color;
    
    PlotLegend() : show(true), position("top-right"), 
                   background_color(Color::White()), text_color(Color::Black()) {}
};

// Main plot class
class Plot {
private:
    std::vector<std::shared_ptr<PlotSeries>> series;
    std::string title;
    PlotAxis x_axis;
    PlotAxis y_axis;
    PlotLegend legend;
    Color background_color;
    int width;
    int height;
    ChartType chart_type;
    
public:
    Plot(int w = 800, int h = 600, ChartType type = ChartType::LINE);
    
    // Data management
    void add_series(const std::vector<double>& x, const std::vector<double>& y, 
                   const std::string& label = "", const Color& color = Color::Blue());
    void add_line(const std::vector<double>& x, const std::vector<double>& y,
                  const std::string& label = "", const Color& color = Color::Blue());
    void add_scatter(const std::vector<double>& x, const std::vector<double>& y,
                     const std::string& label = "", const Color& color = Color::Red(),
                     MarkerStyle marker = MarkerStyle::CIRCLE);
    void add_bar(const std::vector<double>& x, const std::vector<double>& y,
                 const std::string& label = "", const Color& color = Color::Green());
    void add_histogram(const std::vector<double>& data, int bins = 20,
                       const std::string& label = "", const Color& color = Color::Orange());
    
    // Styling
    void set_title(const std::string& plot_title);
    void set_xlabel(const std::string& label);
    void set_ylabel(const std::string& label);
    void set_xlim(double min_val, double max_val);
    void set_ylim(double min_val, double max_val);
    void set_background_color(const Color& color);
    void show_grid(bool show = true);
    void show_legend(bool show = true);
    void set_legend_position(const std::string& position);
    
    // Series styling
    void set_line_style(size_t series_index, LineStyle style, double width = 1.0);
    void set_marker_style(size_t series_index, MarkerStyle style, double size = 3.0);
    void set_series_color(size_t series_index, const Color& color);
    
    // Rendering
    void render();
    void show();
    void save_png(const std::string& filename, int dpi = 300);
    void save_svg(const std::string& filename);
    void save_pdf(const std::string& filename);
    
    // Getters
    size_t get_series_count() const { return series.size(); }
    const PlotSeries& get_series(size_t index) const { return *series[index]; }
    std::string get_title() const { return title; }
    
private:
    void calculate_auto_limits();
    void draw_background();
    void draw_axes();
    void draw_grid();
    void draw_series();
    void draw_legend();
    void draw_title();
    
    // Helper functions
    std::pair<double, double> data_to_screen(double x, double y);
    void draw_line(double x1, double y1, double x2, double y2, const Color& color, double width);
    void draw_marker(double x, double y, MarkerStyle style, double size, const Color& color);
    void draw_text(double x, double y, const std::string& text, const Color& color);
};

// Convenience functions for quick plotting
std::shared_ptr<Plot> plot(const std::vector<double>& y, const std::string& title = "");
std::shared_ptr<Plot> plot(const std::vector<double>& x, const std::vector<double>& y, 
                          const std::string& title = "");
std::shared_ptr<Plot> scatter(const std::vector<double>& x, const std::vector<double>& y,
                             const std::string& title = "");
std::shared_ptr<Plot> bar_chart(const std::vector<double>& x, const std::vector<double>& y,
                                const std::string& title = "");
std::shared_ptr<Plot> histogram(const std::vector<double>& data, int bins = 20,
                               const std::string& title = "");

// Statistical plotting functions
std::shared_ptr<Plot> box_plot(const std::vector<std::vector<double>>& data,
                              const std::vector<std::string>& labels = {},
                              const std::string& title = "");
std::shared_ptr<Plot> violin_plot(const std::vector<std::vector<double>>& data,
                                 const std::vector<std::string>& labels = {},
                                 const std::string& title = "");

// Multi-plot support
class SubplotGrid {
private:
    int rows, cols;
    std::vector<std::vector<std::shared_ptr<Plot>>> plots;
    std::string main_title;
    
public:
    SubplotGrid(int nrows, int ncols);
    
    std::shared_ptr<Plot> subplot(int row, int col);
    void set_main_title(const std::string& title);
    void show();
    void save_png(const std::string& filename, int dpi = 300);
    void save_svg(const std::string& filename);
    void save_pdf(const std::string& filename);
};

// Color palettes
class ColorPalette {
public:
    static std::vector<Color> Default();
    static std::vector<Color> Viridis();
    static std::vector<Color> Plasma();
    static std::vector<Color> Set1();
    static std::vector<Color> Pastel();
    static Color get_color(size_t index, const std::vector<Color>& palette = Default());
};

} // namespace Plotting
} // namespace stdlib
} // namespace sapphire

// C API for Sapphire integration
extern "C" {
    // Plot creation and management
    void* sapphire_plot_create(int width, int height, int chart_type);
    void sapphire_plot_destroy(void* plot);
    
    // Data functions
    void sapphire_plot_add_line(void* plot, double* x_data, double* y_data, 
                               int data_size, const char* label, 
                               double r, double g, double b);
    void sapphire_plot_add_scatter(void* plot, double* x_data, double* y_data,
                                  int data_size, const char* label,
                                  double r, double g, double b, int marker_style);
    void sapphire_plot_add_bar(void* plot, double* x_data, double* y_data,
                              int data_size, const char* label,
                              double r, double g, double b);
    void sapphire_plot_add_histogram(void* plot, double* data, int data_size,
                                    int bins, const char* label,
                                    double r, double g, double b);
    
    // Styling functions
    void sapphire_plot_set_title(void* plot, const char* title);
    void sapphire_plot_set_xlabel(void* plot, const char* label);
    void sapphire_plot_set_ylabel(void* plot, const char* label);
    void sapphire_plot_set_xlim(void* plot, double min_val, double max_val);
    void sapphire_plot_set_ylim(void* plot, double min_val, double max_val);
    void sapphire_plot_show_grid(void* plot, bool show);
    void sapphire_plot_show_legend(void* plot, bool show);
    
    // Rendering functions
    void sapphire_plot_show(void* plot);
    void sapphire_plot_save_png(void* plot, const char* filename, int dpi);
    void sapphire_plot_save_svg(void* plot, const char* filename);
    void sapphire_plot_save_pdf(void* plot, const char* filename);
    
    // Convenience functions
    void* sapphire_quick_plot(double* y_data, int data_size, const char* title);
    void* sapphire_quick_scatter(double* x_data, double* y_data, int data_size, const char* title);
    void* sapphire_quick_histogram(double* data, int data_size, int bins, const char* title);
    
    // Subplot functions
    void* sapphire_subplot_grid_create(int rows, int cols);
    void* sapphire_subplot_get(void* grid, int row, int col);
    void sapphire_subplot_grid_show(void* grid);
    void sapphire_subplot_grid_save_png(void* grid, const char* filename, int dpi);
}