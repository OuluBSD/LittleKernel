Package CURRENT TASK: Vfs

Theme
- Consolidate VfsValue and related infrastructure into `uppsrc/Vfs/*` with clear layering.

Goals (Phase 1)
- Define target package boundaries and responsibilities (Core, Factory, Overlay, Storage, Ctrl).
- Do not break existing includes: prepare staged adapter approach from Core2.
- Document header include policy and Env adapter expectations.

Progress
- ✅ Docs established (`AGENTS.md`, this file).
- ✅ Header scaffolding for Core/Factory/Overlay/Storage created.
- ✅ IDE package now depends on new scaffolds and ships default precedence stub.
- ✅ Factory implementations (`IsType`, `Create`, `Clone`, etc.) moved out of `Core2/VfsValue.cpp` into `Vfs/Factory`.
- ✅ Core2 VfsValue base types (`EntityData`, `VfsValueExt`, helpers) now live in `Vfs/Core` while Core2 keeps the structural class.
- ✅ Legacy VfsValue implementation and enums now build from `Vfs/Core`.
- ✅ ECS runtime (Atom/Component/Engine/etc.) moved from Core2 into `Vfs/Ecs` package.
- ✅ Legacy `Vfs2` runtime helpers split across `Vfs/Meta`, `Vfs/Dataset`, `Vfs/Analysis`, `Vfs/Solver`, with `Vfs/Runtime` as the umbrella.

Planned Steps (next phase)
1) Gradually migrate remaining `VfsValueExtFactory` definitions (registration helpers, data maps) into `Vfs/Factory`.
2) Audit Core2 and downstream packages to include the new `Vfs` headers (`Vfs/Core`, `Vfs/Ecs`) directly and prune compatibility stubs.
3) Implement overlay view logic leveraging `SourceRef` and precedence provider interfaces.
4) Implement JSON serialization in `Vfs/Storage` and add round-trip tests.
5) Update IDE Env adapters to construct overlays using the precedence provider and new storage APIs.

Notes
- Keep GUI controls in a separate package (Vfs/Ctrl) to respect BLITZ and reduce coupling.
- Maintain backward compatibility: old IDE Vfs dumps accepted by `Storage` loaders.
- Stub harness for overlay/serialization/builder scenarios now lives in `upptst/MetaEnvironment`; expand with concrete assertions once overlay behavior is implemented.

Next
- Update downstream packages to include `Vfs/Core/Core.h` directly and drop the temporary Core2 stubs.
