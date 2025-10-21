#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP


PlatformManagerCtrl::PlatformManagerCtrl() {
	Add(hsplit.VSizePos(0,20).HSizePos());
	
	hsplit.Horz() << platforms << vsplit;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << plat << bottom;
	bottom.Horz() << plat_tabs << epk_tabs;
	bottom.SetPos(3333);
	
	CtrlLayout(plat);
	
	plat_tabs.Add(roles.SizePos(), "Society Roles");
	epk_tabs.Add(epk_text_fields.SizePos(), "EPK Text Fields");
	epk_tabs.Add(epk_photo_types.SizePos(), "EPK Photo Types");
	epk_tabs.Add(epk_photo_prompts.SizePos(), "EPK Photo Prompts");
	
	roles.AddColumn(t_("Role"));
	roles.AddColumn(t_("Description"));
	roles.ColumnWidths("1 4");
	
	epk_text_fields.AddColumn(t_("Key"));
	epk_text_fields.AddColumn(t_("Description"));
	epk_text_fields.ColumnWidths("1 4");
	
	epk_photo_types.AddColumn(t_("Key"));
	epk_photo_types.AddColumn(t_("Description"));
	epk_photo_types.ColumnWidths("1 4");
	
	epk_photo_prompts.AddColumn(t_("Type"));
	epk_photo_prompts.AddColumn(t_("Text"));
	epk_photo_prompts.AddIndex("IDX0");
	epk_photo_prompts.AddIndex("IDX1");
	epk_photo_prompts.ColumnWidths("1 3");
	epk_photo_prompts.WhenCursor << THISBACK(OnPhotoPrompt);
	
	//epk_photo_prompt_split.Horz() << epk_photo_prompts << epk_photo_prompt_example;
	
	platforms.AddColumn(t_("Platform"));
	//platforms.AddColumn(t_("Type"));
	platforms.AddColumn(t_("Family Chosen By Me"));
	platforms.AddColumn(t_("Rights representatives"));
	platforms.AddColumn(t_("Military Rank"));
	platforms.AddColumn(t_("Sex"));
	platforms.AddIndex("IDX");
	platforms.ColumnWidths("2 1 1 1 1");
	
	for(int i = 0; i < PLATFORM_COUNT; i++) {
		const Platform& plat = GetPlatforms()[i];
		platforms.Set(i, 0, plat.name);
		//platforms.Set(i, 1, plat.group);
		platforms.Set(i, "IDX", i);
	}
	//platforms.SetSortColumn(1);
	platforms.SetCursor(0);
	platforms.WhenCursor << THISBACK(DataPlatform);
	platforms.WhenBar << THISBACK(PlatformMenu);
	
	plat.attrs.AddColumn(t_("Attribute"));
	plat.scores.AddColumn(t_("Score group"));
	plat.scores.AddColumn(t_("Score"));
	plat.scores.ColumnWidths("2 3");
	
}

void PlatformManagerCtrl::Data() {
	PlatformManager& data = GetExt<PlatformManager>();
	for(int i = 0; i < PLATFORM_COUNT; i++) {
		const Platform& p = GetPlatforms()[i]; // TODO move to file
		const PlatformAnalysis& pa = data.platforms.GetAdd(p.name);
		platforms.Set(i, 0, p.name);
		platforms.Set(i, "IDX", i);
		platforms.Set(i, 1, pa.GetRoleScoreSumWeighted(data, SOCIETYROLE_SCORE_FAMILY_CHOSEN_BY_ME));
		platforms.Set(i, 2, pa.GetRoleScoreSumWeighted(data, SOCIETYROLE_SCORE_REPRESENTATIVE_FOR_RIGHTS_OF_SOMEONE));
		platforms.Set(i, 3, pa.GetRoleScoreSumWeighted(data, SOCIETYROLE_SCORE_MILTARY_RANK_RELATED));
		
		double female = pa.GetRoleScoreSumWeighted(data, SOCIETYROLE_SCORE_FEMALE);
		double male = pa.GetRoleScoreSumWeighted(data, SOCIETYROLE_SCORE_MALE);
		double sum = female + male;
		if (sum == 0)
			platforms.Set(i, 4, 5);
		else
			platforms.Set(i, 4, female / sum * 10);
	}
	platforms.SetSortColumn(3, true);
	INHIBIT_CURSOR(platforms);
	if (!platforms.IsCursor() && platforms.GetCount())
		platforms.SetCursor(0);
	
	DataPlatform();
}

