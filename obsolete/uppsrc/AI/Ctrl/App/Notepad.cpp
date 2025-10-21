#include "App.h"

NAMESPACE_UPP


NotepadCtrl::NotepadCtrl() {
	CtrlLayout(idea);
	
	Add(hsplit.SizePos());
	hsplit.Horz();
	hsplit << list << idea;
	hsplit.SetPos(2000);
	
	list.AddColumn("Title");
	list.AddColumn("Created");
	list.AddIndex("IDX");
	list.ColumnWidths("2 1");
	//list.NoHeader();
	list.WhenBar << THISBACK(OnListMenu);
	list.WhenCursor << THISBACK(DataNote);
	
	idea.title.WhenAction << THISBACK(OnValueChange);
	idea.target.WhenAction << THISBACK(OnValueChange);
	idea.ref.WhenAction << THISBACK(OnValueChange);
	idea.desc.WhenAction << THISBACK(OnValueChange);
}

void NotepadCtrl::Clear() {
	idea.title.Clear();
	idea.target.Clear();
	idea.ref.Clear();
	idea.desc.Clear();
}

void NotepadCtrl::EditPos(JsonIO& json) {
	int notepad_list = list.IsCursor() ? list.GetCursor() : -1;
	json	("notepad_list", notepad_list);
	if (json.IsLoading()) {
		PostCallback([=] {
			if (notepad_list >= 0 && notepad_list < list.GetCount())
				list.SetCursor(notepad_list);
		});
	}
}

void NotepadCtrl::Data() {
	Notepad& np = GetExt<Notepad>();
	
	for(int i = 0; i < np.notes.GetCount(); i++) {
		const auto& idea = np.notes[i];
		list.Set(i, 0, idea.title);
		list.Set(i, 1, idea.created);
		list.Set(i, "IDX", i);
	}
	list.SetCount(np.notes.GetCount());
	list.SetSortColumn(1, true);
	
	if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	else
		DataNote();
}

void NotepadCtrl::DataNote() {
	DatasetPtrs p; GetDataset(p);
	if (!list.IsCursor()) {
		Clear();
		return;
	}
	Notepad& np = GetExt<Notepad>();
	int idea_idx = list.Get("IDX");
	const auto& obj = np.notes[idea_idx];
	
	this->idea.title.SetData(obj.title);
	this->idea.target.SetData(obj.outcome);
	this->idea.ref.SetData(obj.reference);
	this->idea.desc.SetData(obj.description);
}

void NotepadCtrl::OnValueChange() {
	if (!list.IsCursor()) return;
	Notepad& np = GetExt<Notepad>();
	int idea_idx = list.Get("IDX");
	auto& obj = np.notes[idea_idx];
	
	obj.title = idea.title.GetData();
	obj.outcome = idea.target.GetData();
	obj.reference = idea.ref.GetData();
	obj.description = idea.desc.GetData();
	
	list.Set(0, obj.title);
}

void NotepadCtrl::OnListMenu(Bar& bar) {
	bar.Add(t_("Add Idea"), THISBACK(AddIdea));
	bar.Add(t_("Remove Idea"), THISBACK(RemoveIdea));
}

void NotepadCtrl::AddIdea() {
	Notepad& np = GetExt<Notepad>();
	auto& note = np.notes.Add();
	note.created = GetSysTime();
	PostCallback([this]{
		Data();
		if (list.GetCount())
			list.SetCursor(0);
	});
}

void NotepadCtrl::RemoveIdea() {
	if (!list.IsCursor()) return;
	Notepad& np = GetExt<Notepad>();
	int idx = list.Get("IDX");
	np.notes.Remove(idx);
	PostCallback(THISBACK(Data));
}


INITIALIZER_COMPONENT_CTRL(Notepad, NotepadCtrl)

END_UPP_NAMESPACE
