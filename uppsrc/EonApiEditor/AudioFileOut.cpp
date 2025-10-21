#include "EonApiEditor.h"

NAMESPACE_UPP


void InterfaceBuilder::AddAudioFileOut() {
	Package("AudioFileOut", "AFO");
	SetColor(80, 58, 119);
	Dependency("ParallelLib");
	HaveRecvFinalize();
	HaveIsReady();
	
	Interface("Sink");
	
	Vendor("CoreAudio");
	
}


END_UPP_NAMESPACE
