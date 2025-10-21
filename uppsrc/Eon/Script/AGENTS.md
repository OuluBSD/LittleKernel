Eon/Script — AGENTS Guide

Scope
- This file governs the entire `uppsrc/Eon/Script` package tree.
- Audience: contributors extending the Eon script DSL, loaders, and AST→VFS implementation.

Purpose
- Parse the Eon script DSL (AST) into structured definitions and materialize them into the running VFS/Engine.
- Provide a clear separation between parsing (collecting definitions) and implementation (creating VFS objects and linking them).

Package Map (Files + Roles)
- `Script.h` umbrella header. All implementation files in this package must include this first (U++ BLITZ policy).
- `Def.h` lightweight data model: Id/Definition structs for Global/World/Pool/Entity/Component/Chain/Loop/State.
- `Loader.h` loader class hierarchy declarations: `ScriptLoader`, `ScriptSystemLoader`, `ScriptMachineLoader`, `ScriptTopChainLoader`, `ScriptChainLoader`, `ScriptLoopLoader`, `ScriptStateLoader`.
- `ScriptLoader.cpp` AST traversal, error reporting, and implementation orchestration.
- `LoopLoader.cpp`, `ChainLoader.cpp`, `...Loader.cpp` sub-loaders that prepare/validate and then implement specific parts (atoms/links/sides/states).
- `LoaderBase.inl` common helpers for loader specializations.
- `EcsLoader.h`, `EonLoader.*`, `ToyLoader.*`, `ShadertoyLoader.*` examples/integration load helpers.
- `Builder.*` optional construction helpers used by tests/examples.

High-Level Flow
- Compile: `Compiler` produces an `AstNode*` root for the Eon script.
- Parse: `ScriptLoader::LoadCompilationUnit` builds `CompilationUnit` (`Def.h` types) by walking AST.
- Implement: `ScriptLoader::ImplementScript` iterates definitions, calls sub-loaders’ `Load()` to materialize VFS nodes, link atoms, solve side-links, and start loops.
- ECS: After script implementation, `ScriptSystemLoader::LoadEcs()` completes ECS system and pool population.

Key Types
- `ScriptLoader` is the package entry-point System. It owns `CompilationUnit` and a `ScriptSystemLoader` instance and handles post-load queues.
- `Script{System|Machine|TopChain|Chain|Loop|State}Loader` specialize work for each definition level. Each exposes `bool Load()` and `String GetTreeString(int)` and aggregates child loaders.
- `Def.h` holds the minimal, serializable definition data (no heavy logic or VFS mutations here).

Header Include Policy (U++ BLITZ)
- Every `.cpp`/`.icpp` in this package must begin with `#include "Script.h"`.
- Do not include other package headers first. Keep external/rare includes local to the implementation file and after `Script.h`.
- Only `Script.h` aggregates this package’s headers and wraps them in `NAMESPACE_UPP` via the main header.

Subpackage Independence
- `Eon/Script` depends on upstream packages (e.g., `Eon/Core`, `Vfs`) via their main headers only.
- Do not include internals of sibling or parent Eon subpackages. The parent may include this package’s main header when needed, not vice versa.

Error Reporting
- Use `ErrorSource::AddError(loc, msg)` via `ScriptLoaderBase::AddError` where AST `FileLocation` is available.
- Use `ASSERT` for invariants and `TODO` blocks are acceptable for incomplete branches during development, but prefer clear errors where user input is involved.

AST → VFS Responsibilities
- Parsing (collect definitions): stays in `ScriptLoader::*LoadX(Definition&, AstNode*)` and sub-loaders that build `Def.h` structures.
- Implementation (mutate VFS): lives in `*Loader::Load()`, `ConnectSides`, and helpers that create atoms/links/spaces through VFS APIs.

Planned API Migration: Builder/Context style
- Goal: Replace bool-returning parsing helpers like `bool LoadPool(PoolDefinition&, AstNode*)` with direct, declarative builder/context calls that mutate the VFS.
- Example direction (naming subject to `CODESTYLE.md`):
  - From: `bool LoadPool(Eon::PoolDefinition& def, AstNode* n)`
  - To: `PoolContext& AddPool(/* inputs resolved from AST */)` where the context:
    - Owns references to the target VFS node(s) (e.g., world/pool VFS paths).
    - Applies ID mapping and `args` directly into VFS.
    - Exposes fluent methods for nested entities/components: `EntityContext& AddEntity(...)`, `ComponentContext& AddComponent(...)`.
