# Tooling, Packaging, and Core Expansions (2008-08 to 2008-09)
**Date Span:** 2008-08-17 to 2008-09-30

Late August opened with packaging work and repository hygiene. An `rpm` helper landed, soon joined by a generic `.spec` for easy rpmbuilds and an added `expat-devel` dependency for clean builds on RPM-based distros. In parallel, the freshly integrated `usvn` gained resilience: folder-replacement detection and follow-up fixes tightened its handling of edge cases.

Database tooling evolved with `Sql` gaining `ExportSch` to emit `.sch` files and a companion `ExportIds` utility to generate SQLID constants. MySQL character set handling received explicit calls to keep encodings correct across sessions. On the docs and automation side, both `uppweb` and TheIDE improved command-line behavior, while package documentation (notably for `Sql` and sockets in `Web`) expanded.

Core and UI picked up momentum: a Mersenne Twister–based `Random` replaced older behavior and now feeds `Uuid`; a new `CharFilterNotWhitespace` arrived; and GridCtrl/DropGrid updates continued apace. TheIDE gained a handy view to show GCC-generated assembly per file and a refined Run dialog. New UI pieces appeared too, with `ExpandFrame/Ctrl` and Docking updates.

September concentrated on editor ergonomics and parser polish. Fixes and tweaks to T++/A++ formatting landed alongside smarter navigation: a second-chance Alt+I match on method names, improved Alt+J indentation, steadier parenthesis and namespace highlighting, and targeted help-window and Ctrl+F behavior corrections. Down in I/O, new stream helpers (OutStream, TeeStream, Md5Stream, Sha1Stream, Crc32Stream) arrived with corresponding hash utilities, wrapped up with a consistent header rename for `Hash`.

## References
- [1] fba3cd100 — added linux-scripts/rpm (cxl, 2008-08-17)
- [2] e61357ab8 — New generic .spec file tor easy rpmbuild package construction (amrein, 2008-08-19)
- [3] c596e6aeb — Build need expat-devel (amrein, 2008-08-19)
- [4] 771b479eb — Added usvn ability to detect folder replacement and to deal with the situation... (cxl, 2008-08-19)
- [5] 2655d7e19 — Fixed one more issue with usvn replace (cxl, 2008-08-19)
- [6] 450e861f0 — new fn ExportSch in Sql exports .sch file from SqlSession (cxl, 2008-08-20)
- [7] 2fe3327c8 — Fixed encoding issues in MySql, new ExportIds utility (in Sql) creates a file of SQLID constants for given database (cxl, 2008-08-20)
- [8] bbfb17d21 — Minor mysql encoding improvement... (to be sure, we call mysql_set_character_set too) (cxl, 2008-08-20)
- [9] ea699eb2a — Fixed uppweb to work in commandline mode (cxl, 2008-08-26)
- [10] 215bdadf0 — Small fix in theide commandline mode (cxl, 2008-08-26)
- [11] 2e5e103cd — Added documentation to Sql package. (captainc, 2008-08-27)
- [12] 1d1ab72a9 — Fixed a couple of issues of Bazaar packages with GCC/Linux (cxl, 2008-08-28)
- [13] 06758e3da — Added CharFilterNotWhitespace (cxl, 2008-08-28)
- [14] 5fae0ee14 — Added mersenne twister based Random to Core (cxl, 2008-08-28)
- [15] 62760d743 — Fixed Random for Win32, Uuid now uses Random (cxl, 2008-08-28)
- [16] 8ffe31ae7 — GridCtrl and DropGrid updated to the latest versions (unodgs, 2008-08-28)
- [17] 1541414bf — With GCC, TheIDE now can show assembler code for the file (similar to Preprocess file) (cxl, 2008-08-31)
- [18] 6314eac08 — More popup fixes (passing events) (unodgs, 2008-08-31)
- [19] 9261328a2 — ExpandFrame/Ctrl package added (mrjt, 2008-09-02)
- [20] 1bf35c9b4 — Docking update (mrjt, 2008-09-02)
- [21] 45177b29c — Fixed Time support in Sqlite3 (column must have datetime type) (cxl, 2008-09-03)
- [22] 755feab7e — Added socket documentation to srcdoc.tpp group in Web package. -captainc (captainc, 2008-09-05)
- [23] 169c97d10 — Various A++/T++ fixes, Alt+F10 in T++ invokes 'fix current topic' hidden function (cxl, 2008-09-22)
- [24] cbffa8867 — Alt+I now performs second chance match for just method name if full signature counterpart not found (cxl, 2008-09-22)
- [25] c10b76816 — Improved parenthesis CodeEditor highlighting (recovery heurestics), improved namespace highlighting (top-level { are black even after namespace {) (cxl, 2008-09-24)
- [26] 473e8d699 — Copying in OleDB fixed, GccBuilder .map generation fixed (cxl, 2008-09-24)
- [27] 4500cc695 — Fix of Ctrl+F vs HelpWindow problem (cxl, 2008-09-24)
- [28] 701dce0da — Alt+J improved - corrected behaviour with multiline definition, adds appropriate number of tabs (cxl, 2008-09-24)
- [29] b0cc9c51a — OutStream, TeeStream, Md5Stream, Sha1Stream, Crc32Stream, MD5, SHA1, CRC32, optimized C++ parser, CodeEditor annotations (cxl, 2008-09-28)
- [30] 088f16aae — <hash.h> -> <Hash.h> (cxl, 2008-09-30)

