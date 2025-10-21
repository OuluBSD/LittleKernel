#include "ide.h"

#ifndef flagV1
#include <AI/Core/Core.h>


INITBLOCK {
	RegisterGlobalConfig("AITaskDlg2");
}

struct AITaskDlg : TopWindow {
	typedef AITaskDlg CLASSNAME;
	
	virtual bool Key(dword key, int count);
	
	void Serialize(Stream& s);
	
	TabCtrl tabs;
	MetaEnvTree menv;
	TaskCtrl tasks;
	PlaygroundCtrl playground;
	OmniCtrl daemon;
	
	Ide* theide = 0;
	
	AITaskDlg();
	~AITaskDlg();
	
	void Update();
};

AITaskDlg::AITaskDlg() {
	Maximize();
	MaximizeBox();
	MinimizeBox();
	Title("AI");
	
	Add(tabs.SizePos());
	tabs.Add(daemon.SizePos(), "Daemon");
	tabs.Add(tasks.SizePos(), "Tasks");
	tabs.Add(playground.SizePos(), "Playground");
	tabs.Add(menv.SizePos(), "Meta Env. Tree");
	tabs.WhenSet = THISBACK(Update);
	
	playground.SetNode(MetaEnv().root.GetAdd<Engine>("eng").val.GetAdd<Entity>("playground").val);
	playground.CreateThread();
}

AITaskDlg::~AITaskDlg() {
	playground.StoreThis();
}

bool AITaskDlg::Key(dword key, int count) {
	if (key == K_ESCAPE) {
		Close();
		return true;
	}
	return false;
}

void AITaskDlg::Update() {
	int tab = tabs.Get();
	
	switch (tab) {
		case 0: daemon.Data(); break;
		case 1: tasks.Data(); break;
		case 2: playground.Data(); break;
		case 3: menv.Data(); break;
		default: break;
	}
}

void AITaskDlg::Serialize(Stream& s) {
	
}

void Ide::OpenAITaskDlg()
{
	TaskMgr::Setup(this);
	AITaskDlg dlg;
	LoadFromGlobal(dlg, "AITaskDlg");
	dlg.theide = this;
	dlg.Update();
	dlg.Execute();
	StoreToGlobal(dlg, "AITaskDlg");
}

#endif
