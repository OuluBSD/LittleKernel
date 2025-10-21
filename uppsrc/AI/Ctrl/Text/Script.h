#ifndef _AI_Ctrl_Script_h_
#define _AI_Ctrl_Script_h_

NAMESPACE_UPP


class ScriptTextCtrl : public ValueVFSComponentCtrl {
	
	struct SubTab : VNodeComponentCtrl {
		ScriptTextCtrl& owner;
		TabCtrl tabs;
		One<PartContentCtrl> part_view;
		One<StructuredScriptEditor> editor;
		PageCtrl dbproc;
		
		One<AuthorDataCtrl>			srcdata;
		One<ScriptTextDebuggerPage>	dbg;
		One<TokensPage>				tk;
		One<TextElements>			el;
		One<VirtualPhrases>			vp;
		One<VirtualPhraseParts>		vpp;
		One<VirtualPhraseStructs>	vps;
		One<PhrasePartAnalysis>		vpa;
		One<PhrasePartAnalysis2>	vpa2;
		One<ActionAttrsPage>		aap;
		One<Attributes>				att;
		One<TextDataDiagnostics>	diag;
		One<TextDataWordnet>		wn;
		One<PhraseParts>			pp;
		One<ActionParallelsPage>	apar;
		One<ActionTransitionsPage>	atra;
		
		typedef SubTab CLASSNAME;
		SubTab(ScriptTextCtrl&, const VirtualNode& vnode);
		void Data() override;
		void EditPos(JsonIO& json) override;
		void PageView(int page);
		void AddRootTabs();
		void AddLineOwnerTabs();
	};
	
	struct LineTab : VNodeComponentCtrl {
		ScriptTextCtrl& owner;
		TabCtrl tabs;
		ScriptPhrasePartsGroups db;
		LineTab(ScriptTextCtrl&, const VirtualNode& vnode);
		void Data() override;
	};
	
	struct PartTab : VNodeComponentCtrl {
		ScriptTextCtrl& owner;
		PartTab(ScriptTextCtrl&, const VirtualNode& vnode);
		void Data() override;
	};
	
	struct GenerateTab : VNodeComponentCtrl {
		ScriptTextCtrl& owner;
		Splitter vsplit;
		ArrayCtrl params;
		DocEdit output;
		GenerateTab(ScriptTextCtrl&, const VirtualNode& vnode);
		void Data() override;
	};
	
	Value params;
	void AddPart();
	void RemovePart();
	void RefreshParams();
	void ImportProofread(VirtualNode new_node, TranscriptProofread& proofread);
	ScriptTextProcess* active_process = 0;
	bool process_automatically = false;
	
	bool TreeItemString(const VirtualNode& n, const Value& key, String& qtf_value) override;
	void StartProcess(bool user_started);
	void StopProcess();
	void OnStop();
	void PostOnStop();
public:
	typedef ScriptTextCtrl CLASSNAME;
	ScriptTextCtrl();
	
	void EditPos(JsonIO& json) override;
	void DataTree(TreeCtrl& tree) override;
	void ToolMenu(Bar& bar) override;
	void Init() override;
	void RealizeData();
	String GetTitle() const override;
	VNodeComponentCtrl* CreateCtrl(const VirtualNode& vnode) override;
};

INITIALIZE(ScriptTextCtrl)


END_UPP_NAMESPACE

#endif
 
