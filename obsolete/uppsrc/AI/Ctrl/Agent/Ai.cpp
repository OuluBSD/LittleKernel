#include "Agent.h"


NAMESPACE_UPP

void AiThreadCtrlBase::AddModel(String name, bool use_chat) {
	auto& m = models.Add();
	m.name = name;
	m.use_chat = use_chat;
}

void AiThreadCtrlBase::MainMenu(Bar& bar) {
	bar.Add("Update", [this]{this->Data();}).Key(K_F5);
	bar.Add("Submit", [this]{this->Submit();}).Key(K_CTRL_ENTER);
}

void AiThreadCtrlBase::Visit(Vis& s) {
	s.Ver(1)
	(1)	("model_i", model_i)
		;
}

int AiThreadCtrlBase::GetModelCount(bool use_chat) {
	int c = 0;
	for (auto it : models)
		if (it.use_chat == use_chat)
			c++;
	return c;
}

void AiThreadCtrlBase::UpdateCompletionModels() {
	UpdateModels(true);
}

void AiThreadCtrlBase::UpdateChatModels() {
	UpdateModels(false);
}

void AiThreadCtrlBase::UpdateModels(bool completion) {
	TaskMgr& m = AiTaskManager();
	ModelArgs args;
	Ptr<Ctrl> p = GetCtrl();
	m.GetModels(args, [this, p, completion](String res){
		if (!p) return;
		Vector<String> models;
		LoadFromJson(models, res);
		
		Vector<int> rm;
		for(int i = 0; i < this->models.GetCount(); i++)
			if (!this->models[i].use_chat)
				rm << i;
		if (!rm.IsEmpty())
			this->models.Remove(rm);
		
		for (String m : models)
			if (CannotDoCompletion(m) == !completion)
				AddModel(m, false);
		
		Ptr<AiThreadCtrlBase> c = this;
		PostCallback([c]{if (c) c->Data();});
	});
}

bool AiThreadCtrlBase::CannotDoCompletion(String model_name) {
	thread_local static Vector<String> non_completions;
	if (non_completions.IsEmpty()) {
		non_completions	<< "gpt-4"
						<< "dall-e"
						<< "o1"
						<< "tts"
						<< "text-embedding"
						<< "omni-"
						<< "o3"
						<< "computer-"
						<< "chatgpt-"
						<< "whisper-"
						<< "gpt-3.5-turbo-1106"
						<< "gpt-3.5-turbo-0125"
						<< "gpt-3.5-turbo-16k"
						;
	}
	for (const String& n : non_completions)
		if (model_name.GetCount() >= n.GetCount() &&
			model_name.Find(n) == 0)
			return true;
	if (model_name == "gpt-3.5-turbo")
		return true;
	
	return false;
}

CompletionCtrl::CompletionCtrl() {
	CtrlLayout(*this);
	
	/*
	model.Add("gpt-3.5-turbo-instruct-0914");
	model.Add("gpt-3.5-turbo-instruct");
	model.Add("babbage-002");
	model.Add("davinci-002");
	AddModel("gpt-4o", true);
	*/
	
	model_name.WhenAction = [this]{this->model_i = this->model_name.GetIndex();};
	temperature.SetData(1);
	max_length.SetData(2048);
	top_p.SetData(1);
	freq_penalty.SetData(0);
	presence_penalty.SetData(0);
	best_of.SetData(1);
	show_probs.Add("Off");
	show_probs.Add("Most likely");
	show_probs.Add("Least likely");
	show_probs.Add("Full spectrum");
	show_probs.SetIndex(0);
	submit <<= THISBACK(Submit);
	prompt.LineNumbers(true);
	prompt.WithCutLine(true);
}

AiCompletionComponentCtrl::AiCompletionComponentCtrl() {
	Add(ctrl.SizePos());
	
}

void AiCompletionComponentCtrl::Data() {
	TODO //ctrl.SetThread(GetThread());
}


INITIALIZER_COMPONENT_CTRL(AiCompletionComponent, AiCompletionComponentCtrl)




AiChatComponentCtrl::AiChatComponentCtrl() {
	Add(ctrl.SizePos());
	
}

void AiChatComponentCtrl::Data() {
	TODO //ctrl.SetThread(GetThread());
}


INITIALIZER_COMPONENT_CTRL(AiChatComponent, AiChatComponentCtrl)






ChatAiCtrl::ChatAiCtrl() {
	CtrlLayout(*this);
	chat_ctrl.Add(chat.SizePos());
	frequency_penalty.SetData(1);
	presence_penalty.SetData(1);
	top_p.SetData(1);
	temperature.SetData(1);
	model_name.WhenAction = [this]{this->model_i = this->model_name.GetIndex();};
	this->system_instructions.SetData("You are a helpful assistant...");
	
	sessions.AddColumn("Name");
	sessions.AddColumn("Changed");
	sessions.AddIndex("IDX");
	sessions.WhenBar = THISBACK(SessionMenu);
	sessions.WhenCursor = THISBACK(DataSession);
	
	submit <<= THISBACK(Submit);
	clear <<= THISBACK(ClearSession);
	
}

