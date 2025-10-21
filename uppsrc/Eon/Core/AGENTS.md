Eon/Core — AGENTS Guide

Scope
- Applies to the entire `uppsrc/Eon/Core` package tree.
- Audience: contributors implementing core atom/link infrastructure, ECS helpers, and AST‑free VFS builders.

Purpose
- Provide the foundational building blocks used by Eon: atoms, links, exchange points, and related helpers.
- Expose AST‑free, VFS‑first context APIs to build ECS objects (pools/entities/components) and loop/chain graphs directly.
- Never depend on Script/Ast types (keep Script confined to `Eon/Script`).

Package Map (Files + Roles)
- `Core.h` umbrella header. All `.cpp/.icpp` in this package must include this first (BLITZ policy).
- `Types.h` core type aliases and small utilities.
- `Container.{h,cpp}` misc helpers used by atoms/links.
- `Atom.h` atom base add‑ons for Eon.
- `LinkUtil.{h,cpp}`, `LinkBase.{h,cpp}`, `LinkFactory.{h,cpp}` link utilities/base and registration.
- `Base.{h,cpp}`, `BaseAudio.{h,cpp}`, `BaseVideo.{h,cpp}`, `Network.{h,cpp}` core base implementations and categories.
- `GenAtom.inl` generator helpers for atoms.
- `Context.{h,cpp}` AST‑free VFS builder API: `PoolContext`, `EntityContext`, `ComponentContext`, `LoopContext`, `ChainContext`, and `ResolveLoopPath`.

Header Include Policy (U++ BLITZ)
- Every implementation file in this package must start with `#include "Core.h"`.
- Do not include third‑party/system headers in package headers other than the main header aggregation. Keep rare includes local to the `.cpp`.
- Keep this package independent of `Eon/Script` (no `AstNode` etc.). Other packages may include `Core.h`.

Subpackage Independence
- `Eon/Core` is a separate package. Don’t include internals from sibling/parent Eon packages.
- Register types and expose minimal, stable APIs consumed by other packages (Script, Ecs, etc.).

Atoms, Links, Interfaces (Core2)
- Register atoms via `VfsValueExtFactory::RegisterAtom<T>(name)`; implement:
  - `static AtomTypeCls T::GetAtomType()`
  - `static LinkTypeCls T::GetLinkType()` and `static LinkTypeCls T::GetLinkTypeStatic()` as required
  - `static String T::GetAction()` to bind human‑readable action ids
- Register links via `VfsValueExtFactory::RegisterLink<T>(name)`.
- Interface metadata:
  - `IfaceConnTuple` describes sink/source channels and side‑linking; call `iface.Realize(atom_type)` and then populate `iface.src[i]`/`iface.sink[i]` for side connections.

Context API (AST‑free VFS Builders)
- `PoolContext`/`EntityContext`/`ComponentContext`
  - Build ECS trees directly under `Engine::GetRootPool()`.
  - Example:
    - `PoolContext world(eng.GetRootPool());`
    - `auto ppl = world.AddPool("world");`
    - `auto player = ppl.AddEntity("player");`
    - `player.AddComponent("Transform", &args);`
- `LoopContext`
  - Build a loop’s atoms/links under a loop/space node (`Engine::GetRootLoop()` parallel to `GetRootSpace()`).
  - `AddAtom(atom_type, link_type, iface, &args, idx)` creates the atom/link, runs `Initialize*`, and records it in the loop. It does not run `PostInitialize`.
  - `MakePrimaryLinks()` links atoms sequentially via exchange points (optional; used for simple chains).
  - `PostInitializeAll()/StartAll()/UndoAll()` manage lifecycle for created atoms/links. You must call these after all atoms are added and links/sides are connected.
- `ChainContext`
  - Build multiple loops and connect side‑links across them using existing `IfaceConnTuple.conn` ids.
  - `ResolveAction(action, out_atom, out_link)` maps action strings to registered atom/link classes.
  - Aggregate lifecycle: `PostInitializeAll()/StartAll()/UndoAll()`.
- `ResolveLoopPath(Engine&, parts|dotted)`
  - Resolve a loop VFS node without Script/Ast/Id types.

Design Notes
- Contexts never use `AstNode` or Script code paths. They mutate VFS directly.
- Side‑linking must follow `IfaceConnTuple` channel semantics: optional channels may remain unconnected; required channels must be satisfied.
- Primary link creation uses the exchange point registry (`IfaceLinkDataMap`) to instantiate the correct exchange component.

Debugging
- Each context class implements `GetTreeString(int indent)` for quick dumps of what was built.
- Use standard logging macros to trace created atoms/links and loop paths (see `Context.cpp`).

Extending Core
- New atom/link: implement and register via `VfsValueExtFactory`; add `GetAction()` to appear in action resolution.
- New context helpers: keep them AST‑free and avoid adding cross‑package dependencies.
- If you add headers, aggregate only in `Core.h` and keep other headers minimal.

Current Tasks (living list)
- Complete migration of Script loop/chain construction to use `ChainContext`/`LoopContext` (adapters are in place).
- Add small utilities for VFS path resolution (entity/pool) mirroring `ResolveLoopPath` if needed.
- Evaluate rollback improvements in contexts (fine‑grained undo for partial failures).

Lifecycle Order (important)
- Build all atoms: `LoopContext::AddAtom(...)` for each atom in the loop.
- Create primary links if desired: `LoopContext::MakePrimaryLinks()`.
- For side links across loops, use `LoopContext::ConnectSides(loopA, loopB)` after both loops are built.
- Finalize: `ChainContext::PostInitializeAll()` or `LoopContext::PostInitializeAll()`.
- Start: `ChainContext::StartAll()` or `LoopContext::StartAll()`.
- On failure during post‑init or start, call `UndoAll()` on the affected `ChainContext`/`LoopContext` to stop and uninitialize in reverse order.

Manual Usage Example
- Resolve target loop path: `VfsValue* l = ResolveLoopPath(eng, "tester.generator");`
- Prepare `ChainContext::AtomSpec` list with realized interfaces and optional args.
- Add loop: `LoopContext& loop = chain.AddLoop(*l, specs, make_primary_links=true);`
- If you have several loops, connect sides via `ConnectSides` for each pair needing side‑links.
- Call `chain.PostInitializeAll();` then `chain.StartAll();`.

Script parity
- The Script path already performs: build → connect sides → post‑initialize → start. When building via Core contexts directly, replicate this sequencing explicitly as above.
