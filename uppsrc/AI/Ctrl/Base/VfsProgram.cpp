#include <AI/Ctrl/Ctrl.h>


NAMESPACE_UPP


INITIALIZER_COMPONENT_CTRL(VfsProgram, VfsProgramCtrl)


VfsProgramCtrl::VfsProgramCtrl() {
	
}

VfsProgramCtrl::MainTab::MainTab(VfsProgramCtrl& o, const VirtualNode& vnode) : o(o), VNodeComponentCtrl(o, vnode) {
	Add(vsplit.SizePos());
	
	vsplit.Vert() << hsplit << btabs;
	vsplit.SetPos(8000);
	
	btabs.Add(log.SizePos(), "Log");
	
	hsplit.Horz() << prjsplit << ltabs << rtabs;
	hsplit.SetPos(1000,0).SetPos(5000,1);
	
	ltabs.Add(prj_code.SizePos(), "Project Code");
	ltabs.Add(iter_code.SizePos(), "Iteration Code");
	
	rtabs.Add(memoryctrl.SizePos(), "Memory");
	rtabs.Add(stagectrl.SizePos(), "Stage");
	rtabs.Add(formedit.SizePos(), "Form");
	rtabs.WhenSet = [this]{this->o.WhenSaveEditPos();};
	
	stagectrl.Horz() << stagesplit << stage;
	stagectrl.SetPos(2000);
	memoryctrl.Vert() << memtree;
	
	prjsplit.Vert() << prjlist << sessionlist << iterlist;
	
	prjlist.AddColumn("Project");
	prjlist.AddIndex("IDX");
	prjlist.WhenBar = THISBACK(ProjectMenu);
	prjlist.WhenCursor = THISBACK(DataProject);
	
	sessionlist.AddColumn("Session");
	sessionlist.AddIndex("IDX");
	sessionlist.WhenBar = THISBACK(SessionMenu);
	sessionlist.WhenCursor = THISBACK1(DataSession, true);
	
	iterlist.AddColumn("#");
	iterlist.AddColumn("Iteration");
	iterlist.AddIndex("IDX");
	iterlist.WhenBar = THISBACK(IterationMenu);
	iterlist.WhenCursor = THISBACK(DataIteration);
	
	stagesplit.Vert() << stagelist << querylist;
	
	stagelist.AddColumn("Stage");
	stagelist.AddIndex("IDX");
	stagelist.WhenBar = THISBACK(StageMenu);
	stagelist.WhenCursor = THISBACK(DataStage);
	
	querylist.AddColumn("Query");
	querylist.AddIndex("IDX");
	querylist.WhenBar = THISBACK(QueryMenu);
	querylist.WhenCursor = THISBACK(DataQuery);
	
	prj_code.Highlight("cpp");
	prj_code.LineNumbers(true);
	prj_code.WhenAction = [this] {
		if (!prjlist.IsCursor()) return;
		int idx = prjlist.Get("IDX");
		VfsValue& n = *this->o.projects[idx];
		VfsProgramProject& prj = n.GetExt<VfsProgramProject>();
		prj.code = prj_code.GetData();
	};
	
	iter_code.Highlight("cpp");
	iter_code.LineNumbers(true);
	
	stage.Highlight("cpp");
	stage.LineNumbers(true);
	stage.WhenAction = [this] {
		if (!stagelist.IsCursor()) return;
		int idx = stagelist.Get("IDX");
		VfsValue& n = *this->o.stages[idx];
		VfsFarStage& s = n.GetExt<VfsFarStage>();
		s.code = stage.GetData();
	};
	
	
	o.PostCallback([this] {
		VfsProgram* prog = this->o.FindExt<VfsProgram>();
		if (prog)
			formedit.OpenXml(prog->formxml, prog->formxml_compressed);
	});
	
	formedit.WhenEmbeddedSave = [this] {
		VfsProgram* prog = this->o.FindExt<VfsProgram>();
		if (!prog) return;
		prog->formxml_compressed = true;
		formedit.SaveXml(prog->formxml, prog->formxml_compressed);
	};
	
}

