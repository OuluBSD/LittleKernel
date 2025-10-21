#include "Text.h"

NAMESPACE_UPP


ScriptTextSolverCtrl::ScriptTextSolverCtrl() {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << editor << tabs;
	
	editor.owner = this;
	editor.WhenCursor << THISBACK(OnEditorCursor);
	
	
	// Wizard tab
	tabs.Add(wizard_tab.SizePos(), "Wizard");
	
	
	// Suggestions -tab
	tabs.Add(sugg_tab.SizePos(), "Suggestions");
	sugg_tab.Add(sugg_split.VSizePos(0,30).HSizePos());
	sugg_tab.Add(sugg_prog.BottomPos(0,30).HSizePos(300));
	sugg_tab.Add(sugg_remaining.BottomPos(0,30).LeftPos(0,300));
	sugg_split.Vert() << sugg_list << sugg_lyrics;
	sugg_split.SetPos(2500);
	sugg_list.AddColumn("#");
	sugg_list.WhenCursor << [this]() {
		DatasetPtrs p; GetDataset(p);
		if (!sugg_list.IsCursor()) return;
		int i = sugg_list.GetCursor();
		Vector<String> lines = Split(p.lyrics->__suggestions[i], "\n");
		for(int i = 0; i < lines.GetCount(); i++) {
			sugg_lyrics.Set(i, 0, lines[i]);
		}
		sugg_lyrics.SetCount(lines.GetCount());
	};
	sugg_list.WhenBar << [this](Bar& b) {
		b.Add("Set source text", [this]() {
			DatasetPtrs p; GetDataset(p);
			if (!p.lyric_struct) return;
			LyricalStructure& l = *p.lyric_struct;
			Lyrics& ly = *p.lyrics;
			int i = sugg_list.GetCursor();
			l.SetText(ly.__suggestions[i], true);
			editor.ShowSourceText();
		});
	};
	sugg_lyrics.AddColumn("Txt");
	
	
	// Whole song -tab
	tabs.Add(whole_tab.SizePos(), "Whole song");
	
	whole_tab.Add(whole_split.VSizePos(0,30).HSizePos());
	whole_tab.Add(whole_prog.BottomPos(0,30).HSizePos(300));
	whole_tab.Add(whole_remaining.BottomPos(0,30).LeftPos(0,300));
	whole_prog.Set(0,1);
	
	whole_split.Vert() << whole_hsplit0 << whole_hsplit1;
	
	whole_hsplit0.Horz() << colors << attrs;
	whole_hsplit1.Horz() << actions << phrases;
	
	colors.AddColumn("Color");
	
	attrs.AddColumn("Group");
	attrs.AddColumn("Positive");
	attrs.AddColumn("Negative");
	
	actions.AddColumn("Action");
	actions.AddColumn("Arg");
	
	phrases.AddColumn("Phrase");
	phrases.SetLineCy(30);
	
	
	// Part -tab
	tabs.Add(part_tab.SizePos(), "Part");
	
	CtrlLayout(part_form);
	part_tab.Add(part_split.SizePos());
	part_split.Vert() << part_form << part_suggs;
	part_split.SetPos(2000);
	part_form.element <<= THISBACK(OnValueChange);
	part_form.text_type <<= THISBACK(OnValueChange);
	part_form.text_num <<= THISBACK(OnValueChange);
	part_form.do_story << THISBACK1(DoPart, 2);
	
	
	// Sub -tab
	tabs.Add(sub_tab.SizePos(), "Sub");
	CtrlLayout(sub_form);
	sub_tab.Add(sub_split.SizePos());
	sub_split.Vert() << sub_form << sub_suggs;
	sub_split.SetPos(2000);
	sub_form.do_story << THISBACK1(DoSub, 2);
	sub_form.element <<= THISBACK(OnValueChange);
	sub_form.story <<= THISBACK(OnValueChange);
	
	
	// Line -tab
	tabs.Add(line_tab.SizePos(), "Line");
	
	CtrlLayout(line_form);
	line_tab.Add(line_form.SizePos());
	line_form.split.Vert() << line_ref_lines << line_suggs;
	line_form.do_story << THISBACK1(DoLine, 2);
	line_form.do_suggs << THISBACK1(DoLine, 3);
	line_form.safety.Add("Safe");
	line_form.safety.Add("Unsafe");
	line_form.safety.WhenAction << THISBACK(OnValueChange);
	line_form.style_type.WhenAction << THISBACK(OnValueChange);
	line_form.style_entity.WhenAction << THISBACK(OnValueChange);
	line_form.length.Add("Long");
	line_form.length.Add("Medium");
	line_form.length.Add("Short");
	line_form.length.Add("Very short");
	line_form.length.WhenAction << THISBACK(OnValueChange);
	line_form.connector.Add("And then");
	line_form.connector.Add("therefore");
	line_form.connector.Add("but");
	line_form.connector.WhenAction << THISBACK(OnValueChange);
	line_form.begin.WhenAction << THISBACK(OnValueChange);
	line_form.copy_prev << THISBACK1(DoLine, 10);
	line_form.copy_clip << THISBACK1(DoLine, 11);
	line_form.paste_clip << THISBACK1(DoLine, 12);
	
	line_ref_lines.AddColumn("Selected");
	line_ref_lines.AddColumn("Line");
	line_ref_lines.AddColumn("Source line");
	line_ref_lines.ColumnWidths("1 6 6");
	
	line_suggs.AddColumn("#");
	line_suggs.AddColumn("Suggestion");
	line_suggs.ColumnWidths("1 7");
	
	tabs.WhenSet << THISBACK(Data);
	
}

void ScriptTextSolverCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Update Data"), MetaImgs::BlueRing(), THISBACK(Data)).Key(K_CTRL_Q);
	bar.Add(t_("Switch editor text"), MetaImgs::BlueRing(), THISBACK(SwitchEditorText)).Key(K_CTRL_W);
	bar.Add(t_("Copy from previous"), MetaImgs::BlueRing(), THISBACK1(Do, 10)).Key(K_CTRL_R);
	bar.Add(t_("Copy to clipboard"), MetaImgs::BlueRing(), THISBACK1(Do, 11)).Key(K_CTRL_T);
	bar.Add(t_("Copy from clipboard"), MetaImgs::BlueRing(), THISBACK1(Do, 12)).Key(K_CTRL_Y);
	bar.Separator();
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Fn 1"), MetaImgs::RedRing(), THISBACK1(Do, 2)).Key(K_F7);
	bar.Add(t_("Fn 2"), MetaImgs::RedRing(), THISBACK1(Do, 3)).Key(K_F8);
	bar.Add(t_("Fn 3"), MetaImgs::RedRing(), THISBACK1(Do, 4)).Key(K_F9);
	bar.Add(t_("Fn 4"), MetaImgs::RedRing(), THISBACK1(Do, 5)).Key(K_F10);
	bar.Add(t_("Fn 5"), MetaImgs::RedRing(), THISBACK1(Do, 6)).Key(K_F11);
}

void ScriptTextSolverCtrl::SwitchEditorText() {
	editor.SwitchTextSource();
}

void ScriptTextSolverCtrl::Do(int fn) {
	int tab = tabs.Get();
	if (tab == 1) DoSuggestions(fn);
	if (tab == 2) DoWhole(fn);
	if (tab == 3) DoPart(fn);
	if (tab == 4) DoSub(fn);
	if (tab == 5) DoLine(fn);
}

void ScriptTextSolverCtrl::DoSuggestions(int fn) {
	DatasetPtrs p; GetDataset(p);
	if (!p.entity) return;
	String ecs_path = p.entity->val.GetPath();
	ScriptSolver& sdi = ScriptSolver::Get(p, ecs_path);
	sugg_prog.Attach(sdi);
	sdi.WhenRemaining << [this](String s) {
		PostCallback([this,s](){sugg_remaining.SetLabel(s); Refresh();});
	};
	sdi.WhenReady << [this](){PostCallback(THISBACK(Data));};
	
	if (fn == 0)
		sdi.Start();
	else
		sdi.Stop();
}

void ScriptTextSolverCtrl::DoWhole(int fn) {
	DatasetPtrs p; GetDataset(p);
	if (fn >= 0 && fn <= 1) {
		/*ScriptGenerator& sdi = ScriptGenerator::Get(GetDataset());
		whole_prog.Attach(sdi);
		sdi.WhenRemaining << [this](String s) {
			PostCallback([this,s](){whole_remaining.SetLabel(s); Refresh();});
		};
		
		if (fn == 0)
			sdi.Start();
		else
			sdi.Stop();*/
	}
	else if (fn == 2) {
		WriteClipboardText(p.lyric_struct->GetStructText(0));
	}
}

void ScriptTextSolverCtrl::OnEditorCursor() {
	if (editor.selected_line == 0 &&
		editor.selected_sub == 0 &&
		editor.selected_part == 0) {
		tabs.Set(2);
	}
	else if (editor.selected_line == 0 &&
		editor.selected_sub == 0 &&
		editor.selected_part != 0) {
		tabs.Set(3);
	}
	else if (editor.selected_line == 0 &&
		editor.selected_sub != 0 &&
		editor.selected_part == 0) {
		tabs.Set(4);
	}
	else if (editor.selected_line != 0 &&
		editor.selected_sub == 0 &&
		editor.selected_part == 0) {
		tabs.Set(5);
	}
	Data();
}

void ScriptTextSolverCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	if (!p.script) return;
	editor.CheckClearSelected();
	
	if (!HasFocusDeep())
		editor.SetFocus();
	
	editor.Update();
	
	int tab = tabs.Get();
	if (tab == 1) DataSuggestions();
	if (tab == 2) DataWhole();
	if (tab == 3) DataPart();
	if (tab == 4) DataSub();
	if (tab == 5) DataLine();
	
}

void ScriptTextSolverCtrl::DataSuggestions() {
	DatasetPtrs p; GetDataset(p);
	Lyrics& l = *p.lyrics;
	
	for(int i = 0; i < l.__suggestions.GetCount(); i++) {
		sugg_list.Set(i, 0, i);
	}
	sugg_list.SetCount(l.__suggestions.GetCount());
	
}

void ScriptTextSolverCtrl::DataWhole() {
	DatasetPtrs p; GetDataset(p);
	if (!p.src) return;
	auto& src = p.src->Data();
	Script& s = *p.script;
	Lyrics& l = *p.lyrics;
	Entity& a = *p.entity;
	
	
	for(int i = 0; i < s.clr_list.GetCount(); i++) {
		int clr_i = s.clr_list[i];
		Color clr = GetGroupColor(clr_i);
		colors.Set(i, 0, AttrText("#" + IntStr(clr_i))
			.NormalPaper(clr).Paper(clr));
	}
	SetCountForArray(colors, s.clr_list.GetCount());
	
	
	int row = 0;
	for(int i = 0; i < s.simple_attrs.GetCount(); i++) {
		bool negative = s.simple_attrs[i];
		const ExportSimpleAttr& t = src.simple_attrs[i];
		String group = src.simple_attrs.GetKey(i);
		String attr_s0 = src.attrs.GetKey(t.attr_i0).value;
		String attr_s1 = src.attrs.GetKey(t.attr_i1).value;
		attrs.Set(row, 0, Capitalize(group));
		
		if (!negative)
			attrs.Set(row, 1, AttrText(Capitalize(attr_s0)).NormalPaper(Blend(LtGreen(), White())).Paper(Green()));
		else
			attrs.Set(row, 1, Capitalize(attr_s0));
		
		if (negative)
			attrs.Set(row, 2, AttrText(Capitalize(attr_s1)).NormalPaper(Blend(LtRed(), White())).Paper(Red()));
		else
			attrs.Set(row, 2, Capitalize(attr_s1));
		
		row++;
	}
	SetCountForArray(attrs, row);
	
	
	row = 0;
	for(int i = 0; i < s.actions_enabled.GetCount(); i++) {
		if (!s.actions_enabled[i])
			continue;
		const ActionHeader& ah = src.actions.GetKey(i);
		actions.Set(row, 0, ah.action);
		actions.Set(row, 1, ah.arg);
		row++;
	}
	SetCountForArray(actions, row);
	
	
	
	row = 0;
	for(int i = 0; i < PART_COUNT; i++) {
		for(int j = 0; j < s.phrase_parts[i].GetCount(); j++) {
			int pp_i = s.phrase_parts[i][j];
			const PhrasePart& pp = src.phrase_parts[pp_i];
			const TokenText& tt = src.token_texts[pp.tt_i];
			String s = src.GetTokenTextString(tt);
			s.Replace("/", "\n");
			phrases.Set(row, 0, s);
			row++;
		}
	}
	SetCountForArray(phrases, row);
	
	
}

void ScriptTextSolverCtrl::DataPart() {
	DatasetPtrs p; GetDataset(p);
	auto& src = p.src->Data();
	Script& s = *p.script;
	Lyrics& lyr = *p.lyrics;
	Entity& a = *p.entity;
	
	if (part_form.element.GetCount() == 0) {
		part_form.element.Add("");
		for(int i = 0; i < src.element_keys.GetCount(); i++) {
			part_form.element.Add(src.element_keys[i]);
		}
	}
	
	if (part_form.text_type.GetCount() == 0) {
		for(int i = 0; i < TXT_COUNT; i++) {
			part_form.text_type.Add(GetTextTypeString(i));
		}
	}
	
	if (editor.selected_part) {
		const DynPart& part = *editor.selected_part;
		int el_i = src.element_keys.Find(part.el.element) + 1;
		part_form.element.SetIndex(el_i);
		part_form.text_num.SetData(part.text_num+1);
		part_form.text_type.SetIndex((int)part.text_type);
		part_form.story.SetData(part.story);
	}
	
}

void ScriptTextSolverCtrl::DataSub() {
	DatasetPtrs p; GetDataset(p);
	auto& src = p.src->Data();
	Script& s = *p.script;
	Lyrics& lyr = *p.lyrics;
	Entity& a = *p.entity;
	
	if (sub_form.element.GetCount() == 0) {
		sub_form.element.Add("");
		for(int i = 0; i < src.element_keys.GetCount(); i++) {
			sub_form.element.Add(src.element_keys[i]);
		}
	}
	
	if (editor.selected_sub) {
		const DynSub& sub = *editor.selected_sub;
		int el_i = src.element_keys.Find(sub.el.element) + 1;
		sub_form.element.SetIndex(el_i);
		sub_form.story.SetData(sub.story);
	}
	
}

