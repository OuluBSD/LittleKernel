AGENTS

Scope
- Applies to `uppsrc/AI` and its entire sub-tree.

Purpose
- Modular AI framework used across the repository and integrated into TheIDE. It splits into domain logic under `Core/*` and user‑facing UI under `Ctrl/*`.

Top‑Level Packages
- `AI.upp` (meta‑package): pulls in `AI/Ctrl` and, transitively, the required `AI/Core/*` modules.
- `AI/Core/Core.upp` (aggregate): composes the following domains:
  - Base: primitives and algorithms (graphs, search, neural, genetic, action planning, enums, structural traits).
  - Runtime: execution, scheduling, and task orchestration for AI workflows.
  - Prompting: prompt construction, templates, and chain utilities.
  - LLM: language model adapters and related glue.
  - Persona: persona/config models layered over prompting/LLM.
  - Publishing/Marketing/Social: domain workflows that consume content and produce shareable artifacts.
  - Content/Media/Video: data models and transforms for text, images, audio/video; editors exist under `Ctrl`.
  - Tools: general utilities for AI pipelines.
  - DataModel: shared schemas and higher‑level types.
  - Platform: host/platform integrations.

UI Packages (`AI/Ctrl`)
- Public: top‑level UI and common views.
- People: persona editors and related forms.
- Platform: platform managers and selectors.
- Text: text/lyrics/script editors and reasoning tools.
- Video: video storyboard, prompts, and source management.

How It Fits With TheIDE
- The TheIDE integration lives primarily in `uppsrc/ide/AI` (see that package’s AGENTS). `AiProvider` in `uppsrc/ide` bridges IDE UI to the AI framework here.

Related Guides
- IDE integration: `uppsrc/ide/AGENTS.md` and `uppsrc/ide/AI/AGENTS.md`.
- ECS/Dataflow engine used by some AI tools: `uppsrc/Eon/AGENTS.md`.

Conventions
- Keep domain logic in `AI/Core/*` and UI code in `AI/Ctrl/*`.
- Each subpackage carries its own `AGENTS.md`; consult those for deeper details and extension points.
- Prefer adding documentation topics in `.tpp` when exposing public APIs.

Extending
- New domain: create `AI/Core/<Domain>` and register in `AI/Core/Core.upp` aggregate.
- New UI: create `AI/Ctrl/<Domain>` and wire views/dialogs; expose minimal headers for integration.
- When adding new packages, include an `AGENTS.md` and list it first in the `.upp` file list.

