CURRENT TASK

Focus
- Move ECS data holders (components, atoms, links, packets, samples) out of `Core2`.

Next Steps
- Relocate component/entity headers and implementations, ensuring `Core/EcsEngine` keeps working via public APIs.
- Extract packet/format helpers into this package or `Core/MediaFormats` as appropriate.
- Document extension workflows so new systems can hook into the data plane cleanly.

Notes
- Watch for circular dependencies with `Core/EcsEngine`; prefer forward declarations when possible.
