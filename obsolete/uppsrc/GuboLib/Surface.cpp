#include "GuboLib.h"


NAMESPACE_UPP


void TopSurface::CreateGeom2DComponent() {
	using namespace Ecs;
	
	
	
	Engine& mach = GetActiveMachine();
	Gu::SurfaceSystemRef wins = mach.Get<Gu::SurfaceSystem>();
	Gu::SurfaceManager& mgr = wins->GetActiveScope();
	mgr.AddInterface(*this);
	
}


END_UPP_NAMESPACE
