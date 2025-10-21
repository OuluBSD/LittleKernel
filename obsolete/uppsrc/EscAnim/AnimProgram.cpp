#include "EscAnim.h"

NAMESPACE_UPP


EscAnimProgram::EscAnimProgram() {
	Clear();
	user_group = -1;
}

void EscAnimProgram::Clear() {
	vm.Clear();
	code.Clear();
	user = EscValue();
	user_flags = 0;
	user_type = 0;
	//not this: user_group = -1;
	
}

void EscAnimProgram::Init(EscValue lambda) {
	ASSERT(lambda.IsLambda());
	vm = new Esc(ctx->global, ctx->op_limit, lambda.GetLambdaRW());
}

void EscAnimProgram::Continue() {
	vm->StopSleep();
}

void EscAnimProgram::Stop() {
	if (vm) {
		vm->Stop();
		WhenStop(*this);
	}
}

bool EscAnimProgram::Process() {
	Iterate();
	return vm ? vm->IsRunning() : false;
}

void EscAnimProgram::Iterate() {
	if (is_native) {
		if (is_native_running && !native_fn()) {
			is_native_running = false;
			Stop();
		}
	}
	else {
		if (vm) {
			if (vm->CheckSleepFinished()) {
				vm->Run();
				
				if (!vm->IsSleepExit()) {
					Stop();
				}
			}
		}
	}
}

bool EscAnimProgram::IsRunning() const {
	return vm && vm->IsRunning();
}

EscAnimProgram& EscAnimProgram::Set(Gate0 cb, EscValue a0, EscValue a1) {
	Clear();
	this->a0 = a0;
	this->a1 = a1;
	is_native = true;
	is_native_running = true;
	native_fn = cb;
	return *this;
}

EscAnimProgram& EscAnimProgram::Set(EscValue *self, EscValue fn, EscValue a0, EscValue a1) {
	EscGlobal& g = ctx->global;
	
	Clear();
	is_native = false;
	
	// Find & check lambda before setting fields
	EscValue lambda;
	String fn_name;
	if (fn.IsLambda()) {
		lambda = fn;
		fn_name = "<lambda>";
	}
	else {
		fn_name = fn.ToString();
		fn_name.Replace("\"", "");
		if (self && self->IsMap())
			lambda = self->MapGet(fn_name);
		if (!lambda.IsLambda()) {
			lambda = g.Get(fn_name, EscValue());
			if (!lambda.IsLambda()) {
				LOG("Key '" << fn_name << "' is not lambda");
				return *this;
			}
		}
	}
	
	Vector<EscValue> arg;
	if (!a0.IsVoid()) arg << a0;
	if (!a0.IsVoid() && !a1.IsVoid()) arg << a1;
	
	const EscLambda& l = lambda.GetLambda();
	if (arg.GetCount() != l.arg.GetCount()) {
		String argnames;
		for(int i = 0; i < l.arg.GetCount(); i++)
			argnames << (i ? ", " : "") << l.arg[i];
		LOG(Format("invalid number of arguments (%d passed, expected: %s)", arg.GetCount(), argnames));
		return *this;
	}
	
	// Set fields
	this->a0 = a0;
	this->a1 = a1;
	this->fn_name = fn_name;
	
	// Initialize esc runner
	ASSERT(vm.IsEmpty());
	vm = new Esc(g, l.code, ctx->op_limit, l.filename, l.line);
	auto& e = *vm;
	if (self)
		e.Self() = *self;
	for(int i = 0; i < l.arg.GetCount(); i++)
		e.Var().GetAdd(l.arg[i]) = arg[i];
	
	//e.no_return = e.no_break = e.no_continue = true;
	//e.loop = 0;
	//e.skipexp = 0;
	
	ASSERT(vm->IsRunning());
	ASSERT(IsRunning());
	LOG("Script::Set: started " << fn_name);
	
	return *this;
}


END_UPP_NAMESPACE
