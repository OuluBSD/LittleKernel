#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP

void BiographyPlatformCtrl::Clusters::Ctor() {
	this->o.tabs.Add(hsplit.SizePos(), t_("Prompt clusters"));
	
	hsplit.Horz() << image_types << vsplit;
	hsplit.SetPos(1200);
	
	vsplit.Vert() << prompts << final_prompt << bsplit;
	vsplit.SetPos(5000,0);
	
	image_types.AddColumn(t_("Image group"));
	image_types.AddColumn(t_("Prompt count"));
	image_types.AddIndex("IDX");
	image_types.WhenCursor << THISBACK(DataImageType);
	image_types.ColumnWidths("4 1");
	
	prompts.AddColumn(t_("Prompt"));
	
	final_prompt.SetEditable(false);
	
	bsplit.Horz();
	for(int i = 0; i < 4; i++)
		bsplit << epk_photo[i];
}

void BiographyPlatformCtrl::Clusters::Data() {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.analysis) {
		PromptOK("No PlatformManager component found");
		return;
	}
	BiographyPlatform& analysis = o.GetExt<BiographyPlatform>();
	
	analysis.RealizePromptImageTypes();
	
	for(int i = 0; i < analysis.image_types.GetCount(); i++) {
		String key = analysis.image_types.GetKey(i);
		const PhotoPromptGroupAnalysis& ppga = analysis.image_types[i];
		image_types.Set(i, 0, Capitalize(key));
		image_types.Set(i, 1, ppga.image_count);
		image_types.Set(i, "IDX", i);
	}
	image_types.SetSortColumn(1, true);
	INHIBIT_CURSOR(image_types);
	if (!image_types.IsCursor() && image_types.GetCount())
		image_types.SetCursor(0);
	
	DataImageType();
}

void BiographyPlatformCtrl::Clusters::ToolMenu(Bar& bar) {
	/*bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Import Json"), MetaImgs::BlueRing(), THISBACK(ImportJson));*/
}

void BiographyPlatformCtrl::Clusters::Do(int fn) {
	
}

void BiographyPlatformCtrl::Clusters::DataImageType() {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.analysis) {
		PromptOK("No PlatformManager component found");
		return;
	}
	BiographyPlatform& analysis = *mp.analysis;
	
	if (!image_types.IsCursor())
		return;
	
	int img_i = image_types.Get("IDX");
	
	String group = analysis.image_types.GetKey(img_i);
	PhotoPromptGroupAnalysis& ppga = analysis.image_types[img_i];
	Vector<PhotoPromptLink> pps = analysis.GetImageTypePrompts(group);
	
	for(int i = 0; i < pps.GetCount(); i++) {
		PhotoPromptLink& ppl = pps[i];
		prompts.Set(i, 0, ppl.pp->prompt);
	}
	prompts.SetCount(pps.GetCount());
	
	final_prompt.SetData(ppga.prompt);
}


END_UPP_NAMESPACE
