#ifndef _CtrlLib_CtrlLib_h_
#define _CtrlLib_CtrlLib_h_

#include "Defs.h"

#ifndef flagGUI
	#error "LocalCtrl library requires GUI flag to be set"
#endif

NAMESPACE_UPP

#include "Compat.h"
#include "Util.h"
#include "Image.h"
#include "BlueBar.h"
#include "GrayBar.h"
#include "ToolMenu.h"
#include "PathCtrl.h"
#include "Container.h"
#include "TabMgrCtrl.h"

void SetFileDialogDirectory(String path);

END_UPP_NAMESPACE


#endif
