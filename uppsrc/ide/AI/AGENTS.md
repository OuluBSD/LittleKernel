AGENTS

Scope
- Applies to `uppsrc/ide/AI`.

Purpose
- TheIDE AI features and related designer hooks. Bridges IDE UI to AI provider abstractions.

Package Overview
- Manifest: `AI.upp` (uses `ide/Designers`, `AI/Ctrl`, `ide/Vfs`).
- Files: `AI.h`, `Designer.h`, `IdeAIDes.cpp`, `DesT.cpp`, `AICore.cpp`.
- Related in parent package: `AiProvider.{h,inl}`, `AITaskDlg.cpp`.

Extension Points
- Add or extend AI tools in `AICore.cpp` and surface designer integrations in `IdeAIDes.cpp`.
- Provide provider-specific logic behind `AiProvider` interfaces in the main `ide` package.

.upp File Notes
- Keep `AGENTS.md` listed first in `AI.upp`.

