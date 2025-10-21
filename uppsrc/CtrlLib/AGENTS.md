AGENTS

Scope
- Applies to `uppsrc/CtrlLib`.

Purpose
- Widget library atop CtrlCore: controls, dialogs, layout helpers, menus/toolbars, status bar, file dialogs, and utilities.

Key Areas
- `CtrlLib.h` umbrella; controls include `Edit*`, `ArrayCtrl`, `TreeCtrl`, `TabCtrl`, `Splitter`, `SliderCtrl`, `StatusBar`, `MenuBar` etc.
- Platform-specific helpers under `Utilities/*` and tray icons.

Conventions
- Keep controls self-contained with clear public APIs and minimal global state.

Extension Points
- Add new controls with header + implementation and register in demo/test where applicable.

.upp Notes
- Put `AGENTS.md` first in `CtrlLib.upp` `file` list.

