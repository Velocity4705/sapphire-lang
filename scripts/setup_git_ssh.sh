#!/bin/bash

# Sapphire Git SSH Setup Script
# Automates SSH key generation and GitHub setup

set -e

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║         Sapphire Git SSH Setup                               ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Check if SSH key already exists
if [ -f ~/.ssh/id_ed25519 ]; then
    echo "✓ SSH key already exists at ~/.ssh/id_ed25519"
    echo ""
    read -p "Do you want to create a new key? (y/N): " create_new
    if [[ ! $create_new =~ ^[Yy]$ ]]; then
        echo "Using existing key..."
    else
        echo "Creating new key..."
        read -p "Enter your email: " email
        ssh-keygen -t ed25519 -C "$email"
    fi
else
    echo "No SSH key found. Creating one..."
    read -p "Enter your email: " email
    ssh-keygen -t ed25519 -C "$email"
fi

echo ""
echo "Starting SSH agent..."
eval "$(ssh-agent -s)"

echo "Adding SSH key to agent..."
ssh-add ~/.ssh/id_ed25519

echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  Your SSH Public Key (copy this to GitHub)                   ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""
cat ~/.ssh/id_ed25519.pub
echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  Next Steps:                                                  ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""
echo "1. Copy the key above (it's also copied to clipboard if available)"
echo "2. Go to: https://github.com/settings/ssh/new"
echo "3. Paste the key and give it a title (e.g., 'My Laptop')"
echo "4. Click 'Add SSH key'"
echo ""
echo "Then clone Sapphire with:"
echo "  git clone git@github.com:Velocity4705/sapphire-lang.git"
echo ""

# Try to copy to clipboard
if command -v xclip &> /dev/null; then
    cat ~/.ssh/id_ed25519.pub | xclip -selection clipboard
    echo "✓ Key copied to clipboard!"
elif command -v pbcopy &> /dev/null; then
    cat ~/.ssh/id_ed25519.pub | pbcopy
    echo "✓ Key copied to clipboard!"
elif command -v wl-copy &> /dev/null; then
    cat ~/.ssh/id_ed25519.pub | wl-copy
    echo "✓ Key copied to clipboard!"
else
    echo "Note: Install xclip, pbcopy, or wl-copy to auto-copy to clipboard"
fi

echo ""
read -p "Press Enter after adding the key to GitHub..."

echo ""
echo "Testing SSH connection to GitHub..."
if ssh -T git@github.com 2>&1 | grep -q "successfully authenticated"; then
    echo "✓ SSH connection successful!"
    echo ""
    echo "You're all set! Clone Sapphire with:"
    echo "  git clone git@github.com:Velocity4705/sapphire-lang.git"
else
    echo "⚠ SSH connection test failed. Please check:"
    echo "  1. Did you add the key to GitHub?"
    echo "  2. Did you save it correctly?"
    echo "  3. Try: ssh -T git@github.com"
fi

echo ""
echo "For more help, see: docs/GIT_SETUP.md"
