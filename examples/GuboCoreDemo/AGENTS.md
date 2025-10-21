AGENTS

Scope
- Applies to `examples/GuboCoreDemo`.

Notes
- Example package showing a minimal 3D GUI using GuboCore.
- Follows header include policy: source includes only `GuboCoreDemo.h` first; that header aggregates `GuboCore.h`.
- This demo paints a colored 3D box and reacts to mouse enter/leave and capture.

.upp
- Keep `AGENTS.md` first in `GuboCoreDemo.upp`.
- Add `GuboCore`, `GuboLib`, and `Eon` to `uses` for manager/engine wiring.

