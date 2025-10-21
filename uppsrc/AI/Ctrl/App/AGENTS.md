Purpose: Composite application UIs for experimentation and operator workflows (Playground, Omni device/audio, Tasks, Notepad).

Key Components
--------------
- `PlaygroundCtrl` + `PlaygroundApp`:
  - Tabs: Completion, Chat, Stage (`VfsProgramCtrl`), Chain (`AiChainCtrl`), TTS, Assistants, Realtime AI, Biases, Image editors, Tasks.
  - Persists GUI state to `ConfigFile(..)` JSON; binds a node under `MetaEnv().root` and materializes thread components.
  - `TabMenu` delegates to the active tab’s `MainMenu/ToolMenu`.
- `OmniCtrl` (audio, `flagAUDIO`): device I/O page with volume meter; optional detailed view (discussions/messages/phrases wiring left disabled in code).
- `TaskCtrl`: window into `AiTaskManager` queue; shows status, allows processing/retry/return-fail and inline output editing.
- `NotepadCtrl`: simple note manager under a `Notepad` component; CRUD via list context menu, values auto-saved to VFS.

Data Flow & Persistence
-----------------------
- Playground stores/restores both UI state and bound VFS node state (`playground-gui.json`, `playground-node.json`, `playground-root.json`).
- Threads and components are created on demand in the bound VFS node (CompletionThread, ChatThread, VfsFarStage, ChainThread, Agent).

Extending
---------
- Add new tabs to `PlaygroundCtrl` and propagate menu routing in `TabMenu` / `Data`.
- Enable/extend `OmniDetailedCtrl` by wiring a `SoundThreadBase` and handlers when using the audio daemon.
- For new dashboards, follow the `TaskCtrl` pattern: poll or subscribe to backend state, provide menu actions for operations.

Requirements
------------
- `flagGUI` is required; audio tabs require `flagAUDIO` and SoundCtrl runtime support.

