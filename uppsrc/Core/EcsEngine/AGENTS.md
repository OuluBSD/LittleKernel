AGENTS

Scope
- Applies to `uppsrc/Core/EcsEngine`.

Purpose
- Contains the ECS runtime core: engine lifecycle, scheduling, threading helpers, verifier hooks, and system registration glue.
- Replaces the runtime portions of `Core2` with a dedicated package that depends on `Core/EcsFoundation` and migration shims.

Responsibilities
- Destination for `Engine*.{h,cpp}`, `LinkSystem.*`, `Realtime.*`, `Debugging.*`, `Verifier.*`, `Util2.*`, and related runtime helpers.
- Document lifecycle phases (initialize/start/update/stop), threading expectations, and how WorldState flows through system startup.

Guidelines
- Implementation files must include `EcsEngine.h` first, then any rare dependencies.
- Keep APIs explicit about ownership and thread safety; call out invariants in comments when not obvious.
- Reference `Core/VfsBase` where WorldState interactions matter and describe future overlay integration.

Migration Notes
- Track outstanding runtime files in `CURRENT_TASK.md` as they move from `Core2`.
- Update `Core/CompatExt` initializer blocks only after the corresponding files are present here.
