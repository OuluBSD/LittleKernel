AGENTS

Scope
- Applies to `uppsrc` and its entire sub-tree of U++ packages.

Overview
- This tree hosts first‑party Ultimate++ packages and applications.
- Key modules with deeper guides:
  - TheIDE: `uppsrc/ide/AGENTS.md` (and per‑subpackage AGENTS). Provides the IDE application and its feature modules.
  - AI: `uppsrc/AI/AGENTS.md` (and per‑subpackage AGENTS). Modular AI framework with Core logic and Ctrl UI.
  - Eon (ECS + Dataflow + DSL): `uppsrc/Eon/AGENTS.md` for the ECS/dataflow engine and script DSL.
  - Geometry + Vision + Sound: `uppsrc/Geometry/AGENTS.md`, `uppsrc/ComputerVision/AGENTS.md`, `uppsrc/Sound/AGENTS.md`, `uppsrc/SoundCtrl/AGENTS.md`.
  - Forms: `uppsrc/Form/AGENTS.md`, `uppsrc/FormEditor/AGENTS.md`.
  - Meta tooling: `uppsrc/MetaCtrl/AGENTS.md`, `uppsrc/Vfs/AGENTS.md`.
  - Developer console: `uppsrc/DropTerm/AGENTS.md`.

Common Conventions
- Follow `/CODESTYLE.md` for code style and documentation. Prefer Topic++ (`.tpp`) for API/impl docs.
- Scope of AGENTS files: a file applies to the entire directory tree rooted at its folder; nested AGENTS override parents.
- Package manifests (`.upp`): list `AGENTS.md` as the first file in each package’s `file` section for quick discovery.

Development Tips
- Use ripgrep to navigate: `rg -n "AGENTS.md|\.upp$" uppsrc`.
- When adding a new package under `uppsrc`, include:
  - `PkgName.upp` with dependencies and a `file` section beginning with `AGENTS.md`.
  - An umbrella header named after the package when appropriate (speeds BLITZ builds).

Where To Start
- Building TheIDE: open `uppsrc/ide/ide.upp` in TheIDE or build with `umk` using one of its `mainconfig` variants.
- Exploring AI: read `uppsrc/AI/AGENTS.md` and browse `AI/Core/*` and `AI/Ctrl/*` for domain logic and UIs.
- Learning Eon: see `uppsrc/Eon/AGENTS.md` and examples listed there.
 - Geometry/Vision/Sound: see the respective AGENTS in `uppsrc/Geometry`, `uppsrc/ComputerVision`, and `uppsrc/Sound`.
 - Forms: start with `uppsrc/Form/AGENTS.md` then `uppsrc/FormEditor/AGENTS.md` for the editor app.
