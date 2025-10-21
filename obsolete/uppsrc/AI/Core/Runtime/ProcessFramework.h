#ifndef _AICore_ProcessFramework_h_
#define _AICore_ProcessFramework_h_

/*
TODO Read Notes:
	- fun dynamics:
		- add temperature to let agents ignore current AgentInteractionPolicy
			- or "not ignore", but change to secondary less-strict AgentInteractionPolicy
*/

class AgentInteractionPolicy;
class AgentInteractionSession;

struct FarStage : Pte<FarStage> {
	struct Function : Moveable<Function> {
		String name;
		Vector<String> params;
		String ret;
		String body;
		Value value;
		String esc_declaration;
		hash_t hash = 0;
		void Visit(Vis& v) {v.Ver(1)(1) VIS_(name) VIS_(params) VIS_(ret) VIS_(body) VIS_(value) VIS_(esc_declaration) VIS_(hash);}
	};
	
	String body;
	Value value;
	Vector<Function> funcs;
	String system;
	String model_name;
	int max_tokens = 0;
	hash_t hash = 0;
	
	void Visit(Vis& v) {v.Ver(1)(1) VIS_(body) VIS_(value) VISV(funcs) VIS_(system) VIS_(model_name) VIS_(max_tokens) VIS_(hash);}
};

struct VfsFarStage : VfsValueExt {
	String code;
	
	CLASSTYPE(VfsFarStage)
	VfsFarStage(VfsValue& n);
	void Visit(Vis& v) override;
};


struct VfsProgramIteration : VfsValueExt {
	String code, global, vfsvalue, log;
	
	DEFAULT_EXT(VfsProgramIteration)
	void Visit(Vis& v) override;
};

INITIALIZE(VfsProgramIteration);

struct VfsProgramSession : VfsValueExt {
	
	DEFAULT_EXT(VfsProgramSession)
	void Visit(Vis& v) override;
};

INITIALIZE(VfsProgramSession);

struct VfsProgramProject : VfsValueExt {
	String code;
	
	DEFAULT_EXT(VfsProgramProject)
	void Visit(Vis& v) override;
	bool GetAll(Vector<VirtualNode>& v) override;
};

INITIALIZE(VfsProgramProject);

struct VfsProgram : Component {
	String formxml;
	bool formxml_compressed = false;
	
	// Temp
	bool running = false;
	
public:
	CLASSTYPE(VfsProgram)
	VfsProgram(VfsValue& n);
	void Visit(Vis& v) override;
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	void Update(double dt) override;
	
	VirtualNode RealizePath(const VfsPath& path, hash_t type_hash);
	
	template <class T>
	VirtualNode RealizePath(const VfsPath& path) {return RealizePath(path, AsTypeHash<T>());}
	
	Event<> WhenDataTree;
	Event<VfsPath> WhenLayout;
};

INITIALIZE(VfsProgram);

POLYVALUE_STUB_HEADER(VfsForm)


// Note: see 'AiTask::CreateInput_DefaultJson' for predecessor
//       what that was:    complicated initializer for AI calls with json templates
//       what was kept:    json-based RPC for LLMs
//       what was changed: essentially, moved "AI code" from hardcoded C++ to scripting language Esc
//       define FarStage:  a remote call to the place 'far away', with unknown implementation, possibly processed in 'LLM'. Nothing to do with artifical intelligence.
//                         ~ the scripting language lets the process to escape far away ~
class FarStageCompiler {
	One<FarStage> stage;
	Vector<ProcMsg> msgs;
	
public:
	typedef FarStageCompiler CLASSNAME;
	FarStageCompiler();
	
	bool Compile(Val& stage);
	const FarStage& GetResult() const {ASSERT(stage); return *stage;}
	FarStage* PopResult() {return stage.Detach();}
	const Vector<ProcMsg>& GetMessages() const {return msgs;}
	
};

// Note: the Agent has additional Component features,
//       but it's standalone without Engine too.
struct Agent : Component {
private:
	Vector<AgentInteractionSession*> sessions;
	ArrayMap<String, EscValue> global;
	Array<FarStage> stages;
	int oplimit = 50000;
	hash_t compiled_hash = 0;
	bool run = false;
	bool run_update = false;
	
	using MsgCb = Event<Vector<ProcMsg>&>;
	
	bool Catch(Event<> cb, Vector<ProcMsg>& msgs);
	bool CompileLambdas(Vector<ProcMsg>& msgs, MsgCb WhenMessage=MsgCb());
	void RunStage(EscEscape& e, hash_t stage_hash, hash_t fn_hash);
	void CreateForm(EscEscape& e);
	void SetFormLayout(EscEscape& e);
	VfsProgram& GetProgram(EscEscape& e, int i, VfsPath& path);
	
	EscSession esc;
	MsgCb WhenMessage;
	Event<bool> WhenStop;
	bool separate_thread = false;
	EnginePtr eng;
	Ptr<VfsProgramIteration> iter;
	
public:
	CLASSTYPE(Agent)
	Agent(VfsValue& n);
	~Agent();
	
	void Visit(Vis& v) override {}
	void Update(double dt) override;
	bool RealizeLibrary(Vector<ProcMsg>& msgs);
	bool CompileStage(VfsValue& stage, bool force, MsgCb WhenMessage=MsgCb());
	bool Compile(String esc, bool force, MsgCb WhenMessage=MsgCb(), Ptr<VfsProgramIteration> iter=0);
	bool Run(bool update, MsgCb WhenMessage=MsgCb());
	bool Start() override;
	bool Start(bool update);
	bool Start(bool update, MsgCb WhenMessage, Event<bool> WhenStop=Event<bool>());
	void Set(MsgCb WhenMessage, Event<bool> WhenStop);
	void Stop() override;
	
	void SetOpLimit(int i) {oplimit = i;}
	void SetSeparateThread(bool b=true) {separate_thread = b;}
	
	const ArrayMap<String, EscValue>& GetGlobal() const {return global;}
	ArrayMap<String, EscValue>& GetGlobalRW() {return global;}
	
	Event<EscEscape&> WhenPrint;
	Event<EscEscape&> WhenInput;
	
};

using AgentPtr = Ptr<Agent>;

class AgentInteractionSession : public VfsValueExt {
	AgentPtr agent;
	
public:
	CLASSTYPE(AgentInteractionSession)
	AgentInteractionSession(VfsValue& n) : VfsValueExt(n) {}
	
	void Visit(Vis& v) override {}
	
};

class AgentInteractionPolicy : public VfsValueExt {
	enum {
		MODE_AGENT,    // Esc-script based essentially
		MODE_INTERNAL, // for high performance (without Esc script)
	};
	AgentPtr agent;
	
public:
	CLASSTYPE(AgentInteractionPolicy)
	AgentInteractionPolicy(VfsValue& n) : VfsValueExt(n) {}
	
	void Visit(Vis& v) override {}
	
};


#if 0
class AgentInteractionSystem : public VfsValueExt {
	Vector<AgentPtr> agents;
	
	void Runner();
	RunningFlagSingle flag;
	
public:
	CLASSTYPE(AgentInteractionSystem)
	AgentInteractionSystem(VfsValue& n) : VfsValueExt(n) {}
	
	void Visit(Vis& v) override {}
	bool Start() override;
	void Stop() override;
	void Attach(Agent* a);
	void Detach(Agent* a);
	
	static void Setup();
	static void Uninstall();
	static AgentInteractionSystem* sys;
};
#endif

#endif
