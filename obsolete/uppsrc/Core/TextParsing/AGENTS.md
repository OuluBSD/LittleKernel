AGENTS

Scope
- Applies to `uppsrc/Core/TextParsing`.

Purpose
- Consolidates lightweight string utilities, tokenizers, and textual parsers migrated from `Core2`.
- Supplies lexer-style helpers for higher layers (Vfs, IDE tooling, configuration readers).

Responsibilities
- Destination for `String.h`, `Tokenizer.*`, `TokenParser.*`, `Html.*`, `Url.*`, and related helpers.
- Document parsing assumptions (encoding, whitespace rules, error handling) for each group of utilities.

Guidelines
- Keep the package gui-free and independent of Vfs-specific data structures.
- Where parsing is performance sensitive, record known hot paths and benchmarks in comments or supporting docs.
- Implementation files must include `TextParsing.h` first to satisfy BLITZ.

Migration Notes
- Track moved files in `CURRENT_TASK.md` until all text utilities leave `Core2`.
- Coordinate include updates in dependent packages (Vfs, IDE) when headers relocate.
