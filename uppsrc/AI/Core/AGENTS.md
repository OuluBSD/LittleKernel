AI/Core — Package Overview

- Purpose: Shared AI domain primitives, models, and pipelines built on top of the engine’s ECS/VFS runtime. This folder is the hub that higher-level apps and tools import to work with text, media, personas, platforms, and LLM-backed processes.
- Umbrellas: Each subpackage exposes a single umbrella header you include from clients. Prefer these over including individual headers.

What Lives Here

- Base: Foundational algorithms and data structures used across the stack (planners, search, graphs, optimizers).
- Core: Common enums, colors, natural language and phoneme utilities, generic helpers.
- DataModel: Data structures for tokenized text, phrases, attributes/actions, exportable analytics, and source-text databases.
- Content: High-level text objects and solvers (biography, lyrics, scripts, transcripts, notes).
- LLM: Lightweight ECS components for Chat/Completion experiences; bridges to Prompting/RemoteTask.
- Marketing: Owner/audience domain, needs and actions, audience processing.
- Media: Image/audio utilities, layers, aspect fixing pipeline.
- Persona: Persona profiles, perspectives, and role datasets.
- Platform: Social-platform analysis, EPK flows, and role scoring.
- Prompting: Prompt builders, JSON/RPC stages, and remote tasks (OpenAI, DALL·E, Whisper-like transcription).
- Publishing: Release objects and multi‑phase release solver.
- Runtime: Agent, process framework, prompt builders, remote stage compilation.
- Social: Biography → social marketplace projections and social-processes.
- Tools: Project wizard and VFS-driven program scaffolding.
- Video: Video prompts, storyboards, and source file primitives.

How Packages Fit Together

- Core enumerations and helpers are consumed everywhere (DataModel, Content, Social, etc.).
- DataModel owns the structured text database used by Content solvers and Platform/Social analyses.
- Prompting provides the transport and formatting to call remote models; LLM exposes ECS components built on that.
- Runtime hosts the agent/process framework used by Prompting, Platform, Content, and Tools.
- Media/Video provide visual assets and analysis that Platform/Social can consume (e.g., EPK photos, biographical imagery).

Include Map (Umbrella headers)

- Base:        `#include <AI/Core/Base/Base.h>`
- Core:        `#include <AI/Core/Core/Core.h>`
- DataModel:   `#include <AI/Core/DataModel/DataModel.h>`
- Content:     `#include <AI/Core/Content/Content.h>`
- LLM:         `#include <AI/Core/LLM/LLM.h>`
- Marketing:   `#include <AI/Core/Marketing/Marketing.h>`
- Media:       `#include <AI/Core/Media/Media.h>`
- Persona:     `#include <AI/Core/Persona/Persona.h>`
- Platform:    `#include <AI/Core/Platform/Platform.h>`
- Prompting:   `#include <AI/Core/Prompting/Prompting.h>`
- Publishing:  `#include <AI/Core/Publishing/Publishing.h>`
- Runtime:     `#include <AI/Core/Runtime/Runtime.h>`
- Social:      `#include <AI/Core/Social/Social.h>`
- Tools:       `#include <AI/Core/Tools/Tools.h>`
- Video:       `#include <AI/Core/Video/Video.h>`

Conventions & Patterns

- Components and Processes: Most domain types are ECS `Component`s; long‑running operations are derived from `SolverBase` or `AiProcessBase` (see Runtime).
- Visitor/Serialization: Types implement `Visit(Vis&)` and often `Serialize(Stream&)` for persistence and JSON IO.
- Hashing and Keys: Many models define stable `GetHashValue()` for deduplication and mapping.
- Color/Attr/Action headers: `Core/Container.h` defines `ActionHeader` and `AttrHeader` used across classification and content modeling.

Extending the System

- New domain: Create a subpackage with an umbrella header, export ECS components and any processes. Follow existing folder patterns.
- New search/optimizer: Add to Base and keep algorithm interfaces (`Generator`, `TerminalTester`, `HeuristicEval`) consistent.
- New remote call: Extend Prompting (RemoteTask) and expose a high-level function on `TaskMgr` (and optionally a small ECS component under LLM).

