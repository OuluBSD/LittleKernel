#include "EonApiEditor.h"

NAMESPACE_UPP


void InterfaceBuilder::AddVolumetric() {
	Package("Volumetric", "Vol");
	SetColor(28, 85, 0);
	Dependency("ParallelLib");
	HaveIsReady();
	
	Interface("StaticSource");
	
	Vendor("RawByte");
	
}


END_UPP_NAMESPACE
