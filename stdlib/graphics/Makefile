# Makefile for Sapphire Graphics Libraries
# Builds SDL2, OpenGL, GLUT, and Vulkan libraries

CXX = g++
CXXFLAGS = -std=c++17 -fPIC -O2 -Wall
AR = ar
ARFLAGS = rcs

# Directories
SDL2_DIR = sdl2
OPENGL_DIR = opengl
GLUT_DIR = glut
VULKAN_DIR = vulkan

# Output libraries
SDL2_LIB = $(SDL2_DIR)/libsapphire_sdl2.a
SDL2_SO = $(SDL2_DIR)/libsapphire_sdl2.so
OPENGL_LIB = $(OPENGL_DIR)/libsapphire_opengl.a
OPENGL_SO = $(OPENGL_DIR)/libsapphire_opengl.so
GLUT_LIB = $(GLUT_DIR)/libsapphire_glut.a
GLUT_SO = $(GLUT_DIR)/libsapphire_glut.so
VULKAN_LIB = $(VULKAN_DIR)/libsapphire_vulkan.a
VULKAN_SO = $(VULKAN_DIR)/libsapphire_vulkan.so

# Object files
SDL2_OBJ = $(SDL2_DIR)/sdl2.o
OPENGL_OBJ = $(OPENGL_DIR)/opengl.o
GLUT_OBJ = $(GLUT_DIR)/glut.o
VULKAN_OBJ = $(VULKAN_DIR)/vulkan.o

# Link flags
SDL2_LDFLAGS = -lSDL2
OPENGL_LDFLAGS = -lGL -lGLU
GLUT_LDFLAGS = -lglut -lGL -lGLU
VULKAN_LDFLAGS = -lvulkan -lSDL2

.PHONY: all clean sdl2 opengl glut vulkan static shared

# Build all libraries (static and shared)
all: static shared

# Build only static libraries
static: $(SDL2_LIB) $(OPENGL_LIB) $(GLUT_LIB) $(VULKAN_LIB)

# Build only shared libraries
shared: $(SDL2_SO) $(OPENGL_SO) $(GLUT_SO) $(VULKAN_SO)

# SDL2 Library
sdl2: $(SDL2_LIB) $(SDL2_SO)

$(SDL2_OBJ): $(SDL2_DIR)/sdl2.cpp $(SDL2_DIR)/sdl2.h
	$(CXX) $(CXXFLAGS) -I$(SDL2_DIR) -c $< -o $@

$(SDL2_LIB): $(SDL2_OBJ)
	$(AR) $(ARFLAGS) $@ $^
	@echo "✅ Built SDL2 static library: $@"

$(SDL2_SO): $(SDL2_OBJ)
	$(CXX) -shared -o $@ $^ $(SDL2_LDFLAGS)
	@echo "✅ Built SDL2 shared library: $@"

# OpenGL Library
opengl: $(OPENGL_LIB) $(OPENGL_SO)

$(OPENGL_OBJ): $(OPENGL_DIR)/opengl.cpp $(OPENGL_DIR)/opengl.h
	$(CXX) $(CXXFLAGS) -I$(OPENGL_DIR) -c $< -o $@

$(OPENGL_LIB): $(OPENGL_OBJ)
	$(AR) $(ARFLAGS) $@ $^
	@echo "✅ Built OpenGL static library: $@"

$(OPENGL_SO): $(OPENGL_OBJ)
	$(CXX) -shared -o $@ $^ $(OPENGL_LDFLAGS)
	@echo "✅ Built OpenGL shared library: $@"

# GLUT Library
glut: $(GLUT_LIB) $(GLUT_SO)

$(GLUT_OBJ): $(GLUT_DIR)/glut.cpp $(GLUT_DIR)/glut.h
	$(CXX) $(CXXFLAGS) -I$(GLUT_DIR) -c $< -o $@

$(GLUT_LIB): $(GLUT_OBJ)
	$(AR) $(ARFLAGS) $@ $^
	@echo "✅ Built GLUT static library: $@"

$(GLUT_SO): $(GLUT_OBJ)
	$(CXX) -shared -o $@ $^ $(GLUT_LDFLAGS)
	@echo "✅ Built GLUT shared library: $@"

# Vulkan Library
vulkan: $(VULKAN_LIB) $(VULKAN_SO)

$(VULKAN_OBJ): $(VULKAN_DIR)/vulkan.cpp $(VULKAN_DIR)/vulkan.h
	$(CXX) $(CXXFLAGS) -I$(VULKAN_DIR) -c $< -o $@

$(VULKAN_LIB): $(VULKAN_OBJ)
	$(AR) $(ARFLAGS) $@ $^
	@echo "✅ Built Vulkan static library: $@"

$(VULKAN_SO): $(VULKAN_OBJ)
	$(CXX) -shared -o $@ $^ $(VULKAN_LDFLAGS)
	@echo "✅ Built Vulkan shared library: $@"

# Clean build artifacts
clean:
	rm -f $(SDL2_OBJ) $(SDL2_LIB) $(SDL2_SO)
	rm -f $(OPENGL_OBJ) $(OPENGL_LIB) $(OPENGL_SO)
	rm -f $(GLUT_OBJ) $(GLUT_LIB) $(GLUT_SO)
	rm -f $(VULKAN_OBJ) $(VULKAN_LIB) $(VULKAN_SO)
	@echo "🧹 Cleaned all build artifacts"

# Install libraries (optional)
install: all
	@echo "Installing libraries to /usr/local/lib..."
	sudo cp $(SDL2_SO) $(OPENGL_SO) $(GLUT_SO) $(VULKAN_SO) /usr/local/lib/
	sudo ldconfig
	@echo "✅ Libraries installed"

# Show library info
info:
	@echo "=== Sapphire Graphics Libraries ==="
	@echo ""
	@echo "SDL2:"
	@ls -lh $(SDL2_LIB) $(SDL2_SO) 2>/dev/null || echo "  Not built"
	@echo ""
	@echo "OpenGL:"
	@ls -lh $(OPENGL_LIB) $(OPENGL_SO) 2>/dev/null || echo "  Not built"
	@echo ""
	@echo "GLUT:"
	@ls -lh $(GLUT_LIB) $(GLUT_SO) 2>/dev/null || echo "  Not built"
	@echo ""
	@echo "Vulkan:"
	@ls -lh $(VULKAN_LIB) $(VULKAN_SO) 2>/dev/null || echo "  Not built"

# Help
help:
	@echo "Sapphire Graphics Libraries Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build all libraries (static and shared)"
	@echo "  static   - Build only static libraries (.a)"
	@echo "  shared   - Build only shared libraries (.so)"
	@echo "  sdl2     - Build SDL2 library"
	@echo "  opengl   - Build OpenGL library"
	@echo "  glut     - Build GLUT library"
	@echo "  vulkan   - Build Vulkan library"
	@echo "  clean    - Remove all build artifacts"
	@echo "  install  - Install shared libraries to /usr/local/lib"
	@echo "  info     - Show library information"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make all          # Build everything"
	@echo "  make sdl2         # Build only SDL2"
	@echo "  make clean all    # Clean and rebuild"
