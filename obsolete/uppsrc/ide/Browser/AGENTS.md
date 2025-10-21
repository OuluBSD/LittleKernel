AGENTS

Scope
- Applies to `uppsrc/ide/Browser`.

Purpose
- Code browser and Topic++ support inside TheIDE: navigate topics, link/code references, and view/edit documentation.

Package Overview
- Manifest: `Browser.upp` (uses `PdfDraw`, `RichEdit`, `plugin/lz4`, `ide/Common`).
- Files:
  - `Browser.h` (PCH umbrella)
  - Topic system: `Topic*.{cpp,lay,iml}`, `File.cpp`, `Link.cpp`, `Template.cpp`
  - Code references: `CodeRef.cpp`, `Move.cpp`
  - Utilities: `Util.cpp`

Extension Points
- Add new content providers or topic linkers in `Topic*.cpp` and `Link.cpp`.
- Expose minimal helpers in `Browser.h` for other packages (e.g., show a topic by id).

Build/Run
- Integrated with `ide`. Validate via Help/Topic browser and code reference navigation.

.upp File Notes
- Keep `AGENTS.md` first in `Browser.upp` file list.

