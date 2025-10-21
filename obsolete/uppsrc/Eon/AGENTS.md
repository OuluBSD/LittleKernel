Purpose: Real‑time ECS + dataflow engine with a small DSL.

- Eon combines two layers:
  - An ECS runtime (entities, components, systems) for world state and logic.
  - A dataflow graph of Atoms connected by Links, described via the Eon script DSL and instantiated at runtime.

- Typical uses:
  - Prototype audio/video/event pipelines with typed channels and side links.
  - Drive an ECS world by loading entities, components, and systems from a script.
  - Build interactive loops (drivers, pipes, pollers) that push/pull packets in real time.


Overview
--------

- Package entry: `Eon.h` aggregates headers; include it from apps that use Eon.
- Registration: `Eon.icpp` registers core systems/components and associates typed names/IDs.
- Script loader: `ScriptLoader` parses `.eon` scripts into an AST, validates them, and materializes Atoms/Links/loops and ECS objects.
- ECS loader: `ExtScriptEcsLoader` creates systems, pools, entities and components declared in scripts, and passes arguments to them.
- Dataflow: Atoms (processing nodes) are chained by Links (schedulers/couplers) into Loops. Optional “side” channels let loops cross‑connect.


Core Concepts
-------------

- Atom: A concrete processing node derived from `AtomBase`. In Eon this is usually the `Atom` helper class, which mixes in both `DefaultInterfaceSink` and `DefaultInterfaceSource`.
  - Implementations define a typed interface (channels) and an associated default Link type.
  - Key hooks you likely implement: `Initialize`, `PostInitialize`, `Start`, `Recv`, `Send`, `Finalize`, `IsReady`, `ForwardPacket`.

- Link: The coupler and scheduler between two neighboring Atoms in a Loop. Common link types here:
  - `CustomerLink`: demand‑driven, couples request/response.
  - `PipeLink`: simple forwarder for single in/out.
  - `PipeOptSideLink`: primary in/out plus optional side channels; can finalize on side.
  - `IntervalPipeLink`: async interval consumer/forwarder (e.g., timed pull of audio).
  - `PollerLink`: frame‑rate poller with periodic `Update`.
  - `ExternalPipeLink`: handoff to an external device/driver.
  - `MergerLink`, `JoinerLink`, `SplitterLink`: multiplex/merge/split flows across channels.

- Interface / Channels:
  - An Atom exposes sink and source channels using `IfaceConnTuple`/`AtomTypeCls`.
  - Channel 0 is the primary path; channels 1..n are side channels (optional/required per type).
  - Types are expressed as value/device pairs (`ValDevTuple` via `VD(DEV,VAL)`), e.g. `VD(CENTER,AUDIO)` or `VD(CENTER,VIDEO)`.

- Loop: A closed chain of Atoms with Links, created from a `loop { ... }` in the script.
  - Every Atom in a loop has a Link (its default `GetLinkType()` unless overridden by script/tooling).
  - Loops compute consistent queue limits across all channels (`ScriptLoopLoader::UpdateLoopLimits`).

- Side links: Cross‑loop connections between side channels.
  - The loader aligns side channels by “connector IDs” defined in interfaces; matching sink/src IDs get linked (`ScriptLoader::ConnectSides`).
  - Per‑loop side connections can be optional or required based on the Atom’s interface.

- ECS world: Entities, Components, Systems loaded via `.eon` under the `ecs { ... }` section.
  - `ExtScriptEcsLoader` drives creation, argument binding, and system startup order.


File Guide (selected)
---------------------

- `Eon.h`: Umbrella include; pulls in container, atom/link helpers, ECS loaders, systems.
- `Eon.icpp`: Registers systems/components; wires `__ecs_script_loader`; typed string hashers for link names.
- `Def.h`: Script model types (IDs, definitions for machine/world/pool/entity/component/loop/state).
- `Loader.h/.cpp`: Script loader implementation and passes that load/validate AST → runtime objects.
- `LoopLoader.cpp`: Loop materialization, atom/link creation, linking logic, queue sizing, start/stop.
- `Ecs.h`: ECS loader passes (systems, pools, entities, components) and extension base.
- `EonLoader.h/.cpp`: Glue to apply loaded ECS definitions to an `Engine` (construct, Arg(..), startup).
- `Atom.h`: Convenience base that composes default sink/source interfaces and wires to `AtomBase`.
- `LinkUtil.h`: Common link helpers like `AsyncMemForwarderBase` and `FramePollerBase`.
- `LinkBase.h/.cpp`: Concrete link classes and their scheduling/processing behavior.
- `CommonComponents.h/.cpp`: Shared ECS components (e.g., `Transform`, 2D/3D transforms).
- `InteractionSystem.h`: Input/controller bridge with fake/VR backends; feeds player components.
- `WorldLogic.h/.cpp`: Simple system that updates transforms or prunes invalid entities.
- `EcsPhysicsSystem.h`: Basic physics body + system, integrates with `Transform`/player.
- `main.cpp`: Optional test harness (guarded by `flagMAIN`) to run `.eon` language tests.


