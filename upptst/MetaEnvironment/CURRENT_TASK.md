Package CURRENT TASK: MetaEnvironment stubs

Goal
- Grow the MetaEnvironment test harness that exercises ScriptBuilder, IDE assist annotations, META storage, and overlay views once the overlay rewrite lands.

Status
- ✅ Package scaffolding (`AGENTS.md`, `.upp`, `main.cpp`) builds with IDE/Vfs dependencies.
- ✅ Placeholder coverage logs TODOs and keeps key entry points linked.

Next
- Replace logging in each stub with verifiable assertions once overlay-union APIs are concrete.
- Add fixtures that load serialized META files and validate overlay assembly.
- Introduce ScriptBuilder-driven compilation fixtures that feed MetaEnvironment overlays end-to-end.

Notes
- Keep this package console-only; use helper adapters if GUI stubs are required.
- Coordinate new tests with `uppsrc/Vfs/CURRENT_TASK.md` before landing larger overlay changes.
- Reference: `uppsrc/ide/Builders/ScriptBuilder.cpp`
- Reference: `uppsrc/ide/Vfs/Vfs.cpp`
- Reference: `uppsrc/ide/clang/clang.h`
- Reference: `uppsrc/Vfs/Core/VfsValue.cpp`
- Reference: `uppsrc/Vfs/Overlay/VfsOverlay.h`
