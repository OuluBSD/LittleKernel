#include "CtrlLib.h"


NAMESPACE_UPP


TabMgrCtrl::TabMgrCtrl() {
	ParentCtrl::Add(tabs.SizePos());
	
}
	
void TabMgrCtrl::Updated() {
	for (Ctrl& c : ctrls)
		c.Updated();
}


END_UPP_NAMESPACE