Script DSL (glossary)
---------------------

- Top‑level:
  - `machine { ... }`: dataflow section; defines chains, loops, drivers, and atoms in the loop blocks.
  - `ecs { ... }`: ECS section; defines worlds, systems, pools, entities, and components with arguments.

- IDs: Deep IDs form a dotted path assembled from nested blocks (e.g., `machine.a.b.loop`).

- Loops and atoms:
  - `loop <id> { atom_id(args...); atom_id(args...); ... }`
  - `driver <id> { atom_id(args...) }` → a single atom driver that acts as an entry point.
  - Atoms are matched by their action string (e.g., `center.audio.loader`). Interfaces and link types come from the registered Atom type (`GetAtomType()` and `GetLinkType()`).
  - Optional “connector” expressions inside an atom block can require side link properties (e.g., key=value) to help resolver match side channels across loops.

- ECS:
  - `world <id> { system("id") with (k=v,...); pool <id> { entity <id> { component <id>(k=v,...) } } }`
  - Arguments map to `System::Arg(key,value)` and `Component::Arg(key,value)` calls during load.

Note: See `obsolete/share/eon/lang` and `obsolete/share/eon/tests` for many real script examples used by the harness (`main.cpp`).


Script Examples
---------------

- Minimal audio pipeline (from tests):

```
loop player.audio.generator:
    center.customer
    center.audio.src.dbg_generator
    center.audio.sink.hw
```

- SDL + OGL program with standalone FBO stage:

```
machine sdl.app:
    driver context:
        sdl.context

    chain program:
        loop ogl.fbo:
            ogl.customer
            sdl.fbo.standalone:
                filepath = "shaders/toys/simple/simple_single/stage0.glsl"
```

- ECS world with player and interaction (from `07d_ecs_first_person_cam.eon`):

```
machine sdl.app:
    driver context:
        sdl.context

    chain program:
        state event.register

        loop center.events:
            center.customer
            sdl.event.pipe
            state.event.pipe:
                target = event.register

        loop ogl.fbo:
            ogl.customer
            sdl.ogl.fbo.program:
                program = "ecs_view"
            sdl.fbo.sink:
                env = event.register

world ecs.dummy:
    system rendering
    system events
    system interaction:
        env = event.register
    system physics
    system player

    pool world:
        entity player.body:
            comp transform3
            comp physics:
                bind = true
            comp player.body:
                height = 1.74

        entity player.head:
            comp transform3
            comp viewable
            comp viewport:
                fov = 90
            comp camera.chase
            comp player.head:
                body = world.player.body
```

Notes specific to the examples/tests:
- `state event.register` declares a state endpoint; `state.event.pipe` routes events to that state. Systems/atoms can take `env = event.register` to bind to the same environment.
- Drivers must contain exactly one atom. Loops must contain two or more atoms (they are circular by default).
- Dotted IDs like `world.player.body` reference other ECS objects by deep path; the loader resolves them.


Runtime: How Loading Works
--------------------------

1) `ScriptLoader` compiles the Eon script string/file into an AST using the `Compiler`.
2) It builds `Eon::CompilationUnit` and `GlobalScope` (`LoadCompilationUnit`, `LoadGlobalScope`).
3) For the machine section it builds `ScriptSystemLoader → ScriptMachineLoader → ScriptTopChainLoader → ScriptChainLoader → ScriptLoopLoader`.
   - Each `ScriptLoopLoader` translates atom statements into concrete atom/link objects via `VfsValueExtFactory::AtomDataMap()` / `::LinkDataMap()` and `AddAtomTypeCls` / `AddLinkTypeCls`.
   - All loop atoms initialize with a `WorldState` populated from script args; links initialize after their atoms.
   - Loops are linked (primary channels), side links are matched across loops, post‑initialized, then started.
4) For the ECS section it builds `ScriptWorldLoader` and children; `ExtScriptEcsLoader` applies them to the live `Engine`:
   - Systems are looked up by ID then `Arg(..)` is called and `SystemStartup` is performed.
   - Pools and entities are created in the engine’s VFS tree; components are created via `Entity::CreateEon(name)` and configured via `Arg(..)`.


