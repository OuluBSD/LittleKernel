#include "Text.h"

NAMESPACE_UPP

PartContentCtrl::PartContentCtrl() {
	AddFrame(scroll);
	fnt = Monospace(15);
	
	scroll.SetLine(lh);
	scroll.WhenScroll << THISBACK(Layout);
}

PartContentCtrl::PartContentCtrl(ScriptReferenceMakerCtrl& o) : o(&o) {
	AddFrame(scroll);
	fnt = Monospace(15);
	
	scroll.SetLine(lh);
	scroll.WhenScroll << THISBACK(Layout);
}

void PartContentCtrl::Paint(Draw& d) {
	
}

int PartContentCtrl::GetCursor() const {
	return selected_line;
}

int PartContentCtrl::Find(const PartLineCtrl* line) const {
	for(int i = 0; i < lines.GetCount(); i++)
		if (&lines[i] == line)
			return i;
	return -1;
}

void PartContentCtrl::Layout() {
	Size sz = GetSize();
	int y = -scroll;
	int w = sz.cx;
	for(int i = 0; i < lines.GetCount(); i++) {
		PartLineCtrl& line = lines[i];
		int h = line.line_i < 0 ? lh : lh * 2;
		Rect r = RectC(0, y, w, h);
		line.SetRect(r);
		
		y += h;
	}
	scroll.SetPage(sz.cy);
}
void PartContentCtrl::Select(PartLineCtrl* line) {
	int prev_sel = selected_line;
	if (line) for(int i = 0; i < lines.GetCount(); i++) {
		if (line == &lines[i]) {
			selected_line = i;
			break;
		}
	}
	else selected_line = -1;
	
	if (prev_sel >= 0 && prev_sel < lines.GetCount())
		lines[prev_sel].Refresh();
	if (selected_line >= 0 && selected_line < lines.GetCount())
		lines[selected_line].Refresh();
}


void PartContentCtrl::MoveSelection(int diff) {
	int prev_sel = selected_line;
	selected_line += diff;
	selected_line = max(0, min(lines.GetCount()-1, selected_line));
	if (prev_sel >= 0 && prev_sel < lines.GetCount())
		lines[prev_sel].Refresh();
	if (selected_line >= 0 && selected_line < lines.GetCount()) {
		lines[selected_line].Refresh();
	}
	WhenCursor();
}

void PartContentCtrl::InitDefault(PartLineCtrl& l) {
	/*AddElements(l.element);
	
	int appmode = o.GetAppMode();
	DatabaseBrowser& b = DatabaseBrowser::Single(this->o.GetAppMode());
	
	if (l.attr.GetCount() == 0) {
		for(int i = 0; i < b.attrs.GetCount(); i++) {
			const auto& ah = b.attrs[i];
			l.attr.Add(Capitalize(ah.group) + " / " + Capitalize(ah.value));
		}
	}
	
	if (l.clr.GetCount() == 0) {
		for(int i = 0; i < b.colors.GetCount(); i++) {
			const auto& c = b.colors[i];
			AttrText at(c.name);
			at	.NormalPaper(c.clr).NormalInk(Black())
				.Paper(Blend(GrayColor(), GetGroupColor(i))).Ink(White());
			l.clr.Add(at);
		}
	}
	
	if (l.action.GetCount() == 0) {
		for(int i = 0; i < b.actions.GetCount(); i++) {
			l.action.Add(b.actions[i].action);
		}
		PartLineCtrl* p = &l;
		l.action.WhenAction = [this,p]{
			OnLineValueChange(p);
			DataSelAction(p);
		};
	}
	
	
	if (l.typeclass.GetCount() == 0) {
		l.typeclass.Add("");
		const auto& tcs = GetTypeclasses(appmode);
		for(int i = 0; i < tcs.GetCount(); i++) {
			l.typeclass.Add(tcs[i]);
		}
	}
	
	if (l.content.GetCount() == 0) {
		l.content.Add("");
		const auto& cons = GetContents(appmode);
		for(int i = 0; i < cons.GetCount(); i++) {
			l.content.Add(cons[i].key);
		}
	}
	
	l.attr			<<= THISBACK1(OnLineValueChange, &l);
	l.clr			<<= THISBACK1(OnLineValueChange, &l);
	//NO l.action		<<= THISBACK1(OnLineValueChange, &l);
	l.action_arg	<<= THISBACK1(OnLineValueChange, &l);
	l.typeclass		<<= THISBACK1(OnLineValueChange, &l);
	l.content		<<= THISBACK1(OnLineValueChange, &l);
	*/
}

