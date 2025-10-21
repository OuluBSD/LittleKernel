Purpose: Explore and manage platform-specific analysis: roles, scores, EPK text fields, photo prompt groups, and per-platform summaries.

Key Control
-----------
- `PlatformManagerCtrl`:
  - Left platform list; right side shows platform attributes, role score summaries, and EPK data across text fields, photo types, and prompts. Additional tabs display role tables.
  - Menu: Start/Stop (drives `PlatformProcess`), fetch text-prompt images (TODO), and Import JSON for analysis state (`LoadFromJsonFile_VisitorNodePrompt`).
  - Sorting helpers allow ranking by various score families.

Data Flow
---------
- `PlatformManager` holds per-platform analysis (`PlatformAnalysis` with roles, EPK fields, photo groups/prompts). UI computes weighted score aggregates for display.

Extending
---------
- Hook Start/Stop to a backend pipeline generating/updating platform analysis. Implement photo prompt preview by connecting `epk_photo_prompt_example` to image cache.

