AGENTS

Scope
- Applies to `uppsrc/VirtualGui` and subpackages (e.g., `VirtualGui/SDL2GL`).

Purpose
- Abstract GUI implementation decoupled from native window managers, used to run U++ on custom or embedded backends.

Key Areas
- `VirtualGui.h` API and glue (`Local.h`, `Ctrl.h`), image handling, input/keys, and top/ctrl event routing. Platform-specific parts live in subpackages.

.upp Notes
- Ensure `AGENTS.md` is first in `VirtualGui.upp`.

