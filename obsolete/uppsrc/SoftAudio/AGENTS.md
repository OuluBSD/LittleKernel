# SoftAudio – AGENTS Guide

This guide covers how to work in the `SoftAudio` package: structure, coding conventions, the U++ header include policy (BLITZ), and the ongoing non‑GUI, graph‑based audio engine task.

Scope: This file applies to `uppsrc/SoftAudio` and all subdirectories unless a nested AGENTS.md overrides it.

## Package Overview

- Main header: `SoftAudio.h` aggregates this package’s headers. Do not add cross‑package includes inside non‑main headers.
- Dependencies: `SoftAudio` uses core U++ facilities and depends on `Geometry` and `MidiFile` via `.upp`. `SoftAudio.h` includes `Core2/Core.h`, `Geometry/Geometry.h`, and `MidiFile/MidiFile.h`.
- Core types:
  - `Audio`: base class with sample rate ownership and utilities.
  - `AudioFrames`: interleaved floating‑point buffer helper.
  - `Instrument`, `Effect`, `Function`, `Generator`, `WaveOut`: processing primitives.

## Header Include Policy (U++ BLITZ)

Follow these rules strictly to keep BLITZ effective and builds stable:

- Every `.cpp`/`.icpp`/`.c` file must include only the package’s main header first: `#include "SoftAudio.h"`.
- After the main header, add other includes only when the implementation truly needs a forward include from a later package in the dependency queue. Keep them local to that implementation file.
- Provide exactly one main header for the package (`SoftAudio.h`) and keep it minimal beyond aggregation.
- Prefer that headers other than the main header do not declare a namespace; `SoftAudio.h` wraps aggregated headers with `NAMESPACE_UPP` via the `NAMESPACE_AUDIO_BEGIN/END` macros.
- Do not add system/third‑party includes into non‑main headers to avoid them being placed under `NAMESPACE_UPP` and to keep BLITZ lean.
- `.icpp` files are compiled, not included companions. Start them with `#include "SoftAudio.h"` as well.

## Coding Conventions

- Follow repository `CODESTYLE.md` for naming, layout, and error handling.
- Real‑time audio thread safety:
  - Avoid dynamic allocations, locks, and blocking calls in processing code.
  - Prefer pre‑allocation and reuse of buffers; use stack or pre‑allocated arenas when possible.
- Sample rate policy:
  - Read via `Audio::GetSampleRate()`; hook changes by overriding `SampleRateChanged` where applicable.
  - Only adjust node internals in response to `Prepare` or explicit notifications; do not rely on global state changing mid‑block.
- Parameter validation:
  - Use `Audio::HandleError` and `AudioError` categories for reporting.
  - Clamp or warn consistent with existing patterns (see `Effect::SetEffectMix`).

## Extending SoftAudio

### New Instrument

- Derive from `Instrument`.
- Implement `NoteOn(frequency, amplitude)`, `NoteOff(amplitude)`, `Tick(...)` (sample or block versions), and optionally `ControlChange`.
- Keep allocations out of the audio path; preallocate working buffers in the constructor or via a `Prepare`‑like method.

### New Effect

- Derive from `Effect`.
- Implement `Clear()`, `Tick(input, channel)` or `Tick2(...)` for stereo, and optionally `SetEffectMix` and `LoadState`.
- Maintain `last_frame_` semantics for consistency with `GetLastFrame()`.

### File/Device I/O

- For device output, prefer `PortaudioSystem` / `PortaudioStream` integration through `WaveOut` wrappers.
- For offline rendering, use `FileWaveOut` with controlled buffer sizes and clip checks.

## Graph-Based Audio Engine (Non-GUI)

We are introducing a non‑GUI, graph‑based system to connect Instruments and Effects. See `CURRENT_TASK.md` for the full plan and roadmap. Key points:

- Compose processing as a DAG; sinks pull from sources in block order.
- Standardize on block processing using `AudioFrames`.
- Provide wrappers: `InstrumentNode`, `EffectNode`, mixers/routers, MIDI input, and sinks.
- Real‑time safe execution with preallocated scratch buffers per node.
- Integration path: a new subpackage `SoftAudio/Graph` with `Graph.h` as its main header.

### Subpackage Independence

- Subpackages such as `SoftAudio/Graph` must be independent packages with their own main header.
- A parent package may include only the subpackage’s main header (e.g., `#include "Graph.h"` inside the parent’s main header), but subpackages must not include parent internals directly.
- Chain dependencies correctly to avoid circular references.

### Interfaces (early sketch)

- `IAudioNode { Prepare(rate, block); Process(dst, frames); GetInputPorts(); GetOutputPorts(); }`
- `Graph { AddNode, Connect, Compile, Process, SetSink }`
- `EventQueue { Push(param, value, frame_offset), PopRange(...) }`

## Testing & Validation

- Add small integration examples:
  - `SineWave → Gain → FreeVerb → Output (PortAudio)`
  - `MIDI → Voicer(Instrument) → Mixer → Compressor → FileWaveOut`
- Verify: no allocations on audio thread, no clipping beyond ±1.0, and deterministic output for offline renders.
- Prefer unit tests local to the graph subpackage; avoid introducing global test harness changes.

Reference App
- See `reference/SoftAudioGraph` for a minimal offline example chain using the graph engine.

## Contributor Checklist

- Start every implementation file with `#include "SoftAudio.h"`.
- Avoid adding includes to non‑main headers.
- Keep processing code lock‑free and allocation‑free on the audio path.
- When adding files, update `SoftAudio.upp`. Ensure `AGENTS.md` is the first entry in the `file` section.
- For graph engine work, keep interfaces minimal and stable; iterate behind wrappers.

## Current Task

See `CURRENT_TASK.md` for the up‑to‑date plan, milestones, and open questions for the graph‑based audio engine.
