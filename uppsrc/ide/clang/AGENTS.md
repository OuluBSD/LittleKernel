AGENTS

Scope
- Applies to `uppsrc/ide/clang`.

Purpose
- Clang/libclang integration: indexing, parsing, and code model support for assist/navigation features.

Package Overview
- Manifest: `clang.upp` (uses `ide/Core`; links against `libclang` depending on platform).
- Files: `clang.{h,cpp,dli}`, `libclang.{h,cpp}`, `Visitor.cpp`, `CurrentFile.cpp`, `Indexer.cpp`, `macros.cpp`, `util.cpp`.

Libraries
- See `.upp` for `library(...)` conditions. On Windows, `libclang.lib`/`clang` depending on toolchain; on POSIX, appropriate `.a/.so` linkage.

Extension Points
- Add AST visitors in `Visitor.cpp` and indexing strategies in `Indexer.cpp`.
- Expose high-level queries via headers for `ide` and `ide/Vfs`.

.upp File Notes
- Ensure `AGENTS.md` is the first file entry in `clang.upp`.

