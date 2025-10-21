AGENTS

Scope
- Applies to `uppsrc/ide/Edit3D`.

Purpose
- 3D editing and remote-control demo components used by TheIDE tooling or as a sample.

Package Overview
- Manifest: `Edit3D.upp` (uses `CtrlLib`, `Geometry`, `ComputerVision`, `plugin/jpg`, `plugin/enet`).
- Files include editors, geometry/renderers, video import pipeline, and remote client service components: `Edit3D.{h,cpp}`, `Editor*.{h,cpp}`, `Geom*.{h,cpp}`, `Renderer*.{h,cpp}`, `Video*.{h,cpp}`, `SoftRendCtrl.*`, `EditClientService.*`, `Remote*.{h,cpp}`, `main.cpp`.

Extension Points
- Add new renderers or importers in separate components and wire into the UI.
- Keep I/O and rendering off UI thread where possible.

.upp File Notes
- Place `AGENTS.md` first in `Edit3D.upp` `file` list.

