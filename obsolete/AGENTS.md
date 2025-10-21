# AGENTS

Read These First
- `CODESTYLE.md`: coding conventions and design tenets.
- `TASKS.md`: tasks and roadmap for the repository.
- `HIERARCHY.md`: overview of important folders.
- `agents/`: helper code and examples used by AGENTS guides.
- `stdsrc/AGENTS.md`: STL-backed Core (U++-compatible) for agents and tests.

Deep Dives
- TheIDE (IDE application and modules): see `uppsrc/ide/AGENTS.md`. Each subpackage under `uppsrc/ide/*` also has its own AGENTS with extension points and file maps.
- AI (Core + UI framework): see `uppsrc/AI/AGENTS.md` for the package map and how it integrates with TheIDE.
- Geometry + Vision + Sound: `uppsrc/Geometry/AGENTS.md`, `uppsrc/ComputerVision/AGENTS.md`, `uppsrc/Sound/AGENTS.md`, `uppsrc/SoundCtrl/AGENTS.md`.
- Forms: `uppsrc/Form/AGENTS.md`, `uppsrc/FormEditor/AGENTS.md`.
- Meta tools: `uppsrc/MetaCtrl/AGENTS.md`, `uppsrc/Vfs/AGENTS.md`.
- Developer console: `uppsrc/DropTerm/AGENTS.md`.
- Eon (ECS + Dataflow + DSL): see `uppsrc/Eon/AGENTS.md` for a deep dive into:
  - The script DSL (`machine`, `ecs`, `loop`, `driver`, `state`) and examples from `obsolete/share/eon/tests`.
  - Atoms/Links/Loops, side links, queue sizing, and how the loader materializes graphs.
  - Extending with new Systems, Components, Atoms, and Links (registration patterns included).

Conventions For Packages
- Place an `AGENTS.md` in every package directory; it applies to that directory tree. Nested AGENTS override parents.
- In `.upp` manifests, list `AGENTS.md` as the first entry in the `file` section for quick discoverability. All relevant `.upp` files in `uppsrc` have been updated accordingly.

Current Task Files (`CURRENT_TASK.md`)
- Any directory or package may contain a `CURRENT_TASK.md`. Treat it as the authoritative, living note for what is being worked on right now in that scope.
- Before starting changes in that scope, read `CURRENT_TASK.md` and align the plan; after completing changes, update it to reflect what was done and what’s next.
- If `CURRENT_TASK.md` resides in a package (a directory with a `.upp` manifest), add it to the package’s `file` list:
  - Preferably first; if `AGENTS.md` exists, list `CURRENT_TASK.md` immediately after it.
- Rationale: we keep tasks in the working tree so they’re visible in TheIDE and play nicely with AI/developer tools.


Header Include Policy (U++ BLITZ)
- Source files (`.cpp`, `.icpp`, `.c`) in any package (with a `.upp` file) must include only the package's main header first, using a relative include: `#include "PackageName.h"`.
- Rationale: we use U++'s BLITZ (custom header precompilation). It is intentionally simple; including only the main header first keeps BLITZ effective and build times stable.
- After the main header, add other includes only if the implementation needs to forward-include something from a later package in the dependency queue. Avoid direct intra-package header includes from source files.
- If an implementation file requires a rare or file-specific include, keep it local to that implementation file; do not add it to `PackageName.h`. We want the global header include stack optimally short for later packages.
- Every package must provide a main header named `PackageName.h`. This header aggregates/includes all other headers in the package. Keep it minimal beyond aggregation for clarity (small packages may be exceptions).
- Prefer that headers other than the main header do not declare a namespace. Instead, the main header should wrap the aggregated headers in `namespace Upp` via the `NAMESPACE_UPP` macro. This reduces clutter and plays better with BLITZ.
- Treat this as a no-exceptions pattern unless explicitly justified. Many non-U++ C++ conventions differ; we require this approach here.
- Do not add `#include` statements to any package headers other than the main header. The only exception is truly inline header includes that belong to that header. Rationale: it is ugly for BLITZ and third-party/system headers might end up under `NAMESPACE_UPP` when pulled via the main header wrapper.
- `.icpp` files are not "companion includes" for headers. Treat `.icpp` as implementation files (like `.cpp`/`.c`): compile them normally, and have them start with `#include "PackageName.h"` and any rare, file-specific includes afterward as needed.

Subpackage Independence
- Subpackages like `AI`, `AI/Core`, `AI/Core/Core` are independent packages; do not gather headers in the parent package.
- A parent package may include only the subpackage's main header (e.g., `#include "Core.h"` from `AI`). Do not cross-include subpackage internals directly.
- Subpackages must chain dependencies correctly by including previous subpackages or other packages as needed, without creating circular dependencies.
- In general, a subpackage should not include its parent package. The reverse is acceptable when the subpackage genuinely extends the parent.


Book Chronicle
- Chronicle work in `Book/` by pairing first-person chapters (`Book/<index> - <Title>.md`) with compact summaries that reuse the chapter title (`Book/<Title>.md`).
- Keep both files current whenever work progresses; refer to the user in third person with he/him pronouns, selecting from {Spearhead, Captain, Curator, Director, Chief, Ringleader} to match context.
- Preserve prior chapters—append new material so the narrative reflects real-time progress.

Book Contribution Gate (Mandatory)
- Before editing anything under `Book/`, you must read `Book/AGENTS.md` and follow its style rules (headings, Date Span placement, list formats, and reference style).
- Pull requests changing `Book/*` must explicitly confirm compliance (e.g., checklist item: "Read and applied Book/AGENTS.md").
- Maintainers: reject or request changes if the above confirmation or formatting is missing.