void VfsProgramCtrl::MainTab::Data() {
	if (!o.ext)
		return;
	DataProjectList();
	DataStageList();
	DataBottom();
}

void VfsProgramCtrl::MainTab::DataList(bool show_num, bool select_last, ArrayCtrl& list, VfsValue& parent, Vector<Ptr<VfsValue>>& nodes, hash_t type_hash, Event<> WhenData) {
	int prev_count = list.GetCount();
	nodes = parent.FindTypeAllShallowPtr(type_hash);
	if (prev_count != nodes.GetCount())
		select_last = true;
	for(int i = 0; i < nodes.GetCount(); i++) {
		const auto& it = *nodes[i];
		int col = 0;
		if (show_num)
			list.Set(i, col++, i);
		list.Set(i, col++, it.id);
		const AstValue* ast = FindRawValue<AstValue>(it.value);
		list.Set(i, col++, ast ? ast->type : "");
		list.Set(i,"IDX", i);
	}
	list.SetCount(nodes.GetCount());
	if (select_last && list.GetCount())
		list.SetCursor(list.GetCount()-1);
	else if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	else
		WhenData();
}

void VfsProgramCtrl::MainTab::DataProjectList() {
	VfsProgram* prog = o.FindExt<VfsProgram>();
	if (!prog) return;
	DataList(false, false, prjlist, prog->val, o.projects, AsTypeHash<VfsProgramProject>(), THISBACK(DataProject));
}

void VfsProgramCtrl::MainTab::DataProject() {
	if (!prjlist.IsCursor()) {o.cur_project = 0; sessionlist.Clear(); iterlist.Clear(); return;}
	int idx = prjlist.Get("IDX");
	VfsValue& n = *o.projects[idx];
	o.cur_project = &n;
	VfsProgramProject& prj = n.GetExt<VfsProgramProject>();
	
	prj_code.SetData(prj.code);
	
	DataList(false, false, sessionlist, n, o.sessions, AsTypeHash<VfsProgramSession>(), THISBACK1(DataSession, false));
}

void VfsProgramCtrl::MainTab::DataSession(bool by_user) {
	if (!prjlist.IsCursor() || !sessionlist.IsCursor()) {o.cur_session = 0; iterlist.Clear(); return;}
	int ses_idx = sessionlist.Get("IDX");
	VfsValue& n = *o.sessions[ses_idx];
	o.cur_session = &n;
	
	DataList(true, by_user, iterlist, n, o.iterations, AsTypeHash<VfsProgramIteration>(), THISBACK(DataIteration));
}

void VfsProgramCtrl::MainTab::DataIteration() {
	if (!prjlist.IsCursor() || !sessionlist.IsCursor() || !iterlist.IsCursor()) {o.cur_iter = 0; memtree.Clear(); return;}
	int iter_idx = iterlist.Get("IDX");
	Ptr<VfsValue> n = o.iterations[iter_idx];
	if (!n)
		return;
	o.cur_iter = n->FindExt<VfsProgramIteration>();
	if (!o.cur_iter) return;
	
	if (!o.cur_iter->code.IsEmpty())
		iter_code.SetData(o.cur_iter->code);
	else
		iter_code.SetData(Value());
	
	DataMemory();
	DataLog();
}

void VfsProgramCtrl::MainTab::DataMemory() {
	if (!o.cur_iter) return;
	VfsProgramIteration& iter = *o.cur_iter;
	
	ArrayMap<String,EscValue> global;
	o.StringToGlobal(iter.global, global);
	
	memtree.Clear();
	for(int i = 0; i < global.GetCount(); i++) {
		String key = global.GetKey(i);
		EscValue& ev = global[i];
		DataMemoryTree(0, key, ev);
	}
	memtree.OpenDeep(0);
}

void VfsProgramCtrl::MainTab::DataLog() {
	if (!o.cur_iter) return;
	VfsProgramIteration& iter = *o.cur_iter;
	
	log.SetData(iter.log);
}

