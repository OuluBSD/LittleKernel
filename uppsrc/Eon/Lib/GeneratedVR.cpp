#include "Lib.h"

// This file is generated. Do not modify this file.

NAMESPACE_UPP



#if (defined flagFREEBSD && defined flagOPENHMD) || (defined flagLINUX && defined flagOPENHMD)
String OpenHMDPipe::GetAction() {
	return "x11.ogl.ohmd.events";
}

AtomTypeCls OpenHMDPipe::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //OPEN_HMDPIPE;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,ORDER),0);
	t.AddOut(VD(CENTER,EVENT),0);
	return t;
}

LinkTypeCls OpenHMDPipe::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void OpenHMDPipe::Visit(Vis& v) {
	VIS_THIS(OpenHMDSinkDevice);
}

AtomTypeCls OpenHMDPipe::GetType() const {
	return GetAtomType();
}
#endif


#if defined flagLOCALHMD
String LocalHMDPipe::GetAction() {
	return "x11.ogl.holo.events";
}

AtomTypeCls LocalHMDPipe::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //LOCAL_HMDPIPE;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,ORDER),0);
	t.AddOut(VD(CENTER,EVENT),0);
	return t;
}

LinkTypeCls LocalHMDPipe::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void LocalHMDPipe::Visit(Vis& v) {
	VIS_THIS(LocalHMDSinkDevice);
}

AtomTypeCls LocalHMDPipe::GetType() const {
	return GetAtomType();
}
#endif


#if (defined flagFREEBSD) || (defined flagLINUX)
String RemoteVRServerPipe::GetAction() {
	return "tcp.ogl.holo.events";
}

AtomTypeCls RemoteVRServerPipe::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //REMOTE_VRSERVER_PIPE;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,ORDER),0);
	t.AddOut(VD(CENTER,EVENT),0);
	return t;
}

LinkTypeCls RemoteVRServerPipe::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void RemoteVRServerPipe::Visit(Vis& v) {
	VIS_THIS(RemoteVRServerSinkDevice);
}

AtomTypeCls RemoteVRServerPipe::GetType() const {
	return GetAtomType();
}
#endif


#if (defined flagFREEBSD && defined flagHACK) || (defined flagLINUX && defined flagHACK)
String BluetoothHoloPipe::GetAction() {
	return "bluetooth.holo.events";
}

AtomTypeCls BluetoothHoloPipe::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //BLUETOOTH_HOLO_PIPE;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,ORDER),0);
	t.AddOut(VD(CENTER,EVENT),0);
	return t;
}

LinkTypeCls BluetoothHoloPipe::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void BluetoothHoloPipe::Visit(Vis& v) {
	VIS_THIS(DevBluetoothSinkDevice);
}

AtomTypeCls BluetoothHoloPipe::GetType() const {
	return GetAtomType();
}
#endif


#if (defined flagUWP && defined flagDX12)
String HoloContextAtom::GetAction() {
	return "holo.context";
}

AtomTypeCls HoloContextAtom::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //HOLO_CONTEXT_ATOM;
	t.role = AtomRole::DRIVER;
	t.AddIn(VD(CENTER,RECEIPT),0);
	t.AddOut(VD(CENTER,RECEIPT),0);
	return t;
}

LinkTypeCls HoloContextAtom::GetLinkType() {
	return LINKTYPE(DRIVER, DRIVER);
}

void HoloContextAtom::Visit(Vis& v) {
	VIS_THIS(HoloContextBase);
}

AtomTypeCls HoloContextAtom::GetType() const {
	return GetAtomType();
}
#endif


#if (defined flagUWP && defined flagDX12)
String HoloEventAtomPipe::GetAction() {
	return "holo.event.pipe";
}

AtomTypeCls HoloEventAtomPipe::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //HOLO_EVENT_ATOM_PIPE;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,ORDER),0);
	t.AddOut(VD(CENTER,EVENT),0);
	return t;
}

LinkTypeCls HoloEventAtomPipe::GetLinkType() {
	return LINKTYPE(POLLER_PIPE, PROCESS);
}

void HoloEventAtomPipe::Visit(Vis& v) {
	VIS_THIS(HoloEventsBase);
}

AtomTypeCls HoloEventAtomPipe::GetType() const {
	return GetAtomType();
}
#endif


#if (defined flagUWP && defined flagDX12 && defined flagDX12)
String HoloD12FboAtomSA::GetAction() {
	return "holo.fbo.standalone";
}

AtomTypeCls HoloD12FboAtomSA::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //HOLO_D12_FBO_ATOM_SA;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(DX,ORDER),0);
	t.AddOut(VD(DX,RECEIPT),0);
	return t;
}

LinkTypeCls HoloD12FboAtomSA::GetLinkType() {
	return LINKTYPE(POLLER_PIPE, PROCESS);
}

void HoloD12FboAtomSA::Visit(Vis& v) {
	VIS_THIS(HoloD12VideoSinkDevice);
}

AtomTypeCls HoloD12FboAtomSA::GetType() const {
	return GetAtomType();
}
#endif

END_UPP_NAMESPACE

