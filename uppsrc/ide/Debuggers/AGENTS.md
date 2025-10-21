AGENTS

Scope
- Applies to `uppsrc/ide/Debuggers`.

Purpose
- Debugging support for GDB, LLDB, and PDB on supported platforms. Provides UI, protocol handling, disassembly, pretty printers, and symbol handling.

Package Overview
- Manifest: `Debuggers.upp` (uses `ide/Common`, `HexView`, `ide/Core`; optionally `plugin/ndisasm`).
- GDB: `Gdb*.{h,cpp}`, `Terminal.cpp`, `Disas.cpp`.
- LLDB: `LLDB*.{h,cpp}` (TheIDE integration; see also `ide/LLDB` standalone app).
- PDB: `Pdb*.{h,cpp,lay,key}`, `cvconst.h`, `*.cpu`, `Cpu.cpp`.
- Common UI: `Debuggers.h` (PCH), `Debuggers.iml`, `Debug.cpp`, `Mem.cpp`, `Sym.cpp`, `Exp.cpp`, `Pretty*.cpp`, `Scripts.cpp`, `Visualise.cpp`, `Data.cpp`, `Tree.cpp`, `Stack.cpp`, `Code.cpp`.

Extension Points
- Add new debugger backends mirroring the GDB/LLDB folder structure and hook into selection logic.
- Extend pretty printers in `Pretty*.cpp` and scripts in `Scripts.cpp`.

Conventions
- Keep protocol logic isolated from UI; disassembly/heavy lifting should not block UI thread.

.upp File Notes
- Ensure `AGENTS.md` is the first file listed; keep resource `app.tpp` grouped at end.

