#ifndef _ide_Shell_Console_h_
#define _ide_Shell_Console_h_

NAMESPACE_UPP

class DropTerm;
class ToolAppCtrl;


class ConsoleCtrl : public ParentCtrl, IdeShellHost {
	
protected:
	IdeShell cmd;
	One<WidgetCtrl> ext;
	One<ToolAppCtrl> tool;
	Ctrl* active = 0;
	MenuBar menu;
	DropTerm* bridge = NULL;
	int id = -1;
	bool internal_menubar = false;
	
	String cwd;
	String filename;
	Event<> SaveEditPos;
	Event<> LoadEditPos;
	void SetView();
	void SetTitle(String s);
	void AddMenuBar();
	void RemoveMenuBar();
	void ClearActive();
public:
	typedef ConsoleCtrl CLASSNAME;
	ConsoleCtrl();
	~ConsoleCtrl();
	
	void Data();
	void InternalMenuBar(bool b=true) {internal_menubar = b;}
	bool IsMenuBarVisible() const {return ext;}
	IdeShell& Shell() {return cmd;}
	bool RealizeFocus();
	void RemoveCtrl(bool fast_exit=false);
	void RemoveExt(bool fast_exit=false);
	void RemoveTool(bool fast_exit=false);
	void DraftFile(IdeShell& shell, Value value);
	
	void Menu(Bar& bar);
	String GetTitle();
	void SetBridge(DropTerm* bridge, int id) {this->bridge = bridge; this->id = id;}
	ConsoleCtrl* GetConsole() override;
	
	Callback WhenTitle;
	Callback WhenViewChange;
	
	template <class T> void SimpleExt(IdeShell& shell, Value value) {
		this->CreateExt<T>();
		ext->Initialize(value);
		SetView();
		Data();
	}
	
	template <class T> T& EcsExt(IdeShell& shell, Value value) {
		T& o = this->CreateToolApp<T>();
		SetView();
		return o;
	}
	
	template <class T> void SaveExtPos() {
		if (!ext) return;
		auto& cache = EditPosCached<T>::EditPosCache();
		ASSERT(ext);
		T* o = dynamic_cast<T*>(&*ext);
		if (!o) return;
		JsonIO jio;
		o->EditPos(jio);
		cache.GetAdd(filename) = jio.GetResult();
		if (1) // TODO optimize (unnecessarily often here)
			EditPosCached<T>::SaveEditPosCache();
	}
	
	template <class T> void LoadExtPos() {
		if (!ext) return;
		auto& cache = EditPosCached<T>::EditPosCache();
		if (cache.IsEmpty())
			EditPosCached<T>::LoadEditPosCache();
		T* o = dynamic_cast<T*>(&*ext);
		if (!o) return;
		JsonIO jio(cache.GetAdd(filename));
		o->EditPos(jio);
	}
	
	void ClearCtrl() {
		ClearActive();
		if (ext) RemoveExt();
		if (tool) RemoveTool();
	}
	
	template <class T> T& CreateExt() {
		ClearCtrl();
		T* o = new T;
		ext = o;
		o->WhenTitle << Proxy(WhenTitle);
		SaveEditPos = THISBACK(SaveExtPos<T>);
		LoadEditPos = THISBACK(LoadExtPos<T>);
		LoadExtPos<T>();
		return *o;
	}
	
	template <class T> T& CreateToolApp() {
		ClearCtrl();
		T* o = new T;
		tool = o;
		o->WhenTitle << Proxy(WhenTitle);
		SaveEditPos.Clear();
		LoadEditPos.Clear();
		//SaveEditPos = THISBACK(SaveExtPos<T>);
		//LoadEditPos = THISBACK(LoadExtPos<T>);
		//LoadExtPos<T>();
		return *o;
	}
	
};

END_UPP_NAMESPACE

#endif
