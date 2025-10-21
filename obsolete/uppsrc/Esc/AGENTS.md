AGENTS

Scope
- Applies to `uppsrc/Esc`.

Purpose
- Embeddable scripting language (Esc) with bytecode VM and standard library; used across U++ for scripting tasks.

Key Areas
- Values/arrays/maps, compiler/IR/bytecode, VM, stdlib, and integration helpers.

Extension Points
- Add stdlib functions in `EscStdLib.cpp`; extend VM carefully with tests.

.upp Notes
- Ensure `AGENTS.md` is first in `Esc.upp`.

