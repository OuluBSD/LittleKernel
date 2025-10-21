AGENTS

Scope
- Applies to `stdtst` test suite for `stdsrc` packages.

Purpose
- Provide lightweight, self-contained unit tests for STL-backed Core (`stdsrc/Core`).

Conventions
- Each test is a U++ package with a `.upp` manifest and a single `main` or asserts.
- Tests include `#include <Core/Core.h>` and use `Upp::` types.
- Prefer plain `printf`/`std::cout` outputs and exit code to signal success/failure.

