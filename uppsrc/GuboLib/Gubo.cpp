#include "GuboLib.h"

NAMESPACE_UPP


void TopGubo::CreateGeom3DComponent() {
	using namespace Ecs;
	
	
	
	Engine& mach = GetActiveMachine();
	Gu::GuboSystemRef wins = mach.Get<Gu::GuboSystem>();
	Gu::GuboManager& mgr = wins->GetActiveScope();
	mgr.AddInterface(*this);
	
}


END_UPP_NAMESPACE
