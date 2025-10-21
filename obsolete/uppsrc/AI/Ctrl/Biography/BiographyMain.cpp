#include "Biography.h"
#define REF(tab, obj) auto& obj = tab.obj;

NAMESPACE_UPP

void BiographyCtrl::Main_Ctor() {
	REF(main, vsplit)
	REF(main, years)
	REF(main, year)
	
	tabs.Add(vsplit.SizePos(), "Main");
	vsplit.Vert() << years << year;
	vsplit.SetPos(3333);
	
	CtrlLayout(year);
	
	
	years.AddColumn(t_("Year"));
	years.AddColumn(t_("Age"));
	years.AddColumn(t_("Class"));
	years.AddColumn(t_("Keywords"));
	years.AddColumn(t_("Text"));
	years.AddIndex("IDX");
	years.ColumnWidths("1 1 1 5 10");
	years.WhenCursor << THISBACK(Main_DataYear);
	
	
	year.keywords <<= THISBACK(Main_OnValueChange);
	year.native_text <<= THISBACK(Main_OnValueChange);
	year.text <<= THISBACK(Main_OnValueChange);
	
	year.elements.AddColumn("Key");
	year.elements.AddColumn("Value");
	year.elements.AddColumn("Score");
	year.elements.ColumnWidths("2 12 1");
}

void BiographyCtrl::Main_ToolMenu(Bar& bar) {
	bar.Add(t_("Translate"), MetaImgs::BlueRing(), THISBACK(Translate)).Key(K_F5);
	bar.Add(t_("Make keywords"), MetaImgs::BlueRing(), THISBACK(MakeKeywords)).Key(K_F6);
	bar.Add(t_("Get elements"), MetaImgs::BlueRing(), THISBACK(GetElements)).Key(K_F7);
	bar.Add(t_("Get element hints"), MetaImgs::BlueRing(), THISBACK(GetElementHints)).Key(K_F8);
	bar.Add(t_("Get element scores"), MetaImgs::BlueRing(), THISBACK(GetElementScores)).Key(K_F9);
	bar.Separator();
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Main_Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Main_Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Import Json"), MetaImgs::BlueRing(), THISBACK(Main_ImportJson));
}

void BiographyCtrl::Main_DataCategory() {
	Biography& biography = GetExt<Biography>();
	DatasetPtrs p; GetDataset(p);
	REF(main, vsplit)
	REF(main, years)
	REF(main, year)
	
	if (!p.owner) {
		PromptOK("error: no Owner component found");
		return;
	}
	
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(*p.owner, cat_i);
	
	Date today = GetSysDate();
	for(int i = 0; i < bcat.years.GetCount(); i++) {
		const BioYear& by = bcat.years[i];
		Date by_date(by.year, today.month, today.day);
		int age = (by_date - p.owner->born) / 365;
		int cls = age - 7;
		String cls_str;
		if (cls >= 0) {
			int round = cls / 12;
			cls = cls % 12;
			cls_str.Cat('A' + round);
			cls_str += " " + IntStr(1+cls);
		}
		years.Set(i, 0, by.year);
		years.Set(i, 1, age);
		years.Set(i, 2, cls_str);
		years.Set(i, 3, by.keywords);
		years.Set(i, 4, by.text);
		years.Set(i, "IDX", i);
	}
	INHIBIT_CURSOR(years);
	years.SetSortColumn(0, false);
	years.SetCount(bcat.years.GetCount());
	if (years.GetCount() && !years.IsCursor())
		years.SetCursor(0);
	
	Main_DataYear();
}

void BiographyCtrl::Main_DataYear() {
	Biography& biography = GetExt<Biography>();
	DatasetPtrs p; GetDataset(p);
	REF(main, vsplit)
	REF(main, years)
	REF(main, year)
	
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.owner || !categories.IsCursor() || !years.IsCursor())
		return;
	
	Owner& owner = *mp.owner;
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	
	year.year.SetData(by.year);
	year.keywords.SetData(by.keywords);
	year.native_text.SetData(by.native_text);
	year.text.SetData(by.text);
	
	Main_UpdateElements();
}

void BiographyCtrl::Main_UpdateElements() {
	REF(main, vsplit)
	REF(main, years)
	REF(main, year)
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !categories.IsCursor() || !years.IsCursor())
		return;
	Owner& owner = *mp.owner;
	Profile& profile = *mp.profile;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	
	double score_sum = 0;
	for(int i = 0; i < by.elements.GetCount(); i++) {
		const auto& e = by.elements[i];
		double sc = e.GetAverageScore();
		year.elements.Set(i, 0, Capitalize(e.key));
		year.elements.Set(i, 1, e.value);
		year.elements.Set(i, 2, sc);
		score_sum += sc;
	}
	year.elements.SetCount(by.elements.GetCount());
	year.elements.SetSortColumn(2,true);
	
	if (by.elements.GetCount() > 0) {
		double score_av = score_sum / by.elements.GetCount();
		year.score.SetLabel(Format("Score: %.2n", score_av));
	}
	else year.score.SetLabel("");
	
}

void BiographyCtrl::Main_UpdateElementHints() {
	REF(main, year)
	REF(main, element_hints)
	for(int i = 0; i < element_hints.GetCount(); i++) {
		year.elements.Set(i, 0, Capitalize(element_hints.GetKey(i)));
		year.elements.Set(i, 1, element_hints[i]);
		year.elements.Set(i, 2, Value());
	}
	year.elements.SetCount(element_hints.GetCount());
}

void BiographyCtrl::Main_OnValueChange() {
	REF(main, year)
	REF(main, years)
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.profile || !categories.IsCursor() || !years.IsCursor())
		return;
	
	if (!mp.editable_biography)
		return;
	
	Owner& owner = *mp.owner;
	Profile& profile = *mp.profile;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	
	BioYear& by = bcat.years[year_i];
	by.keywords = year.keywords.GetData();
	by.native_text = year.native_text.GetData();
	by.text = year.text.GetData();
	
	years.Set(year_i, 3, by.keywords);
	years.Set(year_i, 4, by.text);
}

void BiographyCtrl::Main_OnTranslate(String s) {
	REF(main, year)
	year.text.SetData(s);
	Main_OnValueChange();
}

void BiographyCtrl::Main_OnKeywords(String s) {
	REF(main, year)
	RemoveEmptyLines2(s);
	Vector<String> parts = Split(s, "\n");
	s = Join(parts, ", ");
	year.keywords.SetData(s);
	Main_OnValueChange();
}

void BiographyCtrl::Main_Do(int fn) {
	TODO
	#if 0
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	if (!mp.editable_biography) {
		PromptOK(t_("Only the latest (and editable) revision can be processed. Select the latest revision."));
		return;
	}
	BiographyProcess& sdi = BiographyProcess::Get(*mp.profile, *mp.snap);
	prog.Attach(sdi);
	sdi.WhenRemaining << [this](String s) {PostCallback([this,s](){remaining.SetLabel(s);});};
	if (fn == 0)
		sdi.Start();
	else
		sdi.Stop();
	#endif
}

void BiographyCtrl::Main_ImportJson() {
	DatasetPtrs p; GetDataset(p);
	Biography& o = GetExt<Biography>();
	if (LoadFromJsonFile_VisitorNodePrompt(o)) {
		PostCallback(THISBACK(Data));
	}
}


END_UPP_NAMESPACE
#undef REF
