# Assist++ Refactor and Editor Overhaul (2008-10)
**Date Span:** 2008-10-01 to 2008-10-31

October concentrated on developer ergonomics and editor depth. Early fixes landed for QTF and Vista menu quirks as licensing notices switched to a modified BSD. Rendering and SQL hooks received attention (GLCtrl flicker fixes; `SqlArray::WhenPreQuery`), while RichText/RichEdit styling behavior evolved through the month.

The headliners were a substantial C++ parser and Assist++ refactor, culminating in a new TheIDE code browser and broader navigator features (e.g., find-in-all-scopes). Assist++ gained THISBACK support and a raft of stability fixes. CodeEditor improvements tightened left-bar visuals, scope data in coderefs, and syntax highlight correctness.

UI/platform polish threaded throughout: X11 GLX fixes, a stubborn tooltip issue resolved, multiple GLCtrl robustness tweaks, and DHCtrl visibility corrections. Tooling progressed with OLEDB logging and insert-id helpers, TreeCtrl API refinements, and layout wizard naming conventions. T++ work continued with annotations, click-to-document, and formatting iterations.

Infrastructure and content rounded out the span: public SVN mirror links on the website, Bazaar examples for ExpandFrame, and ongoing `uppdev` sync points. Core and reference updates included `Xmlize_std` and formatting corrections, plus consistent header/style cleanups.

## References
- [1] 6832764d9 — Fixed QTF issue (cxl, 2008-10-01)
- [2] 5b021343e — Ultimate++ use the modified BSD license now (amrein, 2008-10-01)
- [3] eacb4bc94 — GLCtrl Vista flickering fix (cxl, 2008-10-02)
- [4] 3d59c0efe — SqlArray::WhenPreQuery (cxl, 2008-10-02)
- [5] ff1ec91ef — Changed RichText/RichEdit apply-style operation (cxl, 2008-10-02)
- [6] eb01894cc — Fixed syntax highlighting issue (cxl, 2008-10-03)
- [7] c4bc08111 — Minor CodeEditor left bar fix (cxl, 2008-10-03)
- [8] 79729f6df — Changed scope information in coderefs (cxl, 2008-10-03)
- [9] 5806a3f0f — Fixed 'GLX' issue in X11 (cxl, 2008-10-04)
- [10] 3ed63a135 — Relatively massive C++ parser and Assist++ refactoring... (cxl, 2008-10-05)
- [11] 85b68a9f1 — TheIDE got new code-browser (cxl, 2008-10-06)
- [12] d0ca3b34a — Major Assist++ refactoring (cxl, 2008-10-10)
- [13] 5f09ee9c4 — More A++ fixes, A++ now supports THISBACK (cxl, 2008-10-10)
- [14] 2ff4ba43b — Code navigator: Find in all scopes (cxl, 2008-10-11)
- [15] 26c8472ce — Refactored T++ ReferenceDlg (cxl, 2008-10-11)
- [16] cedd4340c — Modified PCRE to include .c files; fixed serialization of unsigned long/long (32-bit) (cxl, 2008-10-13)
- [17] d7c05223b — Fixed theide debugger crashing and Alt+C of global functions (cxl, 2008-10-13)
- [18] a6962145e — Fixed problem with 'stuck' informational popups (e.g. ToolTip) (cxl, 2008-10-17)
- [19] ef27f6fa5 — Added uppsts OpenGLTest (cxl, 2008-10-19)
- [20] df63d31dd — Fixed GLCtrl (cxl, 2008-10-19)
- [21] 2a0434345 — GLCtrl::BeforeTerminate fix (cxl, 2008-10-19)
- [22] ac090225a — Fixed DHCtrl Show/Hide (cxl, 2008-10-19)
- [23] be5574d06 — OLEDB: GetInsertedId (cxl, 2008-10-21)
- [24] 2621a9c8c — Layout class wizard now adds 'Dlg' after the name (cxl, 2008-10-22)
- [25] ed7653ecb — Improved navigator ergonomics (cxl, 2008-10-22)
- [26] a6ac4848a — TreeCtrl now has global display (cxl, 2008-10-23)
- [27] cdeed639f — T++ now reacts to 'click to document' (cxl, 2008-10-28)
- [28] ba17b0143 — reference: Xmlize_std (cxl, 2008-10-20)
- [29] d8177daa6 — Fixing Format (cxl, 2008-10-30)
- [30] 5c44acf9d — Fixed Format (cxl, 2008-10-31)
