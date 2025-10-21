AGENTS

Scope
- Applies to `uppsrc/ide` and its entire sub-tree for the main `ide` package.

Purpose
- Hosts TheIDE main application: UI, package/workspace management, editors, build, VCS integration, and orchestration of all sub‑packages under `ide/*`.

Package Structure
- `ide.upp`: Main package manifest. Depends on `ide/Common`, `ide/Core`, `ide/Designers`, `ide/Builders`, `ide/Debuggers`, `ide/Browser`, `ide/Android`, `ide/Java`, `ide/clang`, `ide/Edit3D`, `ide/Shell`, `ide/AI`, and non‑ide packages like `CtrlLib`, `CodeEditor`, etc.
- Key code areas (representative):
  - Startup/UI: `main.cpp`, `ide.cpp`, `idewin.cpp`, `idebar.cpp`, `BaseDlg.cpp`.
  - Navigation & Assist: `Assist.*`, `ContextGoto.cpp`, `Navigator.cpp`, `ParamInfo.cpp`, `Annotations.cpp`.
  - Build & Project: `Build.cpp`, `BuildInfo.cpp`, `Methods*.{h,cpp}`, `AutoSetup.cpp`, `InstantSetup.cpp`, `Template.cpp`.
  - Packages/Repo: `UppWspc.cpp`, `NewPackageFile.cpp`, `Organizer.cpp`, `Repo*.cpp`, `urepo.*`, `DirRepoDiff.cpp`.
  - Files/Editors: `Insert*.cpp`, `Find*.cpp`, `Print.cpp`, `Console.cpp`, `EditorTabBar.cpp`.
  - Settings/Config: `Config.cpp`, `MainConfig.cpp`, `Usage.cpp`.
  - System: `Android.cpp`, `Valgrind.cpp`, `Debug.cpp`, `Export.cpp`.
  - AI integration: `AITaskDlg.cpp`, `AiProvider.{h,inl}`.

How Things Fit
- TheIDE is a composition of sub‑packages. `ide/Core` provides common infrastructure; `ide/Common` contributes dialogs and glue; feature packages plug into menus and toolbars via compile‑time registration and runtime setup in `ide.cpp/idewin.cpp`.
- Debuggers, Builders, Designers, Browser, Shell, Vfs, and language tooling (clang) expose APIs that the main package calls from UI actions.

Related Guides
- AI framework overview: see `uppsrc/AI/AGENTS.md` (domain logic and UI controls) and `uppsrc/ide/AI/AGENTS.md` (IDE integration layer).
- Eon ECS + Dataflow DSL: see `uppsrc/Eon/AGENTS.md` (script DSL, Atoms/Links/Loops) — relevant for `ide/Vfs` and meta‑code tools.

Conventions
- Follow repo‑wide coding rules in `/CODESTYLE.md`.
- Public headers live in this package and sub‑packages; implementation files include their package’s umbrella header where applicable to benefit from BLITZ.
- Prefer adding documentation topics in `.tpp` files for API/impl notes (TheIDE’s Topic++), in addition to these AGENTS notes.

Extending TheIDE
- UI: Add commands and menus in `ide.cpp/idewin.cpp`, arrange widgets in `.lay` files, and bind actions to features in sub‑packages.
- Assist: Extend completion/navigation in `Assist.*` or via `ide/clang` and `ide/Vfs`.
- Builders/Debuggers: Prefer implementing within their respective sub‑packages and just register hooks here.
- AI: Extend providers via `AiProvider` abstraction; surface UI in `AITaskDlg.cpp`.

Build/Run
- Build the `ide` package (`ide.upp`) using TheIDE or `umk`. Use one of the `mainconfig` variants (e.g., `GUI`, `GUI NET CURL LCLANG`).

Testing & Validation
- Start the built TheIDE binary; validate features related to your changes through the corresponding sub‑package UIs (e.g., Debuggers dialog, Builders’ compilation, Designers, Browser).

.upp File Notes
- Keep `AGENTS.md` listed as the first entry in the `file` section of `ide.upp`.
- When adding files, include them in `ide.upp` under a logical separator. Keep non‑source assets (`.lay`, `.iml`, `.usc`, icons) grouped under resource sections.

Tips for Agents
- Before editing, skim the relevant sub‑package AGENTS.md for deeper guidance.
- Cross‑check calls into sub‑packages—many user actions surface in `ide.cpp` then delegate.
- Use ripgrep to locate features: e.g., `rg -n "class Navigator" uppsrc/ide`.
Boss Words Style
- When referring to the project lead in narrative, use italicized boss words with a decorative icon. Rotate icons among: ✦, ★, ◆, ⟁, ✧, ◈.
- The first boss-word mention after a while must include the real name in parentheses: e.g., ✦ *Spearhead (Seppo)*.
- Apply this style in Book/II chronicles and UI-facing notes where appropriate; keep technical errors and warnings neutral.
