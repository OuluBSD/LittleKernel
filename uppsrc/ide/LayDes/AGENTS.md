AGENTS

Scope
- Applies to `uppsrc/ide/LayDes`.

Purpose
- Layout designer for `.lay` files. Provides property editors, code generation, and integration with Topic++.

Package Overview
- Manifest: `LayDes.upp` (uses `RichEdit`, `Esc`, `CodeEditor`, `ide/Common`, `ide/Browser`).
- Files: `LayDes.{h,cpp,lay,iml}`, `laylib.cpp`, `layusc.cpp`, `property.cpp`, `textprop.cpp`, `fontprop.cpp`, `propane.cpp`, `item.cpp`, `layout.cpp`, `visgen.cpp`, `layfile.cpp`, `laywin.cpp`, `sdiff.cpp`.

Extension Points
- Add new property editors in `property.cpp` or companion files.
- Extend code generation in `laylib.cpp` and Usc integration in `layusc.cpp`.

.upp File Notes
- Keep `AGENTS.md` as the first file in `LayDes.upp`.

