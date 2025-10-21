#include "GuboCore.h"
#include <GuboCore/GuboCore.h>
#include <GuboLib/ScopeT.h>
#include <Eon/Eon.h>


NAMESPACE_UPP


TopGubo::TopGubo() {
	SetFrameBox(CubfC(0,0,0,320,240,240));
	CreateGeom3DComponent();
	
}

void TopGubo::FocusEvent() {
	using namespace Ecs;
	Parallel::Engine& mach = GetActiveMachine();
	Gu::GuboSystemRef wins = mach.Get<Gu::GuboSystem>();
	if (wins) {
		Gu::GuboManager& mgr = wins->GetActiveScope();
		mgr.FocusHandle(this);
	}
}


/*Surface* TopGubo::GetSurface() {
	return this;
}*/

void TopGubo::RunInMachine() {
	Run();
}

int TopGubo::Run() {
	// Bring to front and enter shared main loop
	FocusEvent();
	Surface::EventLoop();
	return 0;
}

void TopGubo::UpdateFromTransform3D() {
	TODO
	#if 0
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
	#endif
}

void TopGubo::OpenMain() {
	CreateGeom3DComponent();
	FocusEvent();
}

END_UPP_NAMESPACE
