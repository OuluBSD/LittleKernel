AGENTS

Scope
- Applies to `uppsrc/ide/Shell`.

Purpose
- Integrated shell/console UI for TheIDE, with widgets and agent hooks.

Package Overview
- Manifest: `Shell.upp` (uses `AI/Ctrl`).
- Files: `Shell.{h,cpp,lay,key}`, `Console.{h,cpp}`, `Widget.{h,cpp}`, `IdeShell.{h,cpp}`, `IdeShellHost.cpp`, `EscCmds.{h,cpp}`, `SmallWidgets.{h,cpp}`, `Agent.{h,cpp}`.

Extension Points
- Add shell commands in `EscCmds.*` and UI widgets under `Widget.*`/`SmallWidgets.*`.
- Provide host integration via `IdeShell*` and agent protocols in `Agent.*`.

.upp File Notes
- List `AGENTS.md` first in `Shell.upp` `file` list.

