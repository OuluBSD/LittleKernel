#include "Biography.h"


NAMESPACE_UPP


BiographyCtrl::BiographyCtrl() {
	Add(hsplit.VSizePos(0,20).HSizePos());
	hsplit.Horz() << categories << tabs;
	hsplit.SetPos(1500);
	
	categories.AddColumn(t_("Category"));
	categories.AddColumn(t_("Texts"));
	categories.AddColumn(t_("Images"));
	categories.AddIndex("IDX");
	for(int i = 0; i < BIOCATEGORY_COUNT; i++) {
		categories.Set(i, 0, GetBiographyCategoryKey(i));
		categories.Set(i, "IDX", i);
	}
	categories.ColumnWidths("5 1 1");
	categories.SetSortColumn(0);
	categories.SetCursor(0);
	categories <<= THISBACK(DataCategory);
	
	Main_Ctor();
	El_Ctor();
	Summary_Ctor();
	Image_Ctor();
	ImageSummary_Ctor();
	tabs.WhenSet = THISBACK(DataCategory);
	
	
	
}

void BiographyCtrl::Data() {
	Biography& biography = GetExt<Biography>();
	DatasetPtrs p; GetDataset(p);
	if (!p.owner) {PromptOK("No owner was found"); return;}
	if (!p.owner->born.IsValid() || p.owner->born.year < -2000 || p.owner->born.year >= 3000) {
		PromptOK("Error: Owner has no valid birth date");
		return;
	}
	
	for(int i = 0; i < categories.GetCount(); i++) {
		int cat_i = categories.Get(i, "IDX");
		BiographyCategory& bcat = biography.GetAdd(*p.owner, cat_i);
		int c0 = bcat.GetFilledCount();
		int c1 = bcat.GetFilledImagesCount();
		categories.Set(i, 1, c0 > 0 ? Value(c0) : Value());
		categories.Set(i, 2, c1 > 0 ? Value(c1) : Value());
	}
	
	DataCategory();
}

void BiographyCtrl::DataCategory() {
	int tab = tabs.Get();
	if (tab == 0) Main_DataCategory();
	if (tab == 1) El_DataCategory();
	if (tab == 2) Summary_DataCategory();
	if (tab == 3) Image_DataCategory();
	if (tab == 4) ImageSummary_DataCategory();
}

void BiographyCtrl::ToolMenu(Bar& bar) {
	int tab = tabs.Get();
	if (tab == 0) Main_ToolMenu(bar);
	if (tab == 1) El_ToolMenu(bar);
	if (tab == 2) Summary_ToolMenu(bar);
	if (tab == 3) Image_ToolMenu(bar);
	if (tab == 4) ImageSummary_ToolMenu(bar);
}

void BiographyCtrl::EditPos(JsonIO& json) {
	#define GET(arr, idx) idx = arr.IsCursor() ? arr.GetCursor() : -1;
	int tab = tabs.Get();
	int category = -1;
	GET(categories, category)
	int list = -1, sublist = -1;
	if (tab == 0) {
		GET(main.years, list)
		GET(main.year.elements, sublist)
	}
	else if (tab == 1) {
		GET(el.elements, list)
		GET(el.block.elements, sublist)
	}
	else if (tab == 2) {
		GET(summary.blocks, list)
		GET(summary.block.elements, sublist)
	}
	else if (tab == 3) {
		GET(image.years, list)
		GET(image.entries, sublist)
	}
	else if (tab == 4) {
		GET(image_summary.blocks, list)
		GET(image_summary.block.elements, sublist)
	}
	else if (tab == 5) {
		GET(audience.blocks, list)
		GET(audience.block.elements, sublist)
	}
	#undef GET
	
	json("biography_tab", tab)
		("biography_category", category)
		("biography_list", list)
		("biography_sublist", sublist)
		;
		
	if (json.IsLoading()) {
		PostCallback([=] {
			tabs.Set(tab);
			#define SET(arr, idx) if (idx >= 0 && idx < arr.GetCount()) arr.SetCursor(idx);
			SET(categories, category);
			if (tab == 0) {
				SET(main.years, list);
				SET(main.year.elements, sublist);
			}
			else if (tab == 1) {
				SET(el.elements, list);
				SET(el.block.elements, sublist);
			}
			else if (tab == 2) {
				SET(summary.blocks, list)
				SET(summary.block.elements, sublist)
			}
			else if (tab == 3) {
				SET(image.years, list)
				SET(image.entries, sublist)
			}
			else if (tab == 4) {
				SET(image_summary.blocks, list)
				SET(image_summary.block.elements, sublist)
			}
			else if (tab == 5) {
				SET(audience.blocks, list)
				SET(audience.block.elements, sublist)
			}
			#undef SET
		});
	}
}

void BiographyCtrl::EntryListMenu(Bar& bar) {
	
}

void BiographyCtrl::MakeKeywords () {
	TODO
	#if 0
	if (!MetaPtrs::Single().editable_biography)
		return;
	TaskMgr& m = AiTaskManager();
	SocialArgs args;
	args.text = year.text.GetData();
	m.GetSocial(args, [this](String s) {PostCallback(THISBACK1(OnKeywords, s));});
	#endif
}

void BiographyCtrl::Translate() {
	TODO
	#if 0
	if (!MetaPtrs::Single().editable_biography)
		return;
	TaskMgr& m = AiTaskManager();
	
	String src = year.native_text.GetData();
	
	m.Translate("FI-FI", src, "EN-US", [this](String s) {PostCallback(THISBACK1(OnTranslate, s));});
	#endif
}

