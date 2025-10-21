# USVN And Tooling Take Shape (2008-07 to 2008-08)
**Date Span:** 2008-07-26 to 2008-08-16

By late July, outside contributions appeared: **bytefield** addressed **Debian** package dependencies, while **mrjtuk** fixed MultiList scrolling. In early August, **fidler** introduced “usvn — svn GUI frontend,” followed by a `git-svn` id marker, anchoring the bridge between systems.

Mid-August marked a burst of activity from **cxl**: “uvs2 migration,” `uppdev` workspace additions, and a stream of `git-svn` anchor commits. **SvnFs** (an svn repo commit-browser) emerged, along with CRLF fixes and Windows-safe deletion in **usvn**. A wave of format conversions made **Topics++** more VCS-friendly and includeable, signaling a design tuned for distributed workflows. The “Removed Common directory” note shows consolidation to simplify structure.

## References
- [1] 07e9f1b8a — Solved libstdc++6-dev dependence problem for Debian package. (bytefield, 2008-07-26)
- [2] a38813667 — Fixed MultiList scrolling bug (mrjtuk, 2008-07-30)
- [3] f71334963 — new uvs2 releases : uppsrc-2630  tutorial-38  examples-142  reference-115 (mdelfede, 2008-08-02)
- [4] 549ba2fcd — usvn - svn GUI frontend (fidler, 2008-08-03)
- [5] e443b9cb5 — git-svn-id: svn://ultimatepp.org/upp/trunk@326 f0d560ea-af0d-0410-9eb7-867de7ffcac7 (fidler, 2008-08-03)
- [6] 0fab88012 — uvs2 migration (cxl, 2008-08-15)
- [7] 351994a6c — Adding uppdev.... (cxl, 2008-08-15)
- [8] af712dddc — git-svn-id: svn://ultimatepp.org/upp/trunk@329 f0d560ea-af0d-0410-9eb7-867de7ffcac7 (cxl, 2008-08-15)
- [9] 58769f83e — Developing SvnFs (svn repo commit-browser) (cxl, 2008-08-15)
- [10] ba45f0afb — Fixed cr-lf problem in .upp files (found when working with svn) (cxl, 2008-08-15)
- [11] d242f7cfc — svn test (cxl, 2008-08-16)
- [12] c57a0135e — testing svnsync.. (cxl, 2008-08-16)
- [13] 7f84abf57 — testing svnsync 329 (cxl, 2008-08-16)
- [14] f1c9e4d21 — Removed Common directory, T++ stylesheets replaced by templates (cxl, 2008-08-16)
- [15] 2383192b1 — Changed .tpp format to be more svn firendly; groups that need to be included must be now marked includeable (cxl, 2008-08-16)
- [16] 01547d41e — Fixed usvn in Win32 to delete even read-only .svn dirs when required (cxl, 2008-08-16)
- [17] 29fd35282 — All Topics++ now converted to the new format (where required, they are made includeable) (cxl, 2008-08-16)
