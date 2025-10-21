#ifndef _Esc_Bytecode_h_
#define _Esc_Bytecode_h_

#undef Yield

struct VmState {
	static const int REG_COUNT = IrValue::REG_COUNT;
	
	
	struct SRVal : Moveable<SRVal> {
		EscValue *lval;
		EscValue  rval;
		EscValue  sbs;

		SRVal()                    { lval = NULL; }
		SRVal(const EscValue& v)    { lval = NULL; rval = v; }
		SRVal(double n)            { lval = NULL; rval = n; }
		SRVal(int64 n)             { lval = NULL; rval = n; }
		SRVal(uint64 n)            { lval = NULL; rval = (int64)n; }
		SRVal(bool n)              { lval = NULL; rval = (int64)n; }
		
		void operator=(const EscValue& v)    { lval = NULL; rval = v; sbs = EscValue();}
		void operator=(double n)            { lval = NULL; rval = n; sbs = EscValue();}
		void operator=(int64 n)             { lval = NULL; rval = n; sbs = EscValue();}
		void operator=(uint64 n)            { lval = NULL; rval = (int64)n; sbs = EscValue();}
		void operator=(bool n)              { lval = NULL; rval = (int64)n; sbs = EscValue();}
		void operator=(const SRVal& v)		{ lval = v.lval; rval = v.rval; sbs = v.sbs; }
	};
	
	Array<SRVal> r_stack;
	Array<SRVal> temp_stack;
	Vector<SRVal> rself_stack;
	Array<EscValue> var_stack;
	Array<EscValue> self_stack;
	Array<Array<SRVal>> argvec_stack;
	EscValue regs[REG_COUNT];
	int pc = 0;
	int max_pc = 0;
	
};

struct EscCompiler : public CParser {
	
protected:
	struct LoopStack : Moveable<LoopStack> {
		IrValue repeat, exit;
		
	};
	
	Vector<IR> ir;
	Vector<LoopStack> loop_stack;
	int lbl_counter = 0;
	
	
	void		Emit_(IrCode x, const char* file, int line);
	void		Emit1_(IrCode x, IrValue a, const char* file, int line);
	void		Emit2_(IrCode x, IrValue a, IrValue b, const char* file, int line);
	void		Emit3_(IrCode x, IrValue a, IrValue b, IrValue c, const char* file, int line);
	void		EmitLabel_(IrValue l, const char* file, int line);
	IrValue		EmitPushVar_(const IrValue& v, const char* file, int line);
	IrValue		EmitPopVar_(const IrValue& v, int reg, const char* file, int line);
	IrValue		EmitPopVar_(const IrValue& v, const IrValue& avoid0, const char* file, int line);
	IrValue		EmitSelfLambdaCheck(String id, IrValue& tmp, const char* file, int line);
	IrValue		EmitGlobalLambdaCheck(String id, IrValue& tmp, const char* file, int line);
	IrValue		EmitSelfLvalCheck(const char* file, int line);
	
	IrValue CreateLabel();
	void PushLoop(IrValue exit);
	void PushLoop(IrValue exit, IrValue repeat);
	void CreateSwitchDefault();
	void PopLoop();
	
	void OnError(String msg);
	
public:
	
	int			r_stack_level;
	int			loop;
	bool		fail = false;
	
	static int stack_level;
	
	void		OutOfMemory();
	
	void		TestLimit();
	double      DoCompare(const EscValue& a, const EscValue& b, const char *op);
	void		DoCompare(const IrValue& a, const IrValue& b, const char *op);
	void		DoCompare(const char *op);
	String		ReadName();
	IrValue		IsTrue(const IrValue& v);
	
	IrValue		Get();
	void		Assign(const IrValue& src);

	IrValue		GetExpand();
	IrValue		GetExp();
	

	void  Subscript(String id);
	void  Subscript();
	void  Term();
	void  Unary();
	void  Mul();
	void  Add();
	void  Shift();
	void  Compare();
	void  Equal();
	void  BinAnd();
	void  BinXor();
	void  BinOr();
	void  And();
	void  Or();
	void  Cond();
	void  Assign();
	void  Exp();

	void  SkipTerm();
	void  SkipExp();
	IrValue  PCond();
	void  FinishSwitch();
	void  DoStatement();

	void  Run();
	void  WriteLambda(EscLambda& l);
	void  SwapIR(Vector<IR>& ir);

	EscCompiler(const char *s, const String& fn, int line = 1)
	: CParser(s, fn, line)
	{ r_stack_level = stack_level;}
	~EscCompiler() { stack_level = r_stack_level; }
};

struct IrVM {
	static const int REG_COUNT = 5;
	using SRVal = VmState::SRVal;
	
	Esc&					esc;
	IrVM*					parent = 0;
	VmState*				s;
	VmState					state;
	RunningFlagSingle		flag;
	int64&					op_limit;
	Vector<int>				lbl_pos;
	VectorMap<String, int>	lbl_names;
	bool					fail = 0;
	EscLambda*				fn = 0;
	EscLambda*				call_fn = 0;
	SRVal*					call_self = 0;
	Array<SRVal>*			call_arg = 0;
	String					call_id;
	bool					is_calling = 0;
	bool					is_subcall = 0;
	bool					is_sleeping = 0;
	bool					run_ins_again = 0;
	Array<EscValue>			argvar;
	String					return_argname;
	
