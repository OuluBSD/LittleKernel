#include "Biography.h"
#define REF(tab, obj) auto& obj = tab.obj;

NAMESPACE_UPP


void BiographyCtrl::ImageSummary_Ctor() {
	REF(image_summary, vsplit);
	REF(image_summary, blocks);
	REF(image_summary, block);
	
	tabs.Add(vsplit.SizePos(), "Image Summary");
	
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
	blocks.WhenCursor << THISBACK(ImageSummary_DataYear);
	
	
	block.keywords <<= THISBACK(ImageSummary_OnValueChange);
	block.native_text <<= THISBACK(ImageSummary_OnValueChange);
	block.text <<= THISBACK(ImageSummary_OnValueChange);
	
}

void BiographyCtrl::ImageSummary_DataCategory() {
	REF(image_summary, vsplit);
	REF(image_summary, blocks);
	REF(image_summary, block);
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !categories.IsCursor()) {
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
	//blocks.SetSortColumn(0, false);
	blocks.SetCount(bcat.summaries.GetCount());
	if (blocks.GetCount() && !blocks.IsCursor())
		blocks.SetCursor(0);
	
	ImageSummary_DataYear();
}

void BiographyCtrl::ImageSummary_DataYear() {
	REF(image_summary, vsplit);
	REF(image_summary, blocks);
	REF(image_summary, block);
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !categories.IsCursor() || !blocks.IsCursor())
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
}

void BiographyCtrl::ImageSummary_OnValueChange() {
	REF(image_summary, vsplit);
	REF(image_summary, blocks);
	REF(image_summary, block);
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.owner || !categories.IsCursor() || !blocks.IsCursor())
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

void BiographyCtrl::ImageSummary_MakeKeywords () {
	REF(image_summary, block);
	
	TaskMgr& m = AiTaskManager();
	SocialArgs args;
	args.text = block.text.GetData();
	m.GetSocial(args, [this](String s) {PostCallback(THISBACK1(ImageSummary_OnKeywords, s));});
}

void BiographyCtrl::ImageSummary_Translate() {
	REF(image_summary, block);
	
	TaskMgr& m = AiTaskManager();
	String src = block.native_text.GetData();
	
	m.Translate("FI-FI", src, "EN-US", [this](String s) {PostCallback(THISBACK1(ImageSummary_OnTranslate, s));});
}

void BiographyCtrl::ImageSummary_OnTranslate(String s) {
	REF(image_summary, block);
	block.text.SetData(s);
	ImageSummary_OnValueChange();
}

void BiographyCtrl::ImageSummary_OnKeywords(String s) {
	REF(image_summary, block);
	RemoveEmptyLines2(s);
	Vector<String> parts = Split(s, "\n");
	s = Join(parts, ", ");
	block.keywords.SetData(s);
	ImageSummary_OnValueChange();
}

void BiographyCtrl::ImageSummary_ToolMenu(Bar& bar) {
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(ImageSummary_Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(ImageSummary_Do, 1)).Key(K_F6);
	//bar.Add(t_("Translate"), MetaImgs::BlueRing(), THISBACK(Translate)).Key(K_F5);
	//bar.Add(t_("Make keywords"), MetaImgs::BlueRing(), THISBACK(MakeKeywords)).Key(K_F6);
}

void BiographyCtrl::ImageSummary_Do(int fn) {
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	if (mp.editable_biography) {
		PromptOK(t_("The latest (and editable) revision can't be processed. Select older than latest revision."));
		return;
	}
	
	ImageBiographySummaryProcess& ss = ImageBiographySummaryProcess::Get(*mp.profile, *mp.snap);
	if (fn == 0) {
		ss.Start();
	}
	else if (fn == 1) {
		ss.Stop();
	}
}

void BiographyCtrl::ImageSummary_EntryListMenu(Bar& bar) {
	
}


END_UPP_NAMESPACE
#undef REF
