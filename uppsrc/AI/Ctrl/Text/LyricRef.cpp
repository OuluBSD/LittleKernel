#include "Text.h"


NAMESPACE_UPP


ScriptPhrasePartsGroups::ScriptPhrasePartsGroups(ComponentCtrl& o) : o(o) {
	Add(mode.TopPos(0,30).HSizePos());
	Add(hsplit.VSizePos(30,0).HSizePos());
	
	sort.SetCount(DatabaseBrowser::TYPE_COUNT, -1);
	
	mode.Add("Any");
	for(int i = 0; i < DatabaseBrowser::MODE_COUNT; i++) {
		mode.Add(DatabaseBrowser::GetModeString(i));
	}
	mode.SetIndex(0);
	
	hsplit.Horz() << vsplit << parts;
	hsplit.SetPos(3000);

	vsplit.Vert();

	InitArray(attr_groups, "Attr Group", DatabaseBrowser::ATTR_GROUP);
	InitArray(attr_values, "Attr Value", DatabaseBrowser::ATTR_VALUE);
	InitArray(colors, "Color", DatabaseBrowser::COLOR);
	InitArray(actions, "Action", DatabaseBrowser::ACTION);
	InitArray(action_args, "Arg", DatabaseBrowser::ACTION_ARG);
	InitArray(elements, "Element", DatabaseBrowser::ELEMENT);
	InitArray(typeclasses, "Typeclass", DatabaseBrowser::TYPECLASS);
	InitArray(contents, "Content", DatabaseBrowser::CONTENT);
	
	parts.AddColumn(t_("Phrase"));
	parts.AddColumn(t_("Actions"));
	parts.AddColumn(t_("Group"));
	parts.AddColumn(t_("Value"));
	parts.AddColumn(t_("Element"));
	//parts.AddColumn(t_("Scores")).SetDisplay(Single<ScoreDisplay>());
	parts.AddColumn(t_("Score-sum"));
	parts.AddIndex("IDX");
	parts.ColumnWidths("16 8 6 6 6 3");
	parts.WhenBar << [this](Bar& bar){
		bar.Add("Copy", [this]() {
			int i = parts.GetCursor();
			AttrText text = parts.Get(i, 3);
			String s = text.text.ToString();
			WriteClipboardText(s);
		});
	};
	
	PostCallback([this]{
		DatabaseBrowser& b = DatabaseBrowser::Single();
		DatasetPtrs p; this->o.GetDataset(p);
		if (!p.src)
			return;
		b.SetCtrl(this->o);
		b.SetMode(p, 0);
		Data();
	});
}

hash_t ScriptPhrasePartsGroups::GetModeHash() const {
	if (mode.GetCount() == 0)
		return 0;
	int idx = mode.GetIndex()-1;
	return DatabaseBrowser::GetModeHash(idx);
}

void ScriptPhrasePartsGroups::UpdateNavigator() {
	vsplit.Clear();
	DatabaseBrowser& b = DatabaseBrowser::Single();
	int mode = b.GetMode();
	for(int i = 0; i < DatabaseBrowser::TYPE_COUNT; i++) {
		DatabaseBrowser::ColumnType t = b.GetOrder(i);
		switch (t) {
			case DatabaseBrowser::ELEMENT:
				vsplit << elements;
				break;
			case DatabaseBrowser::ATTR_GROUP:
				vsplit << attr_groups;
				break;
			case DatabaseBrowser::ATTR_VALUE:
				vsplit << attr_values;
				break;
			case DatabaseBrowser::COLOR:
				vsplit << colors;
				break;
			case DatabaseBrowser::ACTION:
				vsplit << actions;
				break;
			case DatabaseBrowser::ACTION_ARG:
				vsplit << action_args;
				break;
			case DatabaseBrowser::TYPECLASS:
				vsplit << typeclasses;
				break;
			case DatabaseBrowser::CONTENT:
				vsplit << contents;
				break;
			default:
				break;
		}
	}
}

