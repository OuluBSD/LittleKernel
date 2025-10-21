#ifndef _AI_TextCtrl_LyricContent_h_
#define _AI_TextCtrl_LyricContent_h_

NAMESPACE_UPP


class ToolEditor;
class ScriptReferenceMakerCtrl;
class PartContentCtrl;
class PartLineCtrl;

class PartLineCtrl : public Ctrl {
	
public:
	PartContentCtrl& o;
	int sub_i = -1, line_i = -1;
	
	
public:
	typedef PartLineCtrl CLASSNAME;
	PartLineCtrl(PartContentCtrl& o);
	void Paint(Draw& d) override;
	void PaintTextBlock(Draw& d, int& x, int off, Rect& out, Color bg, const String& txt, const Font& fnt);
	static const int indent;
	
	void LeftDown(Point p, dword keyflags) override;
	void GotFocus() override;
	void LostFocus() override;
	bool Key(dword key, int count) override;
	void MouseWheel(Point p, int zdelta, dword keyflags) override;
	
	void Select();
	bool IsSelected() const;
	
	LineElement* GetLineEl() const;
	DynLine* GetDynLine() const;
	
};

// TODO rename
class PartContentCtrl : public Ctrl {
	
protected:
	friend class ScriptReferenceMakerCtrl;
	friend class PartLineCtrl;
	ScriptReferenceMakerCtrl* o = 0;
	Vector<String> element_keys;
	ScrollBar scroll;
	Array<PartLineCtrl> lines;
	int lh = 20;
	int selected_line = -1;
	Font fnt;
	
public:
	typedef PartContentCtrl CLASSNAME;
	PartContentCtrl();
	PartContentCtrl(ScriptReferenceMakerCtrl& o);
	
	void Paint(Draw& d) override;
	void Layout() override;
	void Data();
	void MoveSelection(int i);
	void InitDefault(PartLineCtrl& l);
	void AddElements(DropList& dl);
	int FindElement(const String& s);
	void OnElementChange(int sub_i, int line_i, DropList* dl);
	int GetCursor() const;
	const PartLineCtrl& Get(int i) const {return lines[i];}
	PartLineCtrl& Get(int i) {return lines[i];}
	void DataSelAction(PartLineCtrl* l);
	void OnLineValueChange(PartLineCtrl* l);
	void DataLine(PartLineCtrl& l);
	void Select(PartLineCtrl* line);
	int Find(const PartLineCtrl* line) const;
	PartLineCtrl* GetActiveLine();
	int GetLineCount() const {return lines.GetCount();}
	void SetFont(Font fnt) {this->fnt = fnt;}
	
	Event<> WhenCursor;
	
};


END_UPP_NAMESPACE

#endif
 
