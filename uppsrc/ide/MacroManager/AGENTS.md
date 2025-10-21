AGENTS

Scope
- Applies to `uppsrc/ide/MacroManager`.

Purpose
- Manage extensions/macros for TheIDE: parse macro files, present UI for managing, and integrate with CodeEditor.

Package Overview
- Manifest: `MacroManager.upp` (uses `CtrlLib`, `CodeEditor`, `ide/Common`).
- Files: `MacroManager.{h,cpp,lay,iml}`, `MacroElement.cpp`, `UscFileParser.cpp`.

Extension Points
- Extend macro parsing in `UscFileParser.cpp` and UI behavior in `MacroManager.cpp`.
- Surface minimal interfaces in `MacroManager.h` for main `ide` package hooks.

.upp File Notes
- Ensure `AGENTS.md` comes first in `file` entries of `MacroManager.upp`.

