AGENTS

Scope
- Applies to `uppsrc/Vfs/Factory` package.

Purpose
- Houses factory/registration interfaces (`VfsValueExtFactory`).
- Remains header-only until implementation migrates from legacy `Core2`.

Guidelines
- Include `Vfs/Core/Core.h` for shared types.
- Dependent sources must include `VfsFactory.h` after their package header per BLITZ rules.

Manifest
- `Factory.upp` lists `AGENTS.md` then `VfsFactory.h`.
