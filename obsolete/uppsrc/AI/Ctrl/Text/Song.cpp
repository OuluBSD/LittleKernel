#include "Text.h"

NAMESPACE_UPP


SongInfoCtrl::SongInfoCtrl() {
	CtrlLayout(*this);
	
	comp_entity <<= THISBACK(OnValueChange);
	comp_prj_name <<= THISBACK(OnValueChange);
	reference <<= THISBACK(OnValueChange);
	origins <<= THISBACK(OnValueChange);
	music_style <<= THISBACK(OnValueChange);
	
	scripts.AddColumn(t_("Typeclass & Content"));
	scripts.AddColumn(t_("Script"));
	scripts.ColumnWidths("1 5");
	scripts.AddIndex("IDX");
	
	scripts.WhenCursor << THISBACK(DataScript);
	
	set.WhenAction << THISBACK(SetScript);
	
}

void SongInfoCtrl::ToolMenu(Bar& bar) {
	
}

void SongInfoCtrl::Clear() {
	this->comp_entity				.Clear();
	this->comp_prj_name				.Clear();
	
}

void SongInfoCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	
	Clear();
	
	if (p.song) {
		Song& s = *p.song;
		
		comp_entity.SetData(s.entity);
		comp_prj_name.SetData(s.prj_name);
		reference.SetData(s.reference);
		origins.SetData(s.origins);
		music_style.SetData(s.style);
	}
	
	
	if (!p.entity) {
		scripts.Clear();
		return;
	}
	
	Entity& a = *p.entity;
	if (p.song)
		focus_lyr = a.val.FindPos<Script>(p.song->scripts_file_title);
	
	if (focus_lyr < 0) {
		LOG("TODO"); //focus_lyr = p.GetActiveScriptIndex();
	}
	
	/*
	const auto& tcs = GetTypeclasses();
	for(int i = 0; i < a.typeclasses.GetCount(); i++) {
		const auto& t = tcs[i];
		const auto& tc = a.typeclasses[i];
		typeclasses.Set(i, "IDX", i);
		typeclasses.Set(i, 0, t);
		typeclasses.Set(i, 1, a.typeclasses[i].GetScriptCount());
	}
	INHIBIT_CURSOR(typeclasses);
	typeclasses.SetSortColumn(1, true);
	
	int cursor = max(0, focus_tc);
	if (cursor >= 0 && cursor < typeclasses.GetCount())
		SetIndexCursor(typeclasses, cursor);

	DataTypeclass();*/
	/*
	DatasetPtrs p; GetDataset(p);
	if (!p.entity || !typeclasses.IsCursor()) {
		contents.Clear();
		scripts.Clear();
		return;
	}
	
	Entity& a = *p.entity;
	Typeclass& t = a.typeclasses[typeclasses.Get("IDX")];
	const auto& cons = GetContents();
	for(int i = 0; i < t.contents.GetCount(); i++) {
		const auto& con = cons[i];
		const auto& at = t.contents[i];
		contents.Set(i, "IDX", i);
		contents.Set(i, 0, con.key);
		contents.Set(i, 1, at.scripts.GetCount());
	}
	INHIBIT_CURSOR(contents);
	contents.SetSortColumn(1, true);
	
	int cursor = max(0, focus_arch);
	if (cursor >= 0 && cursor < contents.GetCount())
		SetIndexCursor(contents, cursor);

	DataContent();
	

	
	DatasetPtrs p; GetDataset(p);
	if (!p.entity || !typeclasses.IsCursor() || !contents.IsCursor()) {
		scripts.Clear();
		return;
	}*/
	
	//Entity& a = *p.entity;
	LOG("TODO"); return;
	
	#if 0
	int row = 0;
	const auto& tcs = GetTypeclasses();
	const auto& cons = GetContents();
	for(int i = 0; i < a.scripts.GetCount(); i++) {
		Script& sc = a.scripts[i];
		String g = tcs[sc.typeclass] + ": " + cons[sc.content].key;
		scripts.Set(row, "IDX", i);
		scripts.Set(row, 0, g);
		scripts.Set(row, 1, sc.GetAnyTitle());
		row++;
	}
	INHIBIT_CURSOR(scripts);
	scripts.SetCount(row);
	int cursor = max(0, focus_lyr);
	if (cursor >= 0 && cursor < scripts.GetCount())
		scripts.SetCursor(cursor);
	#endif
	
	DataScript();
}

void SongInfoCtrl::DataScript() {
	DatasetPtrs p; GetDataset(p);
	if (!p.entity || !scripts.IsCursor()) {
		scripts_text.Clear();
		return;
	}
	
	TODO
	#if 0
	Entity& a = *p.entity;
	Script& lyr = a.scripts[scripts.Get("IDX")];
	
	String text = lyr.GetText(GetAppMode());
	if (text.GetCount())
		scripts_text.SetData(text);
	else
		scripts_text.SetData("<no scripts>");
	#endif
}

void SongInfoCtrl::OnValueChange() {
	DatasetPtrs p; GetDataset(p);
	
	TODO
	#if 0
	if (p.song && p.editor->components.IsCursor()) {
		Component& s = *p.song;
		
		s.entity = comp_entity.GetData();
		s.prj_name = comp_prj_name.GetData();
		s.reference = reference.GetData();
		s.origins = origins.GetData();
		s.style = music_style.GetData();
		
		int c = p.editor->components.GetCursor();
		p.editor->components.Set(c, 0, s.entity);
		p.editor->components.Set(c, 1, s.prj_name);
	}
	#endif
}

void SongInfoCtrl::SetScript() {
	DatasetPtrs p; GetDataset(p);
	Component& s = *p.song;
	
	if (!p.entity || !p.song || !scripts.IsCursor()) {
		return;
	}
	
	TODO
	#if 0
	int l_i = scripts.Get("IDX");
	Script& l = p.entity->scripts[l_i];
	s.scripts_file_title = l.file_title;
	#endif
}



INITIALIZER_COMPONENT_CTRL(Song, SongInfoCtrl)

END_UPP_NAMESPACE