void PartContentCtrl::DataLine(PartLineCtrl& pl) {
	if (!o) return;
	auto& o = *this->o;
	DatasetPtrs p; o.GetDataset(p);
	if (!p.script || !o.parts.IsCursor())
		return;
	DatabaseBrowser& b = DatabaseBrowser::Single();
	
	LyricalStructure& l = *p.lyric_struct;
	int part_i = o.parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	
	LineElement* el = 0;
	if (pl.sub_i < 0 && pl.line_i < 0) {
		el = &dp.el;
	}
	else if (pl.sub_i >= 0) {
		DynSub& ds = dp.sub[pl.sub_i];
		if (pl.line_i < 0) {
			el = &ds.el;
		}
		else {
			DynLine& dl = ds.lines[pl.line_i];
			el = &dl.el;
		}
	}
	if (!el)
		return;
	
	// Set attr
	/*int attr_i = pl.attr.GetIndex();
	if (attr_i <= 0) {
		el->attr.group = "";
		el->attr.value = "";
	}
	else {
		el->attr = b.attrs[attr_i].GetAttrHeader();
	}
	
	#define SET_INDEX(IDX, VAL) if (VAL+1 >= 0 && VAL+1 < IDX.GetCount()) IDX.SetIndex(VAL+1)

	// Set color
	SET_INDEX(pl.clr, el->clr_i);
	
	// Set action group
	int action_i = b.FindAction(el->act.action);
	SET_INDEX(pl.action, action_i);
	DataSelAction(&pl);
	int arg_i = b.FindArg(el->act.arg);
	SET_INDEX(pl.action_arg, arg_i);
	SET_INDEX(pl.typeclass, el->typeclass_i);
	SET_INDEX(pl.content, el->content_i);
	#undef SET_INDEX*/
}

void PartContentCtrl::OnLineValueChange(PartLineCtrl* l_) {
	if (!o) return;
	auto& o = *this->o;
	PartLineCtrl& pl = *l_;
	DatabaseBrowser& b = DatabaseBrowser::Single();
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.script || !o.parts.IsCursor())
		return;
	
	LyricalStructure& l = *p.lyric_struct;
	int part_i = o.parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	
	LineElement* el = 0;
	if (pl.sub_i < 0 && pl.line_i < 0) {
		el = &dp.el;
	}
	else if (pl.sub_i >= 0) {
		DynSub& ds = dp.sub[pl.sub_i];
		if (pl.line_i < 0) {
			el = &ds.el;
		}
		else {
			DynLine& dl = ds.lines[pl.line_i];
			el = &dl.el;
		}
	}
	if (!el)
		return;
	
	// set attr
	/*int attr_i = pl.attr.GetIndex();
	if (attr_i <= 0)
		el->attr = AttrHeader();
	else
		el->attr = b.attrs[attr_i].GetAttrHeader();
	
	// Set action
	int act_i = pl.action.GetIndex();
	if (act_i <= 0)
		el->act.action = "";
	else
		el->act.action = b.actions[act_i].action;
	
	// Set action arg
	int arg_i = pl.action_arg.GetIndex();
	if (arg_i <= 0)
		el->act.arg = "";
	else
		el->act.arg = b.args[arg_i].arg;
	
	#define SET_INDEX(IDX, VAL) VAL = IDX.GetIndex()-1;
	SET_INDEX(pl.clr, el->clr_i);
	SET_INDEX(pl.typeclass, el->typeclass_i);
	SET_INDEX(pl.content, el->content_i);
	#undef SET_INDEX*/
}

void PartContentCtrl::DataSelAction(PartLineCtrl* l_) {
	DatabaseBrowser& b = DatabaseBrowser::Single();
	PartLineCtrl& l = *l_;
	
	
	/*b.SetGroup(l.action.GetIndex());
	
	l.action_arg.Clear();
	l.action_arg.Add("");
	for(int i = 0; i < b.actions.GetCount(); i++) {
		l.action_arg.Add(b.actions[i].action);
	}
	*/
}

void PartContentCtrl::Data() {
	if (!o) return;
	auto& o = *this->o;
	DatasetPtrs p; o.GetDataset(p);
	if (!p.script || !o.parts.IsCursor())
		return;
	
	LyricalStructure& l = *p.lyric_struct;
	int part_i = o.parts.GetCursor();
	const DynPart& dp = l.parts[part_i];
	
	for (auto& e : lines)
		RemoveChild(&e);
	lines.Clear();
	
	int h = 0;
	
	{
		PartLineCtrl& line = lines.Add(new PartLineCtrl(*this));
		Add(line);
		line.sub_i = -1;
		
		InitDefault(line);
		//line.element.SetIndex(FindElement(dp.element)+1);
		//line.element <<= THISBACK3(OnElementChange, -1, -1, &line.element);
		
		h += lh;
	}
	
	for(int i = 0; i < dp.sub.GetCount(); i++) {
		const DynSub& ds = dp.sub[i];
		{
			PartLineCtrl& line = lines.Add(new PartLineCtrl(*this));
			Add(line);
			line.sub_i = i;
			
			InitDefault(line);
			//line.element.SetIndex(FindElement(ds.element0)+1);
			//line.element <<= THISBACK3(OnElementChange, i, -1, &line.element);
			
			h += lh;
		}
		
		for(int j = 0; j < ds.lines.GetCount(); j++) {
			const DynLine& dl = ds.lines[j];
			PartLineCtrl& line = lines.Add(new PartLineCtrl(*this));
			Add(line);
			line.sub_i = i;
			line.line_i = j;
			
			InitDefault(line);
			//line.element.SetIndex(FindElement(dl.element)+1);
			//line.element <<= THISBACK3(OnElementChange, i, j, &line.element);
			h += lh*2;
		}
	}
	
	for(int i = 0; i < lines.GetCount(); i++) {
		DataLine(lines[i]);
	}
	
	scroll.SetTotal(h);
	scroll.SetPage(GetSize().cy);
	
	Layout();
}

