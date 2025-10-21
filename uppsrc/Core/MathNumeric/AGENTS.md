AGENTS

Scope
- Applies to `uppsrc/Core/MathNumeric`.

Purpose
- Houses numerical helpers, math kernels, random utilities, and analytics code carved out of `Core2`.
- Supplies reusable math primitives for media, AI, and ECS runtime code.

Responsibilities
- Destination for `Math.*`, `FastMath.cpp`, `Random.*`, `CKMeans.*`, `DCT.*`, and similar helpers.
- Document assumptions about units, coordinate spaces, and precision in this file when migrating code.

Guidelines
- Keep external dependencies minimal; prefer Core-only types unless documenting the exception.
- Benchmark-heavy changes should link to measurements or corresponding tests.
- Implementation files must include `MathNumeric.h` first.

Migration Notes
- Track outstanding migrations via `CURRENT_TASK.md` and update dependent packages’ include lists as files move.
- Coordinate with `Core/ColorGeom` when moving geometry-adjacent helpers to avoid duplication.
