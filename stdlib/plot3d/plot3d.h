#pragma once
#include <string>
#include <vector>

namespace sapphire { namespace stdlib { namespace Plot3D {

enum class Plot3DType { SURFACE, SCATTER, LINE, BAR3D };

struct Series3D {
    Plot3DType type;
    std::vector<double> x, y, z;
    // For surface: z is row-major grid, rows/cols define shape
    int rows = 0, cols = 0;
    std::string label;
    std::string colorscale = "Viridis"; // Viridis, Plasma, RdBu, Blues, etc.
    double opacity = 0.9;
};

class Plot3D {
public:
    Plot3D();
    void add_surface(const std::vector<double>& z_flat, int rows, int cols,
                     const std::string& label = "", const std::string& colorscale = "Viridis");
    void add_scatter(const std::vector<double>& x, const std::vector<double>& y,
                     const std::vector<double>& z, const std::string& label = "");
    void add_line(const std::vector<double>& x, const std::vector<double>& y,
                  const std::vector<double>& z, const std::string& label = "");
    void set_title(const std::string& t);
    void set_xlabel(const std::string& l);
    void set_ylabel(const std::string& l);
    void set_zlabel(const std::string& l);
    void save_html(const std::string& filename);

private:
    std::vector<Series3D> series;
    std::string title, xlabel = "X", ylabel = "Y", zlabel = "Z";
    std::string build_trace(const Series3D& s, size_t idx) const;
};

}}} // namespace

extern "C" {
    void* plot3d_create();
    void  plot3d_destroy(void* p);
    void  plot3d_add_surface(void* p, double* z_flat, int rows, int cols,
                             const char* label, const char* colorscale);
    void  plot3d_add_scatter(void* p, double* x, double* y, double* z,
                             int n, const char* label);
    void  plot3d_add_line(void* p, double* x, double* y, double* z,
                          int n, const char* label);
    void  plot3d_set_title(void* p, const char* title);
    void  plot3d_set_xlabel(void* p, const char* l);
    void  plot3d_set_ylabel(void* p, const char* l);
    void  plot3d_set_zlabel(void* p, const char* l);
    void  plot3d_save_html(void* p, const char* filename);
}
