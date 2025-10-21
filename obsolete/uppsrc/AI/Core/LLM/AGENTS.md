LLM — ECS Components for Chat/Completion

Overview

- Minimal ECS components that surface Chat and Completion capabilities to the rest of the system. They delegate actual remote calls and prompt construction to the Prompting package.

Components

- AiChatComponent
  - ECS `Component` labeled "AI: Chat". Extend this when binding chat sessions or UIs to Prompting’s `TaskMgr::GetChat` APIs.

- AiCompletionComponent
  - ECS `Component` labeled "AI: Completion". Extend this when binding single‑shot completions to Prompting’s `TaskMgr::GetCompletion` or `GetBasic/GetJson` flows.

Usage

- From a process or UI controller, construct prompts using Runtime’s `BasicPrompt`/`TaskTitledList`, then call into Prompting’s `TaskMgr` for the transport.
- The components exist to make these features discoverable in the ECS model tree and to attach per‑node settings in the future.

Extending

- Add settings/fields to components for model choice, temperature, system prompts, etc.
- Implement per‑component helpers that wrap `TaskMgr` calls for the common tasks your app needs.

Public Surface

- Include umbrella: `#include <AI/Core/LLM/LLM.h>`
