#include "EonApiEditor.h"

NAMESPACE_UPP


void InterfaceBuilder::AddHolographic() {
	Package("Holograph", "Holo");
	SetColor(85, 42, 150);
	Dependency("ParallelLib");
	Dependency("IMedia");
	Dependency("ports/hcidump", "LINUX", false);
	Dependency("LocalHMD", "LOCALHMD");
	Library("openhmd hidapi-libusb", "LINUX & OPENHMD");
	HaveRecvFinalize();
	HaveIsReady();
	
	Interface("SinkDevice");
	
	Vendor("OpenHMD", "LINUX&OPENHMD|FREEBSD&OPENHMD");
	Vendor("LocalHMD", "LOCALHMD");
	Vendor("RemoteVRServer", "LINUX|FREEBSD");
	Vendor("DevUsb", "LINUX|FREEBSD");
	Vendor("DevBluetooth", "LINUX&HACK|FREEBSD&HACK");
	Vendor("OpenVR", "WIN32&OPENVR");
	
	
}


END_UPP_NAMESPACE
