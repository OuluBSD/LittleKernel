AGENTS

Scope
- Applies to `uppsrc/CodeEditor`.

Purpose
- Source code editor component with syntax highlighting, lexers, and editing utilities used by TheIDE and apps.

Key Areas
- Syntax: multiple lexers (C/C++, diff, tags, Python, logs) and registry.
- Editor widgets: search/replace, language helpers, layouts.

Extension Points
- Add new lexers under `*Syntax` groups and register in `RegisterSyntax.cpp`.

.upp Notes
- List `AGENTS.md` first in `CodeEditor.upp`.

