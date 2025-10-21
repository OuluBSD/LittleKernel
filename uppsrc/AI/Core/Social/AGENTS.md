Social — Biography To Social, Roles, Marketplace

Overview

- Translates biographical/persona information into social-domain artifacts: audience reactions, role analyses, EPK platform descriptors, and marketplace items.

Key Modules

- BiographyPlatform (Component)
  - Holds per-profile/category analyses and platform-specific biography projections (`PlatformBiographyPlatform`).
  - Stores image prompt group analyses and a marketplace item list.
  - Helpers to realize prompts and extract required roles/categories.

- SocialHeaderProcess (SolverBase)
  - Produces merged audience reactions and header summaries based on `RoleProfile` and biography analyses.

- Justice.h and other role datasets
  - Small component stubs for public‑domain roles (e.g., `Litigation`, `Lawyer`, `Judge`).

Workflows

- Summarize biography for social platforms
  - Fill `BiographyPlatform` from Content/Platform processes; run `SocialHeaderProcess` to merge and produce shareable headers.

Public Surface

- Include umbrella: `#include <AI/Core/Social/Social.h>`

