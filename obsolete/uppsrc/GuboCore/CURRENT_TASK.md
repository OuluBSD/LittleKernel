CURRENT TASK

Objective
- Bring GuboCore (3D GUI) to practical parity with CtrlCore’s developer-facing API while preserving its manager-driven rendering/event model.

Progress
- Phase 1 implemented: SurfaceFrame capture plumbing now delegates to Gu::SurfaceManager; Surface container-level frame capture getters/setters are manager-backed.
- Phase 2 (core methods): Implemented
  - Surface::GetWorkArea, ReleaseSurfaceCapture, GetCaptureSurface
  - Gubo::GetWorkArea, ReleaseGuboCapture, GetCaptureGubo
  These delegate to the active Gu::*Manager with fallbacks to local frame size.
- CtrlEvent unified: Introduced GuboCore/CtrlEvent.h (alias to Core2::GeomEvent) and included from GuboCore.h; all GuboCore event dispatch compiles consistently.
- Frame name collisions fixed: Renamed GuboCore frame types to avoid CtrlCore conflicts:
  - NullFrameClass → GuboNullFrameClass, MarginFrame → GuboMarginFrame, BorderFrame → GuboBorderFrame; NullFrame() → GuboNullFrame().
- Demos: Added examples demonstrating 2D embedding (SurfaceCtrl demo with DrawCommand replay) and 3D embedding (GuboGLCtrl demo via new GuboCtrl package). 3D GL replay lives outside GuboCore; core APIs remain unchanged.

Plan (phased)
- Phase 1 — Capture/Mouse plumbing
  - Implement frame-level capture helpers by delegating to managers:
    - SurfaceFrame::{Get/Set}Captured, {Get/Set}WithMouse -> Gu::SurfaceManager
    - GuboFrame equivalents -> Gu::GuboManager
  - Implement container-level capture accessors:
    - Surface::{Get/Set}FrameCaptured/WithMouse (manager-backed)
    - Gubo::{Get/Set}FrameCaptured/WithMouse (already wired; verify)

- Phase 2 — Missing core methods (mirroring CtrlCore)
  - Gubo::GetWorkArea, ReleaseGuboCapture, GetCaptureGubo
  - Surface::GetWorkArea, ReleaseSurfaceCapture, GetCaptureSurface
  - Use manager’s capture root and platform work-area from active scope.

- Phase 3 — Event loop and focus hooks
  - Implemented
    - TopGubo::Run/OpenMain/FocusEvent
    - TopSurface::Run/OpenMain/FocusEvent
  - Behavior
    - OpenMain registers in manager and focuses the handle.
    - Run focuses the container and enters the shared loop via Surface::EventLoop().
    - FocusEvent asks the active manager to FocusHandle(this).
  - UpdateFromTransform2D/3D remain TODO (deferred).

- Phase 4 — 3D interaction parity
  - Base `GeomInteraction3D` now matches `GeomInteraction2D` defaults: content handlers are no-ops (return false), captured-path returns false. Frame-aware routing belongs to `Gubo` and is already implemented. No additional changes required unless specialized controls need overrides.

- Phase 5 — Paint pipeline verification
  - Audit `GeomInteraction3D::Redraw` to ensure `CtrlDrawBegin/End` begin/end symmetry and clipping on frame/content match 2D.
  - Ensure `PaintPreFrame`/`PaintPostFrame` frame clipping mirrors `Surface` behavior.

- Phase 6 — Utility completion
  - ProgDraw3::GetFrameSize and `operator Image()` (if required by callers). For now, keep as command recording; implement image extraction only if needed by tests or previews.
  - Add minor diagnostics (DumpDrawCommands) to aid bringing up complex UIs.
  - Status: 2D counterpart has ProgDraw::operator Image(); 3D remains as command recording. No change needed in GuboCore.

Parity Checklist (CtrlCore → GuboCore)
- Focus/capture API: SetCapture/ReleaseCapture, IsCaptured, GetCaptured (per-manager)
- Frames: FrameLayout/Paint/AddSize, OverPaint, mouse enter/leave, capture inside frame
- Event dispatch: mouse move/wheel/button, deep dispatch, key/hotkey paths
- Layout: DeepLayout/DeepFrameLayout, SizePos helpers
- Redraw: pending flags, effect vs. content redraw, debug draw toggles

Validation
- Minimal demo: create a `TopGubo`, add a child `Gubo` with simple `Paint(Draw3&)` and mouse handlers; exercise capture/with-mouse transitions via `Dispatch(CtrlEvent)`.
- Verify manager (GuboLib) reports capture and with-mouse consistently while moving the pointer across nested children and frames.
 - External demos (in examples/): SurfaceCtrlDemo (2D replay inside Ctrl) and GuboGLCtrlDemo (GLX-backed 3D replay inside Ctrl) validate integration patterns without modifying GuboCore.

Repository Hygiene
- Update `GuboCore.upp` to list `AGENTS.md` first and `CURRENT_TASK.md` second (done).
- Follow Header Include Policy in all new/modified `.cpp` files; defer cleanup of existing violations to a separate pass.

Notes
- Keep API names and behavior aligned with CtrlCore for developer familiarity. Where 3D requires different semantics (e.g., depth-aware hit-testing), document the differences inline in the implementation.
