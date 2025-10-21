Publishing — Releases and Multi‑phase Solver

Overview

- Encapsulates release artifacts and a solver to generate analyses and marketing collateral.

Key Modules

- Release (Component)
  - Fields: `title`, `date`, key/value `data`, `ideas`, `year_of_content`, summaries/analyses (lyric, song, general), and `cover_suggestions`.
  - `Store(Entity&)` and `LoadTitle(Entity&, title)` synchronize to/from the owning entity.
  - `GetSongs()` links release to associated songs.

- SnapSolver (SolverBase)
  - Phases: lyric summaries/analysis, psychoanalysis/social-psychology, market value, marketing suggestions, art/cover suggestions, and DALL·E cover images.
  - `Get(Release&)` factory runs end-to-end generation for a release.

- ReleaseBriefing (Component)
  - Placeholder for a structured brief; expand with sections required by your publishing flow.

Workflows

- Build a release package
  - Aggregate content (songs, lyrics) under `Release` → run `SnapSolver` to fill analyses and suggestions → attach images via Media.

Public Surface

- Include umbrella: `#include <AI/Core/Publishing/Publishing.h>`

