AGENTS

Scope
- Applies to `stdsrc/Core` (STL/Boost-backed replacement for U++ Core API) and any nested subpackages created under this directory.

Purpose
- Provide the same developer-facing API names as U++ Core (e.g., `Upp::String`, `Upp::WString`, containers) but implement functionality using C++ standard library (and Boost if needed).

Guidelines
- Preserve public class names and method shapes where practical. Favor adapter wrappers that forward to STL types.
- Prefer inheritance from STL types when it’s safe and ergonomic (e.g., `class String : public std::string {}`), otherwise use composition.
- Keep implementation minimal first; expand only as API needs arise. Focus on source compatibility for common use.
- Avoid pulling platform headers here. Keep this package portable and header-only where possible.

Header Include Policy (U++ BLITZ)
- Source files must include only the package’s main header first: `#include "Core.h"`.
- The main header `Core.h` aggregates all headers and wraps them within `namespace Upp` via `NAMESPACE_UPP` macros.
- Do not add `#include` lines to non-aggregator headers; include system/third-party headers only from `Core.h`.

Notes
- This package is independent of `uppsrc/Core`. Do not cross-include internals from `uppsrc`.
- If a feature cannot be mapped 1:1 to STL cleanly, document the deviation in `CURRENT_TASK.md` and add shim helpers as needed.

