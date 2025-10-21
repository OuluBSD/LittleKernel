Core — Enums, Phonemes, Helpers

Overview

- Central definitions used across AI/Core:
  - Global enumerations and macro-generated tag sets (attributes, actions, content, phonemes, etc.).
  - Natural language helpers (tokenization), phoneme models and distances.
  - Color utilities and general-purpose helper functions.
  - Container header types for actions/attributes shared by classifiers and content models.

Key Modules

- Enums.*
  - Heavily macro-driven enumerations for attributes, content types, and phonemes.
  - Also defines phoneme alt tables and counts; generated enums such as `PHONOME_VOWEL_*`, `PHONOME_CONSONANT_*`, and unified `PHONOME_*`.
  - Additions require updating the macro lists and the associated string tables in `EnumsContent.cpp`.

- Phoneme.h
  - Defines the phoneme universe and provides:
    - `GetPhonemeEnum`, `IsPhonemeVowel/Consonant`, `GetPhonemeDuration`, `GetPhonemeRepeats`
    - Spelling distance functions: `GetSpellingDistance/Raw/Relative`
    - Distance matrices: `vowel_distance[...][...]`, `consonant_distance[...][...]`

- NatLang.h
  - `NaturalTokenizer` splits text into token lines (`Vector<Vector<WString>>`) with foreign-language detection.
  - Use `Parse(text)`, inspect `HasForeign()` and `GetLines()`.

- Container.h
  - `ActionHeader {action,arg}` and `AttrHeader {group,value}` with hashing, ordering, JSON/Visit, and helpers like `Trim()` and `ToString()`.
  - Widely used in DataModel exports, Lyrical content, and Platform/Social analytics.

- Fn.h
  - Color tools: color groups, HSV conversion, `GetColorDistance`, and assorted sorters.
  - Text helpers: `DeHtml`, `GetWords`, case utilities, line cleanup (`RemoveEmptyLines*`, `RemoveQuotes*`).
  - Map helpers: `MapGetAdd`, `FixedIndexFindAdd`.
  - JSON visitor loader: `LoadFromJsonFile_VisitorNode`.

- Types.h
  - Declares dataset structs via `DATASET_LIST` macros; other packages define the concrete structs.

- Common.h/Defs.h
  - Common includes and UI/inspection helpers (e.g., dbrowser modes in `Defs.h`).

Patterns & Conventions

- Macro enums: Macro lists define both the enum and the stringification. Keep additions grouped and sorted logically to avoid churn.
- `Visit(Vis&)`: Prefer visiting fields over manual JSON to keep serialization unified.
- Container headers: Use `Trim()` after constructing from free-form text; `IsEmpty()` guards against incomplete entries.

Typical Workflows

- Add a new attribute group/value
  - Extend the appropriate macro list in `Enums.h` and propagate names in `EnumsContent.cpp`.
  - Use `AttrHeader(group,value)` across the codebase; `AttrKeys` lookup and `FindAttrGroup/Value` from `Fn.h` can assist.

- Compare words/phonetics
  - Use `GetSpellingDistance` for weighted phonetic-aware distance or `GetSpellingRawDistance` for raw edit distance.

Public Surface

- Include umbrella: `#include <AI/Core/Core/Core.h>`
