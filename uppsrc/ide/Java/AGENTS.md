AGENTS

Scope
- Applies to `uppsrc/ide/Java`.

Purpose
- Java support for TheIDE: Java toolchain detection and integration.

Package Overview
- Manifest: `Java.upp` (uses `ide/Core`).
- Files: `Java.{h,cpp}`, `JavaVersion.cpp`, `Jdk.cpp`.

Extension Points
- Extend JDK/Java detection and version parsing in `Jdk.cpp` / `JavaVersion.cpp`.
- Expose minimal API in `Java.h` for `ide/Builders`.

.upp File Notes
- List `AGENTS.md` first in `Java.upp`.

