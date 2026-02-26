# Quick Fix: Git Authentication Error

## The Error

```
remote: Invalid username or token. Password authentication is not supported for Git operations.
fatal: Authentication failed for 'https://github.com/Velocity4705/sapphire-lang.git/'
```

## Quick Fix (Choose One)

### Fix 1: Use SSH (Easiest - 2 minutes)

```bash
# 1. Run the setup script
bash scripts/setup_git_ssh.sh

# 2. Follow the prompts to add your key to GitHub

# 3. Clone with SSH
git clone git@github.com:Velocity4705/sapphire-lang.git
cd sapphire-lang
make quick
```

### Fix 2: Use Personal Access Token (3 minutes)

```bash
# 1. Create token at: https://github.com/settings/tokens
#    - Click "Generate new token (classic)"
#    - Select "repo" scope
#    - Copy the token

# 2. Clone (use token as password)
git clone https://github.com/Velocity4705/sapphire-lang.git
# Username: Velocity4705
# Password: <paste your token>

# 3. Cache credentials
cd sapphire-lang
git config credential.helper cache

# 4. Build
make quick
```

### Fix 3: Already Cloned? Switch to SSH

```bash
# If you already cloned with HTTPS, switch to SSH:
cd sapphire-lang
git remote set-url origin git@github.com:Velocity4705/sapphire-lang.git

# Then setup SSH (if not done):
bash scripts/setup_git_ssh.sh
```

## Detailed Help

See [docs/GIT_SETUP.md](docs/GIT_SETUP.md) for complete instructions.

## Still Having Issues?

1. Check if SSH key is added:
   ```bash
   ssh -T git@github.com
   ```

2. Check remote URL:
   ```bash
   git remote -v
   ```

3. Clear cached credentials:
   ```bash
   git credential reject
   # Enter: protocol=https, host=github.com, then Ctrl+D
   ```

## Why This Happens

GitHub disabled password authentication in 2021. You must use:
- SSH keys (recommended)
- Personal Access Tokens (PAT)
- GitHub CLI

Regular passwords no longer work for Git operations.