void VfsProgramCtrl::MainTab::DataMemoryTree(int parent, String key, const EscValue& ev) {
	if (ev.IsMap()) {
		String s;
		s << key;
		int tree_idx = memtree.Add(parent, MetaImgs::RedRing(), s);
		
		const auto& map = ev.GetMap();
		for(auto it : ~map) {
			String key1 = it.key.ToString(100,0);
			key1.Replace("\n","");
			DataMemoryTree(tree_idx, key1, it.value);
		}
	}
	else if (ev.IsArray()) {
		String s;
		s << key;
		
		if (ev.IsStringLike()) {
			s << ": " << ev.ToString(100) << " (string)";
			memtree.Add(parent, MetaImgs::RedRing(), s);
		}
		else {
			int tree_idx = memtree.Add(parent, MetaImgs::RedRing(), s);
			int i = 0;
			const auto& arr = ev.GetArray();
			for(auto& it : arr) {
				String key1 = IntStr(i++);
				DataMemoryTree(tree_idx, key1, it);
			}
		}
	}
	else {
		String s;
		s << key << ": ";
		s << ev.ToString();
		if (!ev.IsVoid())
			s << " (" << ev.GetTypeName() << ")";
		int tree_idx = memtree.Add(parent, MetaImgs::BlueRing(), s);
	}
}

void VfsProgramCtrl::MainTab::DataStageList() {
	VfsProgram* prog = o.FindExt<VfsProgram>();
	if (!prog) return;
	DataList(false, false, stagelist, prog->val, o.stages, AsTypeHash<VfsFarStage>(), THISBACK(DataStage));
}

void VfsProgramCtrl::MainTab::DataStage() {
	if (!stagelist.IsCursor()) return;
	int idx = stagelist.Get("IDX");
	VfsValue& n = *o.stages[idx];
	o.cur_stage = &n;
	VfsFarStage& s = n.GetExt<VfsFarStage>();
	stage.SetData(s.code);
}

void VfsProgramCtrl::MainTab::DataQuery() {
	
}

void VfsProgramCtrl::MainTab::DataBottom() {
	
}

bool VfsProgramCtrl::CompileStages(bool force) {
	bool succ = false;
	
	Agent* agent = ext->val.FindOwnerWith<Agent>();
	if (!agent) {
		PrintString("error: could not find Agent in MetaEnv");
		return false;
	}
	
	for(int i = 0; i < stages.GetCount(); i++) {
		VfsValue& n = *stages[i];
		
		VfsFarStage& s = n.GetExt<VfsFarStage>();
		String esc = s.code;
		if (esc.IsEmpty()) continue;
		
		succ = agent->CompileStage(n, force, THISBACK(PrintLog));
	}
	return true;
}

bool VfsProgramCtrl::MainTab::Compile(bool force) {
	if (!prjlist.IsCursor()) return false;
	int idx = prjlist.Get("IDX");
	o.cur_project = o.projects[idx];
	return o.Compile(force);
}

bool VfsProgramCtrl::Compile(bool force) {
	VfsValue& n = *cur_project;
	bool succ = false;
	
	VfsProgramProject& prj = n.GetExt<VfsProgramProject>();
	String esc = prj.code;
	if (esc.IsEmpty()) return false;
	if (!ext) return false;
	
	Agent* agent = ext->val.FindOwnerWith<Agent>();
	if (!agent) {
		PrintString("error: could not find Agent in MetaEnv");
		return false;
	}
	
	if (!this_iter)
		this_iter = &cur_session->Add<VfsProgramIteration>("");
		
	this_iter->code = esc;
	
	succ = agent->Compile(esc, force, THISBACK(PrintLog), this_iter);
	
	return true;
}

