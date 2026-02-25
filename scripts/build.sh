#!/bin/bash
# Sapphire build script

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building Sapphire...${NC}"

# Create build directory
mkdir -p build
cd build

# Configure
echo -e "${YELLOW}Configuring...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_TOOLS=ON

# Build
echo -e "${YELLOW}Compiling...${NC}"
make -j$(nproc)

# Run tests
echo -e "${YELLOW}Running tests...${NC}"
ctest --output-on-failure

echo -e "${GREEN}✓ Build complete!${NC}"
echo -e "Binary: ${PWD}/sapphire"
