Prompting — Prompts, RPC Stages, Remote Tasks

Overview

- Centralizes prompt construction and transport for remote model calls.
  - Builders for structured prompts (Runtime’s `BasicPrompt`/`TaskTitledList`)
  - JSON-based RPC stages (`FarStage`) compiled from prompt specs
  - Transport layer wrappers for OpenAI‑style chat/completion/vision and transcription

Key Modules

- RemoteTask.h
  - Response types: `OpenAiModelResponse`, `OpenAiResponse` (chat/completion), `DalleResponse` (images).
  - `TaskMgr`: high-level API for remote calls:
    - `GetModels`, `GetCompletion`, `GetChat`, `GetGenericPrompt`
    - `GetVision`, `GetTranscription`
    - `GetBasic/GetJson/Get` for flexible text/JSON tasks
    - `GetFarStage` to execute compiled `FarStage` JSON over the wire
  - Also hosts deprecated, more specialized helpers (token/phrase/action analysis) retained for backwards‑compatibility.

- CreateInput*.cpp
  - Routines to convert structured requests into `BasicPrompt` trees (e.g., translation, image creation/editing).

- StageCompiler.cpp (in Runtime)
  - Compiles a high‑level stage specification into `FarStage` (JSON with function defs, system, model name, and tokens).

Workflows

- Text completion and chat
  - Build `BasicPrompt` (lists, title/value formatting) or direct chat messages; call `TaskMgr::GetCompletion`/`GetChat`.

- Vision and transcription
  - For images: prepare JPEG bytes and call `GetVision` with `VisionArgs`.
  - For audio: call `GetTranscription` with `TranscriptionArgs`.

- JSON RPC over FarStage
  - Compile stage (Runtime) → call `TaskMgr::GetFarStage(stage, fn_i, args, WhenResult, WhenDone)`.

Extending

- Add a provider: implement HTTP plumbing and add `TaskMgr` entrypoints mirroring the existing OpenAI methods.
- Add a prompt builder: extend `CreateInput*.cpp` or add new functions that populate `BasicPrompt`.

Public Surface

- Include umbrella: `#include <AI/Core/Prompting/Prompting.h>`
