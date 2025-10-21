# Vfs Storage Schema (Draft)

## Goals
- Persist per-file Vfs fragments without physical merging.
- Preserve provenance (package/file hashes) for overlay reconstruction.
- Allow streaming/partial loading for large trees and incremental updates.
- Maintain backward compatibility with existing IDE dumps.

## Fragment Format
```json
{
  "version": 1,
  "pkg_hash": 123456789,
  "file_hash": 987654321,
  "root": { "...": "Node" }
}
```

`root` is the logical fragment root, serialized recursively.

### Node Object
- `id` (string): identifier within parent scope.
- `type_hash` (uint64): identifies `VfsValueExt` type or 0 for plain Value/Ast.
- `value` (object):
  - `kind`: `plain` | `ast` | `ext`.
  - `payload`: storage-specific data (mirrors Upp::Value/AstValue/jsonized ext).
- `ext` (object, optional): additional extension payload keyed by `type_hash` when `kind == "ext"`.
- `flags` (uint32): bits for disabled/temp/etc.
- `children` (array<Node>): child nodes.
- `sources` (array<SourceRef>): optional provenance overrides per node.

### SourceRef
- `pkg_hash` (uint64)
- `file_hash` (uint64)
- `local_path` (string)
- `priority` (int)
- `flags` (uint32)

## Overlay Index
To avoid merging/unmerging, persist a lightweight index mapping logical paths to contributing fragments:
```json
{
  "version": 1,
  "nodes": [
    {
      "path": "Ecs|Player",
      "sources": [ { "pkg_hash": ..., "file_hash": ... } ]
    }
  ]
}
```
The index can live alongside fragment files or be recomputed on load. For now we expect IDE to recompute if missing.

## Backward Compatibility
Implement `VfsLoadLegacy` to parse previous binary/JSON dumps, transform them into the new fragment structure, and mark nodes as coming from a single-source fragment.

## Next Steps
1. Implement JSON writer/reader in `VfsStorage.cpp`.
2. Add streaming helpers for incremental saving.
3. Provide tests that round-trip fragments and overlay indices.
4. Document field-level flags and versioning strategy.
