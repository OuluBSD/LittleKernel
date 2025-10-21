Purpose: Interactive controls to talk to AI backends (LLM chat/completion) and edit prompts/build artifacts.

Key Classes
-----------
- `AiThreadCtrlBase`: base for model-driven threads. Manages available models (`UpdateModels` via `AiTaskManager::GetModels`), persists selected model (`Visit(Vis&)`), and exposes `MainMenu` actions (Update/Submit).
- `CompletionCtrl`: text completion UI; builds `CompletionArgs` from form values and calls `AiTaskManager::GetCompletion`, appending results to the editor.
- `AiCompletionComponentCtrl`: component wrapper that hosts `CompletionCtrl` for Meta usage.
- `ChatAiCtrl`: multi-session chat UI; manages chat history and sends `ChatArgs` to `AiTaskManager::GetChat`. Supports session CRUD and hotkeys via `MainMenu`.
- `AiChatComponentCtrl`: component wrapper hosting `ChatAiCtrl` for Meta.
- `AgentPromptEdit`: small form to edit prompt text/attachments; used in agent-related tooling.
- `DialogueBuilder` (builder plugin skeleton): hooks into U++ builders to preprocess/link dialogue artifacts (currently TODO stubs).

Data Flow
---------
- Controls keep a pointer to their `VfsValue` node (`SetNode`) and often bind to thread-like components (e.g., `CompletionThread`, `ChatThread`).
- Model lists retrieved asynchronously via `AiTaskManager::GetModels`; filtering decides which models support completion vs chat.
- Submit builds argument structs and invokes `AiTaskManager` APIs; responses are posted back to update the UI thread.

Extending
---------
- Implement new thread UIs by deriving from `AiThreadCtrlBase`, providing `Data` (to refresh selections) and `Submit` (to dispatch tasks) and embedding via an `AiComponentCtrl` wrapper.
- Extend `DialogueBuilder` if your pipeline requires custom preprocessing/packaging.

Notes
-----
- Non-completion model prefixes are filtered in `CannotDoCompletion` (e.g., `gpt-4`, `o1`, `tts`, embeddings, etc.). Adjust as backends evolve.
- Many controls use `WhenAction` lambdas to live-update underlying VFS state.