bool VfsProgramCtrl::Run(bool update) {
	if (!cur_session || !this_iter) {
		LOG("VfsProgramCtrl::Run: error: no current session yet");
		return false;
	}
	
	
	if (iterations.GetCount() && !update) {
		if (!PromptYesNo("Are you sure you want to clear all iterations?"))
			return false;
		cur_session->RemoveAllShallow<VfsProgramIteration>();
	}
	
	if (agent) {
		agent->Stop();
	}
	
	VfsProgram& prog = GetExt<VfsProgram>();
	prog.WhenDataTree = Proxy(WhenEditorChange); /*[this]{
		WhenEditorChange();
		//EntityEditorCtrl* eec = ext->val.FindOwner<EntityEditorCtrl>();
		//if (eec)
		//	this->DataExtCtrl();
	};*/
	
	this_iter = &cur_session->Add<VfsProgramIteration>("");
	
	if (!Compile(false))
		return false;
	
	if (!cur_session) {
		PrintString("error: no active session");
		return false;
	}
	
	
	PostCallback(THISBACK1(DataSession, true));
	
	agent = ext->val.FindOwnerWith<Agent>();
	if (!agent) {
		PrintString("error: could not find Agent in MetaEnv");
		return false;
	}
	
	agent->WhenPrint = THISBACK(Print);
	agent->WhenInput = THISBACK(Input);
	
	if (update) {
		auto iters = cur_session->FindTypeAllShallow(AsTypeHash<VfsProgramIteration>());
		int c = iters.GetCount();
		if (c >= 2) {
			VfsValue& prev = *iters[c-2];
			auto& prev_iter = prev.GetExt<VfsProgramIteration>();
			StringToGlobal(prev_iter.global, agent->GetGlobalRW());
		}
	}
	
	//agent->SetSeparateThread();
	agent->Start(update, THISBACK(PrintLog), [this](bool succ) {
		if (!succ && this_iter)
			this_iter->log << "Running failed\n";
		if (!succ)
			PostCallback(THISBACK(DataCurrentIteration));
		if (this_iter)
			this_iter->global = GlobalToString(agent->GetGlobal());
		PostCallback(THISBACK1(DataSession, false));
	});
	
	return true;
}

void VfsProgramCtrl::DataSession(bool by_user) {
	if (main)
		main->DataSession(by_user);
}

void VfsProgramCtrl::DataCurrentIteration() {
	if (main)
		main->DataCurrentIteration();
}

void VfsProgramCtrl::MainTab::DataCurrentIteration() {
	if (o.cur_iter == o.this_iter && o.this_iter) {
		log.SetData(o.this_iter->log);
		log.SetCursor(log.GetLength());
	}
}

void VfsProgramCtrl::Print(EscEscape& e) {
	PrintString(e[0]);
}

void VfsProgramCtrl::PrintString(String s) {
	this_iter->log += s;
	this_iter->log.Cat('\n');
	
	PostCallback(THISBACK(DataCurrentIteration));
	
	PostCallback([this]{
		if (this_iter && main) {
			main->log.SetData(this_iter->log);
			main->log.SetCursor(this_iter->log.GetLength());
		}
	});
}

void VfsProgramCtrl::Input(EscEscape& e) {
	String s;
	EditText(s, "Input", "Text");
	e = s;
}

void VfsProgramCtrl::PrintLog(Vector<ProcMsg>& msgs) {
	if (!this_iter)
		return;
	for (ProcMsg& m : msgs) {
		this_iter->log << m.ToString() << "\n";
	}
	PostCallback([this]{
		if (this_iter && main) {
			main->log.SetData(this_iter->log);
			main->log.SetCursor(this_iter->log.GetLength());
		}
	});
}

void VfsProgramCtrl::EditPos(JsonIO& json) {
	//json("process_automatically", process_automatically);
	VirtualFSComponentCtrl::EditPos(json);
	
	//if (json.IsLoading() && process_automatically)
	//	PostCallback(THISBACK1(StartProcess, false));
}

VirtualNode VfsProgramCtrl::Root() {
	if (!root) {
		VfsPath root_path; // empty
		VfsValue& val = this->ext->val;
		if (val.value.Is<AstValue>()) {
			LOG("ValueVFSComponentCtrl::Root: warning: resetting AstValue to Value");
			val.value = Value();
		}
		auto& data = root.CreateValue(root_path, &val.value);
		root.SetType(AsTypeHash<VfsProgram>());
		data.vfs_value = &val;
	}
	return root;
}

