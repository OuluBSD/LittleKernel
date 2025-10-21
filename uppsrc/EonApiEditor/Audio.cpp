#include "EonApiEditor.h"

NAMESPACE_UPP


void InterfaceBuilder::AddAudio() {
	Package("Audio", "Aud");
	SetColor(226, 212, 0);
	Dependency("ParallelLib");
	Dependency("ports/portaudio", "BUILTIN_PORTAUDIO");
	Library("portaudio", "PORTAUDIO");
	HaveNegotiateFormat();
	
	Interface("SinkDevice");
	Interface("SourceDevice");
	
	Vendor("Portaudio", "BUILTIN_PORTAUDIO|PORTAUDIO");
	
}


END_UPP_NAMESPACE
