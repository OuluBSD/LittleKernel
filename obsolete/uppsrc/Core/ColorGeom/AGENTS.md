AGENTS

Scope
- Applies to `uppsrc/Core/ColorGeom`.

Purpose
- Encapsulates color utilities, geometry math, and event primitives that are independent of GUI stacks.
- Feeds shared math/state to ECS systems, renderers, and non-GUI consumers.

Responsibilities
- Target files: `Color.*`, `Coordinate.*`, `Geom.*`, `GeomEvent.*`, and related helpers.
- Document coordinate conventions (left/right-handed, units) and event routing semantics.

Guidelines
- Avoid coupling to CtrlCore/Gubo UI specifics; this package should remain reusable in headless builds.
- Implementation files must include `ColorGeom.h` first per BLITZ policy.
- Note any dependencies on `MathNumeric` when migrating code.

Migration Notes
- Update dependent packages (e.g. `GuboCore`, `Core2` wrappers) when headers relocate.
- Track remaining geometry helpers in `CURRENT_TASK.md` until the migration completes.
