#include "App.h"

NAMESPACE_UPP

INITIALIZER_COMPONENT_CTRL(ChainThread, AiChainCtrl)

AiChainCtrl::AiChainCtrl() {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << msplit << structure << rsplit;
	hsplit.SetPos(1500,0).SetPos(5750,1);
	
	msplit.Vert() << session;
	
	session.AddColumn("Session");
	session.AddColumn("Version");
	session.AddIndex("IDX");
	session.WhenBar = THISBACK(SessionMenu);
	session.WhenCursor = THISBACK(DataSession);
	
	structure.WhenBar = THISBACK(StageMenu);
	structure.WhenCursor = THISBACK(DataItem);
	
}

void AiChainCtrl::Data() {
	if (!ext) return;
	auto& list = this->session;
	ChainThread& t = dynamic_cast<ChainThread&>(*ext);
	VfsValue& n = GetValue();
	sessions = n.FindTypeAllShallow(AsTypeHash<ChainThread>());
	for(int i = 0; i < sessions.GetCount(); i++) {
		const auto& it = *sessions[i];
		list.Set(i, 0, it.id);
		list.Set(i, 1, it.GetTypeString());
		list.Set(i,"IDX", i);
	}
	list.SetCount(sessions.GetCount());
	if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	else
		DataSession();
}

void AiChainCtrl::DataSession() {
	VfsValue* ses = GetSession();
	if (!ses) {
		return;
	}
	
	structure_nodes.Clear();
	structure_nodes.Add(0, ses);
	structure.Clear();
	structure.SetRoot(MetaImgs::StageRoot(), "Session");
	structure_values.Clear();
	VisitNode(0, *ses, "");
	structure.OpenDeep(0);
	
	
	
}

void AiChainCtrl::DataItem() {
	
}

void AiChainCtrl::ToolMenu(Bar& bar) {
	
}

void AiChainCtrl::VisitNode(int tree_i, VfsValue& n, String path) {
	for(int i = 0; i < n.sub.GetCount(); i++) {
		auto& s = n.sub[i];
		/*if (s.kind == METAKIND_ECS_COMPONENT_AI_CHAIN_EXAMPLE)
			continue;*/
		Image img;
		bool is_value_node = false;
		/*switch (s.kind) {
			case METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_NODE:          img = MetaImgs::StageNode(); break;
			case METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_QUERY:         img = MetaImgs::StageQuery(); break;
			case METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_RESPONSE:      img = MetaImgs::StageResponse(); break;
			case METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_COMMENT:       img = MetaImgs::StageComment(); break;
			case METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_EXAMPLE_VALUE: img = MetaImgs::StageExample(); is_value_node = true; break;
			case METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_INPUT_VALUE:   img = MetaImgs::StageValue();   is_value_node = true; break;
		}*/
		TODO
		int cur = structure.Add(tree_i, img, s.id);
		structure_nodes.Add(cur, &s);
		String s_path = path + "." + s.id;
		if (is_value_node) {
			/*
			Value val = GetExampleValue(s_path);
			String str = val.ToString();
			int val_cur = structure.Add(cur, MetaImgs::StageDynamic(), str);
			structure_values.Add(val_cur, s_path);*/
		}
		VisitNode(cur, s, s_path);
	}
}

VfsValue* AiChainCtrl::GetSession() {
	if (!session.IsCursor())
		return 0;
	int ses_i = session.Get("IDX");
	return sessions[ses_i];
}

void AiChainCtrl::SessionMenu(Bar& b) {
	b.Add("Add session", THISBACK(AddSession));
	b.Add("Remove session", THISBACK(RemoveSession));
	b.Add("Rename session", THISBACK(RenameSession));
	b.Add("Duplicate session", THISBACK(DuplicateSession));
	b.Add("Set session's version", THISBACK(SetSessionVersion));
}

void AiChainCtrl::AddSession() {
	String name;
	if (!EditText(name, "Session's name", "Name"))
		return;
	ChainThread& t = dynamic_cast<ChainThread&>(*ext);
	for(int i = 0; i < sessions.GetCount(); i++) {
		if (sessions[i]->id == name) {
			PromptOK("Session with that name exists already");
			return;
		}
	}
	auto& ses = GetValue().Add<ChainThread>(name);
	ses.val.value = "1";
	PostCallback(THISBACK(Data));
}

void AiChainCtrl::RemoveSession() {
	if (!session.IsCursor())
		return;
	int ses_id = session.Get("IDX");
	VfsValue* ses = sessions[ses_id];
	ChainThread& t = dynamic_cast<ChainThread&>(*ext);
	GetValue().Remove(ses);
	PostCallback(THISBACK(Data));
}

void AiChainCtrl::RenameSession() {
	if (!session.IsCursor())
		return;
	int ses_id = session.Get("IDX");
	ChainThread& t = dynamic_cast<ChainThread&>(*ext);
	auto& ses = *sessions[ses_id];
	String name = ses.id;
	if (!EditText(name, "Session's name", "Name"))
		return;
	ses.id = name;
	PostCallback(THISBACK(Data));
}

