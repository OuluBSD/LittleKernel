AGENTS

Scope
- Applies to `uppsrc/plugin` and all subdirectories.

Purpose
- Third-party integrations, codecs, and utilities packaged as U++ packages. Examples: `png`, `jpg`, `lz4`, `zstd`, `sqlite3`, `enet`, `glew`, `portaudio`, `openai`, etc.

Guidelines
- Keep each external integration isolated in its own subfolder with a `.upp` manifest.
- Prefer minimal wrappers that expose a clean U++-style API or simply provide libraries via the `.upp` `library(...)` lines.
- Document special link flags or platform conditions in the `.upp` file; add a brief `AGENTS.md` in subfolders for complex packages.

Build Notes
- Many packages only declare libraries and headers; some ship minimal source. Check each `.upp` for `library(...)`, `pkg_config(...)`, and `uses(...)` to compose dependencies.

