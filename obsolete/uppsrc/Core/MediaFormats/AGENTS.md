AGENTS

Scope
- Applies to `uppsrc/Core/MediaFormats`.

Purpose
- Centralizes sample/format descriptors and media-related helpers required by ECS, audio, and video pipelines.
- Keeps media abstractions headless-friendly while higher layers add GUI or hardware bindings elsewhere.

Responsibilities
- Destination for `SampleBase.*`, `Samples.*`, `Formats.*`, `Audio.*`, and allied helpers.
- Document format compatibility expectations (channel counts, sample rates, interchange rules).

Guidelines
- Avoid direct dependencies on rendering or hardware packages; keep conversions pure and well-specified.
- Include targeted comments noting when a format mirrors external standards (e.g., ffmpeg, OpenAL).
- Implementation files must include `MediaFormats.h` first.

Migration Notes
- Update ECS documentation when sample helpers migrate here, noting any behavior changes.
- Maintain a checklist in `CURRENT_TASK.md` for pending format migrations.
