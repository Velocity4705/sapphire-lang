# Git Authentication Setup for Sapphire

## Problem

When cloning with HTTPS, you get:
```
remote: Invalid username or token. Password authentication is not supported for Git operations.
fatal: Authentication failed for 'https://github.com/velo4705/sapphire-lang.git/'
```

## Solution 1: Use SSH (Recommended)

### Step 1: Generate SSH Key

```bash
# Generate a new SSH key
ssh-keygen -t ed25519 -C "your_email@example.com"

# Press Enter to accept default location (~/.ssh/id_ed25519)
# Enter a passphrase (optional but recommended)
```

### Step 2: Add SSH Key to SSH Agent

```bash
# Start the SSH agent
eval "$(ssh-agent -s)"

# Add your SSH key
ssh-add ~/.ssh/id_ed25519
```

### Step 3: Add SSH Key to GitHub

```bash
# Copy your public key
cat ~/.ssh/id_ed25519.pub
```

Then:
1. Go to GitHub.com
2. Click your profile picture → Settings
3. Click "SSH and GPG keys"
4. Click "New SSH key"
5. Paste your key and save

### Step 4: Clone with SSH

```bash
# Use SSH URL instead of HTTPS
git clone git@github.com:velo4705/sapphire-lang.git
cd sapphire-lang
make quick
```

### Step 5: Update Existing Repository

If you already cloned with HTTPS:

```bash
cd sapphire-lang
git remote set-url origin git@github.com:velo4705/sapphire-lang.git
```

## Solution 2: Use Personal Access Token (PAT)

### Step 1: Create Personal Access Token

1. Go to GitHub.com
2. Click your profile picture → Settings
3. Click "Developer settings" (bottom left)
4. Click "Personal access tokens" → "Tokens (classic)"
5. Click "Generate new token" → "Generate new token (classic)"
6. Give it a name (e.g., "Sapphire Development")
7. Select scopes: `repo` (full control of private repositories)
8. Click "Generate token"
9. **Copy the token immediately** (you won't see it again!)

### Step 2: Clone with Token

```bash
# Use token as password
git clone https://github.com/velo4705/sapphire-lang.git
# Username: velo4705
# Password: <paste your token>
```

### Step 3: Cache Credentials

```bash
# Cache credentials for 1 hour
git config --global credential.helper 'cache --timeout=3600'

# Or store permanently (less secure)
git config --global credential.helper store
```

### Step 4: Update Stored Credentials

```bash
# If you already have wrong credentials stored
git credential reject
# Then enter:
protocol=https
host=github.com
# Press Ctrl+D

# Next git operation will ask for credentials again
```

## Solution 3: Use GitHub CLI (gh)

### Step 1: Install GitHub CLI

```bash
# On Ubuntu/Debian
sudo apt install gh

# On Fedora
sudo dnf install gh

# On macOS
brew install gh
```

### Step 2: Authenticate

```bash
gh auth login
# Follow the prompts:
# - Choose GitHub.com
# - Choose HTTPS or SSH
# - Authenticate via browser
```

### Step 3: Clone

```bash
gh repo clone velo4705/sapphire-lang
cd sapphire-lang
make quick
```

## Quick Start Commands

### For New Users (SSH - Recommended)

```bash
# 1. Generate SSH key
ssh-keygen -t ed25519 -C "your_email@example.com"

# 2. Copy public key
cat ~/.ssh/id_ed25519.pub
# Add this to GitHub: Settings → SSH and GPG keys

# 3. Clone
git clone git@github.com:velo4705/sapphire-lang.git
cd sapphire-lang
make quick
```

### For New Users (HTTPS with Token)

```bash
# 1. Create token at: https://github.com/settings/tokens

# 2. Clone
git clone https://github.com/velo4705/sapphire-lang.git
# Username: velo4705
# Password: <your token>

# 3. Cache credentials
git config --global credential.helper cache

# 4. Build
cd sapphire-lang
make quick
```

## Troubleshooting

### "Permission denied (publickey)"

```bash
# Test SSH connection
ssh -T git@github.com

# If it fails, check:
ssh-add -l  # List keys
ssh-add ~/.ssh/id_ed25519  # Add key if missing
```

### "Authentication failed"

```bash
# Clear cached credentials
git credential reject
# Enter:
protocol=https
host=github.com
# Press Ctrl+D

# Try again
git pull
```

### "Could not read from remote repository"

```bash
# Check remote URL
git remote -v

# Update to SSH
git remote set-url origin git@github.com:velo4705/sapphire-lang.git

# Or update to HTTPS
git remote set-url origin https://github.com/velo4705/sapphire-lang.git
```

## Update README.md

The README should show both options:

```markdown
## Quick Start

### Option 1: SSH (Recommended)

```bash
git clone git@github.com:velo4705/sapphire-lang.git
cd sapphire-lang
make quick
```

### Option 2: HTTPS

```bash
git clone https://github.com/velo4705/sapphire-lang.git
cd sapphire-lang
make quick
```

Note: HTTPS requires a Personal Access Token. See [Git Setup Guide](docs/GIT_SETUP.md).
```

## Security Best Practices

1. **Use SSH keys** - More secure than tokens
2. **Add passphrase to SSH keys** - Extra security layer
3. **Use credential cache** - Don't store permanently
4. **Rotate tokens regularly** - Generate new tokens every 90 days
5. **Limit token scopes** - Only grant necessary permissions
6. **Never commit tokens** - Add to .gitignore

## References

- [GitHub SSH Documentation](https://docs.github.com/en/authentication/connecting-to-github-with-ssh)
- [GitHub PAT Documentation](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token)
- [Git Credential Storage](https://git-scm.com/book/en/v2/Git-Tools-Credential-Storage)
