#include "Geometry.h"


NAMESPACE_UPP




WmrFusionSystem::WmrFusionSystem() {
	
}

void WmrFusionSystem::Process() {
	
	if (in_bright.lock && *in_bright.lock) {
		LOG("WmrFusionSystem::Process: fake bright image process");
		Sleep(2);
		
		*in_bright.lock = false;
	}
	
	if (in_dark.lock && *in_dark.lock) {
		LOG("WmrFusionSystem::Process: fake dark image process");
		Sleep(2);
		
		*in_dark.lock = false;
	}
	
	
	if (new_ev) {
		LOG("WmrFusionSystem::Process: fake EventCtrl process");
		Sleep(2);
		
		new_ev = false;
	}
}

void WmrFusionSystem::PutSensorData(GeomEvent& ev) {
	new_ev = true;
	if (ev.ctrl)
		in_ctrl = *ev.ctrl;
	if (ev.trans)
		in_trans = *ev.trans;
	in_ev = ev;
	in_ev.ctrl = 0;
	in_ev.trans = 0;
}

void WmrFusionSystem::Attach(SerialServiceServer& server) {
	TODO
	#if 0
	server.AddStream(NET_SEND_FUSION_DATA, THISBACK(SendFusionData));
	#endif
}

void WmrFusionSystem::SendFusionData(Stream& in, Stream& out) {
	
	TODO
	
}

void WmrFusionSystem::PutBrightFrame(bool& lock, Size sz, const Vector<byte>& data) {
	lock = true;
	in_bright.lock = &lock;
	in_bright.sz = sz;
	in_bright.data = &data;
}

void WmrFusionSystem::PutDarkFrame(bool& lock, Size sz, const Vector<byte>& data) {
	lock = true;
	in_dark.lock = &lock;
	in_dark.sz = sz;
	in_dark.data = &data;
}











WmrFusionSystemReceiver::WmrFusionSystemReceiver() {
	
}

bool WmrFusionSystemReceiver::UpdateFrom(SerialServiceClient& client) {
	
	TODO
	#if 0
	if (!client.CallStream(NET_LATEST_BRIGHT_FRAME, THISBACK(GetEvent)))
		return false;
	#endif
	
	return ev_sendable;
}

void WmrFusionSystemReceiver::GetEvent(Stream& in, Stream& out) {
	
	TODO
	
	ev.ctrl = &ctrl;
	ev.trans = &trans;
	ev_sendable = true;
}

END_UPP_NAMESPACE

