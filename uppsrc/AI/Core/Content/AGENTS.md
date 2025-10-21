Content — Text Objects and Solvers

Overview

- High-level text constructs and processes built on `DataModel`:
  - Biographies, image-biographies and their summaries
  - Lyrical structures, song building helpers, and script text
  - Source text ingestion and transcripts
  - Utility editors (notepad), reasoning stubs, and content factories

Key Modules

- Biography.*
  - `BioImage`, `BioYear`, `BiographyCategory`, and `Biography` ECS component.
  - Supports timelines with yearly entries, images (with AI-derived keywords/text), and summary ranges (`BioRange`).
  - `Sort()` ensures categories follow a canonical order (see Core enums).
  - Helpers to realize image summaries and compute average per-year scores.

- ImageBiography.* and ImageBiographySummary.*
  - `ImageBiographyProcess` and `ImageBiographySummaryProcess` are `SolverBase`-derived multi-phase analyzers.
  - Traverse imagery, produce AI prompts, and generate clustered/summary text for ranges and entire categories.

- Lyrical.* / LyricsSolver.* / LyricStructSolver.*
  - `Lyrical` holds dynamic lines (`DynLine`) with elements (`LineElement`) and scoring.
  - Structure solvers suggest line partitions, connectors, and styles.
  - Use alongside `Song` to scaffold verse/chorus/bridge flows.

- ScriptText.* / Transcript.* / SourceText.*
  - Script authoring helpers and transcripts for speech.
  - `SourceText` bridges to `DataModel::SrcTxtHeader`/`SrcTextData` for tokenization and classification.

- Artist.* / Composition.* / Notepad.* / Reasoning.* / Factory.*
  - Small ECS components and scaffolds for higher-level content operations.

Workflows

- Build a biography from media
  - Ingest images and text → fill `BioYear::images` and text/keywords.
  - Run `ImageBiographyProcess::Get(profile,snapshot)` to analyze image content.
  - Optionally summarize with `ImageBiographySummaryProcess` to produce per-range narratives.

- Generate lyric suggestions
  - Prepare `Lyrical` lines; let structure solvers propose `DynLine` attributes (style, connector).
  - Integrate with Prompting if remote LLM guidance is needed.

- Script/script-like text
  - Use `ScriptText` to organize segments and generate/transform with `DataModel` classifiers and Prompting.

Extending

- Add new content type: create a component + optional process; expose under `Content.h`.
- Add solvers: derive from `SolverBase` with phases that read/write component data.

Public Surface

- Include umbrella: `#include <AI/Core/Content/Content.h>`