- Rationale:
  - Reduce double handling of definitions: parse once, then build directly with contextual, testable objects.
  - Make script features available for reuse from `Eon/Core` without binding to AST types.

Migration Constraints and Plan
- Non-breaking path:
  - Keep current `Load*` functions during transition; implement them as thin adapters that translate AST to context calls.
  - Add the new context types in `Eon/Core` (so other systems can use them). `Eon/Script` will depend on these via `Eon/Core/Core.h` or the package main header.
- Boundary: Context classes live in `Eon/Core` to avoid circular dependency; `Eon/Script` uses them but does not define them.
- Error model:
  - Context methods should not return `bool`. Prefer either:
    - Record errors via an injected `ErrorSource` (propagating `FileLocation` when available), and/or
    - Throw internal exceptions for invariants only (not for user errors).
- VFS mutation guidelines:
  - Contexts should accept a root `VfsValue&` and perform `GetAdd()`/`GetAdd(part, 0)` style creation, ensuring idempotency.
  - Provide `Undo()` where feasible for partial construction if later steps fail. Sub-loaders already implement rollback for loops (`UndoLoad`). Aim for parity for Pool/Entity/Component contexts.

Sketch of the Target API (for orientation)
- Pool
  - `class PoolContext { public: VfsValue& v; /* +refs to world/pool nodes */ /* args accessors */ };`
  - `PoolContext& AddPool(VfsValue& world, const Id& id, const ArrayMap<String,Value>& args, const FileLocation* loc);`
- Entity
  - `EntityContext& AddEntity(PoolContext&, const Id&, const ArrayMap<String,Value>&, const FileLocation*);`
- Component
  - `ComponentContext& AddComponent(EntityContext&, const Id&, const ArrayMap<String,Value>&, const FileLocation*);`
- Chains/Loops
  - Analogous `AddChain/ AddLoop/ AddAtom` that return contexts bound to loop/space nodes and link helpers to connect atoms and side-links.

Where to Wire the Adapter
- In `ScriptLoader::LoadWorld/LoadPool/LoadEntity/LoadComponent`, resolve Id + args from AST, then call `AddPool/AddEntity/...` on the appropriate context root.
- Keep existing `Script*Loader::Load()` semantics intact until all callers adopt contexts.

Side Links and Interfaces
- `ScriptLoopLoader` currently computes and validates side-links and then calls `LinkBase::LinkSideSink(...)`.
- As contexts get introduced, provide equivalent helpers that:
  - Expose `IfaceConnTuple` and resolve `IfaceConnLink`s consistently with `Def.h`.
  - Offer deterministic rollback if any linking step fails.

Copying Features to Eon/Core
- Place reusable contexts and low-level VFS mutation helpers in `Eon/Core`.
- `Eon/Script` provides only AST→Context translation and high-level orchestration.
- Do not introduce Core→Script dependencies; keep the direction one-way (Script→Core).

Extending the DSL or Runtime
- New AST constructs: add cursor handling in `ScriptLoader.cpp` and extend the appropriate `*Definition` in `Def.h`.
- New atom/link types: register them with `VfsValueExtFactory::AtomDataMap()` and ensure `ScriptLoopLoader` can discover and match actions to types.
- New systems/components/pools: add context methods in Core, then route AST in Script.

Debugging Tips
- Use `GetTreeString()` on definitions and loaders to inspect constructed trees in logs.
- `VERBOSE_SCRIPT_LOADER` in `ScriptLoader.cpp` helps print AST snippets.
- `ScriptLoader::Dump()` prints current loader tree and accumulated error state.

Package Conventions Checklist
- `.cpp/.icpp` include `Script.h` first.
- Do not include external headers in package headers other than `Script.h`.
- Keep `AGENTS.md` listed first in `Script.upp`’s `file` section for discoverability.
- Prefer small, composable helpers with clear responsibilities; avoid cross-package header includes that break BLITZ or subpackage independence.

Current Tasks (keep updated in a local CURRENT_TASK.md if actively working here)
- Add Core contexts (Pool/Entity/Component/Chain/Loop/Atom) and wire Script adapters.
- Replace `bool Load*(..., AstNode*)` call sites incrementally with `Add*` context calls.
- Add minimal rollback for Pool/Entity/Component creation on error to match loop rollback behavior.

