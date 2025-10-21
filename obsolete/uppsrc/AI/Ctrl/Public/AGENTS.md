Purpose: Public-facing metadata editors for profiles, releases, artists, and perspectives, plus a small solver to derive perspective attributes.

Key Components
--------------
- `ProfileInfoCtrl`: edits profile metadata (name, created, description, preferences) and a language set via a table of `Option` controls.
- `ReleaseInfoCtrl`: edits release metadata (title, album date, year of content).
- `ArtistInfoCtrl`: edits artist fields (names, years, biography, text style/vibe, tools, speaker visuals, visual gender, language).
- `PerspectiveCtrl` + `PerspectiveProcess`:
  - Edits perspective description/reference and user-provided inputs; shows derived positive/negative attributes.
  - `PerspectiveProcess` fetches positives, then negatives, via `AiTaskManager` using `BeliefArgs`; results populate `PerspectiveComponent::Attr` list.

Extending
---------
- Add new profile/release/artist fields to forms and map to component data. Extend `PerspectiveProcess` phases or add new derived views.