void ScriptTextSolverCtrl::DoPart(int fn) {
	DatasetPtrs p; GetDataset(p);
	ASSERT(p.entity);
	String ecs_path = p.entity->val.GetPath();
	
	const DynPart* part = 0;
	int part_i = -1;
	GetPart(&part, &part_i);
	
	if (fn == 2) {
		if (part == 0) return;
		
		ScriptSolver& sdi = ScriptSolver::Get(p, ecs_path);
		if (sdi.IsRunning()) {
			PromptOK("Wait until ScriptSolver has ended");
			return;
		}
		sdi.GetPartStory(part_i, [this](){
			PostCallback([this]() {
				editor.Refresh();
				DataPart();
			});
		});
	}
	
}

void ScriptTextSolverCtrl::DoSub(int fn) {
	DatasetPtrs p; GetDataset(p);
	ASSERT(p.entity);
	String ecs_path = p.entity->val.GetPath();
	const DynPart* part = 0;
	const DynSub* sub = 0;
	int part_i = -1, sub_i = -1;
	GetSub(&part, &sub, &part_i, &sub_i);
	
	if (fn == 2) {
		if (sub == 0) return;
		
		ScriptSolver& sdi = ScriptSolver::Get(p, ecs_path);
		if (sdi.IsRunning()) {
			PromptOK("Wait until ScriptSolver has ended");
			return;
		}
		sdi.GetSubStory(part_i, sub_i, [this](){
			PostCallback([this]() {
				editor.Refresh();
				DataSub();
			});
		});
	}
	
}

void ScriptTextSolverCtrl::DoLine(int fn) {
	DatasetPtrs p; GetDataset(p);
	ASSERT(p.entity && p.script);
	String ecs_path = p.entity->val.GetPath();
	const DynPart* part = 0;
	const DynSub* sub = 0;
	const DynLine* line = 0;
	int part_i = -1, sub_i = -1, line_i = -1;
	Vector<const DynLine*> g = GetLineGroup(&part, &sub, &line, &part_i, &sub_i, &line_i);
	LyricalStructure& l = *p.lyric_struct;
	
	if (fn == 2 || fn == 3 || fn == 5 || fn == 6) {
		if (g.IsEmpty()) return;
		
		ScriptSolver& sdi = ScriptSolver::Get(p, ecs_path);
		if (sdi.IsRunning()) {
			PromptOK("Wait until ScriptSolver has ended");
			return;
		}
		
		if (fn == 2) {
			sdi.GetExpanded(part_i, sub_i, line_i, [this](){
				PostCallback([this]() {
					editor.Refresh();
					DataLine();
				});
			});
		}
		else if (fn == 3) {
			sdi.GetSuggestions2(part_i, sub_i, g, [this](){
				PostCallback([this]() {
					editor.Refresh();
					DataLine();
				});
			});
		}
		else if (fn == 5) {
			sdi.GetSuggestions(*part, *sub, g, [this](){
				PostCallback([this]() {
					editor.Refresh();
					DataLine();
				});
			});
		}
		else if (fn == 6) {
			sdi.GetStyleSuggestion(part_i, sub_i, g, [this](){
				PostCallback([this]() {
					editor.Refresh();
					DataLine();
				});
			});
		}
	}
	else if (fn == 4) {
		if (!line_suggs.IsCursor()) return;
		int sugg_i = line_suggs.GetCursor();
		Vector<const DynLine*> g = GetLineGroup();
		for (const DynLine* l : g) {
			DynLine& dl = const_cast<DynLine&>(*l);
			if (sugg_i < dl.suggs.GetCount())
				dl.user_text = dl.suggs[sugg_i];
			else
				dl.user_text = "";
		}
		editor.ShowNormalText();
		DataLine();
	}
	// Copy prev
	else if (fn == 10) {
		if (line_i == 0) {
			if (sub_i == 0) {
				if (part_i == 0)
					return;
				else part_i--;
				sub_i = l.parts[part_i].sub.GetCount()-1;
			}
			else sub_i--;
			line_i = l.parts[part_i].sub[sub_i].lines.GetCount()-1;
		}
		else line_i--;
		const DynLine& prev = l.parts[part_i].sub[sub_i].lines[line_i];
		DynLine& dl = const_cast<DynLine&>(*line);
		dl.CopySuggestionVars(prev);
		DataLine();
	}
	// Copy clipboard
	else if (fn == 11) {
		WriteClipboardText(StoreAsJson(*line, true));
	}
	// Paste clipboard
	else if (fn == 12) {
		DynLine temp;
		String s = ReadClipboardText();
		if (s.IsEmpty()) return;
		LoadFromJson(temp, s);
		DynLine& dl = const_cast<DynLine&>(*line);
		dl.CopySuggestionVars(temp);
		DataLine();
	}
}

