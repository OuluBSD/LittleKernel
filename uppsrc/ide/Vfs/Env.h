#ifndef _AI_TextCtrl_Env_h_
#define _AI_TextCtrl_Env_h_



class EnvEditorCtrl : public ToolAppCtrl {
	Splitter hsplit;
	ArrayCtrl ctxlist;
	WithEnvEditor<Ctrl> edit;
	
protected:
	Ptr<VfsValue> file_root;
	Vector<Vector<VfsValue*>> ctx_dbs;
	Vector<VfsValue*> ctxs;
	Vector<Ptr<VfsValue>> dbs
	;
	
	
public:
	typedef EnvEditorCtrl CLASSNAME;
	EnvEditorCtrl();
	
	void Data() override;
	void DataItem();
	void RefreshDatabases();
	void ToolMenu(Bar& bar) override;
	void SetFont(Font fnt);
	VfsSrcFile& RealizeFileRoot();
	void AddContext();
	void RemoveContext();
	void Visit(Vis& v) override;
	void OnValueChange();
	void OnOption(Option* opt, VfsValue* db);
	
	static bool AcceptsExt(String e) { return e == ".env"; }
	static String GetID() { return "Environment Editor"; }
	static String MakeIdString(const Vector<VfsValue*>& v);
	
	Event<> WhenSaveEditPos;
	
};


#endif
