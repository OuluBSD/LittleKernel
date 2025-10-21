#include "Runtime.h"
#include <AI/Core/Prompting/Prompting.h>
#ifdef flagGUI
#include <CtrlCore/CtrlCore.h>
#endif


NAMESPACE_UPP


INITBLOCK {
	
}


Agent::Agent(VfsValue& n) : Component(n) {
	eng = n.FindOwner<Engine>();
	if (!eng)
		eng = n.FindOwnerWith<Engine>();
	if (eng)
		eng->AddUpdated(this);
}

Agent::~Agent() {
	if (eng)
		eng->RemoveUpdated(this);
}

bool Agent::RealizeLibrary(Vector<ProcMsg>& msgs) {
	if (global.IsEmpty()) {
		Escape(global, "Print(x)", Proxy(WhenPrint));
		Escape(global, "Input()", Proxy(WhenInput));
		Escape(global, "CreateForm(path)", THISBACK(CreateForm));
		Escape(global, "SetFormLayout(form, path)", THISBACK(SetFormLayout));
		StdLib(global);
	}
	for(const FarStage& fs : stages) {
		for (const auto& fn : fs.funcs) {
			if (fn.esc_declaration.GetCount()) {
				Escape(global, fn.esc_declaration, THISBACK2(RunStage, fs.hash, fn.hash));
			}
		}
	}
	return true;
}

void Agent::RunStage(EscEscape& e, hash_t stage_hash, hash_t fn_hash) {
	FarStage* stagep = 0;
	FarStage::Function* fnp = 0;
	int fn_i = 0;
	for (FarStage& s : stages) {
		if (s.hash != stage_hash) continue;
		stagep = &s;
		for (auto& fn : s.funcs) {
			if (fn.hash != fn_hash) {fn_i++; continue;}
			fnp = &fn;
			break;
		}
		break;
	}
	if (!stagep || !fnp)
		e.esc.ThrowError("Stage not found");
	
	VfsPath ret_path;
	ret_path.SetPosixPath(fnp->ret);
	if (ret_path.IsEmpty())
		e.esc.ThrowError("Invalid return path in stage");
	
	if (e.GetCount() != fnp->params.GetCount())
		e.esc.ThrowError("Unexpected number of params (" + IntStr(e.GetCount()) + ", expected " + IntStr(fnp->params.GetCount()) + ")");
	ValueMap params;
	for(int i = 0; i < fnp->params.GetCount(); i++) {
		Value arg = StdValueFromEsc(e[i]);
		if (arg.IsError())
			e.esc.ThrowError("argument conversion failed: " + GetErrorText(arg));
		if (arg.IsNull())
			e.esc.ThrowError("null argument");
		params.Add(
			fnp->params[i],
			arg);
	}
	
	TaskMgr& m = AiTaskManager();
	
	// Prevent write to freed memory on late result: use Ptr<>
	struct Result : Pte<Result> {String s;};
	Result result;
	Ptr<Result> result_ptr = &result;
	
	bool done = false;
	m.GetFarStage(stagep, fn_i, params,
	[this,result_ptr](String res) {
		if (result_ptr) // valid if timeout haven't been triggered
			result_ptr->s = res;
	},
	[this,&done]{
		done = true;
	});
	int max_wait = 200; // *0.1 seconds = 20 seconds
	int wait = 0;
	while (!done) {
		Sleep(100);
		if (++wait >= max_wait) {
			e.esc.ThrowError("Stage timeout");
		}
	}
	if (result.s.IsEmpty())
		e.esc.ThrowError("Empty result string");
	
	// try parse the result
	Value v = ParseJSON(result.s, true);
	Value res = FindValuePath(v, ret_path);
	if (res.IsNull() && ret_path.Parts()[0] == "response") {
		ret_path.Remove(0);
		res = FindValuePath(v, ret_path);
	}
	e = EscFromStdValue(res);
}

bool Agent::Catch(Event<> cb, Vector<ProcMsg>& msgs) {
	bool succ = true;
	try {
		cb();
	}
	catch(CParser::Error e) {
		ProcMsg& m = msgs.Add();
		m.msg = e;
		m.severity = PROCMSG_ERROR;
		succ = false;
	}
	catch(Exc e) {
		ProcMsg& m = msgs.Add();
		m.msg = "Exception: " + e;
		m.severity = PROCMSG_ERROR;
		succ = false;
	}
	catch(...) {
		ProcMsg& m = msgs.Add();
		m.msg = "Unknown error";
		m.severity = PROCMSG_ERROR;
		succ = false;
	}
	return succ;
}

bool Agent::CompileStage(VfsValue& stage, bool force, MsgCb WhenMessage) {
	TimeStop ts;
	
	ASSERT(stage.type_hash == AsTypeHash<VfsFarStage>());
	if (stage.type_hash != AsTypeHash<VfsFarStage>())
		return false;
	
	bool succ = true;
	FarStageCompiler comp;
	
	Vector<ProcMsg> msgs;
	succ = Catch([this,&comp,&stage]{
		comp.Compile(stage);
	}, msgs);
	
	msgs.Append(comp.GetMessages());
	
	if (succ) {
		ProcMsg& m = msgs.Add();
		m.msg = "Stage compilation succeeded in " + ts.ToString();
		m.severity = PROCMSG_INFO;
		
		stages.Add(comp.PopResult());
	}
	
	if (msgs.GetCount())
		WhenMessage(msgs);
	
	return succ;
}

