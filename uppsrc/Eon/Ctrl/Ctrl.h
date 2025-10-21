#ifndef _Eon_Ctrl_Ctrl_h_
#define _Eon_Ctrl__Ctrl_h_

#include <Eon/Lib/Lib.h>

#ifndef flagGUI
	#error "EscCtrl library requires GUI flag to be set"
#endif

NAMESPACE_UPP

#include "Parts.h"
#include "ComponentCtrl.h"
#include "EntityCtrl.h"
#include "InterfaceConnectionCtrl.h"
#include "InterfaceCtrl.h"
#include "InterfaceSystemCtrl.h"
#include "DesktopSystem.h"

END_UPP_NAMESPACE

#endif

