AGENTS

Scope
- Applies to `uppsrc/Sound`.

Purpose
- Cross-platform audio I/O built primarily on PortAudio. Provides sound device discovery, streaming, and a simple daemon/threading model.

Key Areas
- Types and API: `Types.h`, `Sound.h`, `SoundSystem.h`.
- Engine: `Sound.cpp`, `SoundStream.cpp`, `SoundSystem.cpp`.
- Background processing: `SoundDaemon.{h,cpp}`, `SoundThread.{h,cpp,inl}`.
- Media helpers: `SoundClip.h` and simple clip/discussion helpers.

Dependencies
- Uses `plugin/portaudio` by default or `SYS_PORTAUDIO` system libraries per `.upp` conditions.

Extension Points
- Implement new stream processors or device strategies by extending `SoundStream` and wiring into `SoundSystem`.
- Keep real-time code minimal and lock-free where possible.

.upp Notes
- List `AGENTS.md` first in `Sound.upp` `file` section.

