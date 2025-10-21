#include "Effect.h"

#ifdef flagLV2

NAMESPACE_UPP



bool FxLV2::Effect_Create(NativeEffect*& dev){
	
}

void FxLV2::Effect_Destroy(NativeEffect*& dev){
	
}

bool FxLV2::Effect_Initialize(NativeEffect&, AtomBase&, const WorldState&){
	
}

bool FxLV2::Effect_PostInitialize(NativeEffect&, AtomBase&){
	
}

bool FxLV2::Effect_Start(NativeEffect&, AtomBase&){
	
}

void FxLV2::Effect_Stop(NativeEffect&, AtomBase&){
	
}

void FxLV2::Effect_Uninitialize(NativeEffect&, AtomBase&){
	
}

bool FxLV2::Effect_Send(NativeEffect&, AtomBase&, RealtimeSourceConfig& cfg, PacketValue& out, int src_ch){
	
}

void FxLV2::Effect_Visit(NativeEffect&, AtomBase&, Visitor& vis){
	
}

bool FxLV2::Effect_Recv(NativeEffect&, AtomBase&, int, const Packet&){
	
}

void FxLV2::Effect_Finalize(NativeEffect&, AtomBase&, RealtimeSourceConfig&){
	
}

bool FxLV2::Effect_IsReady(NativeEffect&, AtomBase&, PacketIO& io){
	
}



END_UPP_NAMESPACE

#endif
