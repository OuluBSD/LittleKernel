#ifndef _EscLib_AnimContext_h_
#define _EscLib_AnimContext_h_


class EscAnimContext {
	
	
public:
	VectorMap<String, String>	code;
	ArrayMap<String, EscValue>	global;
	Array<EscAnimProgram>		progs;
	Animation	a;
	AnimPlayer	p;
	
	int64 op_limit = 1000000;
	
	bool keep_running = false;
	
	
	
public:
	typedef EscAnimContext CLASSNAME;
	EscAnimContext();
	
	void Clear();
	bool Init(bool run_main=true);
	void Iterate();
	void InitializeEmptyScene();
	void CreateProgram(String name);
	bool AddCodePath(String path);
	bool IsRunning() const;
	void StopProgram(EscAnimProgram& p);
	void RemoveProgramGroup(int group);
	void RemoveStopped();
	void RemoveStoppedGroup(int group);
	void ProcessAndRemoveGroup(int group);
	void KeepRunning(bool b=true) {keep_running = b;}
	bool HasProgram(EscAnimProgram& p);
	EscValue GetGlobal(String key);
	
	void ESC_DrawText(EscEscape& e);
	
	EscAnimProgram* FindProgram(EscEscape& e);
	Vector<EscAnimProgram*> FindGroupPrograms(int group);
	
	template <class T>
	T& CreateProgramT(String name, int group) {
		T* o = new T();
		o->ctx = this;
		o->user_group = group;
		progs.Add(o);
		return *o;
	}
	
	
	Callback WhenStop;
	
};


#endif
