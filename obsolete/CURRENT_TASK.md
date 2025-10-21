Repo-wide CURRENT TASK

VfsValue Rewrite: Core split, overlays, and serialization

Objectives
- Collect Vfs/VfsValue-related code into focused Vfs packages under `uppsrc/Vfs/*` and minimize spread.
- Replace merge/unmerge with a virtual overlay model retaining per-source fragments and provenance.
- Redesign serialization to store per-file fragments plus overlay index; ensure backward compatibility with IDE data.
- Provide clean Env API adapters in `uppsrc/ide/Vfs` over the new core.

Scope & Ownership
- Core data structures: `uppsrc/Vfs/Core` (VfsValue, VfsValueExt, AstValue, enums, paths).
- Factory/registration: `uppsrc/Vfs/Factory`.
- Overlay/union view and provenance: `uppsrc/Vfs/Overlay`.
- Serialization: `uppsrc/Vfs/Storage`.
- IDE integration: `uppsrc/ide/Vfs` (adapters, UI; no core logic).

Design Notes
- Keep multiple potential sources for a node; treat logical tree as union with conflict resolution policies.
- Provenance: track `{pkg_hash, file_hash, local_path/id, priority, flags}` per contribution.
- Extensions: maintain 1:1 mapping between `VfsValueExt` (Core) and `VfsValueExtCtrl` (Ctrl); register via Factory.
- Header policy (U++ BLITZ): source files include only package main header first.

Deliverables (phased)
1) Documentation updates (this file, AGENTS) and package scaffolding plan.
2) Extract minimal `Vfs/Core` API surface (headers only, no behavior change) and adapt includes.
3) Introduce `Overlay` structs (`SourceRef`, `OverlayView`) with no behavior change; add tests/examples later.
4) Define JSON/Stream schema in `Vfs/Storage` and implement no-op loaders that accept old format.
5) Wire IDE Env adapters to call overlay APIs; deprecate physical un-merge routines.

Open Questions
- Staged refactor (adapters) vs. hard move of includes?
- Deterministic overlay ordering (workspace-configured priority) vs. default first-wins?
- Persist overlay index to disk or compute in-memory on load?

Status
- Header scaffold for `Vfs/Core`, `Vfs/Factory`, `Vfs/Overlay`, `Vfs/Storage` in place.
- `VfsValueExtFactory` core methods moved from `Core2/VfsValue.cpp` into new `Vfs/Factory` package (still depending on legacy structures).

Next
- Continue migrating remaining `VfsValue` helpers into the new packages while keeping builds stable.
- Flesh out overlay implementation using precedence provider.
- Implement serialization format and adapters, then update IDE Env to consume it.
