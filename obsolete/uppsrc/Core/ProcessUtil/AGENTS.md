AGENTS

Scope
- Applies to `uppsrc/Core/ProcessUtil`.

Purpose
- Provides process-management helpers, filesystem/stream utilities, and miscellaneous runtime helpers split from `Core2`.
- Serves as the bridge between low-level OS concerns and higher-level ECS/Vfs code.

Responsibilities
- Target files: `Process.*`, `Util*.{h,cpp}`, `Chrono.h`, directory scanners, and general-purpose helpers.
- Document platform-specific behavior (POSIX vs. Windows) and threading expectations.

Guidelines
- Keep the package UI-free and avoid dependencies on ECS-specific headers.
- Implementation files must include `ProcessUtil.h` first per BLITZ policy; add rare system headers locally within `.cpp`.
- Provide concise comments near tricky platform code (e.g., spawn flags, pipe setups).

Migration Notes
- Record remaining TODOs in `CURRENT_TASK.md` while porting code from `Core2`.
- Audit dependencies after each migration to ensure packages only include what they consume.
