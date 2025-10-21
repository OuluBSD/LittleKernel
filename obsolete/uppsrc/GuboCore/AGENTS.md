AGENTS

Scope
- Applies to `uppsrc/GuboCore` and its subtree.

Purpose
- 3D GUI core mirroring CtrlCore’s developer-facing API to keep 2D/3D usage symmetric.
- Provides both 3D containers (`Gubo`, `TopGubo`) and a 2D peer (`Surface`, `TopSurface`) for interop and gradual migration.

Architecture
- Main header: `GuboCore.h` (umbrella). All `.cpp/.icpp` in this package must include this header first.
- Geometry + Interaction
  - `GeomInteraction.h` declares common interaction base (`GeomInteraction`) and 2D/3D specializations (`GeomInteraction2D`, `GeomInteraction3D`).
  - `GIBase.cpp`, `GI2D.cpp`, `GI3D.cpp` implement focus/mouse/key dispatch, layout, and programmable draw-command chaining.
- Draw Pipeline (3D)
  - `DrawCommand.h/.cpp` define `DrawCommand3` (linked-list command queue) and cache.
  - `SDraw3.h/.cpp` provides a software `Draw3` base with clip/op stack.
  - `ProgPainter3.h/.cpp` builds command streams from `Draw3` ops.
  - `Draw3.h/.cpp` wraps painter and screen begin/end commands.
- 3D Containers
  - `Gubo.h/.cpp`: 3D control container, frame/content handling, mouse routing, pending redraw/layout flags, and debug draw.
  - `TopGubo.h/.cpp`: top-level 3D window. Wires into `GuboLib` manager (`Gu::GuboManager`) via `CreateGeom3DComponent()`.
- 2D Containers (peer layer for interop)
  - `Surface.h/.cpp`, `TopSurface.h/.cpp`: 2D analogs to ease reuse of CtrlCore-like patterns; integrate with `Gu::SurfaceManager`.
- Frames
  - `Frame.h`: `SurfaceFrame`/`GuboFrame` logic (per-container frame painting/layout and event capture for borders/toolbars etc.).
- Misc
  - `Defs.h`, `Util.h/.cpp`, `Images.h/.cpp` supporting definitions and assets.

Relation To CtrlCore (API Parity)
- Intent: preserve naming and semantics where possible to keep 2D/3D usage consistent.
  - Ctrl -> Gubo/Surface: methods like `SetFrameRect/Box`, `GetContentRect/Box`, `Layout`, `DeepLayout`, `Refresh`, mouse/key dispatch follow the same shape.
  - `TopWindow` -> `TopGubo`/`TopSurface`: top-level containers with event loop hooks (`Run`, `OpenMain`, focus management).
  - `CtrlFrame` -> `SurfaceFrame`/`GuboFrame`: inset/outset layout, overpaint regions, capture inside frame.
  - Event routing mirrors CtrlCore: frame -> content -> children (Z-order walk), capture overrides, and deep-mouse/leave transitions.

Integration & Managers
- `GuboLib` provides managers (`Gu::GuboManager` for 3D, `Gu::SurfaceManager` for 2D) that act as the draw-begin/capture root.
- `TopGubo::CreateGeom3DComponent()` registers a top container into the active manager; capture/with-mouse getters/setters delegate to the manager via `GetGeomDrawBegin()`.
- Event sources (e.g., SDL/HAL) deliver `CtrlEvent` to `GeomInteraction::Dispatch`; `GI2D/GI3D` handle mouse/key/wheel codes.

Header Include Policy (U++ BLITZ)
- Every implementation file in this package must include only `#include "GuboCore.h"` as the first include.
- Do not add intra-package `#include` lines in implementation files beyond the umbrella; keep additional rare includes local to that `.cpp`.
- Keep headers other than the main header free of third-party/system includes; aggregation and `NAMESPACE_UPP` wrapping happens in `GuboCore.h`.

Subpackage Independence
- `GuboCore` is independent. Parents may include only `GuboCore.h`; do not reach into its internals from outside.
- If adding subpackages under `GuboCore/*`, give each its own `AGENTS.md` and main header, and avoid cross-including internals.

Extension Points
- New 3D widgets: derive from `Gubo` or `GeomInteraction3D`, implement `Paint(Draw3&)` and relevant mouse/key handlers. Use `SizePos/TopPos/LeftPos` helpers for layout.
- 2D overlays in 3D: use `Surface`/`SurfaceFrame` when embedding 2D panels.
- Use `SetPendingRedraw/EffectRedraw/Layout`, `PostCallback`, and `Refresh` to schedule updates; managers coalesce via command queues.

Open TODOs / Gaps (as of this snapshot)
- Capture helpers in frames (`SurfaceFrame`/`GuboFrame`) are stubbed and should delegate to the corresponding manager.
- `Gubo::GetWorkArea`, `ReleaseGuboCapture`, `GetCaptureGubo` and Surface analogs contain TODOs.
- Top-level run/focus hooks in `TopGubo`/`TopSurface` are placeholders; align with CtrlCore’s `TopWindow::Run` semantics or bridge to the active `Gu::*Manager` loop.
- `ProgDraw3` has missing implementations (e.g., `GetFrameSize`, conversion to `Image`) and is currently used as a command recorder.

.upp Notes
- List `AGENTS.md` first in `GuboCore.upp`. If `CURRENT_TASK.md` exists, list it immediately after.

Coding Conventions
- Follow repository `CODESTYLE.md` and the Header Include Policy above.
- Keep changes minimal and focused; avoid adding global includes to `GuboCore.h` unless necessary for aggregation.

