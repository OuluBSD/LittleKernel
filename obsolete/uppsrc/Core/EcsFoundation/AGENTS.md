AGENTS

Scope
- Applies to `uppsrc/Core/EcsFoundation`.

Purpose
- Collects the fundamental ECS type declarations, hash utilities, and registration macros extracted from `Core2`.
- Serves as the lightest-weight dependency for code that only needs ECS identifiers and type metadata.

Responsibilities
- Target headers: `EcsDefs.h`, `Keys.h`, `AtomType.*`, `LinkType.*`, `Interface.*`, and similar metadata carriers.
- Document how type hashes are formed, how to register new atoms/links, and where runtime behavior lives (see `Core/EcsEngine`, `Core/EcsDataflow`).

Guidelines
- Keep headers self-contained and minimal; avoid embedding runtime state or heavy includes.
- Ensure registration macros remain compatible with serialized data and IDE tooling.
- Implementation files (if any) must include `EcsFoundation.h` first.

Migration Notes
- Update this AGENTS doc whenever new enum spaces or registration paths are introduced.
- Cross-link to `Core/CompatExt` if temporary shims remain there.
