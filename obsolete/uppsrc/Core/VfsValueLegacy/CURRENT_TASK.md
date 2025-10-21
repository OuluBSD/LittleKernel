CURRENT TASK

Focus
- Track remaining `VfsValue`/`VfsEnum` pieces that continue to live under `Core2`.
- Coordinate with `uppsrc/Vfs/Core`, `Vfs/Factory`, and `Vfs/Overlay` so we avoid diverging behavior.

Next Steps
- Move the struct definitions and basic helpers into `Vfs/Core` once header dependencies are ready.
- Port registration glue to `Vfs/Factory` and update call sites.
- Remove transitional stubs (`*_mcp_stubs.cpp`) after IDE consumers switch to the new APIs.

Notes
- Keep this package free of new features; it should disappear when the migration completes.
