#include "Runtime.h"

NAMESPACE_UPP

AgentTaskExt::AgentTaskExt(VfsValue& n) : VfsValueExt(n) {
	
}

void AgentTaskExt::Visit(Vis& v) {
	v
		("status", (int&)status)
		VIS_(prompt)
		;
}


END_UPP_NAMESPACE
