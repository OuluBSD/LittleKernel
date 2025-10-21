#include "Imaging.h"


NAMESPACE_UPP


ReleaseCoverImageCtrl::ReleaseCoverImageCtrl() {
	this->Add(vsplit.SizePos());
	
	vsplit.Vert();
	for(int i = 0; i < 3; i++)
		vsplit << hsplit[i];
	
	hsplit[0].Horz() << attr_list << attr_text;
	hsplit[1].Horz() << sugg_list << sugg_text;
	hsplit[2].Horz();
	for(int i = 0; i < 4; i++)
		hsplit[2] << suggestion[i];
	hsplit[0].SetPos(2500);
	hsplit[1].SetPos(2500);
	
	attr_list.AddColumn(t_("Attribute"));
	//attr_list.Set(0, 0, t_("Lyrics combined"));
	//attr_list.Set(1, 0, t_("Lyric summaries combined"));
	for(int i = 0; i < SNAPANAL_COUNT; i++) {
		attr_list.Set(i, 0, GetSnapshotAnalysisKey(i));
	}
	attr_list.SetCursor(0);
	attr_list.WhenCursor << THISBACK(DataAttribute);
	attr_text.WhenAction << THISBACK(OnAttributeChange);
	
	sugg_list.AddColumn(t_("Cover suggestion"));
	sugg_list.WhenCursor << THISBACK(DataSuggestion);
	sugg_text.WhenAction << THISBACK(OnSuggestionChange);
	
	sugg_list.WhenBar << THISBACK(SuggestionMenu);
}

void ReleaseCoverImageCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	if (!p.release) return;
	
	for(int i = 0; i < p.release->cover_suggestions.GetCount(); i++) {
		sugg_list.Set(i, 0, p.release->cover_suggestions[i]);
	}
	INHIBIT_CURSOR(sugg_list);
	sugg_list.SetCount(p.release->cover_suggestions.GetCount());
	if (!sugg_list.IsCursor() && sugg_list.GetCount())
		sugg_list.SetCursor(0);
	
	DataAttribute();
	DataSuggestion();
}

void ReleaseCoverImageCtrl::DataAttribute() {
	DatasetPtrs p; GetDataset(p);
	if (!p.release) return;
	
	if (!attr_list.IsCursor())
		return;
	
	int attr_i = attr_list.GetCursor();
	if (attr_i < 0 || attr_i >= p.release->analysis.GetCount())
		return;
	
	attr_text.SetData(p.release->analysis[attr_i]);
}

void ReleaseCoverImageCtrl::DataSuggestion() {
	DatasetPtrs p; GetDataset(p);
	if (!p.release) return;
	
	if (!sugg_list.IsCursor())
		return;
	
	int sugg_i = sugg_list.GetCursor();
	if (sugg_i < 0 || sugg_i >= p.release->cover_suggestions.GetCount())
		return;
	
	sugg_text.SetData(p.release->cover_suggestions[sugg_i]);
	
	DataSuggestionImage();
}

void ReleaseCoverImageCtrl::DataSuggestionImage() {
	DatasetPtrs p; GetDataset(p);
	if (!p.release) return;
	
	if (!sugg_list.IsCursor())
		return;
	
	int sugg_i = sugg_list.GetCursor();
	if (sugg_i < 0 || sugg_i >= p.release->cover_suggestions.GetCount())
		return;
	
	String title = IntStr64(p.release->cover_suggestions[sugg_i].GetHashValue());
	
	//const String& dir = MetaDatabase::Single().dir;
	//const String& share = MetaDatabase::Single().share;
	//String img_dir = dir + DIR_SEPS + share + DIR_SEPS + GetAppModeDir() + DIR_SEPS + "images" + DIR_SEPS;
	//String img_dir = ConfigFile("images");
	
	TODO
	#if 0
	String img_dir = AppendFileName(MetaDatabase::GetDirectory(), "images" DIR_SEPS "full");
	RealizeDirectory(img_dir);
	for(int i = 0; i < 4; i++) {
		String path = AppendFileName(img_dir, title + "_" + IntStr(i) + ".jpg");
		if (FileExists(path))
			suggestion[i].SetImage(StreamRaster::LoadFileAny(path));
		else
			suggestion[i].Clear();
	}
	#endif
}

void ReleaseCoverImageCtrl::OnAttributeChange() {
	DatasetPtrs p; GetDataset(p);
	if (!p.release) return;
	
	if (!attr_list.IsCursor())
		return;
	
	int attr_i = attr_list.GetCursor();
	if (attr_i >= p.release->analysis.GetCount())
		p.release->analysis.SetCount(attr_i);
	
	p.release->analysis[attr_i] = attr_text.GetData();
}

void ReleaseCoverImageCtrl::OnSuggestionChange() {
	DatasetPtrs p; GetDataset(p);
	if (!p.release) return;
	
	if (!sugg_list.IsCursor())
		return;
	
	int sugg_i = sugg_list.GetCursor();
	if (sugg_i >= p.release->cover_suggestions.GetCount())
		p.release->cover_suggestions.SetCount(sugg_i);
	
	p.release->cover_suggestions[sugg_i] = sugg_text.GetData();
}

void ReleaseCoverImageCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Make all images"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	
	/*bar.Add(t_("Create suggestions for prompts"), MetaImgs::Part(), THISBACK(CreateSuggestionsForPrompts)).Key(K_F5);
	bar.Add(t_("Make single image"), MetaImgs::Part(), THISBACK(MakeSingleImage)).Key(K_F6);
	bar.Add(t_("Make all images"), MetaImgs::Part(), THISBACK(MakeAllImages)).Key(K_F7);
	*/
}

void ReleaseCoverImageCtrl::Do(int fn) {
	DatasetPtrs p; GetDataset(p);
	if (!p.release) return;
	
	if (fn == 0) {
		SnapSolver& tm = SnapSolver::Get(*p.release);
		tm.Start();
	}
}

void ReleaseCoverImageCtrl::CreateSuggestionsForPrompts() {
	DatasetPtrs p; GetDataset(p);
	if(!p.song || !p.entity || !p.release)
		return;
	
	Release& rel = *p.release;
	
	
}

void ReleaseCoverImageCtrl::SuggestionMenu(Bar& bar) {
	bar.Add(t_("Add suggestion"), MetaImgs::RedRing(), [this]() {
		DatasetPtrs p; GetDataset(p);
		p.release->cover_suggestions.Add();
		PostCallback(THISBACK(Data));
	});
	
}

INITIALIZER_COMPONENT_CTRL(ReleaseCoverImage, ReleaseCoverImageCtrl)

END_UPP_NAMESPACE
