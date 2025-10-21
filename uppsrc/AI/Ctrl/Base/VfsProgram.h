#ifndef _AICtrl_VfsProgram_h_
#define _AICtrl_VfsProgram_h_

NAMESPACE_UPP


class VfsProgramCtrl : public ValueVFSComponentCtrl {
	
	struct MainTab : VNodeComponentCtrl {
		VfsProgramCtrl& o;
		Splitter vsplit, hsplit;
		ArrayCtrl prjlist, stagelist;
		ArrayCtrl sessionlist, iterlist, querylist;
		Splitter prjsplit, stagesplit;
		Splitter stagectrl, memoryctrl;
		CodeEditor prj_code, iter_code, stage;
		TabCtrl btabs, ltabs, rtabs;
		DocEdit log;
		TreeCtrl memtree;
		FormEditCtrl formedit;
		
		typedef MainTab CLASSNAME;
		MainTab(VfsProgramCtrl&, const VirtualNode& vnode);
		void Data() override;
		void EditPos(JsonIO& json) override;
		void DataProjectList();
		void DataProject();
		void DataSession(bool by_user);
		void DataIteration();
		void DataMemory();
		void DataLog();
		void DataStageList();
		void DataStage();
		void DataQuery();
		void DataBottom();
		void DataCurrentIteration();
		void DataList(bool show_num, bool select_last, ArrayCtrl& list, VfsValue& parent, Vector<Ptr<VfsValue>>& nodes, hash_t type_hash, Event<> WhenData);
		void DataMemoryTree(int tree_idx, String key, const EscValue& ev);
		
		void ProjectMenu(Bar& b);
		void AddProject();
		void RemoveProject();
		void RenameProject();
		void DuplicateProject();
		
		void SessionMenu(Bar& b);
		void AddSession();
		void RemoveSession();
		void RenameSession();
		void DuplicateSession();
		
		void IterationMenu(Bar& b);
		void RenameIteration();
		
		void QueryMenu(Bar& b);
		
		void StageMenu(Bar& b);
		void AddStage();
		void RemoveStage();
		void RenameStage();
		void DuplicateStage();
		
		bool Compile(bool force);
	};
	
	struct FormTab : VNodeComponentCtrl {
		Form form;
		VfsPath cur_path;
		VfsProgramCtrl& o;
		typedef FormTab CLASSNAME;
		FormTab(VfsProgramCtrl&, const VirtualNode& vnode);
		void Data() override;
		void EditPos(JsonIO& json) override;
	};
	
	Ptr<MainTab> main;
	Ptr<FormTab> form;
	Ptr<Agent> agent;
	Vector<Ptr<VfsValue>> projects, sessions, iterations, stages;
	Ptr<VfsValue> cur_project, cur_session, cur_stage;
	Ptr<VfsProgramIteration> cur_iter, this_iter;
	
	/*VectorMap<int,VfsValue*> structure_nodes;
	VectorMap<int,String> structure_values;*/
	
	void PrintLog(Vector<ProcMsg>& msgs);
	void Print(EscEscape& e);
	void PrintString(String s);
	void Input(EscEscape& e);
	
	String GlobalToString(const ArrayMap<String,EscValue>& global);
	void StringToGlobal(const String& global_str, ArrayMap<String,EscValue>& global);
public:
	typedef VfsProgramCtrl CLASSNAME;
	VfsProgramCtrl();
	
	bool CompileStages(bool force);
	bool Compile(bool force);
	bool Run(bool update);
	void DataSession(bool by_user);
	void DataCurrentIteration();
	
	void EditPos(JsonIO& json) override;
	void DataTree(TreeCtrl& tree) override;
	void ToolMenu(Bar& bar) override;
	void Init() override;
	String GetTitle() const override;
	VNodeComponentCtrl* CreateCtrl(const VirtualNode& vnode) override;
	void RealizeData();
	VirtualNode Root() override;
	
	VfsValue* GetProgram();
	VfsValue* GetStage();
};

INITIALIZE(VfsProgramCtrl)

END_UPP_NAMESPACE

#endif

