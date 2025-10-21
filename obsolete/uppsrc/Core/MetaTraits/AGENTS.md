AGENTS

Scope
- Applies to `uppsrc/Core/MetaTraits`.

Purpose
- Collects RTTI helpers, type traits, tuple utilities, and hashing helpers formerly embedded in `Core2`.
- Provides the foundational metaprogramming layer required by ECS, Vfs, and higher packages.

Responsibilities
- `TypeTraits*.h`, `RTuple.h`, `GEnums.h`, and related helpers live here once migrated.
- Maintain documentation on how `TypedStringHasher` integrates with runtime registration.

Guidelines
- Keep headers lightweight and header-only; avoid introducing runtime state here.
- When adding new traits, document intended consumers in this file and update dependent AGENTS as needed.
- Follow BLITZ policy: implementation files must include `MetaTraits.h` first (even when only header-only code is present).

Migration Notes
- Update this AGENTS and the package manifest whenever new trait headers migrate from `Core2`.
- Coordinate hash changes with `Core/CompatExt` initializers to avoid breaking serialized data.
