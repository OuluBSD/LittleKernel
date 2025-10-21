Purpose: Person-related component editors. Currently provides an Owner info panel; male/female controls are placeholders for future specialization.

Key Components
--------------
- `OwnerInfoCtrl`: edits owner profile (name, date of birth, description, environment). Fields write-through using `WhenAction` handlers.
- `Male` / `Female`: stubs reserved for future per-gender UIs.

Extending
---------
- Add demographic or preference fields to `OwnerInfoCtrl` and persist into the `Owner` component. Implement specialized views for `Male`/`Female` if needed.

