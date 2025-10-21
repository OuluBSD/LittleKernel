Tools — Project Wizard and VFS Programs

Overview

- Developer-facing utilities to scaffold projects, split configurations, and interact with VFS-backed programs.

Key Modules

- ProjectWizard.h
  - Declarative configuration model: `ConfigurationNode` with options (fixed, buttons, prompts, user inputs, arrays).
  - `ProjectWizardView` stores the graph of nodes, exposes registry helpers (`Register`, `RegisterDynamic`) and a rich set of operations to split components, dependencies, categories, and packages.
  - Integrates with Prompting by producing `GenericPromptArgs` from dynamic nodes.

- VfsProgram.*
  - VFS components representing program code, sessions, and iterations (`VfsProgram`, `VfsProgramProject/Session/Iteration`).
  - `VfsProgram::RealizePath` materializes subtrees under VFS paths for tool-driven codegen or scripting.

Workflows

- Add a project template
  - `ProjectWizardView::Register(path,title)` → define options → implement callbacks (e.g., `SplitComponents`) to construct downstream artifacts.

Public Surface

- Include umbrella: `#include <AI/Core/Tools/Tools.h>`

