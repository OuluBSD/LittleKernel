#include "EonApiEditor.h"

NAMESPACE_UPP


void InterfaceBuilder::AddEffect() {
	Package("Effect", "Fx");
	SetColor(28, 255, 150);
	Dependency("AudioCore");
	Dependency("ParallelLib");
	Dependency("ports/lilv", "LV2", false);
	Dependency("AudioHost", "LV2", false);
	HaveRecvFinalize();
	HaveIsReady();
	
	Interface("Effect");
	
	Vendor("AudioCore");
	Vendor("LV2", "LV2");
	
}


END_UPP_NAMESPACE
