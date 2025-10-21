AGENTS

Scope
- Applies to `uppsrc/ide/Core` and its sub-tree.

Purpose
- Core infrastructure for TheIDE: workspace/package model, build cache, host/platform abstraction, logging, and code/documentation support.

Package Overview
- Manifest: `Core.upp` (uses `Esc`, compression plugins).
- Key areas:
  - `Core.h` (PCH umbrella)
  - Build/cache: `Builder.cpp`, `Cache.cpp`, `BinObj.cpp`, `Signature.cpp`, `PPinfo.cpp`
  - Package/workspace: `Package.cpp`, `Workspace.cpp`, `Assembly.cpp`
  - Host/platform: `Host.{h,cpp}`
  - Logging: `Logger.{h,cpp}`
  - IDE infra: `Ide.cpp`, `Core.cpp`, `Util.cpp`
  - Topics: `src.tpp` referenced via `.upp` (documentation)

Conventions
- Keep interfaces cohesive; avoid leaking platform details outside `Host.*`.
- Prefer existing containers/utilities from U++ (`Vector`, `Array`, `Index`) per `/CODESTYLE.md`.

Extension Points
- Add new cache strategies or signature rules near `Cache.cpp`/`Signature.cpp`.
- Extend package/workspace parsing in `Package.cpp` and `Workspace.cpp`.
- Add logs via `Logger` and keep categories minimal.

Build/Run
- Library package used by `ide` and sub‑packages. Validate by building `ide`.

.upp File Notes
- Keep `AGENTS.md` first in `file` list in `Core.upp`.
- Keep `.tpp` documentation updated for public APIs.

