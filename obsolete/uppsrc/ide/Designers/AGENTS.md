AGENTS

Scope
- Applies to `uppsrc/ide/Designers`.

Purpose
- Collection of smaller designers and viewers (image, QTF, tree, xml/json visualizers) integrated into TheIDE.

Package Overview
- Manifest: `Designers.upp` (uses `HexView`, `ide/IconDes`, `ide/Common`).
- Files: `Designers.h`, `Png.cpp`, `Img.cpp`, `Qtf.cpp`, `HexView.cpp`, `TreeDes.cpp`, `Xml.cpp`, `Json.cpp`, `md.cpp`.

Extension Points
- Add new viewers/designers as separate `*.cpp` files and register commands in TheIDE menus/toolbars from `ide`.
- Reuse shared dialogs from `ide/Common` and image support from `ide/IconDes`.

.upp File Notes
- List `AGENTS.md` as first in the `file` list.

