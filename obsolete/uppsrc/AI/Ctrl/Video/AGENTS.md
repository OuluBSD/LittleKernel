Purpose: Video source management, timeline/range preview, storyboard visualization, and prompt authoring for image/video generation workflows.

Key Controls
------------
- `VideoSourceFileCtrl`:
  - Selects a video file and runs `ffprobe` to parse container/stream metadata (JSON) and displays duration, codecs, frame rate, etc. Writes parsed fields to component `val`.
- `VideoSourceFileRangeCtrl` + `RangeCtrl`:
  - Lists available `VideoSourceFile` siblings; shows single-file duration and range (begin/end) with an interactive slider.
  - Extracts a preview frame using ffmpeg `select=eq(n,frame)` and displays it; can open `mpv` to play full file or the selected range.
- `VideoStoryboardCtrl`:
  - Lists text-to-storyboard parts and displays up to 4 associated images per line, loading thumbnails/full images from package or cache.
- `VideoPromptMakerCtrl`:
  - Shows storyboard parts, per-part storyboard prompts, and text-to-storyboard mappings; includes TODO workflow for generation via `VideoSolver`.
- `VideoSolver`:
  - Multi-phase solver skeleton (storyboard generation, prompt derivation, safe prompts, image retrieval, etc.). Phases are scaffolded as TODOs.

Cross-Package Links
-------------------
- `AudioTranscriptCtrl` uses the `VideoSourceFileRange` component to determine the audio extraction window.

Requirements
------------
- `ffprobe`/`ffmpeg`/`mpv` must be available on PATH for metadata, frame grabbing, and playback.

