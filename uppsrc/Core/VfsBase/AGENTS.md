AGENTS

Scope
- Applies to `uppsrc/Core/VfsBase`.

Purpose
- Hosts legacy VFS abstractions that pre-date the new `uppsrc/Vfs/*` stack.
- Provides filesystem adapters, visitors, and the evolving `WorldState` context for Atom initialization.

Responsibilities
- Destination for `Mount.*`, `VFS.*`, `VCS.*`, `Visitor.*`, `VirtualNode.*`, and `WorldState.*` as they migrate out of `Core2`.
- Document how `WorldState` wraps `BinaryWorldState` (ActionPlanner heritage) and currently exposes a `ValueMap`-backed view of an Atom's world subset.

Guidelines
- Keep behavior compatible while code transitions to the overlay-based Vfs model; note divergences explicitly.
- Implementation files must include `VfsBase.h` first; additional includes should be added only when required locally.
- Avoid introducing new features here unless they directly support the staged migration.

Migration Notes
- Track remaining legacy pieces in `CURRENT_TASK.md`, referencing the repo root Vfs objectives.
- As overlays mature, describe the intended convergence between `WorldState` and overlay subsets, or flag planned deprecations.
