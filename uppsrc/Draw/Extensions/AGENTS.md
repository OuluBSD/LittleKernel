AGENTS

Scope
- Applies to `uppsrc/Draw/Extensions`.

Purpose
- Hosts Draw add-ons (backends, FFT, simple image helpers) used by higher level rendering stacks.

Conventions
- Keep backend-specific code isolated; share abstractions via `Extensions.h` only.
- Stick to U++ BLITZ rules: implementation files include `Extensions.h` first, extra includes stay local.

