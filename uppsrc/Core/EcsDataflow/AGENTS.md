AGENTS

Scope
- Applies to `uppsrc/Core/EcsDataflow`.

Purpose
- Houses ECS data-plane constructs: entities, components, atom/link instances, exchange points, packet buffers, and sample helpers.
- Complements `Core/EcsEngine` by providing the structures exchanged at runtime.

Responsibilities
- Target files: `Component.*`, `Entity*.{h,cpp}`, `Atom.*`, `Link.*`, `Exchange.*`, `Packet*.{h,cpp}`, `Samples.*`, `Formats.*`, `Audio.*`, `GeomEvent.*`, and related helpers.
- Document ownership rules, serialization expectations, and extension patterns (e.g., how to add a new component or exchange).

Guidelines
- Implementation files must include `EcsDataflow.h` first; avoid including engine headers except where absolutely required.
- Keep data structures independent of GUI code; note any dependencies on `Core/ColorGeom` or `Core/MediaFormats` explicitly.
- When adding new data types, update AGENTS with the intended system-level consumers.

Migration Notes
- Track in `CURRENT_TASK.md` which files still live under `Core2`.
- Coordinate with `Core/MediaFormats` and `Core/VfsBase` to avoid duplicated definitions.
