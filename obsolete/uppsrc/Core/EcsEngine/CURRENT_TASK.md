CURRENT TASK

Focus
- Relocate ECS runtime (`Engine*`, `Realtime`, `LinkSystem`, `Verifier`, etc.) out of `Core2`.

Progress
- `Util2.{h,cpp}` moved here from `Core2` and now aggregated via `EcsEngine.h`.
- `Verifier.{h,cpp}` and legacy `Core2/Core.icpp` registrations moved here as `Registration.cpp`; INITBLOCK now lives in this package.

Next Steps
- Continue relocating remaining runtime files (Engine, LinkSystem, Realtime, etc.) into this package.
- Keep INITBLOCK registrations in sync and document lifecycle expectations once migration is complete.
- Verify threadsafety assumptions after relocation (especially `RunningFlagSingle` usage).

Notes
- Coordinate with `Core/EcsDataflow` for data-plane structures; avoid circular dependencies by including only the necessary headers.
