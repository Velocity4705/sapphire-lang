# GUI Window System Implementation Plan

## Goal

Create a cross-platform GUI window system that works on Linux, Windows, and macOS, with a simple test that pops up a window displaying "Hello Sapphire".

## Technology Choice

### Option 1: SDL2 (Recommended)
**Pros:**
- Cross-platform (Linux, Windows, macOS, and more)
- Mature and stable
- Good performance
- Easy to use
- Widely adopted in game development

**Cons:**
- Primarily for games, not native UI
- Custom UI components needed

### Option 2: GLFW
**Pros:**
- Lightweight
- OpenGL-focused
- Cross-platform

**Cons:**
- No built-in UI components
- More low-level

### Option 3: Qt
**Pros:**
- Native UI components
- Professional look
- Comprehensive

**Cons:**
- Large dependency
- Complex licensing
- Heavyweight

### Option 4: GTK
**Pros:**
- Native on Linux
- Good UI components

**Cons:**
- Less native on Windows/macOS
- Complex API

**Decision:** Use **SDL2** for initial implementation (games/graphics focus), then add native UI layer later.

## Implementation Plan

### Phase 1: SDL2 Integration (Week 1)

**Tasks:**
1. Add SDL2 dependency to build system
2. Create Window class wrapper
3. Implement basic window creation
4. Add event loop
5. Test on Linux

**Deliverables:**
- `stdlib/gui/sdl_window.h`
- `stdlib/gui/sdl_window.cpp`
- `stdlib/tests/test_window.cpp`

### Phase 2: Cross-Platform Testing (Week 2)

**Tasks:**
1. Test on Windows (if available)
2. Test on macOS (if available)
3. Fix platform-specific issues
4. Document platform requirements

**Deliverables:**
- Working window on all platforms
- Platform-specific documentation

### Phase 3: Basic UI (Week 3)

**Tasks:**
1. Add text rendering (SDL_ttf)
2. Create "Hello Sapphire" demo
3. Add mouse/keyboard input
4. Add window controls (close, resize)

**Deliverables:**
- Text rendering working
- Interactive demo
- Input handling

### Phase 4: UI Components (Week 4)

**Tasks:**
1. Implement Button
2. Implement Label
3. Implement TextBox
4. Create layout system

**Deliverables:**
- Basic UI component library
- Example applications

## Installation Requirements

### Linux (Fedora/RHEL)
```bash
sudo dnf install SDL2-devel SDL2_ttf-devel SDL2_image-devel
```

### Linux (Ubuntu/Debian)
```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
```

### macOS
```bash
brew install sdl2 sdl2_ttf sdl2_image
```

### Windows
- Download SDL2 development libraries from https://www.libsdl.org/
- Add to project include/lib paths

## Minimal Example

```cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int main() {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Hello Sapphire",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Load font
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 48);
    
    // Render text
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Hello Sapphire", white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // Event loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Draw text
        SDL_Rect rect = {250, 250, 300, 100};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        
        // Present
        SDL_RenderPresent(renderer);
        
        SDL_Delay(16); // ~60 FPS
    }
    
    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
```

## Sapphire API Design

```sapphire
import gui

# Create window
let window = Window("Hello Sapphire", 800, 600)

# Add text label
let label = Label("Hello Sapphire", 48)
label.set_position(250, 250)
window.add(label)

# Show window
window.show()

# Event loop
while window.is_open():
    window.handle_events()
    window.render()
    sleep(0.016)  # 60 FPS
```

## Testing Strategy

### Test 1: Window Creation
```bash
# Should pop up a black window
./test_window_basic
```

### Test 2: Text Rendering
```bash
# Should show "Hello Sapphire" in white text
./test_window_text
```

### Test 3: Input Handling
```bash
# Should respond to mouse clicks and keyboard
./test_window_input
```

### Test 4: Cross-Platform
```bash
# Should work on Linux, Windows, macOS
./test_window_cross_platform
```

## Performance Targets

- Window creation: < 100ms
- Frame rate: 60 FPS (16.67ms per frame)
- Input latency: < 10ms
- Memory usage: < 50MB for basic window

## Success Criteria

✅ Window pops up on Linux  
✅ Window pops up on Windows (if tested)  
✅ Window pops up on macOS (if tested)  
✅ "Hello Sapphire" text displays  
✅ Window can be closed  
✅ Window can be resized  
✅ 60 FPS rendering  

## Next Steps

1. Install SDL2 on current system
2. Create basic window test
3. Verify window pops up
4. Add text rendering
5. Test "Hello Sapphire" display

Would you like me to start implementing the SDL2 window system?
