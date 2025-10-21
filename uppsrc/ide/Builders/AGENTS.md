AGENTS

Scope
- Applies to `uppsrc/ide/Builders`.

Purpose
- Builders implement compilation/linking for C/C++ (GCC/MSC/Cocoa), Java, Android, scripts, and VFS-related builds.

Package Overview
- Manifest: `Builders.upp` (uses `ide/Core`, `ide/Android`, `ide/Java`).
- Highlights:
  - Core: `Builders.h` (PCH), `Build.{h,cpp}`, `Blitz.cpp`, `Install.cpp`, `BuilderUtils.{h,cpp}`.
  - C/C++: `CppBuilder.cpp`, `GccBuilder.cpp`, `MscBuilder.cpp`, `MakeFile.cpp`, `CCJ.cpp`, `coff.h`.
  - Java: `JavaBuilder.cpp`.
  - Script: `ScriptBuilder.cpp`.
  - Apple: `Cocoa.cpp` (where applicable).
  - Android: `Android*.cpp`, `Android*.h`, `AndroidBuilder*.*`, `AndroidProject.cpp`.
  - VFS: `VfsBuilder.cpp`.

Extension Points
- Add new toolchains by following the pattern in `GccBuilder.cpp`/`MscBuilder.cpp` and wiring into selection in `Build.cpp`.
- Extend Android support in dedicated `Android*` files; keep common pieces in `BuilderUtils.*`.

Conventions
- Keep platform checks minimal and localized; prefer feature flags in the .upp or `mainconfig`.
- Ensure builders log clearly using `ide/Core` Logger.

.upp File Notes
- Keep `AGENTS.md` first in `file` list and use separators for logical groups.

