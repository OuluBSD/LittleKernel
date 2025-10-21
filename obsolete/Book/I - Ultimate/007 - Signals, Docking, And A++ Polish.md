# Signals, Docking, and A++ Polish (2008-11)
**Date Span:** 2008-11-01 to 2008-11-30

November brought breadth: a new Bazaar `Signals` package (with docs and sample), expanded apps (PDF printer frontend), and steady compiler/docs refresh. Database/UI APIs evolved: DBF gained FPT memo support; `SqlArray` topics started; `Date::Compare` arrived; and assorted ArrayCtrl/HeaderCtrl capabilities were refined and surfaced.

Docking advanced with feature additions, appearance fixes, restore-position support, and example updates. Assist++ continued its quality push with forward-declaration handling, navigator range and ordering controls, popup-driven insertion, improved param-info tips (nested assists), and Alt+I/insert fixes. TheIDE accrued conveniences like ToUpper/Lower/InitCaps/SwapCase, printing, SVN sync improvements (single-package/nest, revert, empty commit message warning), and better T++ integration (auto-adding .tpp groups, context menus, annotations rendering).

Across platform/tooling, GLCtrl refactoring landed; `LocalProcess` moved from usvn to Core; OLEDB gained `GetInsertedId` and `SqlRaw` support; PGSQL quoting improved; X11 unicode handling was proposed; and website/docs received many updates. Visual ergonomics improved with LOG macro highlighting, underlining polish, popup fixes, and help pane flicker avoidance.

## References
- [1] 034bfef45 — Added Signals package with doc and sample (micio, 2008-11-01)
- [2] 91402998d — SqlArray topics started (cxl, 2008-11-02)
- [3] 35b49037c — Docking: Restore window position added (mrjt, 2008-11-03)
- [4] f52e0397c — Date::Compare (cxl, 2008-11-03)
- [5] b01eea2a6 — GLCtrl refactoring by Koldo (cxl, 2008-11-05)
- [6] 3c0a6f1aa — DropList::DropWidth, DropList::DropWidthZ (cxl, 2008-11-05)
- [7] 2adc3116b — A++ - fixed forward struct/class declaration problem (cxl, 2008-11-05)
- [8] 64e4c9751 — T++ Move fix, DropList fix (cxl, 2008-11-05)
- [9] ae2468876 — A++: fixed typecast operator; navigator scope double-click behavior (cxl, 2008-11-05)
- [10] fad284c73 — A++ fixes (cxl, 2008-11-06)
- [11] 6443f4fa9 — A++ navigator range button (cxl, 2008-11-06)
- [12] 6a19814b0 — T++ - insert before logic (cxl, 2008-11-06)
- [13] d32fe2062 — Auto-adding edited .tpp groups to package, if missing (cxl, 2008-11-07)
- [14] 1ec6aeac9 — U++ LOG macros now highlighted (cxl, 2008-11-07)
- [15] a1f17348e — LoadFrom/StoreTo fixed; usvn keeps commit message if no commit (cxl, 2008-11-07)
- [16] 36aa0b972 — HeaderCtrl::Fix; ArrayCtrl::GetScreenRect(i, j)/RefreshRow (now public) (cxl, 2008-11-10)
- [17] 3799d6846 — ArrayCtrl::GetScreenRect renamed to GetScreenCellRect (cxl, 2008-11-10)
- [18] d2e36924c — Improved CallbackArgTarget (cxl, 2008-11-11)
- [19] a3de6030b — MSSQL Date::Low() fix (cxl, 2008-11-11)
- [20] 80d1a2e54 — TheIDE SVN: revert in sync dialog; sync single package or nest (cxl, 2008-11-15)
- [21] 0f4b4434e — TheIDE SVN: empty commit message warning (cxl, 2008-11-15)
- [22] 64582a58f — DropChoice DropWidth[Z] (cxl, 2008-11-16)
- [23] c9aad0697 — LocalProcess moved from usvn to Core (cxl, 2008-11-16)
- [24] 7db347d0a — T++ file menu: 'svn synchronize' (cxl, 2008-11-16)
- [25] 819966a8f — Ide editor ToUpper/Lower/InitCaps/SwapCase (cxl, 2008-11-17)
- [26] de2191b22 — SqlRaw support in OleDB (cxl, 2008-11-21)
- [27] 77b3fc691 — A++ inserting function shows popup; no param insertion (cxl, 2008-11-22)
- [28] 29ac096dc — A++ param info tip supports nested assists; Esc closes tip (cxl, 2008-11-23)
- [29] 53867ea6a — TheIDE now can Print (cxl, 2008-11-23)
- [30] fc49df8db — ide help: flicker avoided; font resize button fixed (cxl, 2008-11-29)
- [31] 66dc3e935 — LogFile procexepath_ issue fix (cxl, 2008-11-30)
- [32] 44771e884 — Assist 'Insert'; copy of Image name (cxl, 2008-11-30)
