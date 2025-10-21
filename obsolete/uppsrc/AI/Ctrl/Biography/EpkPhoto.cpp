#include "Biography.h"

NAMESPACE_UPP

void BiographyPlatformCtrl::Platforms::EpkPhoto::Ctor() {
	this->p.tabs.Add(epk_photo_prompt_split.SizePos(), t_("EPK Photo prompts"));
	
	epk_photo_prompt_split.Vert() << epk_photo_prompts << epk_photo_multi_image_split;
	epk_photo_multi_image_split.Horz();
	for(int i = 0; i < 4; i++)
		epk_photo_multi_image_split << epk_photo[i];
	
	epk_photo_prompts.AddColumn(t_("Type"));
	epk_photo_prompts.AddColumn(t_("Text"));
	epk_photo_prompts.AddIndex("IDX0");
	epk_photo_prompts.AddIndex("IDX1");
	epk_photo_prompts.ColumnWidths("1 3");
	epk_photo_prompts.WhenCursor << THISBACK(OnPhotoPrompt);
	epk_photo_prompts.WhenBar << THISBACK(PhotoPromptMenu);
	
}

void BiographyPlatformCtrl::Platforms::EpkPhoto::ToolMenu(Bar& bar) {
	/*bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Import Json"), MetaImgs::BlueRing(), THISBACK(ImportJson));*/
}

void BiographyPlatformCtrl::Platforms::EpkPhoto::DataPlatform() {
	DatasetPtrs mp; o.GetDataset(mp);
	BiographyPlatform& analysis = o.GetExt<BiographyPlatform>();
	int plat_i = p.platforms.Get("IDX");
	const Platform& pl = GetPlatforms()[plat_i];
	const PlatformAnalysis& pa = mp.platmgr->GetPlatform(plat_i);
	if (plat_i >= analysis.platforms.GetCount()) return;
	const PlatformBiographyPlatform& pba = analysis.platforms[plat_i];
	
	
	int row = 0;
	for(int i = 0; i < pba.epk_photos.GetCount(); i++) {
		const PlatformAnalysisPhoto& pap = pba.epk_photos[i];
		String group = pba.epk_photos.GetKey(i);
		for(int j = 0; j < pap.prompts.GetCount(); j++) {
			const PhotoPrompt& pp = pap.prompts[j];
			epk_photo_prompts.Set(row, 0, group);
			epk_photo_prompts.Set(row, 1, pp.prompt);
			epk_photo_prompts.Set(row, "IDX0", i);
			epk_photo_prompts.Set(row, "IDX1", j);
			row++;
		}
	}
	epk_photo_prompts.SetCount(row);
	if (row && !epk_photo_prompts.IsCursor())
		epk_photo_prompts.SetCursor(0);
	
	OnPhotoPrompt();
}

void BiographyPlatformCtrl::Platforms::EpkPhoto::PhotoPromptMenu(Bar& bar) {
	
	bar.Add("Set as groups Top 1 prompt", [this]() {
		if (!p.platforms.IsCursor() || !epk_photo_prompts.IsCursor())
		return;
	
		DatasetPtrs mp; o.GetDataset(mp);
		BiographyPlatform& analysis = *mp.analysis;
		int plat_i = p.platforms.Get("IDX");
		const Platform& pl = GetPlatforms()[plat_i];
		const PlatformAnalysis& pa = mp.platmgr->GetPlatform(plat_i);
		if (plat_i >= analysis.platforms.GetCount()) return;
		PlatformBiographyPlatform& pba = analysis.platforms[plat_i];
		
		int i = epk_photo_prompts.Get("IDX0");
		int j = epk_photo_prompts.Get("IDX1");
		PlatformAnalysisPhoto& pap = pba.epk_photos[i];
		if (j > 0) {
			PhotoPrompt& pp0 = pap.prompts[0];
			PhotoPrompt& pp1 = pap.prompts[j];
			Swap(pp0, pp1);
		}
		o.PostCallback(THISBACK(DataPlatform));
	});
	bar.Add("Edit prompt", [this]() {
		if (!p.platforms.IsCursor() || !epk_photo_prompts.IsCursor())
		return;
	
		DatasetPtrs mp; o.GetDataset(mp);
		BiographyPlatform& analysis = *mp.analysis;
		int plat_i = p.platforms.Get("IDX");
		const Platform& pl = GetPlatforms()[plat_i];
		const PlatformAnalysis& pa = mp.platmgr->GetPlatform(plat_i);
		if (plat_i >= analysis.platforms.GetCount()) return;
		PlatformBiographyPlatform& pba = analysis.platforms[plat_i];
		
		int i = epk_photo_prompts.Get("IDX0");
		int j = epk_photo_prompts.Get("IDX1");
		PlatformAnalysisPhoto& pap = pba.epk_photos[i];
		PhotoPrompt& pp = pap.prompts[j];
		
		bool b = EditTextNotNull(
			pp.prompt,
			t_("Prompt text"),
			"",
			0
		);
		o.PostCallback(THISBACK(DataPlatform));
	});
}

void BiographyPlatformCtrl::Platforms::EpkPhoto::OnPhotoPrompt() {
	if (!o.p.platforms.IsCursor() || !epk_photo_prompts.IsCursor())
		return;
	
	DatasetPtrs mp; o.GetDataset(mp);
	String dir = GetFileDirectory(o.GetFilePath());
	
	BiographyPlatform& analysis = *mp.analysis;
	int plat_i = p.platforms.Get("IDX");
	const Platform& pl = GetPlatforms()[plat_i];
	const PlatformAnalysis& pa = mp.platmgr->GetPlatform(plat_i);
	if (plat_i >= analysis.platforms.GetCount()) return;
	const PlatformBiographyPlatform& pba = analysis.platforms[plat_i];
	
	int i = epk_photo_prompts.Get("IDX0");
	int j = epk_photo_prompts.Get("IDX1");
	const PlatformAnalysisPhoto& pap = pba.epk_photos[i];
	const PhotoPrompt& pp = pap.prompts[j];
	
	for(int i = 0; i < 4; i++) {
		String path = pp.GetFilePath(dir, i);
		if (FileExists(path)) {
			Image img = StreamRaster::LoadFileAny(path);
			epk_photo[i].SetImage(img);
		}
		else {
			epk_photo[i].Clear();
		}
	}
}

void BiographyPlatformCtrl::Platforms::Do(int fn) {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	String dir = GetFileDirectory(o.GetFilePath());
	PlatformProfileProcess& ss = PlatformProfileProcess::Get(mp, dir);
	if (fn == 0) {
		ss.Start();
	}
	else if (fn == 1) {
		ss.Stop();
	}
}

END_UPP_NAMESPACE
