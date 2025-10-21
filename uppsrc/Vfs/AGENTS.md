AGENTS

Scope
- Applies to `uppsrc/Vfs` (standalone VFS + language tools) separate from `uppsrc/ide/Vfs`.

Purpose
- Core Vfs types and services for AST-backed virtual filesystem trees.
- AST, parser, and tooling for VFS/Eon meta-language: build AST, export/import, semantic analysis, and compilation helpers.

Current Refactor (VfsValue rewrite)
- Consolidate VfsValue and related code here from scattered locations.
- Introduce layers:
  - `Vfs/Core`: `VfsValue`, `VfsValueExt`, `AstValue`, enums, path/types.
  - `Vfs/Factory`: `VfsValueExtFactory` and registration utilities.
  - `Vfs/Overlay`: virtual merge of per-file trees with provenance.
  - `Vfs/Storage`: serialization/deserialization of per-file fragments and overlay index.
  - `Vfs/Meta`, `Vfs/Dataset`, `Vfs/Analysis`, `Vfs/Solver`: runtime helpers previously in `Vfs2`, with `Vfs/Runtime` as the umbrella include.
- GUI counterparts live in a separate `Vfs/Ctrl` package; avoid GUI dependencies here.

Key Areas
- AST and nodes, semantic parser, exporter/importer, compiler front-end glue, Eon standard helpers.

Relation
- Complements `uppsrc/Eon/AGENTS.md` and IDE meta-tools in `uppsrc/ide/Vfs`.

.upp Notes
- List `AGENTS.md` first in `Vfs.upp`.
- If present, add `CURRENT_TASK.md` immediately after for visibility in TheIDE.

