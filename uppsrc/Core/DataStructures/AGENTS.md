AGENTS

Scope
- Applies to `uppsrc/Core/DataStructures`.

Purpose
- Hosts non-gui containers, graph helpers, and scoped data structures extracted from `Core2`.
- Provides reusable primitives for ECS, Vfs, and runtime systems without pulling in heavy dependencies.

Responsibilities
- Target files: `Container.*`, `Index.*`, `Record.*`, `ValDevScope.*`, and similar utilities.
- Document invariants (e.g. parent tracking, ordering guarantees) and when to prefer these over standard Core containers.

Guidelines
- Keep templates header-only when practical; implementations requiring `.cpp` must include `DataStructures.h` first.
- Record any concurrency requirements or locking behavior inline to avoid misuse.
- Add focused unit or reference tests in `autotest` when introducing new containers.

Migration Notes
- Update `CURRENT_TASK.md` when moving additional data-structure headers here or deprecating legacy variants.
- Remove transitional includes from `Core2/Core.h` as files land in this package.
