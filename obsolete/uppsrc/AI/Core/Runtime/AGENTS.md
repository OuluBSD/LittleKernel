Runtime — Agents, Processes, Prompts

Overview

- Hosts the execution primitives that drive AI/Core workflows:
  - Generic process base with parallel workers and progress callbacks
  - Agent that owns sessions/state and compiles remote stages
  - Prompt builders used by Prompting to render human/LLM‑friendly inputs

Key Modules

- ProcessBase.h (AiProcessBase)
  - Manages multi‑phase, multi‑batch processing with optional parallelism.
  - Public API: `Start()/Stop()`, `SetParallel()`, `GetPhaseCount()`, `GetBatchCount()` etc., with progress and remaining time callbacks.

- ProcessFramework.h
  - `FarStage` holds compiled, remote-executable JSON; `FarStageCompiler` turns a scripted value into a stage.
  - VFS ECS components (`VfsProgram`, `VfsProgramProject/Session/Iteration`, `VfsFarStage`) expose code/data trees in the model.
  - `Agent` is an ECS component that manages interaction sessions and global Esc state for stage execution.

- Prompt.h
  - `TaskTitledList` and `BasicPrompt` are composable prompt containers with many formatting toggles (titles, numbering, list characters, separators, capitalization, result lists, etc.).
  - These serialize to structured strings (tree view or flat) for LLM input.

Workflows

- Create a new process
  - Inherit `SolverBase` or `AiProcessBase`; implement `GetPhaseCount()`, `GetBatchCount()`, `DoPhase()`. Use `Start()` and plug into UI via the callbacks.

- Build prompts for remote calls
  - Compose `BasicPrompt` with `TaskTitledList` sublists; pass to Prompting’s `TaskMgr`.

- Compile and run a FarStage
  - Use `FarStageCompiler` to build a stage from a high-level description; dispatch via `TaskMgr::GetFarStage`.

Public Surface

- Include umbrella: `#include <AI/Core/Runtime/Runtime.h>`
