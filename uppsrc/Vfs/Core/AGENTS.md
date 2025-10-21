AGENTS

Scope
- Applies to `uppsrc/Vfs/Core` package.

Purpose
- Provide minimal, GUI-free definitions for Vfs core types (`VfsValue`, `AstValue`, type aliases).
- Acts as the main header entry point included by implementation packages (`Core2`, future `Vfs/*`).

Notes
- Keep this package header-only until implementations migrate from `uppsrc/Core2`.
- Follow BLITZ policy: implementation files in dependent packages must include `Core.h` first.

Manifest
- `Core.upp` must list `AGENTS.md` first, followed by `Core.h`, `Types.h`, `AstValue.h`, `VfsCore.h`.
