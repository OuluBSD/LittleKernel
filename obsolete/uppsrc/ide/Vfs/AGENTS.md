AGENTS

Scope
- Applies to `uppsrc/ide/Vfs`.

Current Task Alignment
- We are migrating Vfs/VfsValue responsibilities into clearer layers and packages.
- IDE package should depend on new core Vfs packages (Vfs/Core, Vfs/Overlay, Vfs/Storage) once introduced, and avoid hosting core types.
- Keep UI/editor code (`*Ctrl`) separate from core data structures; CTRL-side extensions should subclass `VfsValueExtCtrl` matching core `VfsValueExt`.

Purpose
- Virtual filesystem (VFS) and meta-environment integration for TheIDE, plus code generation and editor controls for meta code and ECS scripting.
- Provide adapters for Env APIs (location→node, definitions, references) over the core Vfs overlay model.

Package Overview
- Manifest: `Vfs.upp`.
- IDE bridge: `Ide.h` (Env adapters), `Env.{h,cpp}`.
- Code model: `ClangTypeResolver.{h,cpp}` (works with `ide/clang`), `CodeGenerator.{h,cpp}`.
- Meta UI: `MetaCodeCtrl.*`, `MetaEnvTree.*`, `MetaIndexerCtrl.*`, `MetaTempTask.*`, `EditorCtrl.*`.
- DSL: `EcsLang.{h,cpp}` (see `uppsrc/Eon/AGENTS.md`).

Dependencies (target state)
- Depends on `uppsrc/Vfs/Core` for `VfsValue`, `VfsValueExt`, `AstValue`.
- Depends on `uppsrc/Vfs/Overlay` for virtual merge of per-file trees.
- Depends on `uppsrc/Vfs/Storage` for (de)serialization of per-file fragments.
- Provides default `PackagePrecedenceProvider` implementation based on open workspace order.

Extension Points
- Add new resolvers or generators in `CodeGenerator.*` and `ClangTypeResolver.*`.
- Extend meta editors and views by adding new `*Ctrl` implementations.
- Implement new Env adapters in `Env.*` that call into Vfs overlay APIs.
- Keep code-model logic decoupled from UI and avoid storing core data in IDE package.

.upp File Notes
- Ensure `AGENTS.md` is first in `Vfs.upp` `file` section.
- If `CURRENT_TASK.md` exists in this package, list it immediately after `AGENTS.md`.

