#ifndef _EscLib_AnimProgram_h_
#define _EscLib_AnimProgram_h_

class EscAnimContext;

class EscAnimProgram {
	
protected:
	friend class EscAnimContext;
	
	bool is_native = false;
	bool is_native_running = false;
	Gate0 native_fn;
	EscAnimContext* ctx = 0;
	One<Esc> vm;
	String code;
	String fn_name;
	
public:
	EscValue a0, a1; // main function arguments
	EscValue user;
	dword user_flags;
	int user_type;
	int user_group;
	
public:
	typedef EscAnimProgram CLASSNAME;
	EscAnimProgram();
	virtual ~EscAnimProgram() {}
	
	void Clear();
	
	void Init(EscValue lambda);
	void Continue();
	void Stop();
	bool Process();
	void Iterate();
	EscAnimProgram& Set(Gate0 cb, EscValue a0=EscValue(), EscValue a1=EscValue());
	EscAnimProgram& Set(EscValue *self, EscValue fn, EscValue a0=EscValue(), EscValue a1=EscValue());
	
	bool IsVm() const {return !vm.IsEmpty();}
	Esc& GetVm() {return *vm;}
	bool IsRunning() const;
	
	void ESC_DrawText(EscEscape& e);
	
	Callback ContinueCallback() {return THISBACK(Continue);}
	
	
	
	Callback1<EscAnimProgram&> WhenStop;
	
};


#endif