void VfsProgramCtrl::DataTree(TreeCtrl& tree) {
	VirtualNode vnode = Root();
	
	for (auto& prj : projects) {
		VfsProgramProject* p = prj->FindExt<VfsProgramProject>();
		if (p)
			vnode.GetAdd(p->val.id, p->GetTypeHash());
	}
	
	VirtualFSComponentCtrl::DataTree(tree);
	tree.WhenBar = [this,&tree](Bar& b) {
		int cur = tree.GetCursor();
		if (cur == 0) {
			// root
		}
		else {
			// not root
			// b.Add("Remove part", THISBACK(RemovePart));
		}
	};
}

void VfsProgramCtrl::Init() {
	RealizeData();
	
	
	VfsProgram* prog = this->FindExt<VfsProgram>();
	if (prog) {
		prog->WhenLayout.Clear();
		prog->WhenLayout << [this](VfsPath path) {
			if (this->form) {
				LOG(form->cur_path.ToString());
				LOG(path.ToString());
				if (form->cur_path == path)
					this->PostCallback([this]{form->Data();});
			}
		};
	}
}

void VfsProgramCtrl::RealizeData() {
	VirtualNode root = this->Root();
	/*hash_t type_hash = root.GetTypeHash();
	if (!root.GetTypeHash()) {
		root.SetType(AsTypeHash<VfsProgramCtrl>());
	}*/
	ASSERT(root.GetTypeHash() == AsTypeHash<VfsProgram>());
}

String VfsProgramCtrl::GetTitle() const {
	return "Content";
}

VNodeComponentCtrl* VfsProgramCtrl::CreateCtrl(const VirtualNode& vnode) {
	hash_t type_hash = vnode.GetTypeHash();
	main = 0;
	form = 0;
	if (type_hash == AsTypeHash<VfsProgramCtrl>() ||
		type_hash == AsTypeHash<VfsProgram>()) {
		MainTab* o = new MainTab(*this, vnode);
		main = o;
		return o;
	}
	else if (type_hash == AsTypeHash<VfsForm>()) {
		FormTab* o = new FormTab(*this, vnode);
		form = o;
		return o;
	}
	return 0;
}

void VfsProgramCtrl::ToolMenu(Bar& b) {
	b.Add("Compile Stages", [this]{CompileStages(true);}).Key(K_F8);
	b.Add("Compile Program", [this]{Compile(true);}).Key(K_F7);
	b.Add("Run", [this]{Run(false);}).Key(K_F5);
	b.Add("Run Update Iteration", [this]{Run(true);}).Key(K_F6);
	b.Separator();
	b.Add("Open Form Editor", [this]{
		
	}).Key(K_F9);
	if (main) {
		b.Separator();
		b.Sub("Form Editor", [this](Bar& bar){if (main) main->formedit.CreateMenuBar(bar);});
	}
	#ifdef flagDEBUG
	#if 0
	b.Separator();
	b.Add("Panic ptr release", [this] {
		if (iterations.GetCount())
			ext.PanicRelease();
	});
	#endif
	#endif
}

void VfsProgramCtrl::MainTab::ProjectMenu(Bar& b) {
	b.Add("Add project", THISBACK(AddProject));
	b.Add("Remove project", THISBACK(RemoveProject));
	b.Add("Rename project", THISBACK(RenameProject));
	b.Add("Duplicate project", THISBACK(DuplicateProject));
}

void VfsProgramCtrl::MainTab::SessionMenu(Bar& b) {
	b.Add("Add session", THISBACK(AddSession));
	b.Add("Remove session", THISBACK(RemoveSession));
	b.Add("Rename session", THISBACK(RenameSession));
	b.Add("Duplicate session", THISBACK(DuplicateSession));
}

void VfsProgramCtrl::MainTab::IterationMenu(Bar& b) {
	b.Add("Rename iteration", THISBACK(RenameIteration));
}

void VfsProgramCtrl::MainTab::QueryMenu(Bar& b) {
	
}

void VfsProgramCtrl::MainTab::AddProject() {
	String name;
	if (!EditText(name, "Project's name", "Name"))
		return;
	
	for(int i = 0; i < o.projects.GetCount(); i++) {
		if (o.projects[i]->id == name) {
			PromptOK("Project with that name exists already");
			return;
		}
	}
	VfsProgram& prog = this->o.GetExt<VfsProgram>();
	auto& ses = prog.val.Add<VfsProgramProject>(name);
	PostCallback(THISBACK(DataProjectList));
}