void ScriptTextSolverCtrl::UpdateEntities(DynLine& dl, bool unsafe, bool gender) {
	DatasetPtrs p; GetDataset(p);
	ASSERT(p.srctxt);
	auto& src = *p.srctxt;
	
	ContextType ctxtype = ContextType::Lyrical();
	String str;
	str << (unsafe ? "unsafe " : "safe ") << (gender ? "female" : "male");
	ContextData& data = src.ctxs.Get(ctxtype);
	line_form.style_type.Clear();
	line_form.style_entity.Clear();
	
	for(int i = 0; i < data.typeclasses.GetCount(); i++) {
		line_form.style_type.Add(data.typeclasses[i].name);
	}
	dl.style_type = max(0, min(dl.style_type, data.typeclasses.GetCount()-1));
	line_form.style_type.SetIndex(dl.style_type);
	
	int ent_i = data.entity_groups.Find(str);
	ASSERT(ent_i >= 0);
	int tcent = GetTypeclassEntity(unsafe, gender);
	const auto& types = data.typeclasses[dl.style_type].entities[ent_i];
	
	const auto& ents = types[dl.style_type];
	for(int i = 0; i < ents.GetCount(); i++) {
		line_form.style_entity.Add(ents[i]);
	}
	dl.style_entity = max(0, min(dl.style_entity, ents.GetCount()-1));
	line_form.style_entity.SetIndex(dl.style_entity);
}

void ScriptTextSolverCtrl::DataLine() {
	DatasetPtrs p; GetDataset(p);
	Script& s = *p.script;
	
	if (editor.selected_line) {
		const DynLine* active = 0;
		Vector<const DynLine*> g = GetLineGroup(0, 0, &active, 0, 0, 0);
		
		if (!active) return;
		DynLine& dl = const_cast<DynLine&>(*active);
		
		if (p.entity) {
			UpdateEntities(dl, dl.safety, p.entity->GetGender());
		}
		for(int i = 0; i < g.GetCount(); i++) {
			const DynLine* dl = g[i];
			
			line_ref_lines.Set(i, 0, dl == editor.selected_line ? "X" : "");
			line_ref_lines.Set(i, 1, dl->text);
			line_ref_lines.Set(i, 2, dl->user_text);
		}
		line_ref_lines.SetCount(g.GetCount());
		
		
		int sugg_i = 0;
		while (1) {
			String sugg;
			for(int i = 0; i < g.GetCount(); i++) {
				const DynLine* dl = g[i];
				if (sugg_i < dl->suggs.GetCount()) {
					if (!sugg.IsEmpty()) sugg << "\n";
					sugg << dl->suggs[sugg_i];
				}
				else {
					break;
				}
			}
			if (sugg.IsEmpty())
				break;
			line_suggs.Set(sugg_i, 0, sugg_i+1);
			line_suggs.Set(sugg_i, 1, sugg);
			sugg_i++;
		}
		line_suggs.SetCount(sugg_i);
		line_suggs.ArrayCtrl::SetLineCy(g.GetCount() * 20);
		if (active) {
			line_form.expanded.SetData(active->expanded);
		}
		else {
			line_form.expanded.Clear();
		}
		line_form.safety.SetIndex(dl.safety);
		line_form.length.SetIndex(dl.line_len);
		line_form.connector.SetIndex(dl.connector);
		line_form.begin.SetData(dl.line_begin);
	}
	
}

void ScriptTextSolverCtrl::OnValueChange() {
	DatasetPtrs p; GetDataset(p);
	Script& s = *p.script;
	auto& src = p.src->Data();
	ASSERT(p.src);
	
	int tab = tabs.Get();
	if (tab == 3) {
		if (editor.selected_part) {
			DynPart& part = *const_cast<DynPart*>(editor.selected_part);
			int el_i = part_form.element.GetIndex();
			part.el.element = el_i >= 0 ? src.element_keys[el_i] : String();
			part.text_num = (int)part_form.text_num.GetData() - 1;
			part.text_type = (TextPartType)part_form.text_type.GetIndex();
			
			editor.Refresh();
		}
	}
	if (tab == 4) {
		if (editor.selected_sub) {
			DynSub& sub = *const_cast<DynSub*>(editor.selected_sub);
			int el_i = sub_form.element.GetIndex() - 1;
			sub.el.element = el_i >= 0 ? src.element_keys[el_i] : String();
			sub.story = sub_form.story.GetData();
			
			editor.Refresh();
		}
	}
	if (tab == 5) {
		
		if (editor.selected_line) {
			DynLine& line = *const_cast<DynLine*>(editor.selected_line);
			line.style_type = line_form.style_type.GetIndex();
			line.style_entity = line_form.style_entity.GetIndex();
			line.safety = line_form.safety.GetIndex();
			line.line_len = line_form.length.GetIndex();
			line.connector = line_form.connector.GetIndex();
			line.line_begin = line_form.begin.GetData();
		}
		
		DataLine();
	}
}