void ScriptPhrasePartsGroups::InitArray(ArrayCtrl& arr, String title, DatabaseBrowser::ColumnType t) {
	arr.AddColumn(title);
	arr.AddColumn(t_("Count"));
	arr.AddIndex("IDX");
	arr.AddIndex("STR");
	arr.AddIndex("INT");
	arr.ColumnWidths("1 1");
	arr.WhenCursor << [this,&arr,t]() {
		if (!arr.IsCursor()) return;
		DatabaseBrowser& b = DatabaseBrowser::Single();
		b.ResetCursor(arr.Get("IDX"), t);
		PostCallback(THISBACK(Data));
		WhenBrowserCursor();
	};
	arr.WhenBar << [this,&arr,title](Bar& b) {
		b.Add("Sort by " + ToLower(title), [this,&arr]{INHIBIT_CURSOR(arr); arr.SetSortColumn(0,false); sort[0] = 0;});
		b.Add("Sort by count", [this,&arr]{INHIBIT_CURSOR(arr); arr.SetSortColumn(1,true); sort[0] = 1;});
	};
}

void ScriptPhrasePartsGroups::SetModeCursor(int i) {
	INHIBIT_ACTION(mode);
	mode.SetIndex(i);
}

void ScriptPhrasePartsGroups::Data() {
	UpdateNavigator();
	
	DatabaseBrowser& b = DatabaseBrowser::Single();
	
	
	//INHIBIT_ACTION(mode);
	//auto* line = o.content.GetActiveLine();
	//mode.SetIndex(1 + b.GetMode());
	
	FillArrayCtrl(DatabaseBrowser::ATTR_GROUP, attr_groups);
	FillArrayCtrl(DatabaseBrowser::ATTR_VALUE, attr_values);
	FillArrayCtrlColor(DatabaseBrowser::COLOR, colors);
	FillArrayCtrl(DatabaseBrowser::ACTION, actions);
	FillArrayCtrl(DatabaseBrowser::ACTION_ARG, action_args);
	FillArrayCtrl(DatabaseBrowser::ELEMENT, elements);
	FillArrayCtrl(DatabaseBrowser::TYPECLASS, typeclasses);
	FillArrayCtrl(DatabaseBrowser::CONTENT, contents);
	
	
	DataList();
}

void ScriptPhrasePartsGroups::FillArrayCtrlColor(DatabaseBrowser::ColumnType t, ArrayCtrl& arr) {
	DatabaseBrowser& b = DatabaseBrowser::Single();
	const auto& v = b.Get(t);
	INHIBIT_CURSOR_(arr, c);
	for(int i = 0; i < v.GetCount(); i++) {
		const auto& clr = v[i];
		Color c = GetGroupColor(clr.idx);
		arr.Set(i, "IDX", i);
		arr.Set(i, "INT", clr.idx);
		arr.Set(i, 0,
			AttrText("#" + IntStr(clr.idx))
				.NormalPaper(c).NormalInk(Black())
				.Paper(Blend(GrayColor(), c)).Ink(White()));
		arr.Set(i, 1, clr.count);
	}
	arr.SetCount(v.GetCount());
	if (sort[1] == 0) arr.SetSortColumn(0, false);
	if (sort[1] == 1) arr.SetSortColumn(1, true);
	if (set_cursor) {
		int cur = b.GetColumnCursor(t);
		if (cur >= 0 && cur < arr.GetCount())
			SetIndexCursor(cur, arr);
	}
}

void ScriptPhrasePartsGroups::FillArrayCtrl(DatabaseBrowser::ColumnType t, ArrayCtrl& arr) {
	DatabaseBrowser& b = DatabaseBrowser::Single();
	const auto& v = b.Get(t);
	INHIBIT_CURSOR_(arr, ag);
	for(int i = 0; i < v.GetCount(); i++) {
		const auto& a = v[i];
		arr.Set(i, "IDX", i);
		arr.Set(i, "INT", a.idx);
		arr.Set(i, "STR", a.str);
		arr.Set(i, 0, a.str);
		arr.Set(i, 1, a.count);
	}
	arr.SetCount(v.GetCount());
	if (sort[1] == 0) arr.SetSortColumn(0, false);
	if (sort[1] == 1) arr.SetSortColumn(1, true);
	if (set_cursor) {
		int cur = b.GetColumnCursor(t);
		if (cur >= 0 && cur < arr.GetCount())
			SetIndexCursor(cur, arr);
	}
}

