#include "PortManager.h"


NAMESPACE_UPP


PortManager::PortManager() {
	
	
}


void PortManagerMain() {
	using namespace UPP;
	using namespace UPP;
	SetCoutLog();
	
	PortManager mgr;
	
	String ports_path = GetDataFile("ports.json");
	if (!FileExists) {
		LOG("Could not find ports.json");
		Exit(1);
	}
	
	
}


END_UPP_NAMESPACE


CONSOLE_APP_MAIN {Upp::PortManagerMain();}

