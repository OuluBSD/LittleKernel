Purpose: Image generation, editing (mask-based), aspect correction tooling, and cover image suggestion display for releases.

Key Components
--------------
- `ImageGenTool`:
  - Two tabs: Show (prompt input, generate/translate/upload, preview list with thumbnails) and Edit (inpainting UI with prompt+mask+mode/color/width; generate/variate/upload).
  - `GeneratedImages` draws a grid with context menu; `EditImage` supports painting/erasing into a mask with adjustable brush and color (SHIFT wheel to change color, wheel for size).
  - Maintains a “recents” list serialized in `ConfigFile("recent")` with prompt and preview mosaic.
- `ImageViewerCtrl`: centers and fits an image into the control, with simple context menu hooks.
- `ImageAspectFixerTool`:
  - Loads an image and runs an external `AspectFixer` process to extend/crop to target aspect (width/height + optional extra paddings). Supports queue processing for an entire directory.
  - Emits intermediate result/prompt and a final image (`WhenReady`).
- `ReleaseCoverImageCtrl`:
  - Lists snapshot analysis strings and cover suggestions; shows up to four saved images for a selected suggestion. Triggers `SnapSolver` to generate all cover images.

Extending
---------
- Wire `ImageGenTool` buttons to your generation/edit backend (TaskMgr or direct API).
- Implement `AspectFixer` to perform guided outpainting; results are consumed by `ImageAspectFixerTool`.
- Connect `ReleaseCoverImageCtrl` to your image cache paths and generation solver to preview outputs.

Requirements
------------
- JPEG/PNG plugins are used for I/O; ensure codecs are linked. Some pieces rely on external AI services or solvers not shown here.

