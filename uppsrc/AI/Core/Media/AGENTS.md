Media — Images, Audio, Layers

Overview

- Media-oriented components and solvers:
  - Image layers and generation buffers
  - Aspect-ratio fixing with LLM‑assisted prompt design
  - Audio transcript and script speech value components

Key Modules

- Layer.*
  - `ImageLayer` stores a single `Image` with visit/serialize and string pack/unpack helpers.
  - `TempImageLayer` and `ImageGenLayer` hold temporary/generated image variants.

- AspectFixer.*
  - `AspectFixer` is a `SolverBase` process to expand/crop images to target aspect with inpainting/fillers.
  - Phases: analyze prompt → safe prompt → fillers. Events report intermediate images via `WhenIntermediate`.
  - Utility: `AspectFixer::MakeMask(const Image&)` derives a mask suitable for inpainting.
  - `AspectFixerLayer` stores solver outputs in ECS.

- AudioTranscript.h
  - Value components: `AudioTranscript`, `ScriptSpeech` for transcription pipelines.

- CoverImage.h
  - `ReleaseCoverImage`: ECS component stub for packaging a cover asset for releases.

Workflows

- Fix aspect ratio and generate fills
  - Obtain source `Image` and target `w/h` (+ extra extents).
  - `AspectFixer::Get(src, w, h, w_extra, h_extra)` → `Start()` and subscribe to `WhenIntermediate`.
  - Store variants in `ImageGenLayer` or `AspectFixerLayer` as needed.

Public Surface

- Include umbrella: `#include <AI/Core/Media/Media.h>`
