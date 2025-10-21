#include "EscAnim.h"

NAMESPACE_UPP


EscAnimContext::EscAnimContext() {
	Clear();
}

void EscAnimContext::Clear() {
	code.Clear();
	global.Clear();
	progs.Clear();
	p.Clear();
	a.Clear();
	op_limit = 1000000;
	keep_running = false;
	
	
	p.SetAnimation(a);
}

bool EscAnimContext::Init(bool run_main) {
	
	if (!code.IsEmpty()) {
	    Escape(global, "DrawText(x,y,str,sleep)", THISBACK(ESC_DrawText));
	    StdLib(global);
	    
		try {
			for(int i = 0; i < code.GetCount(); i++)
				Scan(global, code[i], code.GetKey(i));
		}
	    catch(Exc e) {
	        LOG("ERROR: " << e << "\n");
	        return false;
	    }
		
		if (run_main)
			CreateProgram("main");
	}
	
	p.Compile();
	p.Play();
	
	return true;
}

void EscAnimContext::CreateProgram(String name) {
	
	int i = global.Find(name);
	if (i >= 0) {
		EscValue lambda = global[i];
		if (lambda.IsLambda()) {
			EscAnimProgram& prog = progs.Add();
			prog.ctx = this;
			prog.fn_name = name;
			prog.Init(lambda);
		}
	}
	
}

bool EscAnimContext::AddCodePath(String path) {
	String content = LoadFile(path);
	if (content.IsEmpty()) {
		LOG("error: empty script");
		return false;
	}
	
	code.GetAdd(path) = content;
	
	return true;
}

EscAnimProgram* EscAnimContext::FindProgram(EscEscape& e) {
	#ifndef _Esc_Bytecode_h_
	#error wtf
	#endif
	IrVM& vm = e.esc;
	for (EscAnimProgram& prog : progs)
		if (prog.IsVm() && &prog.GetVm() == &vm.esc)
			return &prog;
	return 0;
}

void EscAnimContext::InitializeEmptyScene() {
	ASSERT(a.scenes.IsEmpty());
	AnimScene& s = a.AddScene("empty");
}

void EscAnimContext::Iterate() {
	if (IsRunning()) {
		for (EscAnimProgram& prog : progs)
			prog.Iterate();
		
		p.Data();
		
		if (!IsRunning())
			WhenStop();
	}
}

bool EscAnimContext::IsRunning() const {
	if (keep_running)
		return true;
	
	if (p.IsRunning())
		return true;
	
	for (const EscAnimProgram& prog : progs)
		if (prog.IsRunning())
			return true;
	
	return false;
}

void EscAnimContext::ProcessAndRemoveGroup(int group) {
	int i = 0;
	Vector<int> rm_list;
	for (EscAnimProgram& p : progs) {
		if (p.user_group == group) {
			p.Process();
			
			if (!p.IsRunning())
				rm_list << i;
		}
		i++;
	}
	
	if (rm_list.GetCount())
		progs.Remove(rm_list);
}

void EscAnimContext::StopProgram(EscAnimProgram& p) {
	int i = 0;
	for (EscAnimProgram& prog : progs) {
		if (&prog == &p) {
			if (prog.IsRunning())
				prog.Stop();
			progs.Remove(i);
			return;
		}
		i++;
	}
	ASSERT_(0, "program was not found from context");
}

bool EscAnimContext::HasProgram(EscAnimProgram& p) {
	for (EscAnimProgram& prog : progs)
		if (&prog == &p)
			return true;
	return false;
}

EscValue EscAnimContext::GetGlobal(String key) {
	return global.Get(key, EscValue());
}

void EscAnimContext::RemoveProgramGroup(int group) {
	if (group < 0)
		return;
	Vector<int> rm_list;
	int i = 0;
	for (EscAnimProgram& prog : progs) {
		if (prog.user_group == group) {
			if (prog.IsRunning())
				prog.Stop();
			rm_list << i;
		}
		i++;
	}
	if (!rm_list.IsEmpty())
		progs.Remove(rm_list);
}

void EscAnimContext::RemoveStopped() {
	Vector<int> rm_list;
	int i = 0;
	for (EscAnimProgram& prog : progs) {
		if (!prog.IsRunning()) {
			rm_list << i;
		}
		i++;
	}
	if (!rm_list.IsEmpty())
		progs.Remove(rm_list);
}

void EscAnimContext::RemoveStoppedGroup(int group) {
	if (group < 0)
		return;
	Vector<int> rm_list;
	int i = 0;
	for (EscAnimProgram& prog : progs) {
		if (!prog.IsRunning() && prog.user_group == group)
			rm_list << i;
		i++;
	}
	if (!rm_list.IsEmpty())
		progs.Remove(rm_list);
}

Vector<EscAnimProgram*> EscAnimContext::FindGroupPrograms(int group) {
	Vector<EscAnimProgram*> v;
	for (EscAnimProgram& prog : progs) {
		if (prog.user_group == group)
			v.Add(&prog);
	}
	return v;
}



END_UPP_NAMESPACE
