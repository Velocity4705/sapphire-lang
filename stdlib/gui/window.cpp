#include "window.h"
#include <iostream>

namespace sapphire {
namespace stdlib {

static bool sdl_initialized = false;
static bool ttf_initialized = false;

bool init_gui() {
    if (!sdl_initialized) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        sdl_initialized = true;
    }
    
    if (!ttf_initialized) {
        if (TTF_Init() < 0) {
            std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
            return false;
        }
        ttf_initialized = true;
    }
    
    return true;
}

void quit_gui() {
    if (ttf_initialized) {
        TTF_Quit();
        ttf_initialized = false;
    }
    if (sdl_initialized) {
        SDL_Quit();
        sdl_initialized = false;
    }
}

Window::Window(const std::string& title, int width, int height)
    : window_(nullptr), renderer_(nullptr), title_(title),
      width_(width), height_(height), is_open_(false) {
    
    // Initialize SDL if not already done
    if (!init_gui()) {
        throw std::runtime_error("Failed to initialize GUI system");
    }
    
    // Create window
    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
    );
    
    if (!window_) {
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
    }
    
    // Create renderer
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        SDL_DestroyWindow(window_);
        throw std::runtime_error(std::string("Failed to create renderer: ") + SDL_GetError());
    }
    
    is_open_ = true;
}

Window::~Window() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
}

void Window::show() {
    if (window_) {
        SDL_ShowWindow(window_);
    }
}

void Window::hide() {
    if (window_) {
        SDL_HideWindow(window_);
    }
}

void Window::close() {
    is_open_ = false;
}

bool Window::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            is_open_ = false;
            return false;
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                is_open_ = false;
                return false;
            }
        }
    }
    return is_open_;
}

void Window::clear(uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_RenderClear(renderer_);
}

void Window::present() {
    SDL_RenderPresent(renderer_);
}

void Window::draw_text(const std::string& text, int x, int y, int size,
                       uint8_t r, uint8_t g, uint8_t b) {
    // Try to load a common font
    const char* font_paths[] = {
        // Linux - Fedora/RHEL/CentOS paths
        "/usr/share/fonts/dejavu-sans-mono-fonts/DejaVuSansMono.ttf",
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf",
        // Linux - Debian/Ubuntu paths
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        // macOS
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        // Windows
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf",
        nullptr
    };
    
    TTF_Font* font = nullptr;
    for (int i = 0; font_paths[i] != nullptr; i++) {
        font = TTF_OpenFont(font_paths[i], size);
        if (font) break;
    }
    
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Color color = {r, g, b, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        TTF_CloseFont(font);
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture) {
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer_, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }
    
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}

void Window::draw_rect(int x, int y, int w, int h,
                       uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer_, &rect);
}

void Window::fill_rect(int x, int y, int w, int h,
                       uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer_, &rect);
}

void Window::set_title(const std::string& title) {
    title_ = title;
    if (window_) {
        SDL_SetWindowTitle(window_, title.c_str());
    }
}

} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    bool sapphire_gui_init() {
        return sapphire::stdlib::init_gui();
    }
    
    void sapphire_gui_quit() {
        sapphire::stdlib::quit_gui();
    }
    
    void* sapphire_window_create(const char* title, int width, int height) {
        try {
            return new sapphire::stdlib::Window(title, width, height);
        } catch (...) {
            return nullptr;
        }
    }
    
    void sapphire_window_destroy(void* window) {
        delete static_cast<sapphire::stdlib::Window*>(window);
    }
    
    void sapphire_window_show(void* window) {
        static_cast<sapphire::stdlib::Window*>(window)->show();
    }
    
    void sapphire_window_close(void* window) {
        static_cast<sapphire::stdlib::Window*>(window)->close();
    }
    
    bool sapphire_window_is_open(void* window) {
        return static_cast<sapphire::stdlib::Window*>(window)->is_open();
    }
    
    bool sapphire_window_poll_events(void* window) {
        return static_cast<sapphire::stdlib::Window*>(window)->poll_events();
    }
    
    void sapphire_window_clear(void* window, uint8_t r, uint8_t g, uint8_t b) {
        static_cast<sapphire::stdlib::Window*>(window)->clear(r, g, b);
    }
    
    void sapphire_window_present(void* window) {
        static_cast<sapphire::stdlib::Window*>(window)->present();
    }
    
    void sapphire_window_draw_text(void* window, const char* text, int x, int y, int size,
                                   uint8_t r, uint8_t g, uint8_t b) {
        static_cast<sapphire::stdlib::Window*>(window)->draw_text(text, x, y, size, r, g, b);
    }
    
    void sapphire_window_fill_rect(void* window, int x, int y, int w, int h,
                                   uint8_t r, uint8_t g, uint8_t b) {
        static_cast<sapphire::stdlib::Window*>(window)->fill_rect(x, y, w, h, r, g, b);
    }
}
