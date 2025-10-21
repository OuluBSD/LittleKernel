Purpose: Authoring and analysis for text-heavy domains: lyrics, scripts, and transcripts. Provides structured editors, database-backed element navigation, and scripted processing.

Major Controls
--------------
- `ScriptTextSolverCtrl` + `StructuredScriptEditor` (Lyrics editor):
  - Split editor: visual structure (parts/subs/lines) and multi-tab tool area (Wizard, Suggestions, Whole song, Part, Sub, Line).
  - Shortcuts: Update (Ctrl+Q), Switch source text (Ctrl+W), Start/Stop (F5/F6), function slots (F7..F11), copy helpers.
  - Selection helpers navigate between parts/subs/lines and update linked forms; entities/colors/typeclasses/contents are previewed inline.
- `ScriptReferenceMakerCtrl` with `PartContentCtrl`/`PartLineCtrl`:
  - Element-focused view with a DB browser (`ScriptPhrasePartsGroups`) to explore attributes/colors/actions/elements/typeclasses/contents; supports modes and sorting.
  - Writes back element selections to the underlying `LyricalStructure` in the dataset.
- `ScriptTextCtrl` (Virtual IO script tree):
  - VFS-backed editor for `VirtualIOScript` and its children (`VirtualIOScriptSub`, `VirtualIOScriptLine`, `VirtualIOScriptProofread`).
  - Provides a paged analysis/debugger stack (Source data, Tokenization, Elements, Virtual phrases/parts/structs, diagnostics, wordnets, etc.).
  - Can auto-run a `ScriptTextProcess` if `process_automatically` is enabled.
- Transcripts and conversions:
  - `TranscriptProofreadCtrl`: proofreads audio transcripts; selects ranges to play in mpv, persists corrected segments.
  - `StorylineConversionCtrl`, `StorylineScriptCtrl`, `ScriptConversionCtrl`: placeholders for conversion workflows.

Data Flow
---------
- Controls use `GetDataset` to access `LyricalStructure`, `Lyrics`, `Script`, `SourceText`, and other components; helper processes (e.g., `ScriptSolver`, `ScriptTextProcess`) are accessed by dataset and ECS path.
- `ScriptTextCtrl` creates a rooted `VirtualNode` (`VirtualIOScript`) and materializes subnodes for proofread imports.

Extending
---------
- Add new analysis pages by extending `SubTab::AddLineOwnerTabs` and implementing `Data` for the page.
- For new editor features, add menu entries to `ToolMenu` and handle commands in `Do/Do*` methods.
- To integrate new database grouping modes, extend `DatabaseBrowser` and update `ScriptPhrasePartsGroups::UpdateNavigator`.

Requirements
------------
- Several controls expect downstream core classes (DynLine/Part/Sub, `ScriptSolver`, `DatabaseBrowser`, `TranscriptResponse`, etc.) from AI/Core/Meta modules.
- Audio playback via `mpv` should be on PATH for transcript proofread playback.