void ScriptPhrasePartsGroups::DataList() {
	DatabaseBrowser& b = DatabaseBrowser::Single();
	DatasetPtrs p; o.GetDataset(p);
	if (!p.src) return;
	auto& src = p.src->Data();

	int row = 0, max_rows = 10000;
	for(int i = 0; i < b.phrase_parts.GetCount(); i++) {
		int pp_i = b.phrase_parts[i];
		PhrasePart& pp = src.phrase_parts[pp_i];

		parts.Set(row, "IDX", pp_i);

		String phrase = src.GetWordString(pp.words);
		parts.Set(row, 0,
			AttrText(phrase)
				.NormalPaper(Blend(pp.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(pp.clr, GrayColor())).Ink(White())
			);

		parts.Set(row, 1, src.GetActionString(pp.actions));

		
		if (pp.attr >= 0) {
			const AttrHeader& ah = src.attrs.GetKey(pp.attr);
			parts.Set(row, 2, ah.group);
			parts.Set(row, 3, ah.value);
		}
		else {
			parts.Set(row, 2, Value());
			parts.Set(row, 3, Value());
		}
		
		
		parts.Set(row, 4, pp.el_i >= 0 ? src.element_keys[pp.el_i] : String());
		
		#if 0
		ValueArray va;
		int sum = 0;
		for(int i = 0; i < SCORE_COUNT; i++) {
			va.Add(pp.scores[i]);
			sum += pp.scores[i];
		}
		parts.Set(row, 4, va);
		#else
		int sum = 0;
		for(int i = 0; i < SCORE_COUNT; i++)
			sum += pp.scores[i];
		#endif
		parts.Set(row, 5, sum);

		/*parts.Set(row, 2,
			AttrText(ah.action)
				.NormalPaper(Blend(pp.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(pp.clr, GrayColor())).Ink(White())
			);
		parts.Set(row, 3,
			AttrText(ah.arg)
				.NormalPaper(Blend(pp.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(pp.clr, GrayColor())).Ink(White())
			);*/
		row++;

		if (row >= max_rows)
			break;
	}
	parts.SetCount(row);
	parts.SetSortColumn(5, true);
}

/*void ScriptPhrasePartsGroups::DoPhrases(int fn) {
	TextLib::TaskManager& tm = GetTaskManager();
	tm.DoPhrases(fn);
}*/

void ScriptPhrasePartsGroups::SetIndexCursor(int idx, ArrayCtrl& arr) {
	INHIBIT_CURSOR(arr);
	for(int i = 0; i < arr.GetCount(); i++) {
		if (arr.Get(i, "IDX") == idx) {
			arr.SetCursor(i);
			break;
		}
	}
}

void ScriptPhrasePartsGroups::UpdateCounts() {
	
	/*int count = da.phrase_parts.GetCount();
	int row = 0;
	for(int i = 0; i < count; i++) {
		PhrasePart& pp = da.phrase_parts[i];
		pp.profiles.Clear();
		pp.primary.Clear();
		pp.secondary.Clear();
	}*/
	/*for (ExportAction& ea : da.primaries.GetValues())
		ea.count = 0;
	for (ExportAttr& ea : da.typecasts.GetValues())
		ea.count = 0;
	
	for(int i = 0; i < da.phrase_parts.GetCount(); i++) {
		PhrasePart& pp = da.phrase_parts[i];
		
		for (int ah_i : pp.primaries) {
			ExportAction& ea = da.primaries[ah_i];
			ea.count++;
		}
		
		if (pp.attr >= 0) {
			ExportAttr& ea = da.typecasts[pp.attr];
			ea.count++;
		}
	}*/
}











ScriptReferenceMakerCtrl::ScriptReferenceMakerCtrl() : db0(*this), content(*this) {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << lsplit << db0;
	
	lsplit.Vert() << parts << split << content;
	lsplit.SetPos(3333);
	
	CtrlLayout(form);
	
	parts.AddColumn("Part");
	parts.AddColumn("Element");
	
	parts.WhenBar << [this](Bar& b) {
		b.Add("Add part", THISBACK1(Do, 0));
		if (parts.IsCursor())
			b.Add("Remove part", THISBACK1(Do, 1));
	};
	parts.WhenCursor << THISBACK(DataPart);
	
	split.Horz() << form;
	
	form.lines <<= THISBACK(OnValueChange);
	form.lines_per_sub <<= THISBACK(OnValueChange);
	form.num <<= THISBACK(OnValueChange);
	form.type <<= THISBACK(OnValueChange);
	form.make_lines << THISBACK(MakeLines);
	
	content.WhenCursor << THISBACK(DataLine);
	
	db0.WhenBrowserCursor << THISBACK(OnBrowserCursor);
	db0.mode.WhenAction << THISBACK(UpdateMode);
	
}

void ScriptReferenceMakerCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	if (!p.script || !p.lyric_struct) return;
	Script& s = *p.script;
	LyricalStructure& l = *p.lyric_struct;
	
	for(int i = 0; i < l.parts.GetCount(); i++) {
		const DynPart& p = l.parts[i];
		String name = p.GetName();
		parts.Set(i, 0, name);
		parts.Set(i, 1, Capitalize(p.el.element));
	}
	parts.SetCount(l.parts.GetCount());
	INHIBIT_CURSOR(parts);
	if (!parts.IsCursor() && parts.GetCount())
		parts.SetCursor(0);
	
	DataPart();
}

