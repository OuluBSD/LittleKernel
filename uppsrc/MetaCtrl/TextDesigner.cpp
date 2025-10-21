#include "MetaCtrl.h"
#include <ide/ide.h>

NAMESPACE_UPP


SolverBaseIndicator::SolverBaseIndicator() {
	ProgressIndicator::Set(0,1);
	
}

void SolverBaseIndicator::Attach(SolverBase& sb) {
	Ptr<SolverBase> sbp = &sb;
	sb.WhenProgress << [this,sbp](int a, int t) {if (sbp) PostCallback(THISBACK2(SetProgress, a,t));};
}

void SolverBaseIndicator::SetProgress(int a, int t) {
	ProgressIndicator::Set(a, t);
}



ToolAppCtrl::~ToolAppCtrl() {
	// '!Thread::IsShutdownThreads' prevents crash on ide destruction phase
	if (TheIde() && !Thread::IsShutdownThreads() && TheIde()->addon_ctrl == this) {
		TheIde()->addon_menu.Clear();
		PostCallback([]{TheIde()->SetBar();});
	}
}

bool ToolAppCtrl::IsScript() const {
	DatasetPtrs p;
	GetDataset(p);
	return p.script;
}

bool ToolAppCtrl::HasPointers() const {
	// TODO remove this function
	return true;
}

Script& ToolAppCtrl::GetScript() {
	DatasetPtrs p;
	GetDataset(p);
	if(!p.script)
		throw NoPointerExc("no scripts");
	return *p.script;
}

Component& ToolAppCtrl::GetComponent() {
	DatasetPtrs p;
	GetDataset(p);
	if(!p.song || !p.entity)
		throw NoPointerExc("no song");
	return *p.song;
}

void ToolAppCtrl::AddMenu() {
	//AddFrame(menu);
	//ToolMenu(menu);
	UpdateMenu();
}

void ToolAppCtrl::UpdateMenu() {
	TheIde()->addon_ctrl = this;
	TheIde()->addon_menu = [this](Bar& b){
		auto* ctrl = &*TheIde()->addon_ctrl;
		if (ctrl) {
			ToolAppCtrl* c = dynamic_cast<ToolAppCtrl*>(ctrl);
			if (c)
				c->ToolMenu(b);
		}
	};
	PostCallback([]{TheIde()->SetBar();});
}

Entity& ToolAppCtrl::GetEntity() {
	DatasetPtrs p;
	GetDataset(p);
	if(!p.entity)
		throw NoPointerExc("no artist");
	return *p.entity;
}

String ToolAppCtrl::GetComponentTitle() const {
	/*DatasetPtrs p = GetDataset();
	DatasetPtrs p = GetDataset();
	if(!p.song || !p.entity)
		throw NoPointerExc("no song");
	Component& song = *p.song;
	Entity& artist = *p.entity;
	String s;
	s << artist.english_name << " - " << song.english_title;
	return s;*/
	TODO
	return "";
}

void ToolAppCtrl::GetDataset(DatasetPtrs& p) const {
	VfsValue* n = GetFileNode();
	if (n)
		FillDataset(p, *n, dynamic_cast<Component*>(const_cast<ToolAppCtrl*>(this)));
}

/*const Index<String>& ToolAppCtrl::GetTypeclasses() const {
	TODO static Index<String> i; return i;
}

const Vector<ContentType>& ToolAppCtrl::GetContents() const {
	TODO static Vector<ContentType> i; return i;
}

const Vector<String>& ToolAppCtrl::GetContentParts() const {
	TODO static Vector<String> i; return i;
}*/

/*int ToolAppCtrl::GetDataset() {
	DatasetPtrs p = GetDataset();
	DatasetPtrs p = GetDataset();
	if(!p.song || !p.entity)
		throw NoPointerExc("no song");
	Component& song = *p.song;
	return ScanInt(song.data.Get("ATTR_DATASET", "0"));
}*/

void ToolAppCtrl::GetAttrs(const VectorMap<String,String>& data, VectorMap<String,String>& v) {
	for(int i = 0; i < ATTR_COUNT; i++) {
		const char* key = AttrKeys[i][0];
		int value = StrInt(data.Get(key, "0"));
		
		if (value) {
			if (value > 0) {
				v.GetAdd(key) = AttrKeys[i][2];
			}
			else {
				v.GetAdd(key) = AttrKeys[i][3];
			}
		}
	}
}

void ToolAppCtrl::MakeComponentParts(ArrayCtrl& parts) {
	/*Component& song = GetComponent();
	
	for(int i = 0; i < song.__parts.GetCount(); i++) {
		StaticPart& sp = song.__parts[i];
		//parts.Set(i, 0, sp.name);
		//parts.Set(i, 1, part.GetTypeString());
		parts.Set(i, 0,
			AttrText(sp.name).NormalPaper(GetComponentPartPaperColor(sp.type)));
		
		DropList& dl = parts.CreateCtrl<DropList>(i, 1);
		for(int j = 0; j < StaticPart::PART_TYPE_COUNT; j++)
			dl.Add(StaticPart::GetTypeString(j));
		dl.SetIndex((int&)sp.part_type);
		dl.WhenAction << [&dl,i,&sp]() {(int&)sp.part_type = dl.GetIndex();};
	}
	INHIBIT_CURSOR(parts);
	parts.SetCount(song.__parts.GetCount());
	if (!parts.IsCursor() && parts.GetCount())
		parts.SetCursor(0);*/
}

void ToolAppCtrl::Realize(const String& includes, const String& filename) {
	data_includes = includes;
	data_filepath = filename;
}

bool ToolAppCtrl::Load(const String& includes, const String& filename, Stream& in, byte charset) {
	data = in.Get((int)in.GetSize());
	data_includes = includes;
	data_filepath = filename;
	VisitFromJson(*this, data);
	return true;
}

bool ToolAppCtrl::LoadDirectory(const String& includes, const String& filename, const String& dirpath, byte charset) {
	data.Clear();
	data_includes = includes;
	data_filepath = filename;
	if (DirectoryExists(dirpath)) {
		VersionControlSystem vcs;
		vcs.Initialize(dirpath, false);
		Vis vis(vcs);
		this->Visit(vis);
		vcs.Close();
	}
	return true;
}

void ToolAppCtrl::Save(Stream& s, byte charset) {
	String new_data;
	if (!DoVisitToJson(*this, new_data, true)) {
		s.SetError(1);
		return;
	}
	if (!new_data.IsEmpty())
		data = new_data;
	s.Put(data);
}

void ToolAppCtrl::SaveDirectory(String dirpath, byte charset) {
	VersionControlSystem vcs;
	vcs.Initialize(dirpath, true);
	Vis vis(vcs);
	this->Visit(vis);
	vcs.Close();
	data.Clear();
}

void ToolAppCtrl::EditPos(JsonIO& json) {
	
}

void ToolAppCtrl::SetPickUndoData(LineEdit::UndoData undodata) {
	
}


END_UPP_NAMESPACE
