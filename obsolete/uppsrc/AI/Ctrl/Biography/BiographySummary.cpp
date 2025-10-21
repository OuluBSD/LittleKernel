#include "Biography.h"
#define REF(tab, obj) auto& obj = tab.obj;

NAMESPACE_UPP


void BiographyCtrl::Summary_Ctor() {
	REF(summary, vsplit);
	REF(summary, blocks);
	REF(summary, block);
	
	tabs.Add(vsplit.SizePos(), "Summary");
	
	vsplit.Vert() << blocks << block;
	vsplit.SetPos(3333);
	
	CtrlLayout(block);
	
	
	blocks.AddColumn(t_("First Year"));
	blocks.AddColumn(t_("Last Year"));
	blocks.AddColumn(t_("Year count"));
	blocks.AddColumn(t_("Age"));
	blocks.AddColumn(t_("Class"));
	blocks.AddColumn(t_("Keywords"));
	blocks.AddColumn(t_("Text"));
	blocks.AddIndex("IDX");
	blocks.ColumnWidths("1 1 1 1 1 5 10");
	blocks.WhenCursor << THISBACK(Summary_DataYear);
	
	
	block.keywords <<= THISBACK(Summary_OnValueChange);
	block.native_text <<= THISBACK(Summary_OnValueChange);
	block.text <<= THISBACK(Summary_OnValueChange);
	
	block.elements.AddColumn("Key");
	block.elements.AddColumn("Value");
	block.elements.AddColumn("Score");
	block.elements.ColumnWidths("2 12 1");
	
}

void BiographyCtrl::Summary_DataCategory() {
	REF(summary, blocks);
	REF(summary, block);
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor()) {
		blocks.Clear();
		return;
	}
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	bcat.RealizeSummaries();
	
	Date today = GetSysDate();
	for(int i = 0; i < bcat.summaries.GetCount(); i++) {
		const auto& range = bcat.summaries.GetKey(i);
		const BioYear& by = bcat.summaries[i];
		Date by_date(by.year, today.month, today.day);
		int age = (by_date - owner.born) / 365;
		int cls = age - 7;
		String cls_str;
		if (cls >= 0) {
			int round = cls / 12;
			cls = cls % 12;
			cls_str.Cat('A' + round);
			cls_str += " " + IntStr(1+cls);
		}
		blocks.Set(i, 0, range.off);
		blocks.Set(i, 1, range.off + range.len - 1);
		blocks.Set(i, 2, range.len);
		blocks.Set(i, 3, age);
		blocks.Set(i, 4, cls_str);
		blocks.Set(i, 5, by.keywords);
		blocks.Set(i, 6, by.text);
		blocks.Set(i, "IDX", i);
	}
	INHIBIT_CURSOR(blocks);
	
	blocks.SetCount(bcat.summaries.GetCount());
	if (blocks.GetCount() && !blocks.IsCursor())
		blocks.SetCursor(0);
	
	Summary_DataYear();
}

void BiographyCtrl::Summary_DataYear() {
	REF(summary, blocks);
	REF(summary, block);
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !blocks.IsCursor())
		return;
	
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int block_i = blocks.Get("IDX");
	if (block_i >= bcat.summaries.GetCount()) return;
	BioYear& by = bcat.summaries[block_i];
	
	block.year.SetData(by.year);
	block.keywords.SetData(by.keywords);
	block.native_text.SetData(by.native_text);
	block.text.SetData(by.text);
	
	Summary_UpdateElements();
}

void BiographyCtrl::Summary_UpdateElements() {
	REF(summary, blocks);
	REF(summary, block);
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !blocks.IsCursor())
		return;
	
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int block_i = blocks.Get("IDX");
	if (block_i >= bcat.summaries.GetCount()) return;
	BioYear& by = bcat.summaries[block_i];
	
	for(int i = 0; i < by.elements.GetCount(); i++) {
		block.elements.Set(i, 0, Capitalize(by.elements[i].key));
		block.elements.Set(i, 1, by.elements[i].value);
		block.elements.Set(i, 2, by.elements[i].GetAverageScore());
	}
	block.elements.SetCount(by.elements.GetCount());
	
}

void BiographyCtrl::Summary_OnValueChange() {
	REF(summary, blocks);
	REF(summary, block);
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.owner || !mp.biography ||!categories.IsCursor() || !blocks.IsCursor())
		return;
	if (!mp.editable_biography)
		return;
	
	mp.snap->last_modified = GetSysTime();
	
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int block_i = blocks.Get("IDX");
	BioYear& by = bcat.summaries[block_i];
	
	by.keywords = block.keywords.GetData();
	by.native_text = block.native_text.GetData();
	by.text = block.text.GetData();
	
	blocks.Set(5, by.keywords);
	blocks.Set(6, by.text);
}

void BiographyCtrl::Summary_MakeKeywords () {
	REF(summary, block);
	TaskMgr& m = AiTaskManager();
	SocialArgs args;
	args.text = block.text.GetData();
	m.GetSocial(args, [this](String s) {PostCallback(THISBACK1(Summary_OnKeywords, s));});
}

void BiographyCtrl::Summary_Translate() {
	REF(summary, block);
	TaskMgr& m = AiTaskManager();
	
	String src = block.native_text.GetData();
	
	m.Translate("FI-FI", src, "EN-US", [this](String s) {PostCallback(THISBACK1(Summary_OnTranslate, s));});
}

void BiographyCtrl::Summary_OnTranslate(String s) {
	REF(summary, block);
	block.text.SetData(s);
	Summary_OnValueChange();
}

void BiographyCtrl::Summary_OnKeywords(String s) {
	REF(summary, block);
	RemoveEmptyLines2(s);
	Vector<String> parts = Split(s, "\n");
	s = Join(parts, ", ");
	block.keywords.SetData(s);
	Summary_OnValueChange();
}

void BiographyCtrl::Summary_ToolMenu(Bar& bar) {
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Summary_Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Summary_Do, 1)).Key(K_F6);
	//bar.Add(t_("Translate"), MetaImgs::BlueRing(), THISBACK(Translate)).Key(K_F5);
	//bar.Add(t_("Make keywords"), MetaImgs::BlueRing(), THISBACK(MakeKeywords)).Key(K_F6);
}

void BiographyCtrl::Summary_Do(int fn) {
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	if (mp.editable_biography) {
		PromptOK(t_("The latest (and editable) revision won't be processed. Select other than the latest revision."));
		return;
	}
	TODO
	#if 0
	BiographySummaryProcess& sdi = BiographySummaryProcess::Get(*mp.profile, *mp.snap);
	prog.Attach(sdi);
	sdi.WhenRemaining << [this](String s) {PostCallback([this,s](){remaining.SetLabel(s);});};
	if (fn == 0)
		sdi.Start();
	else
		sdi.Stop();
	#endif
}

void BiographyCtrl::Summary_EntryListMenu(Bar& bar) {
	
}


END_UPP_NAMESPACE
#undef REF
