#include "Graphics.h"
#include <Eon/Lib/Lib.h>


NAMESPACE_UPP


EcsViewProg::EcsViewProg() {
	
}


bool EcsViewProg::Initialize(const WorldState& ws) {
	return true;
}


void EcsViewProg::Uninitialize() {
	
}


bool EcsViewProg::Arg(const String& key, const String& value) {
	
	return true;
}


bool EcsViewProg::Render(Draw& fb) {
	GfxStateDraw* sd = CastPtr<GfxStateDraw>(&fb);
	ASSERT(sd);
	
	GfxDataState& state = sd->GetState();
	ASSERT(state.eng);
	
	Engine& eng = *state.eng;
	
	RenderingSystemPtr rend = eng.Get<RenderingSystem>();
	ASSERT(rend);
	
	
	// Default values
	state.light_dir = vec3 {sinf(DEG2RAD(75)), 0.0f, cosf(DEG2RAD(20))};
	
	
	rend->Render(state);
	
	return true;
}


END_UPP_NAMESPACE