void ScriptReferenceMakerCtrl::DataPart() {
	DatasetPtrs p; GetDataset(p);
	if (!p.script || !p.lyric_struct || !parts.IsCursor())
		return;
	
	Script& s = *p.script;
	LyricalStructure& l = *p.lyric_struct;
	int part_i = parts.GetCursor();
	const DynPart& dp = l.parts[part_i];
	
	form.type.Clear();
	for(int i = 0; i < TXT_COUNT; i++) {
		form.type.Add(GetTextTypeString(i));
	}
	
	
	form.type.SetIndex((int)dp.text_type);
	form.num.SetData(dp.text_num+1);
	form.lines.SetData(dp.text_lines);
	form.lines_per_sub.SetData(dp.text_lines_per_sub);
	
	content.Data();
	db0.Data();
	DataLine();
}

#if 0
void ReadNavigatorState(LyricalStructure& s, int part_i, int sub_i, int line_i, NavigatorState& state, int depth_limit) {
	state.Clear();
	if (part_i < 0 || part_i >= s.parts.GetCount())
		return;
	
	DynPart& dp = s.parts[part_i];
	
	#define COPY(v)   if (state.v.IsEmpty()) state.v = el . v;
	#define COPY_I(v) if (state.v < 0) state.v = el . v;
	#define COPY_S(v) if (state.v == 0) state.v = el . v;
	LineElement* elp = 0;
	if (sub_i >= 0 && line_i >= 0 && depth_limit >= 2) {
		DynSub& ds = dp.sub[sub_i];
		DynLine& dl = ds.lines[line_i];
		auto& el = dl.el;
		COPY(element)
		COPY(attr.group)
		COPY(attr.value)
		COPY_I(clr_i)
		COPY(act.action)
		COPY(act.arg)
		COPY_I(typeclass_i)
		COPY_I(con_i)
		COPY_S(sorter)
		if (state.el == 0) {state.depth = 2; state.el = &el;}
	}
	if (sub_i >= 0 && depth_limit >= 1) {
		DynSub& ds = dp.sub[sub_i];
		auto& el = ds.el;
		COPY(element)
		COPY(attr.group)
		COPY(attr.value)
		COPY_I(clr_i)
		COPY(act.action)
		COPY(act.arg)
		COPY_I(typeclass_i)
		COPY_I(con_i)
		COPY_S(sorter)
		if (state.el == 0) {state.depth = 1; state.el = &el;}
	}
	if (depth_limit >= 0) {
		auto& el = dp.el;
		COPY(element)
		COPY(attr.group)
		COPY(attr.value)
		COPY_I(clr_i)
		COPY(act.action)
		COPY(act.arg)
		COPY_I(typeclass_i)
		COPY_I(con_i)
		COPY_S(sorter)
		if (state.el == 0) {state.depth = 0; state.el = &el;}
	}
	#undef COPY
	#undef COPY_I
	#undef COPY_S
}
#endif