void PlatformManagerCtrl::DataPlatform() {
	if (!platforms.IsCursor()) {
		plat.name.SetData("");
		plat.group.SetData("");
		plat.description.SetData("");
		plat.attrs.Clear();
		return;
	}
	
	PlatformManager& data = GetExt<PlatformManager>();
	int plat_i = platforms.Get("IDX");
	const Platform& p = GetPlatforms()[plat_i];// TODO move to file
	const PlatformAnalysis& pa = data.platforms.GetAdd(p.name);
	
	if (p.name && p.name[0])
		plat.name.SetData(p.name);
	else
		plat.name.SetData(IntStr(plat_i));
	
	plat.group.SetData(p.group);
	plat.description.SetData(p.description);
	
	int row = 0;
	for(int i = 0; i < PLATFORM_ATTR_COUNT; i++) {
		if (p.attrs[i])
			plat.attrs.Set(row++, 0, GetPlatformAttrKey(i));
	}
	plat.attrs.SetCount(row);
	
	for(int i = 0; i < SOCIETYROLE_SCORE_COUNT; i++) {
		plat.scores.Set(i, 0, GetSocietyRoleScoreKey(i));
		plat.scores.Set(i, 1, pa.GetRoleScoreSum(data, i));
	}
	plat.scores.SetCount(SOCIETYROLE_SCORE_COUNT);
	plat.scores.SetSortColumn(1, true);
	
	for(int i = 0; i < pa.roles.GetCount(); i++) {
		int role_i = pa.roles[i];
		roles.Set(i, 0, GetSocietyRoleKey(role_i));
		roles.Set(i, 1, GetSocietyRoleDescription(role_i));
	}
	roles.SetCount(pa.roles.GetCount());
	
	for(int i = 0; i < pa.epk_text_fields.GetCount(); i++) {
		epk_text_fields.Set(i, 0, pa.epk_text_fields.GetKey(i));
		epk_text_fields.Set(i, 1, pa.epk_text_fields[i]);
	}
	epk_text_fields.SetCount(pa.epk_text_fields.GetCount());
	
	for(int i = 0; i < pa.epk_photos.GetCount(); i++) {
		epk_photo_types.Set(i, 0, pa.epk_photos.GetKey(i));
		epk_photo_types.Set(i, 1, pa.epk_photos[i].description);
	}
	epk_photo_types.SetCount(pa.epk_photos.GetCount());
	
	row = 0;
	for(int i = 0; i < pa.epk_photos.GetCount(); i++) {
		const PlatformAnalysisPhoto& pap = pa.epk_photos[i];
		String group = pa.epk_photos.GetKey(i);
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
}

void PlatformManagerCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Fetch text prompt image"), MetaImgs::BlueRing(), THISBACK1(Do, 2)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Import Json"), MetaImgs::BlueRing(), THISBACK(ImportJson));
	
}

void PlatformManagerCtrl::Do(int fn) {
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	
	PlatformProcess& ss = PlatformProcess::Get(mp);
	if (fn == 0) {
		ss.Start();
	}
	else if (fn == 1) {
		ss.Stop();
	}
	else if (fn == 2) {
		PromptOK("TODO");
	}
}

void PlatformManagerCtrl::PlatformMenu(Bar& bar) {
	bar.Add("Sort by: Family Chosen By Me", THISBACK1(SetSorting, 0));
	bar.Add("Sort by: Rights representatives", THISBACK1(SetSorting, 1));
	bar.Add("Sort by: Military Rank", THISBACK1(SetSorting, 2));
	bar.Add("Sort by: Sex", THISBACK1(SetSorting, 3));
}

void PlatformManagerCtrl::SetSorting(int col) {
	platforms.SetSortColumn(1+col, true);
	
	if (!platforms.IsCursor())
		platforms.SetCursor(0);
}

void PlatformManagerCtrl::OnPhotoPrompt() {
	
}

void PlatformManagerCtrl::ImportJson() {
	DatasetPtrs p; GetDataset(p);
	PlatformManager& data = GetExt<PlatformManager>();
	if (LoadFromJsonFile_VisitorNodePrompt(data)) {
		PostCallback(THISBACK(Data));
	}
}


INITIALIZER_COMPONENT_CTRL(PlatformManager, PlatformManagerCtrl)

END_UPP_NAMESPACE
