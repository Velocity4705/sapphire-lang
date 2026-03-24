" Vim syntax file for Sapphire (.spp)
" Language: Sapphire
" Maintainer: Sapphire Language Team
" Latest Revision: 2026

if exists("b:current_syntax")
  finish
endif

" ── Keywords ──────────────────────────────────────────────────────────────
syn keyword sapphireKeyword     if else elif for while break continue return
syn keyword sapphireKeyword     match case default in pass del
syn keyword sapphireKeyword     import from as
syn keyword sapphireDeclaration fn class trait impl macro enum
syn keyword sapphireModifier    pub priv static mut owned where dyn
syn keyword sapphireAsync       async await chan go select
syn keyword sapphireException   try catch finally throw raise
syn keyword sapphireType        int float double str bool void any list dict tuple set bytes char
syn keyword sapphireConstant    true false none None Some Ok Err Self

" ── Decorators ────────────────────────────────────────────────────────────
syn match sapphireDecorator     "@[a-zA-Z_][a-zA-Z0-9_]*"

" ── Doc comments ──────────────────────────────────────────────────────────
syn match sapphireDocComment    "///.*$"
syn match sapphireDocComment    "#!.*$"

" ── Comments ──────────────────────────────────────────────────────────────
syn match sapphireComment       "#[^!][^\n]*$"
syn match sapphireComment       "#$"

" ── Strings ───────────────────────────────────────────────────────────────
syn region sapphireFString      start=/f"/ end=/"/ contains=sapphireInterp,sapphireEscape
syn region sapphireFString      start=/f'/ end=/'/ contains=sapphireInterp,sapphireEscape
syn region sapphireString       start=/"""/ end=/"""/ contains=sapphireEscape
syn region sapphireString       start=/"/ end=/"/ contains=sapphireEscape
syn region sapphireString       start=/'/ end=/'/ contains=sapphireEscape
syn region sapphireInterp       start=/{/ end=/}/ contained contains=TOP
syn match  sapphireEscape       "\\." contained

" ── Numbers ───────────────────────────────────────────────────────────────
syn match sapphireNumber        "\b0[xX][0-9a-fA-F_]\+\b"
syn match sapphireNumber        "\b0[bB][01_]\+\b"
syn match sapphireNumber        "\b0[oO][0-7_]\+\b"
syn match sapphireNumber        "\b[0-9][0-9_]*\.[0-9][0-9_]*\([eE][+-]\?[0-9]\+\)\?\b"
syn match sapphireNumber        "\b[0-9][0-9_]*\b"

" ── Class / type names (PascalCase) ───────────────────────────────────────
syn match sapphireTypeName      "\b[A-Z][a-zA-Z0-9_]*\b"

" ── Function declarations and calls ──────────────────────────────────────
syn match sapphireFuncDecl      "\bfn\s\+\zs[a-zA-Z_][a-zA-Z0-9_]*"
syn match sapphireFuncCall      "\b[a-zA-Z_][a-zA-Z0-9_]*\ze\s*("

" ── Operators ─────────────────────────────────────────────────────────────
syn match sapphireOperator      "=>"
syn match sapphireOperator      "->"
syn match sapphireOperator      "<-"
syn match sapphireOperator      "\*\*"
syn match sapphireOperator      "==\|!=\|<=\|>=\|&&\|||"
syn match sapphireOperator      "[+\-*/%<>!&|^~=?]"

" ── Highlight links ───────────────────────────────────────────────────────
hi def link sapphireKeyword     Keyword
hi def link sapphireDeclaration Keyword
hi def link sapphireModifier    StorageClass
hi def link sapphireAsync       Keyword
hi def link sapphireException   Exception
hi def link sapphireType        Type
hi def link sapphireConstant    Constant
hi def link sapphireDecorator   PreProc
hi def link sapphireDocComment  SpecialComment
hi def link sapphireComment     Comment
hi def link sapphireFString     String
hi def link sapphireString      String
hi def link sapphireInterp      Special
hi def link sapphireEscape      SpecialChar
hi def link sapphireNumber      Number
hi def link sapphireTypeName    Type
hi def link sapphireFuncDecl    Function
hi def link sapphireFuncCall    Function
hi def link sapphireOperator    Operator

let b:current_syntax = "sapphire"
