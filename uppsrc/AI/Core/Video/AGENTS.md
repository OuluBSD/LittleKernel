Video — Prompts, Storyboards, Sources

Overview

- Video-centric scaffolding: prompt makers, storyboard components, and source file definitions.

Key Modules

- VideoPromptMaker (Component)
  - ECS placeholder for defining prompt templates used to generate or edit video content.

- VideoStoryboard (Component)
  - Stores a storyboard structure for later rendering or export.

- VideoSourceFile.h
  - Value components: `ProtoVideoTask1`, `VideoSourceFile`, `VideoSourceFileRange` — primitives for describing and slicing video inputs.

Workflows

- Generate a storyboard from prompts
  - Use `VideoPromptMaker` to collect prompt lines; materialize shots/scenes into `VideoStoryboard`.

Public Surface

- Include umbrella: `#include <AI/Core/Video/Video.h>`
