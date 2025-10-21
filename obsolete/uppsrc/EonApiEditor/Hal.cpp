#include "EonApiEditor.h"

NAMESPACE_UPP


void InterfaceBuilder::AddHal() {
	Package("Hal", "Hal");
	SetColor(198, 127, 200);
	Dependency("ParallelLib");
	Dependency("IGraphics");
	Library("SDL2", "SDL2");
	Library("SDL2_ttf", "SDL2");
	Library("SDL2_image", "SDL2");
	Library("D3D11 DXGI D2d1 Dwrite", "UWP&DX12");
	HaveRecvFinalize();
	HaveUpdate();
	HaveIsReady();
	HaveContextFunctions();
	
	Interface("AudioSinkDevice");
	Interface("CenterVideoSinkDevice");
	Interface("CenterFboSinkDevice");
	Interface("OglVideoSinkDevice", "OGL");
	Interface("D12VideoSinkDevice", "DX12");
	Interface("ContextBase");
	Interface("EventsBase");
	Interface("UppEventsBase", "GUI");
	Interface("UppOglDevice", "OGL");
	
	Vendor("Sdl", "SDL2");
	Vendor("Holo", "UWP&DX12"); // Microsoft Hololens
	
}


END_UPP_NAMESPACE
