CURRENT TASK

Focus
- Extract ECS metadata headers from `Core2` without breaking initialization macros or serialized IDs.

Progress
- ✅ `EcsDefs.h`, `Keys.h`, `AtomType.*`, and `LinkType.*` migrated from Core2 and wired via `EcsFoundation.h`.

Next Steps
- Move `Interface.*` once data-flow dependencies relocate, updating registration docs accordingly.
- Update `Core/CompatExt` initializers to match new header paths.
- Document registration flows once files are in place to ease contributor onboarding.

Notes
- Keep INITBLOCK registrations in `Core/CompatExt` until all consumers switch to the new headers.
