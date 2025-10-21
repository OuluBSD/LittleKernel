Purpose: Common UI infrastructure and helpers shared across Ctrl packages. Hosts the VFS-backed program/stage/session editor used to compile and run agent stages.

Key Classes
-----------
- `AiComponentCtrl` (declared in `Fn.h`): base component ctrl integrating with MetaCtrl’s ComponentCtrl; typed content helpers (typeclasses/contents/parts; currently TODO stubs).
- `VfsProgramCtrl` (`VfsProgram.h/.cpp`): editor for VFS “Program” graphs:
  - Tabs: Main (projects/sessions/iterations/stages, memory tree, log, stage editor) and Form (embedded `FormEditCtrl`).
  - Compiles project/iteration and far stages via an `Agent` owner (`Compile`, `CompileStages`), and executes (`Run`) with live log and memory sync.
  - Global memory serialization helpers: `GlobalToString` / `StringToGlobal` serialize `EscValue` maps to JSON.
- UI helpers in `Fn.h/.cpp`:
  - `SetColoredListValue`, `SetCountWithDefaultCursor`, `SetIndexCursor` for ArrayCtrls.
  - Image helpers: `RescaleToFit`, thumbnail/cache/full path helpers for images based on a content hash.

Data Flow
---------
- `VfsProgramCtrl` navigates a `VfsValue` tree to list Projects → Sessions → Iterations and Program Stages (e.g., `VfsProgramProject`, `VfsProgramSession`, `VfsProgramIteration`, `VfsFarStage`).
- Edits source code (project iteration and stage code strings), persists back into VFS values, compiles via `Agent::Compile(..)` / `Agent::CompileStage(..)` and starts execution via `Agent::Start`.
- Displays iteration memory (`EscValue` tree) and log output streaming from the running agent.

Extending
---------
- Provide new VFS node types for projects/stages or enrich the UI by adding tabs/subviews in `MainTab`/`FormTab`.
- Implement `AiComponentCtrl::GetTypeclasses/GetContents/GetContentParts` and wire content/typeclass taxonomies when available.
- Register the ctrl: `INITIALIZER_COMPONENT_CTRL(VfsProgram, VfsProgramCtrl)` is already present; mirror the pattern for new component UIs.

Requirements
------------
- Requires GUI build (`flagGUI`) and uses `MetaCtrl` and `FormEditor`.
- VFS/Meta environment must provide `Agent` and VFS types referenced by the editor.

