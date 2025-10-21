AGENTS

Scope
- Applies to `uppsrc/GuboCtrl`.

Purpose
- Bridge GuboCore (3D GUI) with CtrlCore by embedding a 3D rendering context into a Ctrl using DHCtrl (native child window).

Design
- GuboGLCtrl: Ctrl derived from DHCtrl that hosts a platform GL context for rendering a Gu::GuboManager scope.
- Event forwarding: maps Ctrl mouse/keys to CtrlEvent and dispatches to the attached TopGubo/manager.
- Rendering: on paint/resize, ensures context current, sets viewport, asks manager to render.

Status
- Skeleton provided. Platform GL context creation per-backend (WGL/GLX/Cocoa) is TODO.

Header Include Policy
- All .cpp include only "GuboCtrl.h" first.

.upp
- List AGENTS.md first.