void ScriptReferenceMakerCtrl::ReadNavigatorState(NavigatorState& state, int depth_limit) {
	state.Clear();
	
	DatasetPtrs p; GetDataset(p);
	if (!p.script || !p.lyric_struct || !parts.IsCursor())
		return;
	
	LyricalStructure& l = *p.lyric_struct;
	int part_i = parts.GetCursor();
	int line_i = content.GetCursor();
	if (line_i < 0 || line_i >= content.lines.GetCount())
		return;
	
	PartLineCtrl& pl = content.Get(line_i);
	
	DynPart& dp = l.parts[part_i];
	if (line_i >= content.GetLineCount())
		return;
	
	::ReadNavigatorState(l, part_i, pl.sub_i, pl.line_i, state, depth_limit);
	
	state.line = &pl;
}

void ScriptReferenceMakerCtrl::DataLine() {
	NavigatorState s;
	ReadNavigatorState(s);
	
	int mode_cursor = 1 + DatabaseBrowser::FindMode(s.el ? s.el->sorter : 0);
	db0.SetModeCursor(mode_cursor);
	
	DatabaseBrowser& b = DatabaseBrowser::Single();
	DatasetPtrs p; GetDataset(p);
	b.SetAll(p, s.sorter, s.element, s.attr, s.clr_i, s.act, s.typeclass_i, s.con_i);
		
	db0.Data();
}

void ScriptReferenceMakerCtrl::MakeLines() {
	DatasetPtrs p; GetDataset(p);
	if (!p.script || !p.lyric_struct || !parts.IsCursor())
		return;
	
	Script& s = *p.script;
	LyricalStructure& l = *p.lyric_struct;
	int part_i = parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	
	if (dp.text_lines_per_sub <= 0)
		dp.text_lines_per_sub = 1;
	
	int sub_count = max(0, (dp.text_lines + dp.text_lines_per_sub - 1) / dp.text_lines_per_sub);
	dp.sub.SetCount(sub_count);
	for(int i = 0; i < dp.sub.GetCount(); i++) {
		DynSub& ds = dp.sub[i];
		ds.lines.SetCount(dp.text_lines_per_sub);
	}
	
	PostCallback(THISBACK(DataPart));
}

void ScriptReferenceMakerCtrl::OnBrowserCursor() {
	NavigatorState s;
	ReadNavigatorState(s); // Get current state
	
	if (!s.el || !s.line)
		return;
	auto& el = *s.el;
	auto& line = *s.line;
	
	ReadNavigatorState(s, s.depth-1); // Get inherited state only
	
	
	// Compare to inherited state, and clear value, if same as inherited
	#define ITEM_STR(x, arr) {String str = db0.arr.Get("STR"); el.x = str == s.x ? String() : str;}
	#define ITEM_INT(x, arr) {int i = db0.arr.Get("INT"); el.x = i == s.x ? -1 : i;}
	
	{
		hash_t sorter = db0.GetModeHash();
		el.sorter = sorter == s.sorter ? 0 : sorter;
	}
	
	if (db0.elements.IsCursor()) {
		ITEM_STR(element, elements)
	}
	if (db0.attr_groups.IsCursor()) {
		ITEM_STR(attr.group, attr_groups)
	}
	if (db0.attr_values.IsCursor()) {
		ITEM_STR(attr.value, attr_values)
	}
	if (db0.colors.IsCursor()) {
		ITEM_INT(clr_i, colors);
	}
	if (db0.actions.IsCursor()) {
		ITEM_STR(act.action, actions)
	}
	if (db0.action_args.IsCursor()) {
		ITEM_STR(act.arg, action_args)
	}
	if (db0.typeclasses.IsCursor()) {
		ITEM_INT(typeclass_i, typeclasses);
	}
	if (db0.contents.IsCursor()) {
		ITEM_INT(con_i, contents);
	}
	
	
	line.Refresh();
}

