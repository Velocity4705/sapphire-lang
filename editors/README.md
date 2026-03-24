# Sapphire Editor Support

Syntax highlighting for Sapphire (`.spp`) across all major editors.

## VSCode

The extension lives in `editors/vscode/`. It provides:
- Syntax highlighting
- LSP (autocomplete, go-to-definition, find references, diagnostics)
- Debug adapter

**Install (one command):**
```bash
code --install-extension editors/vscode/sapphire-lang.vsix
```

Or build and install the latest yourself:
```bash
cd editors/vscode
npx @vscode/vsce package --no-dependencies
code --install-extension sapphire-lang-*.vsix
```

Then reload VSCode (`Ctrl+Shift+P` → "Reload Window").

---

## Vim / Neovim

**Install (manual):**
```bash
mkdir -p ~/.vim/syntax ~/.vim/ftdetect
cp editors/vim/syntax/sapphire.vim ~/.vim/syntax/
cp editors/vim/ftdetect/sapphire.vim ~/.vim/ftdetect/
```

**Install (vim-plug):**
```vim
Plug 'sapphire-lang/sapphire-vim'
```

**Install (lazy.nvim):**
```lua
{ 'sapphire-lang/sapphire-vim' }
```

---

## Sublime Text

Copy the syntax file to your Sublime Text packages:
```bash
# macOS
cp editors/sublime/Sapphire.sublime-syntax \
   ~/Library/Application\ Support/Sublime\ Text/Packages/User/

# Linux
cp editors/sublime/Sapphire.sublime-syntax \
   ~/.config/sublime-text/Packages/User/
```

---

## Nano

```bash
cp editors/nano/sapphire.nanorc ~/.config/nano/
echo 'include "~/.config/nano/sapphire.nanorc"' >> ~/.nanorc
```

---

## Emacs

```elisp
(add-to-list 'load-path "/path/to/editors/emacs")
(require 'sapphire-mode)
```

Or with `use-package`:
```elisp
(use-package sapphire-mode
  :load-path "path/to/editors/emacs")
```

---

## What's highlighted

| Token | Example |
|---|---|
| Keywords | `if`, `fn`, `class`, `match`, `async` |
| Types | `int`, `str`, `bool`, `Vec`, `Option` |
| Constants | `true`, `false`, `none`, `Ok`, `Err` |
| Strings | `"hello"`, `'world'` |
| F-strings | `f"Hello {name}"` |
| Doc comments | `/// This is a doc comment` |
| Comments | `# regular comment` |
| Decorators | `@cache`, `@timing` |
| Numbers | `42`, `3.14`, `0xFF`, `0b1010` |
| Operators | `=>`, `->`, `<-`, `**`, `?` |
| Function names | `fn greet(...)` |
| Type names | `class Dog`, `trait Animal` |