void ScriptTextSolverCtrl::GetPart(const DynPart** part, int* part_iptr) {
	auto selected_part = editor.selected_part;
	if (!selected_part) return;
	DatasetPtrs p; GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	for(int i = 0; i < l.parts.GetCount(); i++) {
		const DynPart& dp = l.parts[i];
		if (&dp == selected_part) {
			if (part && !*part) {*part = &dp; if (part_iptr) *part_iptr = i;}
			return;
		}
	}
}

void ScriptTextSolverCtrl::GetSub(const DynPart** part, const DynSub** sub, int* part_iptr, int* sub_iptr) {
	auto selected_sub = editor.selected_sub;
	if (!selected_sub) return;
	DatasetPtrs p; GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	for(int i = 0; i < l.parts.GetCount(); i++) {
		const DynPart& dp = l.parts[i];
		for(int j = 0; j < dp.sub.GetCount(); j++) {
			const DynSub& ds = dp.sub[j];
			if (&ds == selected_sub) {
				if (part && !*part) {*part = &dp; if (part_iptr) *part_iptr = i;}
				if (sub && !*sub) {*sub = &ds; if (sub_iptr) *sub_iptr = j;}
				return;
			}
		}
	}
}

Vector<const DynLine*> ScriptTextSolverCtrl::GetLineGroup(const DynPart** part, const DynSub** sub, const DynLine** line, int* part_iptr, int* sub_iptr, int* line_iptr) {
	Vector<const DynLine*> ret;
	auto selected_line = editor.selected_line;
	if (!selected_line) return ret;
	DatasetPtrs p; GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	for(int i = 0; i < l.parts.GetCount(); i++) {
		const DynPart& dp = l.parts[i];
		
		int sel_line_i = -1;
		int line_i = 0;
		for(int j = 0; j < dp.sub.GetCount(); j++) {
			const DynSub& ds = dp.sub[j];
			
			for(int k = 0; k < ds.lines.GetCount(); k++) {
				const DynLine& dl = ds.lines[k];
				
				if (&dl == selected_line) {
					if (part && !*part) {*part = &dp; if (part_iptr) *part_iptr = i;}
					if (sub && !*sub) {*sub = &ds; if (sub_iptr) *sub_iptr = j;}
					if (line && !*line) {*line = &dl; if (line_iptr) *line_iptr = k;}
					ret << &dl;
					sel_line_i = line_i;
					break;
				}
				line_i++;
			}
			if (sel_line_i != -1)
				break;
		}
		
		if (sel_line_i == -1)
			continue;
		
		int alt_line;
		bool is_even = line_i % 2 == 0;
		if (is_even) alt_line = line_i+1;
		else alt_line = line_i-1;
		
		line_i = 0;
		for(int j = 0; j < dp.sub.GetCount(); j++) {
			const DynSub& ds = dp.sub[j];
			
			for(int k = 0; k < ds.lines.GetCount(); k++) {
				const DynLine& dl = ds.lines[k];
				
				if (line_i == alt_line) {
					if (part && !*part) {*part = &dp; if (part_iptr) *part_iptr = i;}
					if (sub && !*sub) {*sub = &ds; if (sub_iptr) *sub_iptr = j;}
					if (is_even)
						ret.Add(&dl);
					else
						ret.Insert(0, &dl);
					return ret;
				}
				line_i++;
			}
		}
		break;
	}
	return ret;
}

const DynLine* ScriptTextSolverCtrl::GetAltLine() {
	Vector<const DynLine*> g = GetLineGroup();
	if (g.GetCount() < 2) return 0;
	if (editor.selected_line == g[0]) return g[1];
	else return g[0];
}

















StructuredScriptEditor::StructuredScriptEditor() {
	WantFocus();
	line_h = 16;
	Ctrl::AddFrame(scroll_v);
	scroll_v.WhenScroll << [this]() {
		Refresh();
	};
}

void StructuredScriptEditor::Update() {
	if (!owner) {Refresh(); return;}
	
	int total_h = 0;
	DatasetPtrs p;
	owner->GetDataset(p);
	LyricalStructure& l = *p.lyric_struct;
	for(int i = 0; i < l.parts.GetCount(); i++) {
		const DynPart& dp = l.parts[i];
		total_h += line_h;
		for(int j = 0; j < dp.sub.GetCount(); j++) {
			const DynSub& ds = dp.sub[j];
			total_h += line_h;
			for(int k = 0; k < ds.lines.GetCount(); k++) {
				const DynLine& dl = ds.lines[k];
				total_h += line_h;
			}
		}
		total_h += line_h;
	}
	scroll_v.SetTotal(total_h);
}

