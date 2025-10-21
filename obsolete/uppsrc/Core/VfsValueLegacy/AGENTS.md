AGENTS

Scope
- Applies to `uppsrc/Core/VfsValueLegacy`.

Purpose
- Temporarily hosts legacy `VfsValue` structures and enums until their definitions fully migrate into `uppsrc/Vfs/*`.
- Provides shims for existing consumers while we build the new overlay/serialization stack.

Responsibilities
- Home for `VfsValue.*`, `VfsEnum.*`, `VfsValue_mcp_stubs.cpp`, `LinkFwd.cpp`, and related glue during the transition.
- Clearly document which portions have already moved to the new `Vfs` packages and which remain here.

Guidelines
- Do not introduce new behavior; focus on relocation and documentation.
- Implementation files must include `VfsValueLegacy.h` first and add any unusual includes locally.
- Highlight deprecated APIs and point developers to their replacements under `uppsrc/Vfs`.

Migration Notes
- Maintain `CURRENT_TASK.md` with a checklist of remaining types to move.
- Once the new Vfs packages provide the required APIs, retire this package and update dependents.
