Purpose: Lead discovery and analysis pipeline UI. Surfaces opportunity datasets, runs multi-phase analysis, derives templates, and maintains publisher info.

Key Components
--------------
- `LeadSourceCtrl`:
  - Histograms for payouts and submission prices; main list ranks opportunities with multiple scoring columns (money/opp scores, weighted rank, chance %, average payout estimation).
  - Attribute explorers: raw attributes, analyzed booleans/strings/lists with nested list key→values.
  - Derived insights: potential song typecasts, lyric ideas, and music style text; context menu provides copy helpers for quick reuse.
- `LeadSolver` (SolverBase):
  - Phases include: downloading/parsing websites, analyzing booleans/strings/lists, coarse ranking, average payout estimation, and template derivation/analysis. Progress is batched; low-score opportunities can be skipped.
  - Interacts with `AiTaskManager` for analysis prompts, parses responses into the dataset, and updates UI via callbacks.
- `LeadTemplateCtrl`: displays extracted templates (title, text, price, author class/speciality, top profit/organization reasons) and sorts by derived score.
- `LeadPublisherCtrl`: maintains publisher info (name, description, genres, url) and a list of artists; supports JSON bulk import.

Data Flow
---------
- Uses `LeadData`, `LeadDataTemplate`, and `LeadDataPublisher` components under an entity/owner; JSON imports supported to seed or update datasets.

Extending
---------
- Provide HTTP/download logic outside the Ctrl layer; wire results into `LeadSolver` phases. Add new analysis phases by extending `DoPhase()` and mapping to UI.