void AiChainCtrl::SetSessionVersion() {
	if (!session.IsCursor())
		return;
	int ses_id = session.Get("IDX");
	ChainThread& t = dynamic_cast<ChainThread&>(*ext);
	auto& ses = *sessions[ses_id];
	AstValue& a = ses;
	String s = a.type;
	if (!EditText(s, "Session's version", "Version"))
		return;
	a.type = s;
	PostCallback(THISBACK(Data));
}

void AiChainCtrl::DuplicateSession() {
	if (!session.IsCursor())
		return;
	int ses_id = session.Get("IDX");
	ChainThread& t = dynamic_cast<ChainThread&>(*ext);
	const auto& ses0 = *sessions[ses_id];
	auto& ses1 = GetValue().Add<ChainThread>("");
	VisitCopy(ses0, ses1.val);
	PostCallback(THISBACK(Data));
}

void AiChainCtrl::StageMenu(Bar& b) {
		if (structure.IsCursor()) {
		int cur = structure.GetCursor();
		int val_i = structure_values.Find(cur);
		/*if (val_i >= 0) {
			b.Add("Set String", THISBACK1(EditExampleValue, true));
			b.Add("Set JSON", THISBACK1(EditExampleValue, false));
			return;
		}*/
		/*b.Add("Add comment", THISBACK1(AddStageNode, METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_COMMENT));
		if (!cur) {
			b.Add("Add query", THISBACK1(AddStageNode, METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_QUERY));
			b.Add("Add response", THISBACK1(AddStageNode, METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_RESPONSE));
		}
		else {
			b.Add("Add", THISBACK1(AddStageNode, METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_NODE));
			b.Add("Add example value", THISBACK1(AddStageNode, METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_EXAMPLE_VALUE));
			b.Add("Add input value", THISBACK1(AddStageNode, METAKIND_ECS_COMPONENT_AI_CHAIN_PROMPT_INPUT_VALUE));
		}*/
		b.Add("Remove", THISBACK(RemoveStageNode));
		b.Add("Rename", THISBACK(RenameStageNode));
		/*if (!cur) {
			b.Separator();
			b.Add("Make variation", Callback());
			b.Sub("Templates", [this](Bar& b) {
				if (structure_nodes.GetCount() > 1) {
					b.Add("Save template", THISBACK(SaveTemplate));
					b.Separator();
				}
				b.Sub("Remove", [this](Bar& b) {
					auto tmpls = GetValue().AstFindAllShallow(METAKIND_ECS_COMPONENT_AI_CHAIN_SESSION_TEMPLATE);
					for(int i = 0; i < tmpls.GetCount(); i++) {
						VfsValue& n = *tmpls[i];
						b.Add(n.id, THISBACK1(RemoveTemplate, &n));
					}
				});
				auto tmpls = GetValue().AstFindAllShallow(METAKIND_ECS_COMPONENT_AI_CHAIN_SESSION_TEMPLATE);
				for(int i = 0; i < tmpls.GetCount(); i++) {
					VfsValue& n = *tmpls[i];
					b.Add(n.id, THISBACK1(LoadTemplate, &n));
				}
			});
		}*/
	}
}

#if 0
void AiChainCtrl::AddStageNode(hash_t type_hash) {
	if (!structure.IsCursor()) return;
	int cur = structure.GetCursor();
	int i = structure_nodes.Find(cur);
	if (i < 0) {PromptOK("Internal error"); return;}
	VfsValue& n = *structure_nodes[i];
	String id;
	if (!EditText(id, "Node's id", "id"))
		return;
	VfsValue& s = n.AddType(type_hash, id);
	PostCallback(THISBACK(Data));
}
#endif

void AiChainCtrl::RenameStageNode() {
	if (!structure.IsCursor()) return;
	int cur = structure.GetCursor();
	int i = structure_nodes.Find(cur);
	if (i < 0) {PromptOK("Internal error"); return;}
	VfsValue& n = *structure_nodes[i];
	String id = n.id;
	if (!EditText(id, "Node's id", "id"))
		return;
	n.id = id;
	structure.Set(cur, id);
}

void AiChainCtrl::RemoveStageNode() {
	if (!structure.IsCursor()) return;
	int cur = structure.GetCursor();
	if (!cur) return; // can't remove root
	int i = structure_nodes.Find(cur);
	if (i < 0) {PromptOK("Internal error"); return;}
	VfsValue& n = *structure_nodes[i];
	int owner = structure.GetParent(cur);
	i = structure_nodes.Find(owner);
	if (i < 0) {PromptOK("Internal error"); return;}
	VfsValue& o = *structure_nodes[i];
	o.Remove(&n);
	PostCallback(THISBACK(Data));
}

END_UPP_NAMESPACE
