#include "plotting.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

// For now, we'll use a simple ASCII-based renderer for basic functionality
// In a full implementation, this would use Cairo, matplotlib, or similar

namespace sapphire {
namespace stdlib {
namespace Plotting {

// Plot implementation
Plot::Plot(int w, int h, ChartType type) 
    : width(w), height(h), chart_type(type), background_color(Color::White()) {
    x_axis.label = "X";
    y_axis.label = "Y";
}

void Plot::add_series(const std::vector<double>& x, const std::vector<double>& y, 
                     const std::string& label, const Color& color) {
    auto new_series = std::make_shared<PlotSeries>(label);
    new_series->x_data = x;
    new_series->y_data = y;
    new_series->color = color;
    
    // Set appropriate style based on chart type
    switch (chart_type) {
        case ChartType::LINE:
            new_series->line_style = LineStyle::SOLID;
            new_series->marker = MarkerStyle::NONE;
            break;
        case ChartType::SCATTER:
            new_series->line_style = LineStyle::SOLID;
            new_series->marker = MarkerStyle::CIRCLE;
            break;
        case ChartType::BAR:
            new_series->line_style = LineStyle::SOLID;
            new_series->marker = MarkerStyle::NONE;
            break;
        case ChartType::HISTOGRAM:
            new_series->line_style = LineStyle::SOLID;
            new_series->marker = MarkerStyle::NONE;
            break;
    }
    
    series.push_back(new_series);
}

void Plot::add_line(const std::vector<double>& x, const std::vector<double>& y,
                   const std::string& label, const Color& color) {
    chart_type = ChartType::LINE;
    add_series(x, y, label, color);
}

void Plot::add_scatter(const std::vector<double>& x, const std::vector<double>& y,
                      const std::string& label, const Color& color, MarkerStyle marker) {
    chart_type = ChartType::SCATTER;
    add_series(x, y, label, color);
    if (!series.empty()) {
        series.back()->marker = marker;
        series.back()->line_style = LineStyle::SOLID; // No line for scatter
    }
}

void Plot::add_bar(const std::vector<double>& x, const std::vector<double>& y,
                  const std::string& label, const Color& color) {
    chart_type = ChartType::BAR;
    add_series(x, y, label, color);
}

void Plot::add_histogram(const std::vector<double>& data, int bins,
                        const std::string& label, const Color& color) {
    chart_type = ChartType::HISTOGRAM;
    
    // Calculate histogram bins
    if (data.empty()) return;
    
    double min_val = *std::min_element(data.begin(), data.end());
    double max_val = *std::max_element(data.begin(), data.end());
    double bin_width = (max_val - min_val) / bins;
    
    std::vector<double> bin_centers;
    std::vector<double> bin_counts(bins, 0.0);
    
    // Calculate bin counts
    for (double value : data) {
        int bin_index = static_cast<int>((value - min_val) / bin_width);
        if (bin_index >= bins) bin_index = bins - 1;
        if (bin_index < 0) bin_index = 0;
        bin_counts[bin_index]++;
    }
    
    // Create bin centers
    for (int i = 0; i < bins; i++) {
        bin_centers.push_back(min_val + (i + 0.5) * bin_width);
    }
    
    add_series(bin_centers, bin_counts, label, color);
}

void Plot::set_title(const std::string& plot_title) {
    title = plot_title;
}

void Plot::set_xlabel(const std::string& label) {
    x_axis.label = label;
}

void Plot::set_ylabel(const std::string& label) {
    y_axis.label = label;
}

void Plot::set_xlim(double min_val, double max_val) {
    x_axis.min_value = min_val;
    x_axis.max_value = max_val;
    x_axis.auto_scale = false;
}

void Plot::set_ylim(double min_val, double max_val) {
    y_axis.min_value = min_val;
    y_axis.max_value = max_val;
    y_axis.auto_scale = false;
}

void Plot::set_background_color(const Color& color) {
    background_color = color;
}

void Plot::show_grid(bool show) {
    x_axis.show_grid = show;
    y_axis.show_grid = show;
}

void Plot::show_legend(bool show) {
    legend.show = show;
}

void Plot::set_legend_position(const std::string& position) {
    legend.position = position;
}

void Plot::calculate_auto_limits() {
    if (series.empty()) return;
    
    if (x_axis.auto_scale) {
        double x_min = std::numeric_limits<double>::max();
        double x_max = std::numeric_limits<double>::lowest();
        
        for (const auto& s : series) {
            if (!s->x_data.empty()) {
                if (chart_type == ChartType::HISTOGRAM) {
                    // Extend to bin edges, not just centers
                    size_t n = s->x_data.size();
                    double half_w = n > 1 ? (s->x_data[1] - s->x_data[0]) / 2.0
                                          : 0.5;
                    x_min = std::min(x_min, s->x_data.front() - half_w);
                    x_max = std::max(x_max, s->x_data.back()  + half_w);
                } else {
                    auto mm = std::minmax_element(s->x_data.begin(), s->x_data.end());
                    x_min = std::min(x_min, *mm.first);
                    x_max = std::max(x_max, *mm.second);
                }
            }
        }
        
        x_axis.min_value = x_min;
        x_axis.max_value = x_max;
    }
    
    if (y_axis.auto_scale) {
        double y_min = std::numeric_limits<double>::max();
        double y_max = std::numeric_limits<double>::lowest();
        
        for (const auto& s : series) {
            if (!s->y_data.empty()) {
                auto y_minmax = std::minmax_element(s->y_data.begin(), s->y_data.end());
                y_min = std::min(y_min, *y_minmax.first);
                y_max = std::max(y_max, *y_minmax.second);
            }
        }
        // Histograms always start at 0
        if (chart_type == ChartType::HISTOGRAM || chart_type == ChartType::BAR) {
            y_min = 0;
        }
        y_axis.min_value = y_min;
        y_axis.max_value = y_max;
    }
}

void Plot::render() {
    calculate_auto_limits();
    
    // Simple ASCII rendering for now
    std::cout << "\n=== " << title << " ===\n";
    std::cout << "Chart Type: ";
    switch (chart_type) {
        case ChartType::LINE: std::cout << "Line Chart"; break;
        case ChartType::SCATTER: std::cout << "Scatter Plot"; break;
        case ChartType::BAR: std::cout << "Bar Chart"; break;
        case ChartType::HISTOGRAM: std::cout << "Histogram"; break;
    }
    std::cout << "\n";
    
    std::cout << "X-axis: " << x_axis.label << " [" << x_axis.min_value << ", " << x_axis.max_value << "]\n";
    std::cout << "Y-axis: " << y_axis.label << " [" << y_axis.min_value << ", " << y_axis.max_value << "]\n";
    
    // Display series information
    for (size_t i = 0; i < series.size(); i++) {
        const auto& s = series[i];
        std::cout << "Series " << (i+1) << ": " << s->label 
                  << " (" << s->x_data.size() << " points)\n";
    }
    
    // Simple ASCII plot (40x20 characters)
    const int plot_width = 60;
    const int plot_height = 20;
    std::vector<std::string> plot_grid(plot_height, std::string(plot_width, ' '));
    
    // Draw axes
    for (int y = 0; y < plot_height; y++) {
        plot_grid[y][0] = '|';
    }
    for (int x = 0; x < plot_width; x++) {
        plot_grid[plot_height-1][x] = '-';
    }
    plot_grid[plot_height-1][0] = '+';
    
    // Plot data points
    char markers[] = {'*', 'o', '+', 'x', '#', '@', '%', '&'};
    for (size_t s_idx = 0; s_idx < series.size() && s_idx < 8; s_idx++) {
        const auto& s = series[s_idx];
        char marker = markers[s_idx];
        
        for (size_t i = 0; i < s->x_data.size() && i < s->y_data.size(); i++) {
            double x_norm = (s->x_data[i] - x_axis.min_value) / (x_axis.max_value - x_axis.min_value);
            double y_norm = (s->y_data[i] - y_axis.min_value) / (y_axis.max_value - y_axis.min_value);
            
            int x_pos = static_cast<int>(x_norm * (plot_width - 2)) + 1;
            int y_pos = static_cast<int>((1.0 - y_norm) * (plot_height - 1));
            
            if (x_pos >= 1 && x_pos < plot_width && y_pos >= 0 && y_pos < plot_height-1) {
                plot_grid[y_pos][x_pos] = marker;
            }
        }
    }
    
    // Print the plot
    std::cout << "\n";
    for (const auto& row : plot_grid) {
        std::cout << row << "\n";
    }
    
    // Legend
    if (legend.show && !series.empty()) {
        std::cout << "\nLegend:\n";
        for (size_t i = 0; i < series.size() && i < 8; i++) {
            std::cout << "  " << markers[i] << " " << series[i]->label << "\n";
        }
    }
    
    std::cout << "\n";
}

void Plot::show() {
    render();
}

void Plot::save_png(const std::string& filename, int dpi) {
    // For now, save as text file
    std::ofstream file(filename + ".txt");
    if (file.is_open()) {
        file << "Plot saved as text (PNG export not yet implemented)\n";
        file << "Title: " << title << "\n";
        file << "Series count: " << series.size() << "\n";
        file << "DPI: " << dpi << "\n";
        file.close();
        std::cout << "Plot saved to " << filename << ".txt\n";
    }
}

void Plot::save_svg(const std::string& filename) {
    calculate_auto_limits();

    std::ofstream file(filename);
    if (!file.is_open()) return;

    // Layout constants
    const double margin_left   = 70;
    const double margin_right  = 30;
    const double margin_top    = 50;
    const double margin_bottom = 60;
    const double plot_w = width  - margin_left - margin_right;
    const double plot_h = height - margin_top  - margin_bottom;

    double x_min = x_axis.min_value, x_max = x_axis.max_value;
    double y_min = y_axis.min_value, y_max = y_axis.max_value;
    // Avoid divide-by-zero
    if (x_max == x_min) { x_min -= 1; x_max += 1; }
    if (y_max == y_min) { y_min -= 1; y_max += 1; }
    // Add 5% padding on y top
    y_max += (y_max - y_min) * 0.05;

    // Helper lambdas: data coords -> SVG coords
    auto sx = [&](double v) { return margin_left + (v - x_min) / (x_max - x_min) * plot_w; };
    auto sy = [&](double v) { return margin_top  + (1.0 - (v - y_min) / (y_max - y_min)) * plot_h; };

    // Series colors
    static const char* palette[] = {
        "#2196F3","#F44336","#4CAF50","#FF9800",
        "#9C27B0","#00BCD4","#FFEB3B","#795548"
    };

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg width=\"" << width << "\" height=\"" << height
         << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    // Background
    file << "  <rect width=\"" << width << "\" height=\"" << height << "\" fill=\"#ffffff\"/>\n";
    // Plot area background
    file << "  <rect x=\"" << margin_left << "\" y=\"" << margin_top
         << "\" width=\"" << plot_w << "\" height=\"" << plot_h
         << "\" fill=\"#f8f9fa\" stroke=\"#cccccc\" stroke-width=\"1\"/>\n";

    // Grid lines + tick labels
    const int num_ticks = 5;
    file << "  <g stroke=\"#dddddd\" stroke-width=\"1\" stroke-dasharray=\"4,3\">\n";
    for (int i = 0; i <= num_ticks; i++) {
        double frac = (double)i / num_ticks;
        // Horizontal grid
        double gy = margin_top + frac * plot_h;
        file << "    <line x1=\"" << margin_left << "\" y1=\"" << gy
             << "\" x2=\"" << (margin_left + plot_w) << "\" y2=\"" << gy << "\"/>\n";
        // Vertical grid
        double gx = margin_left + frac * plot_w;
        file << "    <line x1=\"" << gx << "\" y1=\"" << margin_top
             << "\" x2=\"" << gx << "\" y2=\"" << (margin_top + plot_h) << "\"/>\n";
    }
    file << "  </g>\n";

    // Axis lines
    file << "  <g stroke=\"#333333\" stroke-width=\"2\">\n";
    // X axis
    file << "    <line x1=\"" << margin_left << "\" y1=\"" << (margin_top + plot_h)
         << "\" x2=\"" << (margin_left + plot_w) << "\" y2=\"" << (margin_top + plot_h) << "\"/>\n";
    // Y axis
    file << "    <line x1=\"" << margin_left << "\" y1=\"" << margin_top
         << "\" x2=\"" << margin_left << "\" y2=\"" << (margin_top + plot_h) << "\"/>\n";
    file << "  </g>\n";

    // Tick marks and labels
    file << "  <g font-family=\"Arial,sans-serif\" font-size=\"11\" fill=\"#444444\">\n";
    for (int i = 0; i <= num_ticks; i++) {
        double frac = (double)i / num_ticks;
        // X ticks
        double xv = x_min + frac * (x_max - x_min);
        double gx = margin_left + frac * plot_w;
        double gy_base = margin_top + plot_h;
        file << "    <line x1=\"" << gx << "\" y1=\"" << gy_base
             << "\" x2=\"" << gx << "\" y2=\"" << (gy_base + 5)
             << "\" stroke=\"#333333\" stroke-width=\"1\"/>\n";
        // Format tick value
        std::ostringstream xss;
        if (std::abs(xv) >= 1000 || (std::abs(xv) < 0.01 && xv != 0))
            xss << std::scientific;
        xss.precision(3);
        xss << xv;
        file << "    <text x=\"" << gx << "\" y=\"" << (gy_base + 18)
             << "\" text-anchor=\"middle\">" << xss.str() << "</text>\n";
        // Y ticks
        double yv = y_min + (1.0 - frac) * (y_max - y_min);
        double gy = margin_top + frac * plot_h;
        file << "    <line x1=\"" << (margin_left - 5) << "\" y1=\"" << gy
             << "\" x2=\"" << margin_left << "\" y2=\"" << gy
             << "\" stroke=\"#333333\" stroke-width=\"1\"/>\n";
        std::ostringstream yss;
        if (std::abs(yv) >= 10000 || (std::abs(yv) < 0.01 && yv != 0))
            yss << std::scientific;
        yss.precision(3);
        yss << yv;
        file << "    <text x=\"" << (margin_left - 8) << "\" y=\"" << (gy + 4)
             << "\" text-anchor=\"end\">" << yss.str() << "</text>\n";
    }
    file << "  </g>\n";

    // Axis labels
    file << "  <text x=\"" << (margin_left + plot_w / 2) << "\" y=\"" << (height - 10)
         << "\" text-anchor=\"middle\" font-family=\"Arial,sans-serif\" font-size=\"13\" fill=\"#222222\">"
         << x_axis.label << "</text>\n";
    // Y label (rotated)
    file << "  <text transform=\"rotate(-90)\" x=\"" << -(margin_top + plot_h / 2)
         << "\" y=\"18\" text-anchor=\"middle\" font-family=\"Arial,sans-serif\" font-size=\"13\" fill=\"#222222\">"
         << y_axis.label << "</text>\n";

    // Title
    if (!title.empty()) {
        file << "  <text x=\"" << (margin_left + plot_w / 2) << "\" y=\"" << (margin_top - 12)
             << "\" text-anchor=\"middle\" font-family=\"Arial,sans-serif\" font-size=\"16\" font-weight=\"bold\" fill=\"#111111\">"
             << title << "</text>\n";
    }

    // Clip region for data
    file << "  <clipPath id=\"plotarea\">\n";
    file << "    <rect x=\"" << margin_left << "\" y=\"" << margin_top
         << "\" width=\"" << plot_w << "\" height=\"" << plot_h << "\"/>\n";
    file << "  </clipPath>\n";

    // Draw series data
    for (size_t s_idx = 0; s_idx < series.size(); s_idx++) {
        const auto& s = series[s_idx];
        const char* col = palette[s_idx % 8];
        size_t n = std::min(s->x_data.size(), s->y_data.size());
        if (n == 0) continue;

        file << "  <g clip-path=\"url(#plotarea)\">\n";

        if (chart_type == ChartType::HISTOGRAM) {
            // Each bar spans exactly from its left bin edge to its right bin edge.
            // Bin edges are midpoints between adjacent centers; outermost edges
            // are already accounted for in calculate_auto_limits.
            double bin_w = n > 1 ? (s->x_data[1] - s->x_data[0]) : (x_max - x_min);
            for (size_t i = 0; i < n; i++) {
                double left  = sx(s->x_data[i] - bin_w / 2.0);
                double right = sx(s->x_data[i] + bin_w / 2.0);
                double bw = right - left;
                double by = sy(s->y_data[i]);
                double bh = (margin_top + plot_h) - by;
                if (bh < 0) { by = margin_top + plot_h; bh = -bh; }
                file << "    <rect x=\"" << left << "\" y=\"" << by
                     << "\" width=\"" << bw << "\" height=\"" << bh
                     << "\" fill=\"" << col << "\" opacity=\"0.85\""
                     << " stroke=\"white\" stroke-width=\"1\"/>\n";
            }
        } else if (chart_type == ChartType::BAR) {
            // Grouped bar chart — fixed pixel width with small gaps
            double bar_w = (plot_w / (double)n) * 0.7 / (double)series.size();
            for (size_t i = 0; i < n; i++) {
                double cx = sx(s->x_data[i]);
                double offset = (s_idx - (series.size() - 1) / 2.0) * bar_w;
                double bx = cx + offset - bar_w / 2.0;
                double by = sy(s->y_data[i]);
                double bh = (margin_top + plot_h) - by;
                if (bh < 0) { by = margin_top + plot_h; bh = -bh; }
                file << "    <rect x=\"" << bx << "\" y=\"" << by
                     << "\" width=\"" << bar_w << "\" height=\"" << bh
                     << "\" fill=\"" << col << "\" opacity=\"0.85\""
                     << " stroke=\"white\" stroke-width=\"1\"/>\n";
            }
        } else if (chart_type == ChartType::LINE) {
            // Polyline
            file << "    <polyline fill=\"none\" stroke=\"" << col
                 << "\" stroke-width=\"2\" stroke-linejoin=\"round\" points=\"";
            for (size_t i = 0; i < n; i++) {
                file << sx(s->x_data[i]) << "," << sy(s->y_data[i]);
                if (i + 1 < n) file << " ";
            }
            file << "\"/>\n";
            // Dots on data points
            for (size_t i = 0; i < n; i++) {
                file << "    <circle cx=\"" << sx(s->x_data[i]) << "\" cy=\"" << sy(s->y_data[i])
                     << "\" r=\"3\" fill=\"" << col << "\"/>\n";
            }
        } else {
            // Scatter
            for (size_t i = 0; i < n; i++) {
                file << "    <circle cx=\"" << sx(s->x_data[i]) << "\" cy=\"" << sy(s->y_data[i])
                     << "\" r=\"4\" fill=\"" << col << "\" opacity=\"0.8\"/>\n";
            }
        }

        file << "  </g>\n";
    }

    // Legend
    if (!series.empty()) {
        double lx = margin_left + plot_w - 10;
        double ly = margin_top + 10;
        double lw = 120, lh = series.size() * 20 + 10;
        file << "  <rect x=\"" << (lx - lw) << "\" y=\"" << ly
             << "\" width=\"" << lw << "\" height=\"" << lh
             << "\" fill=\"white\" stroke=\"#cccccc\" stroke-width=\"1\" rx=\"3\"/>\n";
        for (size_t i = 0; i < series.size(); i++) {
            const char* col = palette[i % 8];
            double ey = ly + 10 + i * 20 + 6;
            file << "  <rect x=\"" << (lx - lw + 8) << "\" y=\"" << (ey - 6)
                 << "\" width=\"14\" height=\"12\" fill=\"" << col << "\" rx=\"2\"/>\n";
            file << "  <text x=\"" << (lx - lw + 28) << "\" y=\"" << ey
                 << "\" font-family=\"Arial,sans-serif\" font-size=\"11\" fill=\"#333333\">"
                 << series[i]->label << "</text>\n";
        }
    }

    file << "</svg>\n";
    file.close();
    std::cout << "Plot saved to " << filename << "\n";
}

void Plot::save_pdf(const std::string& filename) {
    // For now, save as text file
    std::ofstream file(filename + ".txt");
    if (file.is_open()) {
        file << "Plot saved as text (PDF export not yet implemented)\n";
        file << "Title: " << title << "\n";
        file << "Series count: " << series.size() << "\n";
        file.close();
        std::cout << "Plot saved to " << filename << ".txt\n";
    }
}

// Convenience functions
std::shared_ptr<Plot> plot(const std::vector<double>& y, const std::string& title) {
    std::vector<double> x(y.size());
    for (size_t i = 0; i < y.size(); i++) {
        x[i] = static_cast<double>(i);
    }
    return plot(x, y, title);
}

std::shared_ptr<Plot> plot(const std::vector<double>& x, const std::vector<double>& y, 
                          const std::string& title) {
    auto p = std::make_shared<Plot>(800, 600, ChartType::LINE);
    p->add_line(x, y, "data", Color::Blue());
    p->set_title(title);
    return p;
}

std::shared_ptr<Plot> scatter(const std::vector<double>& x, const std::vector<double>& y,
                             const std::string& title) {
    auto p = std::make_shared<Plot>(800, 600, ChartType::SCATTER);
    p->add_scatter(x, y, "data", Color::Red(), MarkerStyle::CIRCLE);
    p->set_title(title);
    return p;
}

std::shared_ptr<Plot> bar_chart(const std::vector<double>& x, const std::vector<double>& y,
                                const std::string& title) {
    auto p = std::make_shared<Plot>(800, 600, ChartType::BAR);
    p->add_bar(x, y, "data", Color::Green());
    p->set_title(title);
    return p;
}

std::shared_ptr<Plot> histogram(const std::vector<double>& data, int bins,
                               const std::string& title) {
    auto p = std::make_shared<Plot>(800, 600, ChartType::HISTOGRAM);
    p->add_histogram(data, bins, "data", Color::Orange());
    p->set_title(title);
    return p;
}

// Color palettes
std::vector<Color> ColorPalette::Default() {
    return {
        Color::Blue(), Color::Red(), Color::Green(), Color::Orange(),
        Color::Purple(), Color::Cyan(), Color::Yellow(), Color::Gray()
    };
}

std::vector<Color> ColorPalette::Viridis() {
    return {
        Color(0.267, 0.004, 0.329), Color(0.282, 0.140, 0.457),
        Color(0.253, 0.265, 0.529), Color(0.206, 0.371, 0.553),
        Color(0.163, 0.471, 0.558), Color(0.127, 0.566, 0.550),
        Color(0.134, 0.658, 0.517), Color(0.266, 0.748, 0.440),
        Color(0.477, 0.821, 0.318), Color(0.741, 0.873, 0.149),
        Color(0.993, 0.906, 0.144)
    };
}

Color ColorPalette::get_color(size_t index, const std::vector<Color>& palette) {
    if (palette.empty()) return Color::Black();
    return palette[index % palette.size()];
}

} // namespace Plotting
} // namespace stdlib
} // namespace sapphire