void BiographyCtrl::GetElements() {
	TODO
	#if 0
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !categories.IsCursor() || !years.IsCursor())
		return;
	if (!mp.editable_biography)
		return;
	
	Owner& owner = *mp.owner;
	Profile& profile = *mp.profile;
	Biography& biography = GetExt<Biography>();
	int cat_enum = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_enum);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	
	
	BioYear& by = bcat.years[year_i];
	
	BiographyProcessArgs args;
	args.fn = 0;
	args.category = GetBiographyCategoryEnum(cat_enum);
	args.text = by.text;
	args.year = by.year;
	TaskMgr& m = AiTaskManager();
	
	auto* snap_ptr = mp.release;
	auto* by_ptr = &by;
	m.GetBiography(args, [this, snap_ptr, by_ptr](String result) {
		RemoveEmptyLines3(result);
		Vector<String> lines = Split(result, "\n");
		
		snap_ptr->last_modified = GetSysTime();
		
		by_ptr->elements.Clear();
		for (String& line : lines) {
			int a = line.Find(":");
			if (a < 0) continue;
			String key = ToLower(TrimBoth(line.Left(a)));
			String value = TrimBoth(line.Mid(a+1));
			RemoveQuotes(value);
			String lvalue = ToLower(value);
			int i = by_ptr->FindElement(key);
			if (lvalue.IsEmpty() || lvalue == "none." || lvalue == "none" || lvalue.Left(6) == "none (") {
				if (i >= 0)
					by_ptr->elements.Remove(i);
				continue;
			}
			if (i < 0) {
				i = by_ptr->elements.GetCount();
				by_ptr->elements.Add();
			}
			auto& el = by_ptr->elements[i];
			el.key = key;
			el.value = value;
		}
		
		PostCallback(THISBACK(UpdateElements));
	});
	#endif
}

void BiographyCtrl::GetElementHints() {
	TODO
	#if 0
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !categories.IsCursor() || !years.IsCursor())
		return;
	Owner& owner = *mp.owner;
	Profile& profile = *mp.profile;
	Biography& biography = GetExt<Biography>();
	int cat_enum = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_enum);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	
	BioYear& by = bcat.years[year_i];
	
	BiographyProcessArgs args;
	args.fn = 1;
	args.category = GetBiographyCategoryEnum(cat_enum);
	args.text = by.text;
	args.year = by.year;
	TaskMgr& m = AiTaskManager();
	
	auto* snap_ptr = mp.release;
	auto* by_ptr = &by;
	m.GetBiography(args, [this, snap_ptr, by_ptr](String result) {
		RemoveEmptyLines3(result);
		Vector<String> lines = Split(result, "\n");
		
		snap_ptr->last_modified = GetSysTime();
		
		element_hints.Clear();
		for (String& line : lines) {
			int a = line.Find(":");
			if (a < 0) continue;
			String key = ToLower(TrimBoth(line.Left(a)));
			String value = TrimBoth(line.Mid(a+1));
			RemoveQuotes(value);
			String lvalue = ToLower(value);
			if (lvalue.IsEmpty() || lvalue == "none." || lvalue == "none" || lvalue.Left(6) == "none (" || lvalue == "ready." || lvalue == "ready" || lvalue.Left(6) == "ready (" || lvalue == "n/a" || lvalue == "none mentioned.")
				continue;
			element_hints.GetAdd(key) = value;
		}
		
		PostCallback(THISBACK(UpdateElementHints));
	});
	#endif
}

void BiographyCtrl::GetElementScores() {
	TODO
	#if 0
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !categories.IsCursor() || !years.IsCursor())
		return;
	Owner& owner = *mp.owner;
	Profile& profile = *mp.profile;
	Biography& biography = GetExt<Biography>();
	int cat_enum = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_enum);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	
	BioYear& by = bcat.years[year_i];
	
	if (by.elements.IsEmpty())
		return;
	
	BiographyProcessArgs args;
	args.fn = 2;
	args.category = GetBiographyCategoryEnum(cat_enum);
	args.text = by.JoinElementMap(": ", "\n");
	args.year = by.year;
	TaskMgr& m = AiTaskManager();
	
	auto* snap_ptr = mp.release;
	auto* by_ptr = &by;
	m.GetBiography(args, [this, snap_ptr, by_ptr](String result) {
		RemoveEmptyLines3(result);
		Vector<String> lines = Split(result, "\n");
		
		snap_ptr->last_modified = GetSysTime();
		
		for (String& line : lines) {
			int a = line.Find(":");
			if (a < 0) continue;
			String key = ToLower(TrimBoth(line.Left(a)));
			String value = TrimBoth(line.Mid(a+1));
			int i = by_ptr->FindElement(key);
			if (i < 0)
				continue;
			auto& el = by_ptr->elements[i];
			
			Vector<String> scores = Split(value, ",");
			int j = -1;
			el.ResetScore();
			for (String& s : scores) {
				j++;
				Vector<String> parts = Split(s, ":");
				if (parts.GetCount() != 2)
					continue;
				int sc = ScanInt(TrimLeft(parts[1]));
				el.scores[j] = sc;
			}
		}
		
		PostCallback(THISBACK(UpdateElements));
	});
	#endif
}



INITIALIZER_COMPONENT_CTRL(Biography, BiographyCtrl)

END_UPP_NAMESPACE
