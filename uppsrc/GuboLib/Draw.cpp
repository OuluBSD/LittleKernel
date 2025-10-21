#include "GuboLib.h"

NAMESPACE_UPP

void AtomVirtualGui3D::SetTarget(Draw& d) {
	sysdraw.SetTarget(&d);
}

SystemDraw& AtomVirtualGui3D::BeginDraw()
{
	return sysdraw;
}

void AtomVirtualGui3D::CommitDraw()
{
	//gldraw.Finish();
	//SDL_GL_SwapWindow(win);
}

END_UPP_NAMESPACE
