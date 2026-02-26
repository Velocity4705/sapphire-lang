#!/bin/bash

# Quick clone script for Sapphire
# Handles authentication automatically

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║         Sapphire Quick Clone                                  ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Check if SSH key exists
if [ -f ~/.ssh/id_ed25519 ] || [ -f ~/.ssh/id_rsa ]; then
    echo "✓ SSH key found"
    echo "Testing GitHub connection..."
    
    if ssh -T git@github.com 2>&1 | grep -q "successfully authenticated"; then
        echo "✓ SSH authentication working!"
        echo ""
        echo "Cloning with SSH..."
        git clone git@github.com:Velocity4705/sapphire-lang.git
        cd sapphire-lang
        echo ""
        echo "Building Sapphire..."
        make quick
        echo ""
        echo "✓ Done! Run your first program:"
        echo "  ./sapp examples/hello.spp"
        exit 0
    else
        echo "⚠ SSH key exists but not configured with GitHub"
        echo ""
        read -p "Setup SSH now? (Y/n): " setup_ssh
        if [[ ! $setup_ssh =~ ^[Nn]$ ]]; then
            bash scripts/setup_git_ssh.sh
            exit 0
        fi
    fi
fi

# No SSH, try HTTPS
echo "No SSH key found. Using HTTPS..."
echo ""
echo "⚠ HTTPS requires a Personal Access Token"
echo "  Create one at: https://github.com/settings/tokens"
echo ""
read -p "Do you have a token? (y/N): " has_token

if [[ $has_token =~ ^[Yy]$ ]]; then
    echo ""
    echo "Cloning with HTTPS..."
    git clone https://github.com/Velocity4705/sapphire-lang.git
    
    if [ $? -eq 0 ]; then
        cd sapphire-lang
        echo ""
        echo "Caching credentials..."
        git config credential.helper cache
        echo ""
        echo "Building Sapphire..."
        make quick
        echo ""
        echo "✓ Done! Run your first program:"
        echo "  ./sapp examples/hello.spp"
    else
        echo ""
        echo "✗ Clone failed. Try setting up SSH instead:"
        echo "  bash scripts/setup_git_ssh.sh"
    fi
else
    echo ""
    echo "Please choose one:"
    echo ""
    echo "Option 1: Setup SSH (Recommended)"
    echo "  bash scripts/setup_git_ssh.sh"
    echo ""
    echo "Option 2: Create Personal Access Token"
    echo "  1. Go to: https://github.com/settings/tokens"
    echo "  2. Click 'Generate new token (classic)'"
    echo "  3. Select 'repo' scope"
    echo "  4. Copy the token"
    echo "  5. Run this script again"
    echo ""
    echo "See docs/GIT_SETUP.md for detailed help"
fi
