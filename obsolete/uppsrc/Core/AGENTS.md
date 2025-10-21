AGENTS

Scope
- Applies to `uppsrc/Core` (non-GUI core library) and nested subpackages like `Core/SSL`, `Core/POP3`, `Core/SMTP`, `Core/Rpc`, `Core/SSH`.

Purpose
- Foundation library: memory, containers, strings, streams, file/process IO, threading, logging, serialization, XML/JSON, app profile, and platform glue.

Key Areas
- Containers and algorithms: `Index`, `Vector/Array`, sorting, recycler, packed data.
- Strings/Unicode: `String/WString`, UTF, charset tables, filters.
- Streams/Files: `Stream`, `FileMapping`, filters, block streams.
- Processes: `LocalProcess` and helpers.
- Diagnostics: `Profile`, `Diag`, `Log`, debug utilities.
- Build support and misc: `Path`, `Ini`, `Random`, math utils.

Subpackages
- `Core/SSL`, `Core/SSH`, `Core/SMTP`, `Core/POP3`, `Core/Rpc`: protocol/crypto helpers that extend Core.

Conventions
- Keep platform specifics localized; prefer portable abstractions.
- Follow `/CODESTYLE.md`; add API docs in Topic++ (`src.tpp`).

Extension Points
- New containers/algorithms go under `Containers` with measured value and tests.
- Extend logging via `Log.cpp`; consider categories.
- Add stream filters under `FilterStream`.

.upp Notes
- Ensure `AGENTS.md` is the first item in `Core.upp` `file` list.

