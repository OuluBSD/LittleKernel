#ifndef _ide_Shell_Agent_h_
#define _ide_Shell_Agent_h_




class IdeAgent : public ParentCtrl {
	Splitter hsplit;
	Ctrl left;
	Button refresh_tasklist;
	ArrayCtrl tasklist;
	AgentPromptEdit edit;
	One<::RightTabs> tabs;
	Ctrl placeholder;
	TimeCallback tc;
	Ptr<AgentTaskExt> agent_task_ext;
	
public:
	typedef IdeAgent CLASSNAME;
	IdeAgent();
	~IdeAgent();
	void Menu(Bar& bar);
	void OnTab();
	void Data();
	void DataTasklist();
	void DataTask();
	void TaskMenu(Bar& bar);
	void AddTask();
};


#endif
