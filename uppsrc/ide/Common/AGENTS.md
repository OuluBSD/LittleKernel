AGENTS

Scope
- Applies to `uppsrc/ide/Common` and its sub-tree.

Purpose
- Common dialogs, utilities, and glue used across TheIDE. Provides command-line options parsing and shared UI pieces.

Package Overview
- Manifest: `Common.upp` (depends on `ide/Core`, `CtrlLib`, `Esc`, `CodeEditor`).
- Notable files:
  - `Common.h` (PCH umbrella header)
  - `ComDlg.cpp` (common dialogs)
  - `Module.cpp` (module wiring/registration)
  - `Util.cpp` (shared utilities)
  - `CommandLineOptions.h` (CLI flags shared by `ide`)

Conventions
- Keep cross-package utilities small and UI-agnostic where possible.
- Only place generally reusable code here; prefer feature-specific logic in its own package.

Extension Points
- Add new common dialogs in `ComDlg.cpp` and expose minimal helpers in headers.
- Extend CLI support via `CommandLineOptions.h` and implementation inside `ide` where options are consumed.

Build/Run
- Used by `ide` and other sub‑packages; no separate executable. Ensure changes compile with `ide`.

.upp File Notes
- Ensure `AGENTS.md` is the first item in the `file` list in `Common.upp`.
- Group new files under meaningful separators if added.