void ScriptReferenceMakerCtrl::OnValueChange() {
	DatasetPtrs p; GetDataset(p);
	if (!p.script || !p.lyric_struct || !parts.IsCursor())
		return;
	
	Script& s = *p.script;
	LyricalStructure& l = *p.lyric_struct;
	int part_i = parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	
	dp.text_num = (int)form.num.GetData() - 1;
	dp.text_lines = form.lines.GetData();
	dp.text_lines_per_sub = form.lines_per_sub.GetData();
	dp.text_type = (TextPartType)form.type.GetIndex();
	
	
	String name = dp.GetName();
	parts.Set(0, name);
	parts.Set(1, Capitalize(dp.el.element));
}

void ScriptReferenceMakerCtrl::ToolMenu(Bar& bar) {
	/*bar.Add(t_("Jump to previous group value"), MetaImgs::BlueRing(), [this](){
		int tab = tabs.Get();
		if (tab == 0) db0.JumpToGroupValue(-1);
	}).Key(K_CTRL_W);
	bar.Add(t_("Jump to next group value"), MetaImgs::BlueRing(), [this](){
		int tab = tabs.Get();
		if (tab == 0) db0.JumpToGroupValue(+1);
	}).Key(K_CTRL_E);
	bar.Separator();*/
	bar.Add(t_("Set as line text"), MetaImgs::BlueRing(), THISBACK(SetLineText)).Key(K_F4);
	
}

void ScriptReferenceMakerCtrl::SetLineText() {
	DatabaseBrowser& b = DatabaseBrowser::Single();
	
	if (!db0.parts.IsCursor())
		return;
	int line_i = content.GetCursor();
	if (line_i < 0 || line_i >= content.lines.GetCount())
		return;
	auto& line = content.Get(line_i);
	
	DynLine* dl = line.GetDynLine();
	if (!dl)
		return;
	AttrText at = db0.parts.Get(0);
	dl->text = at.text.ToString();
	line.Refresh();
}

void ScriptReferenceMakerCtrl::Do(int fn) {
	DatasetPtrs p; GetDataset(p);
	
	// Add part
	if (fn == 0) {
		if (!p.lyric_struct) return;
		LyricalStructure& l = *p.lyric_struct;
		l.parts.Add();
		PostCallback(THISBACK(Data));
	}
	// Remove part
	if (fn == 1) {
		if (!p.lyric_struct || !parts.IsCursor()) return;
		LyricalStructure& l = *p.lyric_struct;
		int part_i = parts.GetCursor();
		if (part_i >= 0 && part_i < l.parts.GetCount()) {
			l.parts.Remove(part_i);
			PostCallback(THISBACK(Data));
		}
	}
	
}

void ScriptReferenceMakerCtrl::UpdateMode() {
	DatabaseBrowser& b = DatabaseBrowser::Single();
	DatasetPtrs p; GetDataset(p);
	if (!p.src)
		return;
	b.SetMode(p, GetActiveMode());
	b.ResetCursor();
	db0.Data();
	db0.WhenBrowserCursor(); // OnBrowserCursor
}

int ScriptReferenceMakerCtrl::GetActiveMode() {
	int idx = db0.mode.GetIndex()-1;
	if (idx >= 0)
		return idx;
	else
		return GetInheritedMode();
}

