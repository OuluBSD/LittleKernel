AGENTS

Scope
- Applies to `examples/SurfaceCtrlDemo`.

Purpose
- Demonstrates embedding a 2D Surface-like control in a CtrlCore TopWindow alongside CtrlLib widgets.

Status
- Input events are forwarded from Ctrl to a TopSurface instance via CtrlEvent dispatch.
- Painting currently uses a placeholder; to fully render Surface content inside Ctrl, add a DrawCommand replay or complete ProgDraw::operator Image() and blit.