void PartContentCtrl::AddElements(DropList& dl) {
	if (!o) return;
	auto& o = *this->o;
	DatasetPtrs p; o.GetDataset(p);
	ASSERT(p.src);
	auto& src = p.src->Data();
	if (element_keys.IsEmpty()) {
		const auto& el = src.element_keys.GetKeys();
		element_keys <<= el;
		Sort(element_keys, StdLess<String>());
	}
	const auto& el = element_keys;
	
	dl.Add("");
	for (const auto& e : el)
		dl.Add(Capitalize(e));
}

int PartContentCtrl::FindElement(const String& s) {
	const Vector<String>& el = element_keys;// GetElements();
	String e = ToLower(s);
	for(int i = 0; i < el.GetCount(); i++) {
		if (el[i] == e)
			return i;
	}
	return -1;
}

void PartContentCtrl::OnElementChange(int sub_i, int line_i, DropList* dl) {
	if (!o) return;
	auto& o = *this->o;
	DatasetPtrs p; o.GetDataset(p);
	if (!p.script || !o.parts.IsCursor())
		return;
	LyricalStructure& l = *p.lyric_struct;
	int part_i = o.parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	
	if (sub_i < 0) {
		dp.el.element = ToLower((String)dl->GetKey(dl->GetIndex()));
	}
	else {
		DynSub& ds = dp.sub[sub_i];
		if (line_i < 0)
			ds.el.element = ToLower((String)dl->GetKey(dl->GetIndex()));
		else
			ds.lines[line_i].el.element = ToLower((String)dl->GetKey(dl->GetIndex()));
	}
}

PartLineCtrl* PartContentCtrl::GetActiveLine() {
	if (selected_line >= 0 && selected_line < lines.GetCount())
		return &lines[selected_line];
	return 0;
}













void PartLineCtrl::PaintTextBlock(Draw& d, int& x, int off, Rect& out, Color bg, const String& txt, const Font& fnt) {
	if (txt == "All" || txt == "-1" || txt.IsEmpty())
		return;
	
	Size sz = GetTextSize(txt, fnt);
	sz.cx += off * 2;
	sz.cx = max(sz.cx, 30);
	Rect r = RectC(x,1,sz.cx,sz.cy-1);
	d.DrawRect(r, bg);
	
	Color border = Blend(bg, Black());
	d.DrawLine(x,1, x+sz.cx-1,1, 1, border);
	d.DrawLine(x,sz.cy, x+sz.cx-1,sz.cy, 1, border);
	d.DrawLine(x,1, x,sz.cy-1, 1, border);
	d.DrawLine(x+sz.cx-1,1, x+sz.cx-1,sz.cy, 1, border);
	
	d.DrawText(x+off,1, txt, fnt, Black());
	x += sz.cx + 5;
}

void PartLineCtrl::MouseWheel(Point p, int zdelta, dword keyflags) {
	o.scroll.Wheel(zdelta);
}

void PartLineCtrl::LeftDown(Point p, dword keyflags) {
	Select();
	//Refresh();
	SetFocus();
}

void PartLineCtrl::Select() {
	o.Select(this);
	Refresh();
	o.WhenCursor();
}

void PartLineCtrl::GotFocus() {
	Select();
}

void PartLineCtrl::LostFocus() {
	Refresh();
}

bool PartLineCtrl::Key(dword key, int count) {
	
	if (key == K_UP) {
		o.MoveSelection(-1);
		return true;
	}
	if (key == K_DOWN) {
		o.MoveSelection(+1);
		return true;
	}
	
	return false;
}

bool PartLineCtrl::IsSelected() const {
	return o.GetCursor() == o.Find(this);
}

LineElement* PartLineCtrl::GetLineEl() const {
	if (!o.o) return 0;
	auto& o = *this->o.o;
	DatasetPtrs p; o.GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	int part_i = o.parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	LineElement* el = 0;
	if (sub_i < 0 && line_i < 0) {
		String text = "P";
		el = &dp.el;
	}
	else if (sub_i >= 0 && line_i < 0) {
		DynSub& ds = dp.sub[sub_i];
		el = &ds.el;
	}
	else if (sub_i >= 0 && line_i >= 0) {
		DynSub& ds = dp.sub[sub_i];
		DynLine& dl = ds.lines[line_i];
		el = &dl.el;
	}
	return el;
}

DynLine* PartLineCtrl::GetDynLine() const {
	if (!o.o) return 0;
	auto& o = *this->o.o;
	DatasetPtrs p; o.GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	int part_i = o.parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	if (sub_i >= 0 && line_i >= 0) {
		DynSub& ds = dp.sub[sub_i];
		DynLine& dl = ds.lines[line_i];
		return &dl;
	}
	return 0;
}


END_UPP_NAMESPACE
