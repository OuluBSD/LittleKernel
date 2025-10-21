AGENTS

Scope
- Applies to `uppsrc/Core/CompatExt`.

Purpose
- Transitional glue between legacy `Core2` monolith and new modular Core packages.
- Hosts global initializers, compatibility helpers, and macros until consumers migrate.

Responsibilities
- Keep `Core.icpp` init blocks and registration macros accessible while files move to dedicated packages.
- Isolate legacy headers (`Compat.h`, temporary shims) so dependent packages include only what they need.

Guidelines
- Do not add new engine/component logic here; route it to the appropriate ECS or Vfs package.
- Prefer documenting deprecation targets inline when wrapping old APIs.
- Follow BLITZ header policy: implementation files must include `CompatExt.h` first.

Migration Notes
- Remove entries from this package as soon as their code lives under the new Core subpackages.
- Update the repo root `CURRENT_TASK.md` when major compatibility stubs are retired.
