#include "Shell.h"

NAMESPACE_UPP




void MachineEcsInit(Engine& mach) {
	#ifdef flagPHYSICS
	#ifdef flagODE
	mach.FindAdd<SystemT<OdeFys>>();
	#else
	mach.FindAdd<SystemT<TosFys>>();
	#endif
	#endif
}

void EngineEcsInit(Engine& eng) {
    eng.WhenInitialize << callback(EngineEcsInit);
    
	//eng.GetAdd<RegistrySystem>();
	eng.GetAdd<InteractionSystem>();
	
	eng.GetAdd<RenderingSystem>();
	//eng.GetAdd<ComponentStore>();
	eng.GetAdd<EventSystem>();
	
	#if 0
	#if HAVE_WINDOWSYSTEM
	eng.GetAdd<VirtualGuiSystem>();
	eng.GetAdd<WindowSystem>();
	#endif
	#endif
	
	//DefaultSerialInitializerInternalEon();
}


END_UPP_NAMESPACE
