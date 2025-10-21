#include "GuboCore.h"
#include <GuboLib/ScopeT.h>
#include <Eon/Eon.h>


NAMESPACE_UPP


TopSurface::TopSurface() {
	SetFrameRect(RectC(0,0,320,240));
	CreateGeom2DComponent();
	
}

/*Surface* TopSurface::GetSurface() {
	return this;
}*/

void TopSurface::OpenMain() {
	// Ensure registration and focus as main
	CreateGeom2DComponent();
	FocusEvent();
}

void TopSurface::Run() {
	// Bring to front and enter shared main loop
	FocusEvent();
	Surface::EventLoop();
}

void TopSurface::RunInMachine() {
	Run();
}

void TopSurface::FocusEvent() {
	using namespace Ecs;
	Parallel::Engine& mach = GetActiveMachine();
	Gu::SurfaceSystemRef wins = mach.Get<Gu::SurfaceSystem>();
	if (wins) {
		Gu::SurfaceManager& mgr = wins->GetActiveScope();
		mgr.FocusHandle(this);
	}
}

#if 0
void TopSurface::UpdateFromTransform2D() {
	using namespace Ecs;
	ASSERT(cw);
	if (!cw) return;
	
	EntityPtr e = this->cw->GetEntity();
	Ptr<Transform2D> tr = e->Find<Transform2D>();
	ASSERT(tr);
	if (!tr) return;
	
	Transform2D& t = *tr;
	Geom2DComponent& cw = *this->cw;
	
	Rect r = cw.GetFrameRect();
	Size t_size = ToSize(t.size);
	Point t_pos = ToPoint(t.position);
	if (r.Width()  != t_size.cx ||
		r.Height() != t_size.cy ||
		r.left     != t_pos.x ||
		r.top      != t_pos.y) {
		r.left = t_pos.x;
		r.top = t_pos.y;
		r.right = r.left + t_size.cx;
		r.bottom = r.top + t_size.cy;
		cw.SetFrameRect0(r);
	}
	
	if (cw.IsPendingLayout()) {
		cw.DeepLayout();
		cw.SetPendingEffectRedraw();
	}
}
#endif


END_UPP_NAMESPACE
