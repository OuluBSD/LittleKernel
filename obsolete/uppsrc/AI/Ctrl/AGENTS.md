Purpose: UI controls and tooling that orchestrate AI workflows across text, audio, video, imaging, biography, platforms, leads, and public-facing components. Acts as the glue between Meta/VFS entities, solver/agent backends, and end-user editors.

Overview
--------
- Umbrella header `Ctrl.h` aggregates subpackages: `Base`, `Agent`, `Text`, `Audio`, `Video`, `Imaging`, `Biography`, `Public`, `Platform`, `People`, `Leads`, `Internal`, and `App`.
- Controls follow U++ patterns (ArrayCtrl, TabCtrl, Splitter, DocEdit) and are wired to Meta/VFS via Component/Value VFS helpers from MetaCtrl.
- Execution backends:
  - Agent/TaskMgr based LLM calls (chat, completion, transcription, etc.).
  - SolverBase-derived processes for multi-phase operations (e.g., Leads, Perspective, Video, Platform).
- External tools: ffmpeg/ffprobe/mpv used by audio/video controls; image encoders/decoders for imaging.

Core Concepts
-------------
- `AiComponentCtrl`/`ValueVFSComponentCtrl`: base UI components bound to a VFS value or component; implement `Data`, `ToolMenu`, and state persistence via `EditPos(JsonIO&)`.
- Meta/VFS: most controls resolve their context using `GetDataset` and navigate `VfsValue` trees (owner, entity, release, threads, etc.).
- Registration macro `INITIALIZER_COMPONENT_CTRL(Type, Ctrl)`: exposes controls to the Meta environment so they appear in editors.

Subpackages (quick map)
-----------------------
- Base: common UI/helpers and the `VfsProgramCtrl` stage/iteration editor.
- Agent: chat/completion thread UIs, prompt editor, builder integration.
- App: Playground window, Omni device page (audio), task dashboard, notepad.
- Text: structured script/lyrics editors, analyzers, transcript proofread, storyline/script conversions.
- Audio: video-range based audio transcription; script speech stub.
- Video: source file metadata, ranged preview, storyboard and prompt tools.
- Imaging: image generation, inpainting/masking editor, aspect fixer, release cover images.
- Biography: biography editors (by category, summary, images), platform + perspectives.
- Platform: platform manager and EPK photo prompt explorer.
- Leads: opportunity mining UI + solver, templates and publisher info.
- Public: profile, release, artist, perspective components + PerspectiveProcess.
- People: owner info (male/female placeholders).
- Internal: subpages plugged into platform/biography (messaging, clusters).

Typical Data Flow
-----------------
- UI ctrl reads VFS/Entity state → prepares args → calls TaskMgr/Agent/Solver → receives results → writes back to VFS → refreshes view.
- Long-running tasks use callbacks (`WhenReady`, `WhenProgress`, `WhenError`) and UI indicators/progress where provided.

Extending
---------
- Add a new component control by subclassing `AiComponentCtrl` or `ValueVFSComponentCtrl`, implement `Data` and menus, then register with `INITIALIZE`/`INITIALIZER_COMPONENT_CTRL`.
- For multi-step pipelines, derive from `SolverBase`, define phases and `DoPhase()`, and expose a driving control with Start/Stop.
- For DSL/dataflow work, see `uppsrc/Eon/AGENTS.md` for ECS + loop/atom integration patterns.

Requirements & Notes
--------------------
- ffmpeg/ffprobe/mpv binaries should be available on PATH for audio/video tools.
- Some modules contain TODOs; controls are designed to tolerate missing features and refresh safely.