// C API implementation
extern "C" {
    using namespace sapphire::stdlib::Plotting;
    
    // Global storage for plots to manage lifetime
    static std::vector<std::shared_ptr<Plot>> plot_storage;
    
    void* sapphire_plot_create(int width, int height, int chart_type) {
        ChartType type = static_cast<ChartType>(chart_type);
        auto plot = std::make_shared<Plot>(width, height, type);
        plot_storage.push_back(plot);
        return plot.get();
    }
    
    void sapphire_plot_destroy(void* plot) {
        // Remove from storage (simplified - in production would need better management)
        Plot* p = static_cast<Plot*>(plot);
        plot_storage.erase(
            std::remove_if(plot_storage.begin(), plot_storage.end(),
                          [p](const std::shared_ptr<Plot>& ptr) { return ptr.get() == p; }),
            plot_storage.end());
    }
    
    void sapphire_plot_add_line(void* plot, double* x_data, double* y_data, 
                               int data_size, const char* label, 
                               double r, double g, double b) {
        Plot* p = static_cast<Plot*>(plot);
        std::vector<double> x(x_data, x_data + data_size);
        std::vector<double> y(y_data, y_data + data_size);
        p->add_line(x, y, label ? label : "", Color(r, g, b));
    }
    
    void sapphire_plot_add_scatter(void* plot, double* x_data, double* y_data,
                                  int data_size, const char* label,
                                  double r, double g, double b, int marker_style) {
        Plot* p = static_cast<Plot*>(plot);
        std::vector<double> x(x_data, x_data + data_size);
        std::vector<double> y(y_data, y_data + data_size);
        p->add_scatter(x, y, label ? label : "", Color(r, g, b), 
                      static_cast<MarkerStyle>(marker_style));
    }
    
    void sapphire_plot_add_histogram(void* plot, double* data, int data_size,
                                    int bins, const char* label,
                                    double r, double g, double b) {
        Plot* p = static_cast<Plot*>(plot);
        std::vector<double> values(data, data + data_size);
        p->add_histogram(values, bins, label ? label : "", Color(r, g, b));
    }
    
    void sapphire_plot_set_title(void* plot, const char* title) {
        Plot* p = static_cast<Plot*>(plot);
        p->set_title(title ? title : "");
    }
    
    void sapphire_plot_set_xlabel(void* plot, const char* label) {
        Plot* p = static_cast<Plot*>(plot);
        p->set_xlabel(label ? label : "");
    }
    
    void sapphire_plot_set_ylabel(void* plot, const char* label) {
        Plot* p = static_cast<Plot*>(plot);
        p->set_ylabel(label ? label : "");
    }
    
    void sapphire_plot_show(void* plot) {
        Plot* p = static_cast<Plot*>(plot);
        p->show();
    }
    
    void sapphire_plot_save_svg(void* plot, const char* filename) {
        Plot* p = static_cast<Plot*>(plot);
        p->save_svg(filename ? filename : "plot.svg");
    }
    
    void* sapphire_quick_plot(double* y_data, int data_size, const char* title) {
        std::vector<double> y(y_data, y_data + data_size);
        auto p = plot(y, title ? title : "");
        plot_storage.push_back(p);
        return p.get();
    }
    
    void* sapphire_quick_scatter(double* x_data, double* y_data, int data_size, const char* title) {
        std::vector<double> x(x_data, x_data + data_size);
        std::vector<double> y(y_data, y_data + data_size);
        auto p = scatter(x, y, title ? title : "");
        plot_storage.push_back(p);
        return p.get();
    }
    
    void* sapphire_quick_histogram(double* data, int data_size, int bins, const char* title) {
        std::vector<double> values(data, data + data_size);
        auto p = histogram(values, bins, title ? title : "");
        plot_storage.push_back(p);
        return p.get();
    }
}