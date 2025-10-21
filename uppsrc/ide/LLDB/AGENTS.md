AGENTS

Scope
- Applies to `uppsrc/ide/LLDB` (standalone LLDB UI application).

Purpose
- Dedicated LLDB frontend using GLFW/ImGui and OpenGL. Complements `ide/Debuggers` LLDB integration.

Package Overview
- Manifest: `LLDB.upp` (uses `ide`; links `lldb glfw imgui GL GLEW` as per `.upp`).
- Files: `LLDB.{h,cpp}`, `LLDBCommandLine.*`, `DebuggerApp.{h,cpp,lay}`, `Application.*`, `FileSystem.*`, `FileViewer.*`, `StreamBuffer.*`, `Timer.h`, `FPSTimer.h`, `Log.*`, `main.cpp`.

Extension Points
- Extend UI in `DebuggerApp.*` and add LLDB command handling in `LLDBCommandLine.*`.
- Add viewers/tools in `FileViewer.*` and utilities under `FileSystem.*`.

.upp File Notes
- Keep `AGENTS.md` first in `LLDB.upp` `file` list and maintain `library(...)` lines for platform linkage.

