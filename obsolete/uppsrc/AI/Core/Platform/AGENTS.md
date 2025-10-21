Platform — Roles, EPK, and Profile Analysis

Overview

- Analyzes personas and content for distribution platforms: role scoring, EPK text/photos, and profile‑level tasks.

Key Modules

- Browser
  - `DatabaseBrowser` summarizes dataset attributes/actions/colors/elements; exposes multiple display modes and column orderings.

- PlatformManager (Component)
  - Stores per‑platform analysis (`PlatformAnalysis`) and society role scores.
  - `GetPlatform`, `GetAddRole`, and role lookups for platform‑specific summaries.

- PlatformProcess (SolverBase)
  - Phases include role score analysis, platform role mapping, EPK text field analysis, EPK photo type extraction, and AI prompt generation for photos.
  - Batch task structures for image vision tasks and image summary tasks.

- PlatformProfileProcess (SolverBase)
  - Focuses on profile EPK tasks: analyze/summarize photo prompts, prepare DALL·E examples. Maintains `ProfileEPKTask` links for cross‑refs.

- ProfileData
  - Aggregates platform data per profile; provides load/store helpers and a global registry (`GetAll`, `Get`).

Workflows

- Build EPK for a profile
  - Use `PlatformProfileProcess::Get(dataset_ptrs, dir)`; traverse tasks; query `PlatformManager` for platform/role summaries; generate AI prompts and example images.

Public Surface

- Include umbrella: `#include <AI/Core/Platform/Platform.h>`

