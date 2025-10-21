AGENTS

Scope
- Applies to `upptst/MetaEnvironment`.

Purpose
- Provide buildable test scaffolding for MetaEnvironment integration points (builders, assist, storage, overlay).
- Use stubs that compile today and guide future implementation of real test coverage.

Notes
- Try to keep dependencies minimal; prefer Core/Vfs modules and wrap IDE-specific interfaces via adapters.
	- However, linking errors pulled the whole IDE package.
- Convert stubs into assertions once the overlay-backed APIs are finalized.
