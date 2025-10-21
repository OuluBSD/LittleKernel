Eon/Script
==========

Purpose: Script DSL model and loaders.

- Parses and materializes Eon `.eon` scripts: machines, loops, drivers, and ECS sections.
- Owns ShaderToy serial loader and loop/chain builders.
- Exposes `ScriptLoader` as a system to enqueue and load scripts at runtime.

Include
- `#include <Eon/Script/Script.h>`

Notes
- See `obsolete/share/eon/tests` for example scripts.