	ArrayMap<String, EscValue>&	global;
	ArrayMap<String, EscValue>&	var;
	const Vector<IR>&			ir;
	
	IrVM(Esc* esc, ArrayMap<String, EscValue>& g, ArrayMap<String, EscValue>& v, int64& op_limit, const Vector<IR>& ir)
		: esc(*esc), global(g), var(v), op_limit(op_limit), ir(ir)
		{s = &state;}
	
	int		InitLambdaExecution(EscLambda& l, IrVM& parent);
	EscValue	ExecuteLambda(const String& id, EscValue& lambda, SRVal& self, Array<SRVal>& arg);
	void	ExecuteInstruction(const IR& ir);
	void	ExecuteEscape();
	void	SetReturnArg(IrVM& vm, String arg);
	void	InitSubcall();
	void	FinishSubcall();
	void	FinishArgument();
	bool	Execute();
	bool	RefreshLabels(const Vector<IR>& ir);
	void	Get();
	void    Get(const SRVal& r, EscValue& v);
	void    GetSbs(const SRVal& r, EscValue& v);
	bool	IsRunning() const {return flag.IsRunning();}
	void	SetNotRunning() {flag.SetNotRunning();}
	void	OnError(String msg);
	void	ThrowError(String msg);
	void	TestLimit();
	void	AddAssign1(SRVal& r, const EscValue& a, const EscValue& b);
	void	AddAssign2(SRVal& r, const EscValue& a, const EscValue& b);
	void	OutOfMemory();
	void	WriteRegister(const IrValue& reg, const EscValue& v);
	EscValue	ReadVar(const IrValue& v);
	void	Jump(const IrValue& v);
	void	Assign(const SRVal& val, const EscValue& src);
	void	Assign(EscValue& val, const Vector<EscValue>& sbs, int si, const EscValue& src);
	double	DoCompare(const EscValue& a, const EscValue& b, const char *op);
	EscValue	MulArray(EscValue array, EscValue times);
	void	BeginExecutingLambda(const String& id, EscValue& lambda, SRVal& self, Array<SRVal>& arg);
	void	Yield();
	
	double	Number(const EscValue& a, const char *oper);
	int64	Int(const EscValue& a, const char *oper);
	double	Number(const SRVal& a, const char *oper);
	int64	Int(const SRVal& a, const char *oper);
	String	Lims(const String& s) const;
	
	EscValue&	Self();
	
	Event<ProcMsg&> WhenMsg;
	
};

struct Esc {
	ArrayMap<String, EscValue>& global;
	int64& oplimit;
	RunningFlagSingle flag;
	
	typedef enum {
		EVALX,
		SUBCALL,
		LAMBDA,
		STRING,
		FN_NAME,
		FN_EXPAND,
	} CallType;
		
	struct Call {
		CallType type;
		One<IrVM> vm;
		IrVM* parent = 0;
		int parent_arg_i = -1;
		EscLambda* l = 0;
		EscLambda* scope_l = 0;
		EscValue lambda;
		String fn, code;
		int line = 0;
		bool get_exp = false;
		IrValue out_var;
		Vector<IR> tmp_ir;
		ArrayMap<String, EscValue> var;
	};
		
	Array<Call> calls;
	EscValue self;
	
protected:
	friend struct IrVM;
	
	bool fail = false;
	bool sleep = false;
	bool spinning_sleep = false;
	double sleep_s = 0;
	TimeStop ts;
	int code_len = 0;
	
public:
	Esc(ArrayMap<String, EscValue>& global, const char *s, int64& oplimit,
	    const String& fn, int line = 1)
		: global(global), oplimit(oplimit) {
	    Call& c = calls.Add();
	    c.type = STRING;
	    c.fn = fn;
	    c.code = s;
	    c.line = line;
	}
	
	Esc(ArrayMap<String, EscValue>& global, int64& oplimit, EscLambda& l)
		: global(global), oplimit(oplimit) {
	    Call& c = calls.Add();
	    c.type = LAMBDA;
	    c.l = &l;
	}
	
	void		Compile();
	void		CompileCall(Call& c);
	void		Run();
	bool		RunExpand(String& out);
	void		Stop();
	double		Number(const EscValue& a, const char *oper);
	int64		Int(const EscValue& a, const char *oper);
	
	EscValue&	VarGetAdd(const EscValue& key);
	EscValue		GetExp();
	void		OnError(String s);
	void		SleepSpinning(int ms);
	void		SleepReleasing(int ms);
	void		SleepInfiniteReleasing();
	void		StopSleep();
	
	bool		IsRunning() const;
	bool		IsSleepExit() const;
	bool		IsSleepFinished() const;
	bool		CheckSleepFinished();
	IrVM&		GetVM();
	int			GetCodeLength() const;
	
	ArrayMap<String, EscValue>& Var();
	EscValue& Self();
	const Array<Call>& Calls();
	
	void		SetFailed();
	bool		IsFailed() const;
	
	void		ThrowError(const char *s);
	void		ThrowError()                       { ThrowError(""); }

	EscValue return_value;
	
	Event<ProcMsg&> WhenMsg;
	
};

void Compile(ArrayMap<String, EscValue>& global, EscValue *self, EscValue& lambda);

#endif
