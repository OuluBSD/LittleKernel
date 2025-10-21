#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP


void BiographyPlatformCtrl::Platforms::Ctor() {
	this->o.tabs.Add(hsplit.SizePos(), t_("Platforms"));
	
	hsplit.Horz(platforms, tabs);
	hsplit.SetPos(2000);
	
	platforms.AddColumn(t_("Platform"));
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
	platforms.WhenBar << THISBACK(Menu);
	
	header.Ctor();
	messaging.Ctor();
	needs.Ctor();
	marketplace.Ctor();
	epk_photo.Ctor();
}

void BiographyPlatformCtrl::Platforms::Menu(Bar& bar) {
	
}

void BiographyPlatformCtrl::Platforms::Data() {
	DatasetPtrs ptrs;
	o.GetDataset(ptrs);
	if (!ptrs.platmgr) {
		PromptOK("No PlatformManager component found");
		return;
	}
	PlatformManager& plat = *ptrs.platmgr;
	platforms.Clear();
	for(int i = 0; i < PLATFORM_COUNT; i++) {
		const Platform& pl = GetPlatforms()[i];
		const PlatformAnalysis& pa = ptrs.platmgr->GetPlatform(i);
		platforms.Set(i, 0, pl.name);
		platforms.Set(i, "IDX", i);
		platforms.Set(i, 1, pa.GetRoleScoreSumWeighted(plat, SOCIETYROLE_SCORE_FAMILY_CHOSEN_BY_ME));
		platforms.Set(i, 2, pa.GetRoleScoreSumWeighted(plat, SOCIETYROLE_SCORE_REPRESENTATIVE_FOR_RIGHTS_OF_SOMEONE));
		platforms.Set(i, 3, pa.GetRoleScoreSumWeighted(plat, SOCIETYROLE_SCORE_MILTARY_RANK_RELATED));
		
		double female = pa.GetRoleScoreSumWeighted(plat, SOCIETYROLE_SCORE_FEMALE);
		double male = pa.GetRoleScoreSumWeighted(plat, SOCIETYROLE_SCORE_MALE);
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
void BiographyPlatformCtrl::Platforms::DataPlatform() {
	if (!platforms.IsCursor())
		return;
	
	int tab = tabs.Get();
	if (tab == 0) header.DataPlatform();
	if (tab == 1) messaging.DataPlatform();
	if (tab == 2) epk_photo.DataPlatform();
	if (tab == 3) needs.DataPlatform();
	if (tab == 4) marketplace.DataPlatform();
}

void BiographyPlatformCtrl::Platforms::ToolMenu(Bar& bar) {
	int tab = tabs.Get();
	if (tab == 0) header.ToolMenu(bar);
	if (tab == 1) messaging.ToolMenu(bar);
	if (tab == 2) epk_photo.ToolMenu(bar);
	if (tab == 3) needs.ToolMenu(bar);
	if (tab == 4) marketplace.ToolMenu(bar);
}


END_UPP_NAMESPACE