void StructuredScriptEditor::CheckClearSelected() {
	bool line_found = false;
	bool part_found = false;
	bool sub_found = false;
	DatasetPtrs p;
	owner->GetDataset(p);
	if (!p.script) return;
	if (!p.lyric_struct) return;
	LyricalStructure& l = *p.lyric_struct;
	for(int i = 0; i < l.parts.GetCount(); i++) {
		const DynPart& dp = l.parts[i];
		if (&dp == selected_part) part_found = true;
		for(int j = 0; j < dp.sub.GetCount(); j++) {
			const DynSub& ds = dp.sub[j];
			if (&ds == selected_sub) sub_found = true;
			for(int k = 0; k < ds.lines.GetCount(); k++) {
				const DynLine& dl = ds.lines[k];
				if (&dl == selected_line) line_found = true;
			}
		}
	}
	if (!line_found) selected_line = 0;
	if (!part_found) selected_part = 0;
	if (!sub_found) selected_sub = 0;
}

void StructuredScriptEditor::ClearSelected() {
	selected_line = 0;
	selected_part = 0;
	selected_sub = 0;
}

void StructuredScriptEditor::Layout() {
	Size sz = GetSize();
	scroll_v.SetPage(sz.cy);
	scroll_v.SetLine(line_h);
	Refresh();
}

void StructuredScriptEditor::MouseWheel(Point p, int zdelta, dword keyflags) {
	scroll_v.Wheel(zdelta);
}

void StructuredScriptEditor::LeftDown(Point p, dword keyflags) {
	for (const Area& a : areas) {
		if (a.r.Contains(p)) {
			selected_line = a.selected_line;
			selected_part = a.selected_part;
			selected_sub = a.selected_sub;
			SetFocus();
			Refresh();
			WhenCursor();
			return;
		}
	}
	selected_line = 0;
	selected_part = 0;
	selected_sub = 0;
	SetFocus();
	Refresh();
	WhenCursor();
}

bool StructuredScriptEditor::IsAnySelected() const {
	if (selected_line == 0 &&
		selected_part == 0 &&
		selected_sub  == 0)
		return false;
	else
		return true;
}

bool StructuredScriptEditor::Key(dword key, int count) {
	if (IsAnySelected()) {
		if (key == K_UP) {
			MoveSelected(-1);
			return true;
		}
		if (key == K_DOWN) {
			MoveSelected(+1);
			return true;
		}
		if (key == K_HOME) {
			MoveSelected(-1000);
			return true;
		}
		if (key == K_END) {
			MoveSelected(+1000);
			return true;
		}
		if (key == K_PAGEUP) {
			MoveSelected(-10);
			return true;
		}
		if (key == K_PAGEDOWN) {
			MoveSelected(+10);
			return true;
		}
	}
	return scroll_v.Key(key, count);
}

void StructuredScriptEditor::MoveSelected(int i) {
	if (!IsAnySelected()) {
		int new_sel = i >= 0 ? 0 : vert_areas.GetCount()-1;
		if (new_sel < 0 || new_sel >= vert_areas.GetCount())
			return;
		const Area& a = vert_areas[new_sel];
		selected_line = a.selected_line;
		selected_part = a.selected_part;
		selected_sub = a.selected_sub;
		ScrollView(a.r);
		Refresh();
		WhenCursor();
		return;
	}
	else {
		int j = 0;
		for (const Area& a : vert_areas) {
			if ((selected_line && a.selected_line == selected_line) ||
				(selected_part && a.selected_part == selected_part) ||
				(selected_sub  && a.selected_sub  == selected_sub)) {
				int new_sel = j + i;
				new_sel = min(vert_areas.GetCount()-1, max(0, new_sel));
				if (new_sel < 0 || new_sel >= vert_areas.GetCount())
					return;
				const Area& a1 = vert_areas[new_sel];
				selected_line = a1.selected_line;
				selected_part = a1.selected_part;
				selected_sub = a1.selected_sub;
				ScrollView(a1.r);
				Refresh();
				WhenCursor();
				return;
			}
			j++;
		}
	}
}

void StructuredScriptEditor::ScrollView(const Rect& r) {
	Size sz = GetSize();
	if (r.top < 0) {
		int y = scroll_v + r.top;
		scroll_v.ScrollInto(y);
	}
	else if (r.bottom >= sz.cy) {
		int y = scroll_v + r.bottom;// - sz.cy;
		scroll_v.ScrollInto(y);
	}
}

