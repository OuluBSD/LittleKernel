#include <ide/ide.h>

NAMESPACE_UPP

IdeAgent::IdeAgent() {
	tabs.Create();
	Add(hsplit.HSizePos(0,100).VSizePos());
	Add(tabs->RightPos(0,100).VSizePos());
	hsplit.Horz() << left << placeholder;
	hsplit.SetPos(2000);
	
	left.Add(refresh_tasklist.TopPos(0,30).HSizePos());
	left.Add(tasklist.VSizePos(30,0).HSizePos());
	refresh_tasklist.SetLabel("Refresh");
	refresh_tasklist.WhenAction = THISBACK(DataTasklist);
	
	placeholder.Add(edit.SizePos());
	placeholder.SetFrame(InsetFrame());
	
	tabs->Add(IdeImg::IconBuilding(), "Prompt", Black());
	tabs->Add(IdeImg::IconDebugging(), "Process", Black());
	tabs->Add(IdeImg::IconRunning(), "Result", Black());
	tabs->SetCursor(0);
	tabs->WhenAction << THISBACK(OnTab);
	OnTab();
	
	tasklist.AddColumn("Package");
	tasklist.AddColumn("Task name");
	tasklist.WhenBar << THISBACK(TaskMenu);
	tasklist.WhenCursor << THISBACK(DataTask);
	
	edit.prompt.WhenAction = [this]{
		if (agent_task_ext)
			agent_task_ext->prompt = edit.prompt.GetData();
	};
	
	//tc.Set(-1000, THISBACK(Data));
}

IdeAgent::~IdeAgent() {
	tc.Kill();
}

void IdeAgent::Menu(Bar& bar) {
	
}

void IdeAgent::TaskMenu(Bar& bar) {
	bar.Add(t_("Add task"), THISBACK(AddTask));
	if (tasklist.IsCursor()) {
		bar.Add(t_("Remove task"), [this]{
			
		});
	}
}

void IdeAgent::OnTab() {
	int i = tabs->GetCursor();
	
	edit.Show(i == 0);
	if (i == 0) edit.SetFocus();
	
	PostCallback(THISBACK(Data));
}

void IdeAgent::Data() {
	
	
	DataTasklist();
}

void IdeAgent::DataTasklist() {
	MetaEnvironment& env = MetaEnv();
	auto ctx = TheIdeContext();
	if (!ctx) return;
	//WorkspaceWork& wspc = *dynamic_cast<Ide*>(ctx);
	
	//LOG(env.root.GetTreeString());
	int row = 0;
	/*for(int i = 0; i < wspc.package.GetCount(); i++) {
		Value name = wspc.package.Get(i);
		//LOG(name.ToString());
		
		VfsPath path;
		path.Add(name);
		path.Add("agent");
		
		VfsValue* agent = env.root.FindPath(path);
		if (!agent)
			continue;
	}*/
		
	for(int i = 0; i < env.root.sub.GetCount(); i++) {
		VfsValue& s0 = env.root.sub[i];
		
		int j = s0.Find("agent");
		if (j < 0) continue;
		
		VfsValue& agent = s0.sub[j];
		String name = agent.id;
		
		Vector<Ptr<VfsValue>> tasks = agent.FindAllWith<AgentTaskExt>();
		for (Ptr<VfsValue>& task : tasks) {
			if (!task) continue;
			tasklist.Set(row, 0, name);
			tasklist.Set(row, 1, task->id);
			row++;
		}
	}
	tasklist.SetCount(row);
	
	DataTask();
}

void IdeAgent::DataTask() {
	MetaEnvironment& env = MetaEnv();
	WorkspaceWork& wspc = *dynamic_cast<Ide*>(TheIdeContext());
	if (!wspc.package.IsCursor()) return;
	Value name = wspc.package.ColumnList::Get(wspc.package.GetCursor());
	
	if (!wspc.package.IsCursor() || !tasklist.IsCursor()) {
		edit.prompt.Clear();
		edit.prompt.Enable();
		return;
	}
	int task_i = tasklist.GetCursor();
	
	VfsPath path;
	path.Add(name);
	path.Add("agent");
	
	VfsValue* agent = env.root.FindPath(path);
	if (!agent) {
		PromptOK("Current package doesn't have the \"agent.ecs\" file");
		return;
	}
	
	Vector<Ptr<AgentTaskExt>> tasks = agent->FindAllWithT<AgentTaskExt>();
	if (task_i < 0 || task_i >= tasks.GetCount() || !tasks[task_i]) {
		PromptOK("internal error");
		return;
	}
	AgentTaskExt& task = *tasks[task_i];
	agent_task_ext = &task;
	ASSERT(agent_task_ext);
	
	edit.prompt.Enable(agent_task_ext->status == AgentTaskExt::CLEARED);
	edit.prompt.SetData(agent_task_ext->prompt);
}

void IdeAgent::AddTask() {
	MetaEnvironment& env = MetaEnv();
	WorkspaceWork& wspc = *dynamic_cast<Ide*>(TheIdeContext());
	if (!wspc.package.IsCursor()) return;
	String name = wspc.package.Get(wspc.package.GetCursor());
	
	VfsPath path;
	path.Add(name);
	path.Add("agent");
	
	VfsValue* agent = env.root.FindPath(path);
	if (!agent) {
		PromptOK("Current package doesn't have the \"agent.ecs\" file");
		return;
	}
	
	Entity& ent = agent->Add<Entity>("unnamed");
	AgentTaskExt& task = ent.val.Add<AgentTaskExt>();
	
	PostCallback(THISBACK(Data));
	PostCallback([this]{tasklist.SetCursor(tasklist.GetCount()-1);});
}

END_UPP_NAMESPACE
