#include "GuboLib.h"


NAMESPACE_UPP



AtomVirtualGui3D::AtomVirtualGui3D() {
	
}

AtomVirtualGui3D::~AtomVirtualGui3D() {
	Destroy();
}

bool AtomVirtualGui3D::Create(const Rect& rect, const char *title) {
	Engine& mach = Upp::Serial::GetActiveMachine();
	wins = mach.Get<Gu::SurfaceSystem>();
	if (!wins)
		return false;
	
	mgr = wins->GetActiveScope();
	if (!mgr)
		return false;
	
	Size mgr_rect = rect.GetSize();
	mgr->SetFrameBox(mgr_rect);
	
	return true;
}

void AtomVirtualGui3D::Destroy() {
	mgr.Clear();
	wins.Clear();
}

dword AtomVirtualGui3D::GetOptions() {
	return 0;
}

Size AtomVirtualGui3D::GetSize() {
	ASSERT(mgr);
	if (mgr)
		return mgr->GetSize();
	return Size(0,0);
}

void AtomVirtualGui3D::Quit() {
	
}


END_UPP_NAMESPACE