void StructuredScriptEditor::Paint(Draw& d) {
	Size sz = GetSize();
	int cx_2 = sz.cx / 2;
	
	d.DrawRect(sz, White());
	Font fnt = SansSerif(line_h-3); // Monospace(line_h-3);
	if (!owner) {
		d.DrawText(2,2,"Error: no pointer",fnt,Black());
		return;
	}
	DatasetPtrs p;
	owner->GetDataset(p);
	if (!p.lyric_struct) return;
	LyricalStructure& l = *p.lyric_struct;
	int y = -scroll_v;
	int indent_cx = 30;
	Color part_bg = Color(233, 235, 255);
	Color part_border = Color(203, 205, 225);
	Color sub_bg = Color(241, 254, 240);
	Color line_bg = Color(253, 242, 241);
	Color sel_clr = Blend(LtRed(), White());
	Color shadow_clr = GrayColor();
	bool is_sel_shadow = false;
	int off = 3;
	
	const DynLine* selected_alt_line = owner->GetAltLine();
	
	areas.Clear();
	vert_areas.Clear();
	for(int i = 0; i < l.parts.GetCount(); i++) {
		const DynPart& dp = l.parts[i];
		
		String txt = GetTextTypeString(dp.text_type) + " " + IntStr(dp.text_num+1);
		if (dp.el.element.GetCount()) txt << ": " << dp.el.element;
		
		bool sel = &dp == selected_part;
		Color part_clr = sel ? Blend(part_bg, sel_clr) : part_bg;
		Rect part_header_rect = RectC(0,y,sz.cx,line_h);
		d.DrawRect(part_header_rect, part_clr);
		if (is_sel_shadow && sel)
			d.DrawText(off+1,y+1,txt,fnt,shadow_clr);
		d.DrawText(off+0,y,txt,fnt,Black());
		d.DrawLine(0, y, sz.cx, y, 1, part_border);
		areas.Add().Set(part_header_rect, dp);
		vert_areas.Add().Set(part_header_rect, dp);
		
		y += line_h;
		
		for(int j = 0; j < dp.sub.GetCount(); j++) {
			const DynSub& ds = dp.sub[j];
			
			txt.Clear();
			if (ds.el.element.GetCount()) txt << ds.el.element;
			
			bool sel = &ds == selected_sub;
			Color sub_clr = sel ? Blend(sub_bg, sel_clr) : sub_bg;
			int left = indent_cx;
			part_header_rect = RectC(0,y,left,line_h);
			d.DrawRect(part_header_rect, part_clr);
			areas.Add().Set(part_header_rect, dp);
			
			Rect sub_header_rect = RectC(left,y,sz.cx-left,line_h);
			d.DrawRect(sub_header_rect, sub_clr);
			if (is_sel_shadow && sel)
				d.DrawText(off+left+1,y+1,txt,fnt,shadow_clr);
			d.DrawText(off+left,y,txt,fnt,Black());
			areas.Add().Set(sub_header_rect, ds);
			vert_areas.Add().Set(sub_header_rect, ds);
			
			y += line_h;
			
			
			for(int k = 0; k < ds.lines.GetCount(); k++) {
				const DynLine& dl = ds.lines[k];
				
				int l0, l1;
				l0 = 0;
				l1 = indent_cx;
				part_header_rect = Rect(l0,y,l1,y+line_h);
				d.DrawRect(part_header_rect, part_clr);
				areas.Add().Set(part_header_rect, dp);
				
				l0 = indent_cx;
				l1 = indent_cx*2;
				sub_header_rect = Rect(l0,y,l1,y+line_h);
				d.DrawRect(sub_header_rect, sub_clr);
				
				int left = indent_cx*2;
				txt = dl.text;
				Rect line_header_rect = RectC(left,y,sz.cx-left,line_h);
				bool sel = &dl == selected_line;
				bool alt_sel = !sel && &dl == selected_alt_line;
				
				Color bg = line_bg;
				if (sel)
					bg = Blend(line_bg, sel_clr);
				else if (alt_sel)
					bg = Blend(line_bg, sel_clr, 32);
				d.DrawRect(line_header_rect, bg);
				
				if (is_sel_shadow && sel)
					d.DrawText(off+left+1,y+1,txt,fnt,shadow_clr);
				d.DrawText(off+left,y,txt,fnt,Black());
				areas.Add().Set(line_header_rect, dl);
				vert_areas.Add().Set(line_header_rect, dl);
				areas.Add().Set(line_header_rect, dl);
				
				
				d.DrawText(off+cx_2,y,dl.user_text,fnt,Black());
				
				y += line_h;
				
			}
		}
		
		d.DrawLine(0, y, sz.cx, y, 1, part_border);
		y += line_h;
	}
	
	d.DrawLine(cx_2, 0, cx_2, sz.cy, 1, part_border);
}

INITIALIZER_COMPONENT_CTRL(Lyrics, ScriptTextSolverCtrl)



END_UPP_NAMESPACE