bool Agent::Compile(String esc, bool force, MsgCb WhenMessage, Ptr<VfsProgramIteration> iter) {
	TimeStop ts;
	
	this->iter = iter;
	
	hash_t h = esc.GetHashValue();
	if (!force && compiled_hash && compiled_hash == h)
		return true;
	
	compiled_hash = 0;
	bool succ = true;
	
	Vector<ProcMsg> msgs;
	
	global.Clear(); // clear global for now, because of stage functions
	
	if (!RealizeLibrary(msgs)) {
		WhenMessage(msgs);
		return false;
	}
	
	succ = Catch([this,&esc]{
		Scan(global, esc);
	}, msgs);
	
	if (succ)
		succ = CompileLambdas(msgs, WhenMessage);
	
	if (succ) {
		ProcMsg& m = msgs.Add();
		m.msg = "Compile succeeded in " + ts.ToString();
		m.severity = PROCMSG_INFO;
	}
	
	if (msgs.GetCount())
		WhenMessage(msgs);
	
	if (succ)
		compiled_hash = h;
	
	return succ;
}

bool Agent::CompileLambdas(Vector<ProcMsg>& msgs, MsgCb WhenMessage) {
	bool succ = true;
	#if USE_ESC_BYTECODE
	for(int i = 0; i < global.GetCount(); i++) {
		String key = global.GetKey(i);
		EscValue& ev = global[i];
		if (!ev.IsLambda())
			continue;
		EscLambda& l = ev.GetLambdaRW();
		if (l.compiled || l.escape)
			continue;
		
		if (!Catch([this,&ev]{
			::Upp::Compile(global, 0, ev);
		}, msgs))
			succ = false;
	}
	#endif
	return succ;
}

bool Agent::Run(bool update, MsgCb WhenMessage) {
	bool succ = true;
	
	Vector<ProcMsg> msgs;
	succ = Catch([this, update, &msgs]{
		String fn = update ? "update" : "main";
		esc.Execute(global, fn, oplimit, [&msgs](ProcMsg& m) {msgs.Add(m);});
	}, msgs);
	
	if (msgs.GetCount())
		WhenMessage(msgs);
	
	return succ;
}

void Agent::Set(MsgCb WhenMessage, Event<bool> WhenStop) {
	this->WhenMessage = WhenMessage;
	this->WhenStop = WhenStop;
}

bool Agent::Start() {
	return Start(false);
}

bool Agent::Start(bool update, MsgCb WhenMessage, Event<bool> WhenStop) {
	Set(WhenMessage, WhenStop);
	return Start(update);
}

bool Agent::Start(bool update) {
	if (separate_thread) {
		esc.Stop();
		Thread::Start([this, update]{
			bool succ = Run(update, WhenMessage);
			WhenStop(succ);
		});
	}
	else {
		GetEngine().AddUpdated(this);
		run = true;
		run_update = update;
	}
	return true;
}

void Agent::Stop() {
	if (separate_thread) {
		esc.Stop();
	}
	else {
		GetEngine().RemoveUpdated(this);
	}
}

void Agent::Update(double dt) {
	if (run) {
		run = false;
		bool succ = Run(run_update, WhenMessage);
		WhenStop(succ);
	}
}

VfsProgram& Agent::GetProgram(EscEscape& e, int i, VfsPath& path) {
	path.SetPosixPath(e[i]);
	if (path.IsEmpty())
		e.ThrowError("empty vfs path argument");
	
	if (!iter)
		e.ThrowError("no VfsProgramIteration set");
	
	auto prog = iter->val.FindOwner<VfsProgram>();
	if (!prog)
		e.ThrowError("cannot find VfsProgram");
	return *prog;
}

void Agent::CreateForm(EscEscape& e) {
	VfsPath path;
	auto& prog = GetProgram(e, 0, path);
	prog.RealizePath<VfsForm>(path);
	
	e.ret_val = e[0];
	
	#ifdef flagGUI
	PostCallback([&prog]{prog.WhenDataTree();});
	#endif
}

void Agent::SetFormLayout(EscEscape& e) {
	VfsPath path;
	auto& prog = GetProgram(e, 0, path);
	auto form_node = prog.RealizePath<VfsForm>(path);
	ASSERT(form_node.IsValue());
	
	Value v = form_node.GetValue();
	if (!v.IsError()) {
		ValueMap map = v;
		map.Set("layout_path", (String)e[1]);
		form_node.WriteValue(map);
	}
	
	prog.WhenLayout(path);
}


#if 0
bool AgentInteractionSystem::Start() {
	if (flag.IsRunning()) return true;
	flag.Start();
	Thread::Start(THISBACK(Runner));
	return true;
}

void AgentInteractionSystem::Stop() {
	flag.Stop();
}

void AgentInteractionSystem::Runner() {
	
	while (flag.IsRunning() && !Thread::IsShutdownThreads()) {
		
		
		Sleep(1);
	}
	
	flag.SetStopped();
}

void AgentInteractionSystem::Attach(Agent* a) {
	for (auto& ap : agents)
		if (ap == a)
			return;
	
}

void AgentInteractionSystem::Detach(Agent* a) {
	for(int i = 0; i < agents.GetCount(); i++) {
		if (agents[i] == a) {
			agents.Remove(i);
			
			
			return;
		}
	}
}

void AgentInteractionSystem::Setup() {
	AgentInteractionSystem& sys = MetaEnv().root.Add<AgentInteractionSystem>("agentsys");
	AgentInteractionSystem::sys = &sys;
	sys.Start();
}

void AgentInteractionSystem::Uninstall() {
	if (sys)
		sys->Stop();
}

AgentInteractionSystem* AgentInteractionSystem::sys;

#endif



VfsFarStage::VfsFarStage(VfsValue& n) : VfsValueExt(n) {
	const AstValue* a = n;
	if (a) {
		n.value = String(); // the code text is here
	}
}

void VfsFarStage::Visit(Vis& v) {
	v.Ver(1)
	(1)	VIS_(code)
		;
}


END_UPP_NAMESPACE