int ScriptReferenceMakerCtrl::GetInheritedMode() {
	PartLineCtrl* plc_ = content.GetActiveLine();
	if (!plc_)
		return -1;
	PartLineCtrl& plc = *plc_;
	DatasetPtrs p; GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	int part_i = parts.GetCursor();
	DynPart& dp = l.parts[part_i];
	LineElement* el = 0;
	if (plc.sub_i >= 0 && plc.line_i >= 0) {
		DynSub& ds = dp.sub[plc.sub_i];
		DynLine& dl = ds.lines[plc.line_i];
		if (dl.el.sorter != 0)
			el = &dl.el;
	}
	if (!el && plc.sub_i >= 0) {
		DynSub& ds = dp.sub[plc.sub_i];
		if (ds.el.sorter != 0)
			el = &ds.el;
	}
	if (!el) {
		
		if (dp.el.sorter != 0)
			el = &dp.el;
	}
	if (!el)
		return -1;
	int mode =  DatabaseBrowser::FindMode(el->sorter);
	return mode;
}

void ScriptReferenceMakerCtrl::SetFont(Font fnt) {
	content.SetFont(fnt);
}















PartLineCtrl::PartLineCtrl(PartContentCtrl& o) : o(o) {
	NoWantFocus();
	
}

const int PartLineCtrl::indent = 30;


