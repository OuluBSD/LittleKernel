Marketing — Owners, Roles, Audience

Overview

- Models the market-facing side: owners and their roles/needs, and audience analysis processes.

Key Types

- Owner
  - ECS `Component` with `name`, `born`, `description`, `environment`, and arrays of `Role`.
  - `Role` contains `Need`s (with platform toggles) and `RoleAction`s with per‑event text.
  - `GetOpportunityScore(const LeadOpportunity&)` matches external opportunities to owner roles/needs.

- AudienceProcess (SolverBase)
  - Multi‑phase process (e.g., `PHASE_AUDIENCE_PROFILE_CATEGORIES`) for audience profiling.
  - Static `Get(Profile&, BiographyPerspectives&)` factory retrieves a shared instance for profiling runs.

- ConsumerComponent
  - Stub ECS component for consumer‑side concepts; extend as needed.

Workflows

- Define owner roles and needs
  - Populate `Owner.roles[].needs[]` and `actions[]` with event texts.
  - Enable/disable needs per platform with `PlatformNeed.enabled` to tailor EPK/marketing.

- Run audience profiling
  - Use `AudienceProcess::Get` to construct the process with a `Profile` and a `BiographyPerspectives` snapshot; `Start()` the process to fill audience features.

Public Surface

- Include umbrella: `#include <AI/Core/Marketing/Marketing.h>`

