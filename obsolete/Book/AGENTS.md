# Book Authoring Guidelines (Living)

Mandatory Gate
- Do not edit any file under `Book/` until you have read this file end-to-end and verified your chapter follows the conventions below.
- Each PR touching `Book/*` must state that the Book style was reviewed and applied.

# Roles
- Chronicle Writer: drafts long-form chapters with narrative based on commit subjects, adds visible date spans, and maintains numbered References.
- Upstream Scribe: maintains Volume I (Ultimate) with many long markdown files, approximating history when needed and marking guesses explicitly.
- Fork Scribe: maintains Volume II (Topside) with current work, uses inline [n] citations and ordered References; preserves git author names.
- Style Keeper: ensures Markdown headings (#, ##), bold/italic, and occasional underline are used for readability; keeps boss-word styling consistent.
- Boss-Word Herald: uses rotating icons (✦, ★, ◆, ⟁, ✧, ◈) with italicized boss words and includes “(Seppo)” on the first occurrence after a while.
- Manifest Curator: keeps .upp manifests present and quoting file paths, adding new chapters as they land so TheIDE can browse them.

# Conventions
- Prefer fewer, longer markdown files per chapter; avoid tiny files.
- Show “Date Span: YYYY-MM-DD to YYYY-MM-DD” near the top of chapters.
- Use markdown syntax extensively: #, ##, ###, bold, italics, underline, small emoji/icons where helpful.
- Lists must use '-' prefix. Reference lists must use '- [n]' prefix followed by entry text.
- Allow occasional text color with backfill when renderer supports it.
- Use git authors as-is; do not rename.
- When approximating upstream, label sections with “(guess)”.

## Reference List Style
- Use '- [n]' bullets under a '## References' heading.
- Format: SHA — subject (Author, Date)
- Keep inline [n] citations in text. Ordered (1., 2., 3.) lists are acceptable only if a renderer requires them.

# Maintenance
- Always update this file when process or roles evolve.
- Add new roles as workflows expand (e.g., Image Curator, Link Weaver).

## Manifests (.upp) in Book
- Each Book package must list `AGENTS.md` first in its `file` section using a relative path (e.g., `../AGENTS.md`).
- If `CURRENT_TASK.md` exists in the package directory, list it immediately after `AGENTS.md`.
- Keep all chapter `.md` files for the package enumerated in `file` in ascending numeric order.
- Ensure the aggregate `Book/Book/Book.upp` uses subpackages and includes `../AGENTS.md` in its `file` list.

## Session Handoff (Close/Resume)
- Before closing a session, write a concise `Book/CURRENT_TASK.md` that:
  - Links to `Book/AGENTS.md` and confirms compliance.
  - Summarizes what was completed in the session (chapters, summaries, manifests touched).
  - States the immediate next steps (e.g., next date spans to cover, summaries to backfill).
  - Notes any open questions or decisions for the next pass.
- On resume, continue with the next planned step from `Book/CURRENT_TASK.md` without re-laying groundwork.
- When adding new chapters:
  - Create `Book/I - Ultimate/<NNN> - <Title>.md` with Date Span and Reference list.
  - Create/refresh the paired summary `Book/<Title>.md` with the same Date Span.
  - Update the relevant `.upp` manifest(s) to include new files.