void VfsProgramCtrl::MainTab::RemoveProject() {
	if (!prjlist.IsCursor())
		return;
	int id = prjlist.Get("IDX");
	VfsValue* n = o.projects[id];
	o.GetValue().Remove(n);
	PostCallback(THISBACK(DataProjectList));
}

void VfsProgramCtrl::MainTab::RenameProject() {
	if (!prjlist.IsCursor())
		return;
	int id = prjlist.Get("IDX");
	auto& n = *o.projects[id];
	String name = n.id;
	if (!EditText(name, "Project's name", "Name"))
		return;
	n.id = name;
	PostCallback(THISBACK(DataProjectList));
}

void VfsProgramCtrl::MainTab::DuplicateProject() {
	if (!prjlist.IsCursor())
		return;
	int id = prjlist.Get("IDX");
	const auto& n0 = *o.projects[id];
	VfsProgram& prog = this->o.GetExt<VfsProgram>();
	auto& m1 = prog.val.Add<VfsProgramProject>("Duplicate of " + n0.id);
	VisitCopy(n0, m1.val);
	PostCallback(THISBACK(DataProjectList));
}

void VfsProgramCtrl::MainTab::AddSession() {
	String name;
	if (!o.cur_project) return;
	if (!EditText(name, "Session's name", "Name"))
		return;
	
	for(int i = 0; i < o.sessions.GetCount(); i++) {
		if (o.sessions[i]->id == name) {
			PromptOK("Session with that name exists already");
			return;
		}
	}
	int id = prjlist.Get("IDX");
	auto& parent = *o.projects[id];
	auto& ses = parent.Add<VfsProgramSession>(name);
	PostCallback(THISBACK(DataProject));
}

void VfsProgramCtrl::MainTab::RemoveSession() {
	if (!sessionlist.IsCursor() || !o.cur_project)
		return;
	int id = sessionlist.Get("IDX");
	VfsValue* n = o.sessions[id];
	if (o.cur_project)
		o.cur_project->Remove(n);
	PostCallback(THISBACK(DataProject));
}

void VfsProgramCtrl::MainTab::RenameSession() {
	if (!sessionlist.IsCursor())
		return;
	int id = sessionlist.Get("IDX");
	auto& n = *o.sessions[id];
	String name = n.id;
	if (!EditText(name, "Session's name", "Name"))
		return;
	n.id = name;
	PostCallback(THISBACK(DataProject));
}

void VfsProgramCtrl::MainTab::DuplicateSession() {
	if (!sessionlist.IsCursor() || !o.cur_project)
		return;
	int parent_id = prjlist.Get("IDX");
	auto& parent = *o.projects[parent_id];
	int id = sessionlist.Get("IDX");
	const auto& n0 = *o.sessions[id];
	auto& m1 = parent.Add<VfsProgramSession>("Duplicate of " + n0.id);
	VisitCopy(n0, m1.val);
	PostCallback(THISBACK(DataProject));
}

void VfsProgramCtrl::MainTab::RenameIteration() {
	if (!iterlist.IsCursor())
		return;
	int id = iterlist.Get("IDX");
	auto& n = *o.iterations[id];
	String name = n.id;
	if (!EditText(name, "Iteration's name", "Name"))
		return;
	n.id = name;
	PostCallback(THISBACK1(DataSession, false));
}

void VfsProgramCtrl::MainTab::StageMenu(Bar& b) {
	b.Add("Add stage", THISBACK(AddStage));
	b.Add("Remove stage", THISBACK(RemoveStage));
	b.Add("Rename stage", THISBACK(RenameStage));
	b.Add("Duplicate stage", THISBACK(DuplicateStage));
}

void VfsProgramCtrl::MainTab::AddStage() {
	String name;
	if (!EditText(name, "Stage's name", "Name"))
		return;
	
	for(int i = 0; i < o.stages.GetCount(); i++) {
		if (o.stages[i]->id == name) {
			PromptOK("Stage with that name exists already");
			return;
		}
	}
	auto& ses = o.GetValue().Add<VfsFarStage>(name);
	PostCallback(THISBACK(DataStageList));
}

