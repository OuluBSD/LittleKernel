AGENTS

Scope
- Applies to `uppsrc/Vfs/Ecs`.

Purpose
- Hosts ECS-style runtime types (`Component`, `Entity`, `Engine`, `Atom`, `Link`, etc.) that operate on `VfsValue` trees.
- Provides non-UI runtime building blocks used by Vfs/Factory registrations and higher-level tools.

Guidelines
- Implementation files must include `Ecs.h` first (BLITZ compatibility).
- Keep dependencies limited to Core, Vfs/Core, and existing ECS foundation packages; avoid IDE/UI coupling.
- When introducing new extensions, ensure registration happens via `VfsValueExtFactory` in Factory package.

Manifest
- `Ecs.upp` lists `AGENTS.md` followed by `Ecs.h` so it is visible in TheIDE.
