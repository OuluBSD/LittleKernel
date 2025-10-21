#ifndef _AI_TextCtrl_LyricRef_h_
#define _AI_TextCtrl_LyricRef_h_

NAMESPACE_UPP


class ScriptPhrasePartsGroups : public Ctrl {
	ComponentCtrl& o;
	Splitter vsplit, hsplit;
	bool set_cursor = true;
	
protected:
	friend class ScriptReferenceMakerCtrl;
	DropList mode;
	ArrayCtrl attr_groups, attr_values, colors, actions, action_args, parts;
	ArrayCtrl elements, typeclasses, contents;
	
	VectorMap<String, VectorMap<String, int>> uniq_acts;
	Vector<int> sort;
	
public:
	typedef ScriptPhrasePartsGroups CLASSNAME;
	ScriptPhrasePartsGroups(ComponentCtrl& o);
	
	void UpdateNavigator();
	void Data();
	void DataList();
	void UpdateCounts();
	void SetIndexCursor(int idx, ArrayCtrl& arr);
	void InitArray(ArrayCtrl& arr, String title, DatabaseBrowser::ColumnType t);
	void FillArrayCtrlColor(DatabaseBrowser::ColumnType t, ArrayCtrl& arr);
	void FillArrayCtrl(DatabaseBrowser::ColumnType t, ArrayCtrl& arr);
	hash_t GetModeHash() const;
	void SetModeCursor(int mode);
	
	Event<> WhenBrowserCursor;
	
};

void ReadNavigatorState(LyricalStructure& s, int part_i, int sub_i, int line_i, NavigatorState& state, int depth_limit);

// TODO rename
class ScriptReferenceMakerCtrl : public AiComponentCtrl {
	String data;
	
protected:
	friend class PartContentCtrl;
	friend class PartLineCtrl;
	
	Splitter hsplit, lsplit, split;
	ArrayCtrl parts;
	WithPartInfoForm<Ctrl> form;
	PartContentCtrl content;
	ScriptPhrasePartsGroups db0;
	
public:
	typedef ScriptReferenceMakerCtrl CLASSNAME;
	ScriptReferenceMakerCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void DataPart();
	void DataLine();
	void Do(int fn);
	void OnValueChange();
	void MakeLines();
	void OnBrowserCursor();
	void SetLineText();
	void UpdateMode();
	int GetActiveMode();
	int GetInheritedMode();
	void SetFont(Font fnt);
	void ReadNavigatorState(NavigatorState& state, int depth_limit=INT_MAX);
	
};

INITIALIZE(ScriptReferenceMakerCtrl)

END_UPP_NAMESPACE

#endif
 