Extending Eon
-------------

- New Component
  - Derive from `Component` and define your data/behavior; override at least `Initialize`, `Uninitialize`, `Visit`, `Arg` as needed.
  - Register with: `REGISTER_COMPONENT(YourComponent, "your.eon.id", "Category|Sub")` (see `Eon.icpp`).

- New System
  - Derive from `System`; implement `Initialize`, `Update`, `Uninitialize`, optional `Arg` for configuration.
  - Register with: `REGISTER_SYSTEM_ECS(YourSystem, "your.system.id", "Category|Sub")`.
  - To consume input events, implement an `InteractionListener` and register with `InteractionSystem`.

- New Atom
  - Derive from `Atom` (preferred) or a specialized base. Implement:
    - static `String GetAction()` → unique action id used in the script.
    - static `AtomTypeCls GetAtomType()` → define sink/source channels with `AddIn`/`AddOut` and the role.
    - static `LinkTypeCls GetLinkType()` → default link to couple neighbors.
    - Runtime hooks (`Initialize`, `Recv`, `Send`, `Finalize`, etc.).
  - Register with the VFS factory so the loader can instantiate it:
    - `VfsValueExtFactory::RegisterAtom<YourAtom>("YourAtom")` (see `Eon/Lib/EonLib.icpp`).

- New Link
  - Derive from `LinkBase` or one of the helpers (`AsyncMemForwarderBase`, `FramePollerBase`).
  - Implement `ProcessPackets` and any scheduling semantics; optionally override `IsReady`, `Forward`, etc.
  - Provide static `LinkTypeCls GetLinkTypeStatic()` and `LinkTypeCls GetLinkType() const`.
  - Register with: `VfsValueExtFactory::RegisterLink<YourLink>("YourLink")` so `ScriptLoopLoader` can create it.


Key Types and Utilities
-----------------------

- `AtomTypeCls` / `IfaceConnTuple`: Describe the interface of an Atom (sinks/sources, optional vs required, side connectors, roles).
- `ValDevTuple` / `ValDevCls`: Value/device pairs, e.g., `VD(CENTER,AUDIO)`.
- `Packet`, `PacketValue`, `PacketBuffer`, `ValueFormat`: Describe flowing data, formats, queue sizing.
- `RealtimeSourceConfig`: Per‑link/runtime config passed to `Send`/`Finalize`.
- `WorldState`: Gathered script args and environment for initialization.
- `Vis`: Visitor used for debug/inspection; most classes implement `Visit` to expose state.


Flags and Dependencies
----------------------

- Uses: `AICore2`, `Geometry`, `Esc`, `Core2`, `Vfs`, `plugin/enet` (see `Eon.upp`).
- Optional flags:
  - `flagGUI`: enables UI‑bound types like `FrameT`, handle systems.
  - `flagSCREEN`, `flagOGL`, `flagSDL2`, `flagX11`, `flagDX11`, `flagFFMPEG`, `flagOPENCV`, etc. control availability of atoms/links bound to those stacks (see `Eon/Lib`).


Gotchas / Notes
----------------

- Channels are 0‑based, but side channels begin at index 1; code often asserts ranges accordingly.
- Queue limits are harmonized across loop members; if min > max, max is raised to match.
- Some loader parts are marked `TODO`—script features might be partially implemented; prefer examples from `obsolete/share/eon` for reference.
- `main.cpp` test runner is guard‑controlled; enable `flagMAIN` to build/run the language tests.


Quick Start
-----------

1) Ensure an `Engine` contains `ScriptLoader` (use the helper function bound in `ScriptLoader.cpp`).
2) Call `Eon::PostLoadString(engine, your_script_string)` or post a file via `WhenEnterScriptLoad`/`PostLoadFile`.
3) The loader will build loops, connect sides, then `LoadEcs` to create systems/components.
4) Start your engine; systems like `WorldLogicSystem`, `PlayerBodySystem`, `InteractionSystem`, and any declared in script will run.


Troubleshooting
---------------

- If atoms fail to instantiate: confirm the action id matches a registered atom (`VfsValueExtFactory::AtomDataMap()` lists them; see logs).
- Side‑linking errors: verify both source and sink atoms expose matching side connector IDs and that script connectors are consistent.
- ECS load errors: check `System::Arg`/`Component::Arg` returns; logs include `GetErrorString()` from loaders.

