#!/bin/bash
# Build script for graphics examples

if [ $# -eq 0 ]; then
    echo "Usage: ./build_example.sh <example_name>"
    echo ""
    echo "Available examples:"
    echo "  game_pong       - Classic Pong game"
    echo "  game_snake      - Snake game"
    echo "  app_paint       - Paint application"
    echo "  demo_particles  - Particle system demo"
    echo "  demo_3d_cube    - 3D rotating cube"
    echo ""
    echo "Example: ./build_example.sh game_pong"
    exit 1
fi

EXAMPLE=$1
SOURCE="examples/graphics_tests/${EXAMPLE}.spp"
OUTPUT="${EXAMPLE}"

# Check if source exists
if [ ! -f "$SOURCE" ]; then
    echo "Error: $SOURCE not found"
    exit 1
fi

echo "Building $EXAMPLE..."
echo ""

# Step 1: Compile to LLVM IR
echo "[1/4] Compiling to LLVM IR..."
./sapp compile "$SOURCE" 2>/dev/null > "${OUTPUT}.ll"
if [ $? -ne 0 ]; then
    echo "Error: Compilation failed"
    ./sapp compile "$SOURCE" 2>&1 | grep -i error | head -5
    exit 1
fi
echo "✓ LLVM IR generated"

# Step 2: Compile to object file
echo "[2/4] Compiling to object file..."
llc "${OUTPUT}.ll" -filetype=obj -o "${OUTPUT}.o" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "Error: Object file generation failed"
    exit 1
fi
echo "✓ Object file created"

# Step 3: Link
echo "[3/4] Linking..."

# Determine which libraries to link based on example
if [[ "$EXAMPLE" == demo_3d_cube ]]; then
    # OpenGL + GLUT
    clang "${OUTPUT}.o" -o "$OUTPUT" \
        -L./stdlib/graphics/opengl \
        -L./stdlib/graphics/glut \
        -lsapphire_opengl \
        -lsapphire_glut \
        -lGL \
        -lGLU \
        -lglut \
        -lstdc++ 2>/dev/null
    LIBPATH="./stdlib/graphics/opengl:./stdlib/graphics/glut"
else
    # SDL2
    clang "${OUTPUT}.o" -o "$OUTPUT" \
        -L./stdlib/graphics/sdl2 \
        -lsapphire_sdl2 \
        -lSDL2 \
        -lstdc++ 2>/dev/null
    LIBPATH="./stdlib/graphics/sdl2"
fi

if [ $? -ne 0 ]; then
    echo "Error: Linking failed"
    exit 1
fi
echo "✓ Executable created"

# Step 4: Cleanup intermediate files
echo "[4/4] Cleaning up..."
rm -f "${OUTPUT}.ll" "${OUTPUT}.o"
echo "✓ Build complete"

echo ""
echo "Success! Run with:"
echo "  LD_LIBRARY_PATH=${LIBPATH}:\$LD_LIBRARY_PATH ./$OUTPUT"
echo ""
echo "Or simply:"
echo "  ./$OUTPUT"
echo ""
