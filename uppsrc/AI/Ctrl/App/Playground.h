#ifndef _AI_Playground_h_
#define _AI_Playground_h_

NAMESPACE_UPP


struct AiChainCtrl : VfsValueExtCtrl {
	
private:
	Splitter hsplit, msplit, rsplit;
	ArrayCtrl session;
	TreeCtrl structure;
	Vector<VfsValue*> sessions;
	VectorMap<int,VfsValue*> structure_nodes;
	VectorMap<int,String> structure_values;
	
public:
	typedef AiChainCtrl CLASSNAME;
	AiChainCtrl();
	
	void Data() override;
	void DataSession();
	void DataItem();
	void ToolMenu(Bar& bar) override;
	void VisitNode(int tree_i, VfsValue& n, String path);
	VfsValue* GetSession();
	
	void SessionMenu(Bar& b);
	void AddSession();
	void RemoveSession();
	void RenameSession();
	void SetSessionVersion();
	void DuplicateSession();
	
	void StageMenu(Bar& b);
	void AddStageNode(hash_t type_hash);
	void RenameStageNode();
	void RemoveStageNode();
	
};

INITIALIZE(AiChainCtrl)

class TextToSpeechCtrl : public WithTTS<AiThreadCtrlBase> {
	
public:
	typedef TextToSpeechCtrl CLASSNAME;
	TextToSpeechCtrl();
	
	void Data() override;
	Ctrl* GetCtrl() override {return this;}
};

class AssistantCtrl : public WithAssistants<AiThreadCtrlBase> {
	
public:
	typedef AssistantCtrl CLASSNAME;
	AssistantCtrl();
	
	void Data() override;
	Ctrl* GetCtrl() override {return this;}
};

class RealtimeAiCtrl : public WithRealtimeAI<AiThreadCtrlBase> {
	
public:
	typedef RealtimeAiCtrl CLASSNAME;
	RealtimeAiCtrl();
	
	void Data() override;
	Ctrl* GetCtrl() override {return this;}
};

class CustomBiasesCtrl : public WithCustomBiases<AiThreadCtrlBase> {
	
public:
	typedef CustomBiasesCtrl CLASSNAME;
	CustomBiasesCtrl();
	
	void Data() override;
	Ctrl* GetCtrl() override {return this;}
};

class PlaygroundCtrl : public Ctrl {
	TabCtrl tabs;
	CompletionCtrl completion;
	VfsProgramCtrl stage;
	AiChainCtrl chain;
	TextToSpeechCtrl tts;
	AssistantCtrl ass;
	RealtimeAiCtrl rt;
	ChatAiCtrl chat;
	CustomBiasesCtrl bias;
	EditImage edit_img;
	EditImage img_aspect;
	TaskCtrl tasks;
	Ctrl placeholder;
	
	Ptr<VfsValue> node;
	
public:
	typedef PlaygroundCtrl CLASSNAME;
	
	PlaygroundCtrl();
	~PlaygroundCtrl();
	void Visit(Vis&);
	void Data();
	void StoreThis();
	void LoadThis();
	void CreateThread();
	void SetNode(VfsValue& n);
	void TabMenu(Bar& bar);
	
	Event<> WhenTab;
	
};


class PlaygroundApp : public TopWindow {
	PlaygroundCtrl pg;
	MenuBar menu;
	Ptr<VfsValue> omni_node;
	
public:
	typedef PlaygroundApp CLASSNAME;
	PlaygroundApp();
	~PlaygroundApp();
	
	void UpdateMenu();
	void MainMenu(Bar& bar);
	
};

void RunAiPlayground();

END_UPP_NAMESPACE

#endif
 