void PartLineCtrl::Paint(Draw& d) {
	Size sz = GetSize();
	Font mono = o.fnt;
	int fnt_h = mono.GetHeight();
	Font sans = SansSerif(fnt_h);
	bool focused = IsSelected();
	int off = 2;
	
	if (!this->o.o) {
		d.DrawRect(sz, White());
		d.DrawText(5,5,"TODO",StdFont(),Black());
	}
	auto& o = *this->o.o;
	
	// Header
	Color part_bg_clr = White();
	Color sub_bg_clr = Color(225, 227, 255);
	Color line_bg_clr = Color(228, 255, 227);
	Color rest_bg_clr = GrayColor(256-16);
	if (focused) {
		int alpha = 128-64-32;
		part_bg_clr = Blend(part_bg_clr, LtRed(), alpha);
		sub_bg_clr = Blend(sub_bg_clr, LtRed(), alpha);
		line_bg_clr = Blend(line_bg_clr, LtRed(), alpha);
		rest_bg_clr = Blend(rest_bg_clr, LtRed(), alpha);
	}
	
	Rect part_bg_r = RectC(0,0,indent,sz.cy);
	Rect sub_bg_r = RectC(indent,0,indent,sz.cy);
	Rect line_bg_r = RectC(indent*2,0,indent,sz.cy);
	Rect rest_bg_r = RectC(indent*3,0,sz.cx-3*indent,sz.cy);
	d.DrawRect(part_bg_r, part_bg_clr);
	if (sub_i >= 0) d.DrawRect(sub_bg_r, sub_bg_clr);
	if (line_i >= 0) d.DrawRect(line_bg_r, line_bg_clr);
	
	// Line texts
	DatasetPtrs p; o.GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	int part_i = o.parts.GetCursor();
	if (part_i < 0)
		return;
	
	DynPart& dp = l.parts[part_i];
	LineElement* el = 0;
	int left = 0;
	if (sub_i < 0 && line_i < 0) {
		String text = "P";
		d.DrawText(off,off,text,mono,Black());
		left = indent*1;
		el = &dp.el;
		d.DrawRect(rest_bg_r, rest_bg_clr);
	}
	else if (sub_i >= 0 && line_i < 0) {
		String text = IntStr(sub_i);
		d.DrawText(indent+off,off,text,mono,Black());
		left = indent*2;
		DynSub& ds = dp.sub[sub_i];
		el = &ds.el;
		d.DrawRect(rest_bg_r, rest_bg_clr);
	}
	else if (sub_i >= 0 && line_i >= 0) {
		String text = IntStr(line_i);
		d.DrawText(indent*2+off,off,text,mono,Black());
		left = indent*3;
		DynSub& ds = dp.sub[sub_i];
		DynLine& dl = ds.lines[line_i];
		el = &dl.el;
		
		rest_bg_clr = Blend(
			el->clr_i >= 0 && el->clr_i < GetColorGroupCount() ? GetGroupColor(el->clr_i) : rest_bg_clr,
			rest_bg_clr,
			256-64);
		d.DrawRect(rest_bg_r, rest_bg_clr);
		
		int y_2 = sz.cy / 2;
		d.DrawText(left+off, y_2+off, dl.text, sans, Black());
	}
	
	if (el) {
		off = 5;
		int x = left;
		#define RAND_CLR Blend(White(), Color(Random(256),Random(256),Random(256)),128+64)
		Rect r;
		Color element_clr, attr_group_clr, attr_value_clr, clr_clr, act_action_clr, act_arg_clr, typeclass_clr, content_clr;
		element_clr = Color(255, 205, 175);
		attr_group_clr = Color(206, 229, 201);
		attr_value_clr = Color(217, 241, 211);
		clr_clr = el->clr_i >= 0 && el->clr_i < GetColorGroupCount() ? GetGroupColor(el->clr_i) : Color(231, 221, 231);
		act_action_clr = Color(204, 227, 235);
		act_arg_clr = Color(222, 242, 249);
		typeclass_clr = Color(28, 212, 150);
		content_clr = Color(198, 85, 150);
		int mode = DatabaseBrowser::FindMode(el->sorter);
		String tc  = el->typeclass_i >= 0 ? o.GetTypeclasses()[el->typeclass_i] : String();
		int con_i = el->con_i / 3;
		int con_mod = el->con_i % 3;
		String con = el->con_i >= 0 ?
			o.GetContents()[con_i].key + ": " + o.GetContents()[con_i].parts[con_mod] : String();
		if (mode < 0) {
			PaintTextBlock(d, x, off, r, element_clr, el->element, sans);
			PaintTextBlock(d, x, off, r, attr_group_clr, el->attr.group, sans);
			PaintTextBlock(d, x, off, r, attr_value_clr, el->attr.value, sans);
			PaintTextBlock(d, x, off, r, clr_clr, IntStr(el->clr_i), sans);
			PaintTextBlock(d, x, off, r, act_action_clr, el->act.action, sans);
			PaintTextBlock(d, x, off, r, act_arg_clr, el->act.arg, sans);
			PaintTextBlock(d, x, off, r, typeclass_clr, tc, sans);
			PaintTextBlock(d, x, off, r, content_clr, con, sans);
		}
		else {
			Vector<String> parts = Split(DatabaseBrowser::GetModeKey(mode), "_");
			for (const String& p : parts) {
				if (p == "ELEMENT")
					PaintTextBlock(d, x, off, r, element_clr, el->element, sans);
				else if (p == "ATTR") {
					PaintTextBlock(d, x, off, r, attr_group_clr, el->attr.group, sans);
					PaintTextBlock(d, x, off, r, attr_value_clr, el->attr.value, sans);
				}
				else if (p == "COLOR") {
					PaintTextBlock(d, x, off, r, clr_clr, IntStr(el->clr_i), sans);
				}
				else if (p == "ACTION") {
					PaintTextBlock(d, x, off, r, act_action_clr, el->act.action, sans);
					PaintTextBlock(d, x, off, r, act_arg_clr, el->act.arg, sans);
				}
				else if (p == "TYPECLASS") {
					PaintTextBlock(d, x, off, r, typeclass_clr, tc, sans);
				}
				else if (p == "CONTENT") {
					PaintTextBlock(d, x, off, r, content_clr, con, sans);
				}
				else TODO
			}
		}
	}
	
	
	
	// Focus highlight
	if (focused) {
		Color c = LtRed();
		int y = sz.cy-1;
		d.DrawLine(0,0,sz.cx-1,0,1,Blend(c,White()));
		d.DrawLine(0,y,sz.cx-1,y,1,c);
	}
	else {
		// Top border
		d.DrawLine(0,0,sz.cx,0,1,White());
		d.DrawLine(0,sz.cy-1,sz.cx,sz.cy-1,1,GrayColor(128+64));
	}
}

INITIALIZER_COMPONENT_CTRL(Script, ScriptReferenceMakerCtrl)


END_UPP_NAMESPACE
