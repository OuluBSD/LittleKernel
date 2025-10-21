AGENTS

Scope
- Applies to `uppsrc/ide/IconDes`.

Purpose
- Image/Icon designer. Provides painting, image operations, and UI to edit `.iml` and image assets used by packages.

Package Overview
- Manifest: `IconDes.upp` (uses `CtrlLib`, `plugin/gif`, `plugin/jpg`, `Painter`, `RichEdit`).
- Key files: `IconDes.{h,cpp,lay,iml,key}`, `IconDraw.cpp`, `AlphaCtrl.cpp`, `RGBACtrl.cpp`, `ImageOp.cpp`, `Paint.cpp`, `Smoothen.cpp`, `Text.cpp`, `Bar.cpp`, `EditPos.cpp`, `Image.cpp`, `ImlFile.cpp`, `IdeIconDes.cpp`, `IdeDes.cpp`.

Extension Points
- New tools or operations belong in dedicated `*.cpp` files; wire into the toolbars defined in `.lay` files.
- Keep heavy image algorithms separate from UI for reuse.

.upp File Notes
- Ensure `AGENTS.md` is the first `file` entry.