void VfsProgramCtrl::MainTab::RemoveStage() {
	if (!stagelist.IsCursor())
		return;
	int id = stagelist.Get("IDX");
	VfsValue* n = o.stages[id];
	o.GetValue().Remove(n);
	PostCallback(THISBACK(DataStageList));
}

void VfsProgramCtrl::MainTab::RenameStage() {
	if (!stagelist.IsCursor())
		return;
	int id = stagelist.Get("IDX");
	auto& n = *o.stages[id];
	String name = n.id;
	if (!EditText(name, "Stage's name", "Name"))
		return;
	n.id = name;
	PostCallback(THISBACK(DataStageList));
}

void VfsProgramCtrl::MainTab::DuplicateStage() {
	if (!stagelist.IsCursor())
		return;
	int id = stagelist.Get("IDX");
	const auto& n0 = *o.stages[id];
	auto& m1 = o.GetValue().Add<VfsFarStage>("");
	VisitCopy(n0, m1.val);
	PostCallback(THISBACK(DataStageList));
}

VfsValue* VfsProgramCtrl::GetProgram() {
	return cur_project;
}

VfsValue* VfsProgramCtrl::GetStage() {
	return cur_stage;
}

String VfsProgramCtrl::GlobalToString(const ArrayMap<String,EscValue>& global) {
	ValueMap global_map;
	for(int i = 0; i < global.GetCount(); i++) {
		const EscValue& ev = global[i];
		if (ev.IsLambda())
			continue;
		global_map.Add(
			global.GetKey(i),
			StdValueFromEsc(ev));
	}
	Value global_value = global_map;
	String json = AsJSON(global_value);
	return json;
}

void VfsProgramCtrl::StringToGlobal(const String& global_str, ArrayMap<String,EscValue>& global) {
	if (global_str.IsEmpty())
		return;
	try {
		Value global_value = ParseJSON(global_str);
		if (!global_value.Is<ValueMap>()) global_value = ValueMap();
		ValueMap global_map = global_value;
		
		for(int i = 0; i < global_map.GetCount(); i++) {
			const Value& key = global_map.GetKey(i);
			const Value& val = global_map.GetValue(i);
			String key_str = key.ToString();
			global.GetAdd(key) = EscFromStdValue(val);
		}
	}
	catch (Exc e) {
		LOG("VfsProgramCtrl::StringToGlobal: error: " << e);
	}
}

void VfsProgramCtrl::MainTab::EditPos(JsonIO& json) {
	int tab_i = rtabs.Get();
	json("rtab", tab_i);
	if (json.IsLoading() && tab_i >= 0 && tab_i < rtabs.GetCount())
		rtabs.Set(tab_i);
}




VfsProgramCtrl::FormTab::FormTab(VfsProgramCtrl& o, const VirtualNode& vnode) : o(o), VNodeComponentCtrl(o, vnode) {
	Add(form.SizePos());
	
}

void VfsProgramCtrl::FormTab::Data() {
	VirtualNode node = this->GetVnode();
	cur_path = node.GetPath();
	if (!node.IsValue())
		return;
	
	Value v = node.GetValue();
	if (!v.Is<ValueMap>())
		return;
	ValueMap map = v;
	String layout_path = map.Get("layout_path", "");
	if (layout_path.IsEmpty())
		return;
	
	VfsProgram* prog = o.FindExt<VfsProgram>();
	if (!prog)
		return;
	
	form.LoadString(prog->formxml, prog->formxml_compressed);
	
	VfsPath lpath;
	lpath.SetPosixPath(layout_path);
	
	if (lpath.IsEmpty())
		form.Clear();
	else if (lpath.Parts()[0] == ".") {
		lpath.Remove(0);
		String p = lpath.Parts().GetCount() ? (String)lpath.Parts()[0] : "";
		form.Layout(p);
	}
	else {
		TODO
	}
}

void VfsProgramCtrl::FormTab::EditPos(JsonIO& json) {
	
}

END_UPP_NAMESPACE
