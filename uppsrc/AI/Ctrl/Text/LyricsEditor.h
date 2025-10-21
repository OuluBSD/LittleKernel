#ifndef _AI_TextCtrl_LyricsEditor_h_
#define _AI_TextCtrl_LyricsEditor_h_

NAMESPACE_UPP


class ScriptTextSolverCtrl;


class StructuredScriptEditor : public Ctrl {
	struct Area : Moveable<Area> {
		const DynLine* selected_line = 0;
		const DynSub* selected_sub = 0;
		const DynPart* selected_part = 0;
		Rect r;
		void Set(Rect rc, const DynLine& l) {r = rc; selected_line = &l;}
		void Set(Rect rc, const DynSub& l) {r = rc; selected_sub = &l;}
		void Set(Rect rc, const DynPart& l) {r = rc; selected_part = &l;}
	};
	Vector<Area> areas, vert_areas;
	
	int line_h = 20;
	ScrollBar scroll_v;
	int txt_src = 0;
	
	enum {
		SRC_NORMAL,
		SRC_SOURCE,
		
		SRC_COUNT
	};
	
protected:
	friend class ScriptTextSolverCtrl;
	const DynLine* selected_line = 0;
	const DynSub* selected_sub = 0;
	const DynPart* selected_part = 0;
	
public:
	typedef StructuredScriptEditor CLASSNAME;
	StructuredScriptEditor();
	
	void Paint(Draw& d) override;
	void Layout() override;
	void MouseWheel(Point p, int zdelta, dword keyflags) override;
	void LeftDown(Point p, dword keyflags) override;
	bool Key(dword key, int count) override;
	void MoveSelected(int i);
	void Update();
	void CheckClearSelected();
	void ClearSelected();
	void SwitchTextSource() {txt_src = (txt_src + 1) % SRC_COUNT; Refresh();}
	void ShowNormalText(bool b=true) {txt_src = SRC_NORMAL; Refresh();}
	void ShowSourceText(bool b=true) {txt_src = SRC_SOURCE; Refresh();}
	void ScrollView(const Rect& r);
	bool IsAnySelected() const;
	ScriptTextSolverCtrl* owner = 0;
	
	Event<> WhenCursor;
	
};

// TODO rename
class ScriptTextSolverCtrl : public AiComponentCtrl {
	Splitter hsplit;
	StructuredScriptEditor editor;
	TabCtrl tabs;
	
	// Wizard tab
	ParentCtrl wizard_tab;
	
	// Suggestions tab
	ParentCtrl sugg_tab;
	Splitter sugg_split;
	ArrayCtrl sugg_list;
	ArrayCtrl sugg_lyrics;
	SolverBaseIndicator sugg_prog;
	Label sugg_remaining;
	
	// Whole song tab
	ParentCtrl whole_tab;
	SolverBaseIndicator whole_prog;
	Label whole_remaining;
	Splitter whole_split, whole_hsplit0, whole_hsplit1;
	ArrayCtrl colors, attrs, actions, phrases;
	
	// Part tab
	ParentCtrl part_tab;
	WithEditorPart<Ctrl> part_form;
	Splitter part_split;
	ArrayCtrl part_suggs;
	
	// Sub tab
	ParentCtrl sub_tab;
	WithEditorSub<Ctrl> sub_form;
	Splitter sub_split;
	ArrayCtrl sub_suggs;
	
	// Line tab
	ParentCtrl line_tab;
	WithEditorLine<Ctrl> line_form;
	ArrayCtrl line_ref_lines, line_suggs;
	
public:
	typedef ScriptTextSolverCtrl CLASSNAME;
	ScriptTextSolverCtrl();
	
	void SetFont(Font fnt) {}
	void ToolMenu(Bar& bar) override;
	void Data() override;
	void DataSuggestions();
	void DataWhole();
	void DataPart();
	void DataSub();
	void DataLine();
	void SwitchEditorText();
	void Do(int fn);
	void DoSuggestions(int fn);
	void DoWhole(int fn);
	void DoPart(int fn);
	void DoSub(int fn);
	void DoLine(int fn);
	void OnEditorCursor();
	void OnValueChange();
	Vector<const DynLine*> GetLineGroup(const DynPart** part=0, const DynSub** sub=0, const DynLine** line=0, int* part_iptr=0, int* sub_iptr=0, int* line_iptr=0);
	void GetPart(const DynPart** part, int* part_iptr);
	void GetSub(const DynPart** part=0, const DynSub** sub=0, int* part_iptr=0, int* sub_iptr=0);
	const DynLine* GetAltLine();
	void UpdateEntities(DynLine& dl, bool unsafe, bool gender);
	
};

INITIALIZE(ScriptTextSolverCtrl)



END_UPP_NAMESPACE

#endif
 
