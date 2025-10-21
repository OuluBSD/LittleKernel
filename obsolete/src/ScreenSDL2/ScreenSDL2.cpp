#include "ScreenSDL2.h"
#include <SerialLib/SerialLib.h>
#include <AtomSDL2/AtomSDL2.h>
//#include <Physics/Physics.h>

NAMESPACE_UPP


bool SingleMachine::Open(void(*arg_fn)()) {
	using namespace Serial;
	
	const AppFlags& flags = GetAppFlags();
	Engine& mach = GetActiveMachine();
	
	
	RegistrySystemRef reg	= mach.Add<RegistrySystem>();
	LoopStoreRef ents		= mach.Add<LoopStore>();
    
    mach.Add<AtomStore>();
    mach.Add<AtomSystem>();
    mach.Add<ScriptLoader>();
    mach.Add<PacketTracker>();
    
    
    if(arg_fn)
        arg_fn();
    
    reg->SetAppName("SDL2 ECS machine");
    
    if (!mach.Start())
		return false;
    
	return true;
}

void SingleMachine::Close() {
	using namespace Serial;
	Engine& mach = GetActiveMachine();
	mach.Stop();
}



END_UPP_NAMESPACE
