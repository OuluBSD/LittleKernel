CURRENT TASK

Focus
- Stage out legacy VFS helpers from `Core2` into this package without breaking consumers.
- Document how each migrated file maps onto the new `uppsrc/Vfs/*` packages.

Progress
- ✅ `Mount.*`, `VFS.*`, `VCS.*`, `Visitor.*`, `VirtualNode.*`, and `WorldState.*` relocated from Core2 and wired through `VfsBase.h`.

Next Steps
- Update dependent packages (Core2, IDE) to include `<Core/VfsBase/VfsBase.h>` and prune legacy includes.
- Clarify `WorldState` evolution/path toward overlay subsets; note integration expectations in AGENTS.
- Add unit coverage or smoke tests capturing existing semantics before further refactors.

Notes
- Keep compatibility wrappers minimal; long-term home for modern APIs is under `uppsrc/Vfs`.
