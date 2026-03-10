#include "sdl2.h"
#include <stdexcept>
#include <iostream>

namespace sapphire {
namespace stdlib {
namespace graphics {

SDL2Window::SDL2Window(const std::string& title, int width, int height)
    : window_(nullptr), renderer_(nullptr), should_close_(false), 
      keyboard_state_(nullptr), width_(width), height_(height) {
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL2");
    }
    
    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_HIDDEN
    );
    
    if (!window_) {
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL2 window");
    }
    
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL2 renderer");
    }
    
    // Initialize keyboard state - pump events first to get clean state
    SDL_PumpEvents();
    keyboard_state_ = SDL_GetKeyboardState(nullptr);
}

SDL2Window::~SDL2Window() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

void SDL2Window::show() {
    SDL_ShowWindow(window_);
}

void SDL2Window::hide() {
    SDL_HideWindow(window_);
}

void SDL2Window::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            should_close_ = true;
        }
    }
}

bool SDL2Window::is_key_down(int scancode) const {
    if (!keyboard_state_ || scancode < 0 || scancode >= SDL_NUM_SCANCODES) {
        return false;
    }
    return keyboard_state_[scancode] != 0;
}

void SDL2Window::clear(uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_RenderClear(renderer_);
}

void SDL2Window::present() {
    SDL_RenderPresent(renderer_);
}

void SDL2Window::fill_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_RenderFillRect(renderer_, &rect);
}

void SDL2Window::draw_line(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
}

void SDL2Window::draw_point(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
    SDL_RenderDrawPoint(renderer_, x, y);
}

void SDL2Window::delay(uint32_t ms) {
    SDL_Delay(ms);
}

uint32_t SDL2Window::get_ticks() {
    return SDL_GetTicks();
}

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    void* sapphire_sdl2_create_window(const char* title, int width, int height) {
        try {
            return new sapphire::stdlib::graphics::SDL2Window(title, width, height);
        } catch (...) {
            return nullptr;
        }
    }
    
    void sapphire_sdl2_destroy_window(void* window) {
        if (window) {
            delete static_cast<sapphire::stdlib::graphics::SDL2Window*>(window);
        }
    }
    
    void sapphire_sdl2_show_window(void* window) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->show();
        }
    }
    
    void sapphire_sdl2_hide_window(void* window) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->hide();
        }
    }
    
    bool sapphire_sdl2_should_close(void* window) {
        if (window) {
            return static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->should_close();
        }
        return true;
    }
    
    int sapphire_sdl2_get_width(void* window) {
        if (window) {
            return static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->width();
        }
        return 0;
    }
    
    int sapphire_sdl2_get_height(void* window) {
        if (window) {
            return static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->height();
        }
        return 0;
    }
    
    void sapphire_sdl2_poll_events(void* window) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->poll_events();
        }
    }
    
    bool sapphire_sdl2_is_key_down(void* window, int64_t scancode) {
        if (window) {
            return static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->is_key_down(static_cast<int>(scancode));
        }
        return false;
    }
    
    void sapphire_sdl2_clear(void* window, uint8_t r, uint8_t g, uint8_t b) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->clear(r, g, b);
        }
    }
    
    void sapphire_sdl2_present(void* window) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->present();
        }
    }
    
    void sapphire_sdl2_fill_rect(void* window, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->fill_rect(x, y, w, h, r, g, b);
        }
    }
    
    void sapphire_sdl2_draw_line(void* window, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->draw_line(x1, y1, x2, y2, r, g, b);
        }
    }
    
    void sapphire_sdl2_draw_point(void* window, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        if (window) {
            static_cast<sapphire::stdlib::graphics::SDL2Window*>(window)->draw_point(x, y, r, g, b);
        }
    }
    
    void sapphire_sdl2_delay(uint32_t ms) {
        sapphire::stdlib::graphics::SDL2Window::delay(ms);
    }
    
    uint32_t sapphire_sdl2_get_ticks() {
        return sapphire::stdlib::graphics::SDL2Window::get_ticks();
    }
}
