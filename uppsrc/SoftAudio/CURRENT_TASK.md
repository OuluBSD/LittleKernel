# SoftAudio: Current Task — Graph-Based Engine (Non-GUI)

This file reflects what we are actively doing for `SoftAudio` now. See `uppsrc/SoftAudio/AGENTS.md` for background and conventions.

## Phase 1 — Interfaces and Minimal Engine (In Progress)

Status: scaffolding complete; `MixerNode` and `RouterNode` added; reference app updated to mix two sources; `BypassNode`, `SplitterNode`, `CompressorNode`, `VoicerNode`, and `MidiInputNode` added; continuing wrapper iterations.

Done
- Created subpackage: `uppsrc/SoftAudio/Graph` with main header `Graph.h`.
- Implemented core pieces: `Node`, `PortSpec`, `ProcessContext`, `Edge`, and `Graph` with topological sort and per‑block processing.
- Added generic `GainNode` (package: `SoftAudio/Graph`).
- Added SoftAudio wrappers (package: `SoftAudio`):
  - `SineNode` (wraps `SineWave`) — mono source.
  - `FreeVerbNode` (wraps `FreeVerb`) — stereo effect.
  - `FileOutNode` (wraps `FileWaveOut`) — sink for offline renders.
  - `CompressorNode` (wraps `Compressor`) — stereo dynamics.
  - `VoicerNode` — polyphonic instrument source with default `Simple` instrument.
  - `MidiInputNode` — schedules NoteOn/NoteOff to a target `VoicerNode` using a simple queued dispatcher.
- Added reference app `reference/SoftAudioGraph` with an example chain.
  - Now demonstrates 2-source mix: `SineNode + SineNode → MixerNode (stereo, panned) → GainNode → FreeVerbNode → FileOutNode`.
  - Previous single-source example retained in spirit via parameters.

Example Chains (reference app)
- Mix + FX: `Sine(440) + Sine(660) → MixerNode(stereo,panned) → GainNode → FreeVerbNode → FileOut`.
- Compression: `Sine(220) + Sine(330) → MixerNode → CompressorNode → BypassNode(wet) → FileOut`.
- Voicer + MIDI: `MidiInputNode → VoicerNode → RouterNode(stereo) → FreeVerbNode → FileOut`.

Next
- Add minimal parameter API surface to remaining wrappers as needed and unify IDs.
- Validate clip handling and DC checks in reference chains.
- Optional: PortAudio live sink node and transport helpers.

Notes & Constraints
- Graph currently supports DAG only; cycles/feedback are rejected at compile time.
- Buffering: each node owns an output `Bus` sized at `block_size`; wrappers preallocate `AudioFrames` for bridging.
- RT safety: graph construction/compile can allocate; per‑block processing aims to be allocation‑free.

Updates
- Added `BypassNode` (two-input dry/wet selector) and `SplitterNode` (explicit pass-through fan‑out point) to the graph subpackage.
- Added `CompressorNode` wrapper and a second reference chain using it, with `BypassNode` selecting wet output.
- Added `VoicerNode` and `MidiInputNode`; third reference chain uses `MidiInputNode` to schedule events into `VoicerNode` during offline render.
- Added `Graph::SetParam(node_index, id, value)` convenience API for runtime parameter control.
  - Also `Graph::SetParam(name, id, value)` and `Graph::SetParams(name, {id:value,...})` for convenience.
  - Added node naming helpers: `AddNodeWithName(name, node)` and `FindNode(name)` for easier reference.
  - Edge labels: `ConnectWithName(name, ...)`, `SetEdgeName(idx,name)`, `FindEdge(name)`, and `GetEdge(idx)` to identify/edit specific connections.
 - Live PortAudio sink `LiveOutNode` with device selection (`SetOutputDeviceIndex`) and latency control; reference app includes a short live-play demo.
 - Added `GraphPlayer` helper (block-accurate clock + transport): attach/prepare, play N blocks, play seconds/frames, and simple seek/reset.
- Fixed `Voicer::Tick(AudioFrames&, int)` channel stepping so graph playback no longer overruns the buffer.

## Phase 2 — Wrappers and Basic Nodes (Queued)

Targets
- `MixerNode`, `RouterNode`, and a lightweight `BypassNode`.
- MIDI path (basic): a `MidiInputNode` feeding `Instrument::HandleEvent()` via a small event queue (block accuracy).

## Phase 3+ — Control, Runtime, Persistence (Planned)

- Sample‑accurate automation and feedback support via explicit `FeedbackNode` with enforced delay.
- Live output via PortAudio sink node and transport helpers.
- Graph serialization to a `ValueMap`/JSON‑like format.

## Open Questions

- Keep subpackage strictly independent of `SoftAudio` (current approach) vs. allow direct dependency for tighter `AudioFrames` integration?
- Default channel conventions (mono sources, stereo bus) — confirm and codify.


