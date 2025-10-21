Purpose: Audio-centric controls. Extracts audio from video ranges, performs transcription via AI providers, and hosts speech-related stubs.

Key Controls
------------
- `AudioTranscriptCtrl`:
  - Binds to a `VideoSourceFileRange` in the same VFS owner (via `RangeFinder` template) and extracts the selected audio segment with ffmpeg into an MP3 cache.
  - Sends `TranscriptionArgs` to `AiTaskManager::GetTranscription` with chosen AI provider and language; persists JSON response (`TranscriptResponse`) into component.
  - Displays segments in an editable grid; writes user edits back to JSON (`SaveTextChanges`).
- `ScriptSpeechCtrl`: placeholder for text-to-speech/script workflows.

Requirements
------------
- `ffmpeg` must be available on PATH for audio extraction; `mpv` is used for playback of segments in transcript proofread flows.

Extending
---------
- Add new providers to `SetAiProviders` using `AiManager()` capabilities.
- Extend the control to support diarization or timeline edits by augmenting the segment table and JSON schema.

