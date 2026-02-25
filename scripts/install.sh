#!/bin/bash
# Sapphire installation script

set -e

PREFIX="${PREFIX:-/usr/local}"

echo "Installing Sapphire to $PREFIX"

# Check if build exists
if [ ! -f "build/sapphire" ]; then
    echo "Error: Build not found. Run ./scripts/build.sh first"
    exit 1
fi

# Install binaries
sudo install -m 755 build/sapphire "$PREFIX/bin/"
sudo install -m 755 build/tools/spm/spm "$PREFIX/bin/" 2>/dev/null || true

# Install libraries
sudo mkdir -p "$PREFIX/lib/sapphire"
sudo cp -r stdlib "$PREFIX/lib/sapphire/"
sudo cp build/libsapphire_runtime.so "$PREFIX/lib/" 2>/dev/null || true

# Install examples
sudo mkdir -p "$PREFIX/share/sapphire"
sudo cp -r examples "$PREFIX/share/sapphire/"

# Install man pages
sudo mkdir -p "$PREFIX/share/man/man1"
# sudo cp docs/man/sapphire.1 "$PREFIX/share/man/man1/" 2>/dev/null || true

echo "✓ Installation complete!"
echo ""
echo "Run 'sapphire --version' to verify installation"
