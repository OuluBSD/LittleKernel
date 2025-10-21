Purpose: Tools to author and analyze biographies, images, and platform-facing materials. Includes perspective/story frameworks and platform-specific content.

Key Controls
------------
- `BiographyCtrl`:
  - Left category list with per-category counts; tabs include Main (year-by-year), Elements, Summary, Image, Image Summary, and Audience. Each tab has its own splitter layout and forms to edit content.
  - Many actions are TODO stubs (Translate, Keywords, GetElements) but the structure and data plumbing are in place.
- `BiographyPlatformCtrl`:
  - Tabs: Platforms (with subpages Header, Messaging, EPK Photo, Needs, Marketplace), Clusters (prompt clusters), Audience.
  - Subpages live in `BiographyPlatform.h/.cpp` and additional code under `uppsrc/AI/Ctrl/Internal` (e.g., Messaging and Clusters implementations).
- `BiographyPerspectiveCtrl` (via `ConceptualFrameworkNavigator`):
  - Navigator to explore conceptual frameworks, stories, and their improved versions; provides sort, lock, and element extraction hooks.

Data Flow
---------
- Works with Biography/Owner/Profile components; derives counts, fills forms, and writes user edits back to VFS.
- Platform pages surface `PlatformAnalysis` data (roles, scores, EPK text fields and photo prompts); clustered prompts aggregate and preview prompt groups.

Extending
---------
- Fill in the TODO pipelines by adding SolverBase processes (e.g., biography text generation, image analysis) and hook them to the tab menus.
- Add platform integrations to fetch examples, generate EPK prompts, and preview images.

