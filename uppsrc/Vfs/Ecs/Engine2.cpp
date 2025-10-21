#include "Ecs.h"

// Keeping 3 separate Engine files for historical git-log reasons

NAMESPACE_UPP


System::System(VfsValue& n) : VfsValueExt(n) {
	DBG_CONSTRUCT
}

System::~System() {
	DBG_DESTRUCT
}

Engine& System::GetEngine() const {
	Engine* mach = val.FindOwner<Engine>();
	ASSERT(mach);
	if (!mach) throw Exc("no machine");
	return *mach;
}

Val& Engine::GetRootPool() {
	return val.GetAdd("pool", 0);
}

Val& Engine::GetRootLoop() {
	return val.GetAdd("loop", 0);
}

Val& Engine::GetRootSpace() {
	return val.GetAdd("space", 0);
}

void Engine::WarnDeveloper(String msg) {
	if (last_warnings.Find(msg) < 0) {
		last_warnings.Add(msg);
		
		#if !defined flagDEBUG_RT && defined flagDEBUG
		LOG("Machine: developer warning: " << msg);
		#endif
	}
}

bool Engine::StartLoad(String app_name, String override_eon_file, ValueMap extra_args, const char* extra_str) {
	
	if (!override_eon_file.IsEmpty())
		eon_file = override_eon_file;
	
	{
		for(int i = 0; i < extra_args.GetCount(); i++)
			eon_params(extra_args.GetKey(i)) = extra_args.GetValue(i);
	}
	
	if (extra_str) {
		Vector<String> args = Split(extra_str, ";");
		for (String& arg : args) {
			int i = arg.Find("=");
			if (i >= 0) {
				String a = arg.Left(i);
				String b = arg.Mid(i+1);
				eon_params(a) = b;
			}
		}
	}
	
	WhenBoot(*this);
	
	//break_addr = 1;
	if (is_failed)
		return false;
	
	StartMain(eon_script, eon_file, eon_params, 1, break_addr);
	
	return !is_failed;
}

END_UPP_NAMESPACE

