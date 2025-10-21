AGENTS

Scope
- Applies to `uppsrc/CtrlCore`.

Purpose
- GUI core: windowing, input, event loop, platform backends (Win32, X11/GTK, Cocoa), and top-level window management.

Key Areas
- `CtrlCore.h` umbrella, `Ctrl*` files for input/events/layout, `TopWindow`.
- Backends: `Win32/*`, `X11/*`, `Gtk/*`, `Coco*` with drawing and window code.

Conventions
- Keep backend-specific code in respective folders; the common layer should not depend on a specific backend.
- Document new platform code with concise Topic++ entries.

Extension Points
- Add platform features within backend folders and expose minimal cross-platform flags/API at the common layer.

.upp Notes
- List `AGENTS.md` first in `CtrlCore.upp`.

