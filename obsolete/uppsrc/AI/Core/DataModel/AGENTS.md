DataModel — Text Database and Analytics

Overview

- Defines the structured text database used by Content, Platform, and Social modules:
  - Contexts and labeled content/typeclasses per context
  - Tokens, words, translations, and phrase constructs (virtual phrases and structures)
  - Phrase parts annotated with attributes/actions, colors, types, and scores
  - Exportable analytics (attrs, actions, wordnets, transitions, parallels)
  - Source database component with persistence helpers

Key Types

- ContextType / ContextData
  - `ContextType`: bitfield over features like Creativity, Technicality, etc. Helpers for common presets (Programming, Dialog, etc.).
  - `ContextData`: per-context typeclasses, content definitions, parts and entity groups; `Visit` persists names and structures.

- Tokens/Words/Translations
  - `TokenText`: sequence of token ids and word indices with optional virtual phrase mapping.
  - `TokenIdx`, `WordTranslation`, and language-indexed maps inside `SrcTextData` handle multilingual corpora.

- Phrase Models
  - `VirtualPhrase`, `VirtualPhrasePart`, `VirtualPhraseStruct`: abstract composition of word classes into higher-level phrase structures and parts.
  - `PhrasePart`: realized segment with words, type, attr/action annotations, colors, actions/typecasts/contrasts, scores, and language/context.

- Export Models
  - `ExportAttr`, `ExportAction`: aggregated counts/attributes and color data.
  - `ExportParallel`, `ExportTransition`: co-occurrence and transition statistics.
  - `ExportWordnet`: compact wordnet snapshot with main class, color info, and scores across up to `MAX_WORDS`.

- TextKeypoint
  - Holds representative tokens and descriptor vectors with clustering for text-classification.

- SrcTxtHeader / SrcTextData
  - `SrcTxtHeader` is an ECS component owning the persisted `SrcTextData` (lazy-realized via `RealizeData/LoadData/SaveData`).
  - `SrcTextData` aggregates all the above: authors, scripts, contexts, tokens/words, phrase structures, classifiers (elements, attrs/actions), and analytics (wordnets, transitions/parallels).
  - Convenience: stringification helpers (`GetWordString`, `GetTokenTextString`, `GetTypeString`, `GetActionString`), and compact script dumpers.

Workflows

- Ingest and classify source text
  - Attach `SrcTxtHeader` to a VFS node; call `RealizeData()`; feed input text; run Content/Prompting processes for tokenization and structure inference; serialize via `Visit`.

- Compute transitions/wordnets
  - Once phrase parts and attrs/actions are set, fill `Export*` models and use them for platform/social analyses.

Gotchas

- Hashing: Many types define `GetHashValue()` over vectors; preserve order to avoid accidental collisions.
- Language: `SrcTextData::current_language` is mutable state; ensure it matches operations that use language-specific dictionaries/maps.

Public Surface

- Include umbrella: `#include <AI/Core/DataModel/DataModel.h>`
