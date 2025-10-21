#ifndef _AICtrl_Ai_h_
#define _AICtrl_Ai_h_


struct AiThreadCtrlBase : VfsValueExtCtrl {
	struct Model : Moveable<Model> {
		String name;
		bool use_chat = false;
	};
	
	Vector<Model> models;
	Ptr<VfsValue> node;
	
	// Persistent
	int model_i = -1;
	
	void Visit(Vis& s);
	int GetModelCount(bool use_chat);
	bool CannotDoCompletion(String model_name);
	void UpdateCompletionModels();
	void UpdateChatModels();
	void UpdateModels(bool completion);
	void AddModel(String name, bool use_chat=false);
	hash_t GetHashValue() const {return GetVisitJsonHash(*this);}
	void SetNode(VfsValue& n);
	
	virtual ~AiThreadCtrlBase(){}
	virtual Ctrl* GetCtrl() = 0;
	virtual void Data() = 0;
	virtual void Submit() {};
	virtual void MainMenu(Bar& bar);
	
};

class CompletionCtrl : public WithCompletion<AiThreadCtrlBase> {
	
public:
	typedef CompletionCtrl CLASSNAME;
	CompletionCtrl();
	
	void Data() override;
	void Submit() override;
	Ctrl* GetCtrl() override {return this;}
	CompletionThread& Thread();
};

class AiCompletionComponentCtrl : public AiComponentCtrl {
	CompletionCtrl ctrl;
	
public:
	typedef AiCompletionComponentCtrl CLASSNAME;
	AiCompletionComponentCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override {}
	
};

INITIALIZE(AiCompletionComponentCtrl)


class ChatAiCtrl : public WithChatAI<AiThreadCtrlBase> {
	int session_i = -1;
	ChatCtrl chat;
	
public:
	typedef ChatAiCtrl CLASSNAME;
	ChatAiCtrl();
	void Submit() override;
	void Data() override;
	void DataSession();
	void ClearSessionCtrl();
	void AddSession();
	void RemoveSession();
	void ClearSession();
	void MainMenu(Bar& bar) override;
	void SessionMenu(Bar& bar);
	Ctrl* GetCtrl() override {return this;}
};

class AiChatComponentCtrl : public AiComponentCtrl {
	ChatAiCtrl ctrl;
	
public:
	typedef AiChatComponentCtrl CLASSNAME;
	AiChatComponentCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override {}
	
};

INITIALIZE(AiChatComponentCtrl)


#endif
 
