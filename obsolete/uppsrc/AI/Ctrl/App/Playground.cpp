#include "App.h"

NAMESPACE_UPP


void CompletionCtrl::Data() {
	if (!GetModelCount(false)) {
		UpdateCompletionModels();
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
}

void AiThreadCtrlBase::SetNode(VfsValue& n) {
	node = &n;
}

#if 0
VfsProgram& AiThreadExt::GetVfsProgram() {
	auto& o = GetValue();
	return o.GetExt<VfsProgram>();
}

ChainThread& AiThreadExt::GetChainThread() {
	auto& o = GetValue();
	return o.GetExt<ChainThread>();
}
#endif

void CompletionCtrl::Submit() {
	if (!ext) return;
	if (this->model_name.GetCount() == 0) return;
	CompletionThread& t = dynamic_cast<CompletionThread&>(*ext);
	TaskMgr& m = AiTaskManager();
	
	String txt = this->prompt.GetData();
	txt.Replace("\r","");
	
	
	int model_i = this->model_name.GetIndex();
	String model_name = models[model_i].name;
	bool model_uses_chat_mode = models[model_i].use_chat;
	
	if (model_uses_chat_mode) {
		ChatArgs args;
		TODO
	}
	else {
		CompletionArgs args;
		args.prompt = txt;
		args.model_name = model_name;
		args.temperature = this->temperature.GetData();
		args.max_length = this->max_length.GetData();
		args.stop_seq = this->stop_seq.GetData();
		args.top_prob = this->top_p.GetData();
		args.frequency_penalty = this->freq_penalty.GetData();
		args.presence_penalty = this->presence_penalty.GetData();
		args.best_of = this->best_of.GetData();
		args.inject_start_text = this->inject_start.GetData();
		args.inject_restart_text = this->inject_restart.GetData();
		args.show_probabilities = (CompletionArgs::ShowProbs)this->show_probs.GetIndex();
		
		if (args.stop_seq.IsEmpty())
			args.stop_seq = "<|endoftext|>";
		
		m.GetCompletion(args, [this, txt](String res) {
			String new_data = txt + res;
			PostCallback([this,new_data]{
				this->prompt.SetData(new_data);
				this->prompt.SetCursor(this->prompt.GetLength());
			});
		});
	}
}














TextToSpeechCtrl::TextToSpeechCtrl() {
	CtrlLayout(*this);
	
	model.Add("Alloy");
	model.Add("Ash");
	model.Add("Coral");
	model.Add("Echo");
	model.Add("Fable");
	model.Add("Onyx");
	model.Add("Nova");
	model.Add("Sage");
	model.Add("Shimmer");
	model.SetIndex(0);
	quality.Add("tts-1");
	quality.Add("tts-1-hf");
	quality.SetIndex(0);
	speed.Add("0.25x");
	speed.Add("0.5x");
	speed.Add("1x");
	speed.Add("2x");
	speed.Add("4x");
	speed.SetIndex(2);
	fileformat.Add("mp3");
	fileformat.Add("opus");
	fileformat.Add("aac");
	fileformat.Add("flac");
	fileformat.Add("wav");
	fileformat.Add("pcm");
	fileformat.SetIndex(0);
	generations.AddColumn("Time");
	generations.AddColumn("Mode");
	generations.AddColumn("Text");
	generations.AddIndex("IDX");
	generations.ColumnWidths("1 1 2");
	
}

void TextToSpeechCtrl::Data() {
	
}

AssistantCtrl::AssistantCtrl() {
	CtrlLayout(*this);
	
}

void AssistantCtrl::Data() {
	
}

RealtimeAiCtrl::RealtimeAiCtrl() {
	CtrlLayout(*this);
	
}

void RealtimeAiCtrl::Data() {
	
}

CustomBiasesCtrl::CustomBiasesCtrl() {
	CtrlLayout(*this);
	
}

void CustomBiasesCtrl::Data() {
	
}

PlaygroundCtrl::PlaygroundCtrl() {
	Add(tabs.SizePos());
	
	tabs.Add(completion.SizePos(), "Completion");
	tabs.Add(chat.SizePos(), "Chat");
	tabs.Add(stage.SizePos(), "Function");
	tabs.Add(placeholder.SizePos(), "Action Planner");
	#if 0
	tabs.Add(placeholder.SizePos(), "User/System"); // https://sketch.dev/blog/agent-loop (https://news.ycombinator.com/item?id=43998472)
	tabs.Add(placeholder.SizePos(), "Action Planner");
	tabs.Add(placeholder.SizePos(), "Group of 3");
	tabs.Add(placeholder.SizePos(), "Tournament");
	tabs.Add(placeholder.SizePos(), "DM");
	tabs.Add(placeholder.SizePos(), "Team DM");
	tabs.Add(placeholder.SizePos(), "Game of toxic adoration");
	tabs.Add(placeholder.SizePos(), "Courtroom");
	tabs.Add(placeholder.SizePos(), "Multi-team cooperative parallel");
	tabs.Add(placeholder.SizePos(), "Multi-team cooperative hierarchical");
	tabs.Add(placeholder.SizePos(), "Animation");
	tabs.Add(placeholder.SizePos(), "Adventure");
	#endif
	tabs.Add(chain.SizePos(), "Filesystem-chat");
	tabs.Add(edit_img.SizePos(), "Image");
	tabs.Add(img_aspect.SizePos(), "Image Aspect Fixer");
	tabs.Add(placeholder.SizePos(), "Transcribe");
	tabs.Add(tts.SizePos(), "TTS");
	tabs.Add(rt.SizePos(), "Realtime");
	tabs.Add(ass.SizePos(), "Assistant");
	tabs.Add(bias.SizePos(), "Custom Biases");
	tabs.Add(tasks.SizePos(), "Tasks");
	
	tabs.WhenSet = [this]{
		this->Data();
		WhenTab();
	};
	
	PostCallback(THISBACK(LoadThis));
	PostCallback(THISBACK(Data));
	/*
	TODO
	
	void Data();
	void ToolMenu(Bar& bar);
	*/
}

PlaygroundCtrl::~PlaygroundCtrl() {
	stage.ext = 0;
	chain.ext = 0;
	StoreThis();
}

void PlaygroundCtrl::CreateThread() {
	LoadThis();
	//SetThread(*omni);
}

void PlaygroundCtrl::TabMenu(Bar& b) {
	int tab = tabs.Get();
	
	switch (tab) {
		case 0: completion.MainMenu(b); break;
		case 1: chat.MainMenu(b); break;
		case 2: stage.ToolMenu(b); break;
		case 3: chain.ToolMenu(b); break;
		default: break;
	}
}

void PlaygroundCtrl::Data() {
	int tab = tabs.Get();
	
	switch (tab) {
		case 0: completion.Data(); break;
		case 1: chat.Data(); break;
		case 2: stage.Data(); break;
		case 3: chain.Data(); break;
		case 8: tasks.Data(); break;
		default: break;
	}
}

void PlaygroundCtrl::Visit(Vis& s) {
	int tab = tabs.Get();
	
	s.Ver(1)
	(1)	("tab",tab)
		("completion", (AiThreadCtrlBase&)completion, VISIT_NODE)
		("chat", (AiThreadCtrlBase&)chat, VISIT_NODE)
		("stage", (AiThreadCtrlBase&)stage, VISIT_NODE)
		;
	//s % tts;
	//s % ass;
	//s % rt;
	//s % bias;
	//s % edit_img;
	//s % img_aspect;
	//s % tasks;
	
	if (s.IsLoading()) {
		PostCallback([this,tab]{
			tabs.Set(tab);
		});
	}
}

void PlaygroundCtrl::StoreThis() {
	//if (omni)
	//	VisitToJsonFile(*omni, ConfigFile("playground.json"));
	VisitToJsonFile(*this, ConfigFile("playground-gui.json"));
	if (node)
		VisitToJsonFile(*node, ConfigFile("playground-node.json"));
}

void PlaygroundCtrl::LoadThis() {
	//if (omni)
	//	VisitFromJsonFile(*omni, ConfigFile("playground.json"));
	VisitFromJsonFile(*this, ConfigFile("playground-gui.json"));
	if (node) {
		VisitFromJsonFile(*node, ConfigFile("playground-node.json"));
		if (!node->Is<Entity>()) {
			node->CreateExt<Entity>();
		}
		
		auto& compl_n	= node->GetAdd<CompletionThread>("completion");
		completion.ext	= &compl_n;
		
		auto& chat_n	= node->GetAdd<ChatThread>("chat");
		chat.ext		= &chat_n;
		
		auto& stage_n   = node->GetAdd<VfsFarStage>("stage-session");
		stage.ext		= &stage_n;
		
		auto& chain_n   = node->GetAdd<ChainThread>("chain");
		chain.ext		= &chain_n;
		
		auto& agent		= node->GetAdd<Agent>("agent");
		
	}
}

void PlaygroundCtrl::SetNode(VfsValue& n) {
	this->node = &n;
}



PlaygroundApp::PlaygroundApp() {
	Title("Playground").MaximizeBox().MinimizeBox();
	Sizeable().Maximize();
	
	Add(pg.SizePos());
	AddFrame(menu);
	PostCallback(THISBACK(UpdateMenu));
	
	omni_node = &MetaEnv().root.GetAdd<Engine>("eng").val.GetAdd<Entity>("playground").val;
	VisitFromJsonFile(*omni_node, ConfigFile("playground-root.json"));
	pg.SetNode(*omni_node);
	
	pg.WhenTab << THISBACK(UpdateMenu);
	pg.CreateThread();
}

PlaygroundApp::~PlaygroundApp() {
	pg.StoreThis();
	VisitToJsonFile(*omni_node, ConfigFile("playground-root.json"));
}

void PlaygroundApp::UpdateMenu() {
	menu.Set(THISBACK(MainMenu));
}

void PlaygroundApp::MainMenu(Bar& bar) {
	bar.Sub("App", [this](Bar& bar) {
		bar.Add("Quit", [this]{this->Close();});
	});
	bar.Sub("Tab", [this](Bar& bar) {pg.TabMenu(bar);});
}

void RunAiPlayground() {
	PlaygroundApp().Run();
}

END_UPP_NAMESPACE