void ChatAiCtrl::Data() {
	if (!GetModelCount(false)) {
		UpdateChatModels();
	}
	else if (model_name.GetCount() != models.GetCount()) {
		model_name.Clear();
		for (auto it : models)
			model_name.Add(it.name);
		if (model_i >= 0 && model_i < model_name.GetCount())
			model_name.SetIndex(model_i);
		else if (model_name.GetCount())
			model_name.SetIndex(0);
	}
	
	// Update sessions
	if (!ext)
		return;
	ChatThread& t = dynamic_cast<ChatThread&>(*ext);
	
	if (t.sessions.IsEmpty())
		AddSession();
	if (sessions.GetCount() != t.sessions.GetCount()) {
		for(int i = 0; i < t.sessions.GetCount(); i++) {
			auto& session = t.sessions[i];
			sessions.Set(i, "IDX", i);
			sessions.Set(i, 0, session.name);
			sessions.Set(i, 1, session.changed);
		}
		sessions.SetCount(t.sessions.GetCount());
		sessions.SetSortColumn(1, true);
		if (!sessions.IsCursor())
			sessions.SetCursor(0);
	}
	
	DataSession();
}

void ChatAiCtrl::ClearSessionCtrl() {
	chat.Clear();
	
}

void ChatAiCtrl::AddSession() {
	ChatThread& t = dynamic_cast<ChatThread&>(*ext);
	auto& session = t.sessions.Add();
	session.created = GetSysTime();
	session.name = "Unnamed";
}

void ChatAiCtrl::RemoveSession() {
	if (!sessions.IsCursor())
		return;
	int idx = sessions.Get("IDX");
	ChatThread& t = dynamic_cast<ChatThread&>(*ext);
	t.sessions.Remove(idx);
	PostCallback(THISBACK(Data));
}

void ChatAiCtrl::ClearSession() {
	if (!sessions.IsCursor())
		return;
	int idx = sessions.Get("IDX");
	ChatThread& t = dynamic_cast<ChatThread&>(*ext);
	auto& session = t.sessions[idx];
	session.items.Clear();
	session.changed = GetSysTime();
	PostCallback(THISBACK(DataSession));
}

void ChatAiCtrl::MainMenu(Bar& bar) {
	AiThreadCtrlBase::MainMenu(bar);
	bar.Separator();
	SessionMenu(bar);
}

void ChatAiCtrl::SessionMenu(Bar& bar) {
	if (sessions.IsCursor())
		bar.Add("Remove session", THISBACK(RemoveSession));
	bar.Add("Add session", THISBACK(AddSession));
}

void ChatAiCtrl::DataSession() {
	if (!sessions.IsCursor()) {
		ClearSessionCtrl();
		return;
	}
	
	int prev_session_i = session_i;
	session_i = sessions.Get("IDX");
	ChatThread& t = dynamic_cast<ChatThread&>(*ext);
	const auto& session = t.sessions[session_i];
	
	if (session_i != prev_session_i ||
		chat.GetMessageCount() > session.items.GetCount())
		chat.Clear();
	
	int begin = chat.GetMessageCount();
	for(int i = begin; i < session.items.GetCount(); i++) {
		const auto& item = session.items[i];
		chat.AddMessage(
			GetMessageTypeString(item.type),
			item.content);
	}
	if (begin < session.items.GetCount())
		chat.Set(begin);
}

void ChatAiCtrl::Submit() {
	if (this->model_name.GetCount() == 0) return;
	if (session_i < 0) return;
	
	ChatThread& t = dynamic_cast<ChatThread&>(*ext);
	TaskMgr& m = AiTaskManager();
	
	String txt = this->prompt.GetData();
	txt.Replace("\r","");
	
	if (session_i < 0 || session_i >= t.sessions.GetCount())
		return;
	auto& session = t.sessions[session_i];
	{
		bool found = false;
		String c = this->system_instructions.GetData();
		for (int i = session.items.GetCount()-1; i >= 0; i--) {
			const auto& item = session.items[i];
			if (item.type == MSG_SYSTEM) {
				String a = item.content;
				found = a == c;
				break;
			}
		}
		if (!found) {
			auto& item = session.items.Add();
			item.type = MSG_SYSTEM;
			item.content = c;
			session.changed = item.created = GetSysTime();
		}
	}
	{
		bool found = false;
		if (session.items.GetCount()) {
			auto& item = session.items.Top();
			if (item.type == MSG_USER && item.content == txt)
				found = true;
		}
		if (!found) {
			auto& item = session.items.Add();
			item.type = MSG_USER;
			item.content = txt;
			item.username = "";
			session.changed = item.created = GetSysTime();
		}
		PostCallback([this]{
			this->prompt.Clear();
			DataSession();
		});
	}
	
	ChatArgs args;
	for (const auto& it : session.items) {
		auto& msg = args.messages.Add();
		msg.type = it.type;
		msg.content = it.content;
		msg.name = it.username;
	}
	
	args.model_name = models[this->model_name.GetIndex()].name;
	//args.response_format = this->response_format.GetData();
	args.temperature = this->temperature.GetData();
	args.stop_seq = this->stop_seq.GetData();
	args.top_prob = this->top_p.GetData();
	args.frequency_penalty = this->frequency_penalty.GetData();
	args.presence_penalty = this->presence_penalty.GetData();
	
	m.GetChat(args, [this](String res) {
		PostCallback([this,res]{
			ChatThread& t = dynamic_cast<ChatThread&>(*ext);
			if (session_i < 0 || session_i >= t.sessions.GetCount())
				return;
			auto& session = t.sessions[session_i];
			auto& item = session.items.Add();
			item.type = MSG_ASSISTANT;
			item.content = res;
			item.created = GetSysTime();
			DataSession();
		});
	});
}


END_UPP_NAMESPACE
