Persona — Profiles, Perspectives, Role Datasets

Overview

- Encapsulates persona data attached to entities and profiles, plus structured perspective annotations.

Key Modules

- Profile
  - ECS `Component` with `name`, `created`, user `description` and `preferences`, and supported `languages`.
  - `FindSnapshotRevision(i)` integrates with biography snapshots (see Social/Content flows).

- PerspectiveComponent
  - Per‑profile perspective summary with `description`, `reference`, user notes, and positive/negative attributes.

- Male.h / Female.h
  - Dataset component stubs for role‑like facets (e.g., `Leadership`, `Justice`, `Sentimentality`, `Caretaker`). Each is a `COMPONENT_STUB_HEADER(...)` and can be filled later.

Usage

- Attach `Profile` and `PerspectiveComponent` to an entity or project to drive downstream Content and Social analyses.
- Use language indexes to gate NLP operations per profile.

Public Surface

- Include umbrella: `#include <AI/Core/Persona/Persona.h>`
