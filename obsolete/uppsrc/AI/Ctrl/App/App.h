#ifndef _AI_Ctrl_App_App_h_
#define _AI_Ctrl_App_App_h_

#include <AI/Core/Core.h>
#include <CtrlLib/CtrlLib.h>
#ifdef flagAUDIO
	#include <SoundCtrl/SoundCtrl.h>
#endif
#include <MetaCtrl/MetaCtrl.h>
#include <FormEditor/FormEditor.h>

#ifndef flagGUI
#error GUI flag is required
#endif

#include <AI/Ctrl/Agent/Agent.h>
#include <AI/Ctrl/Imaging/Imaging.h>

#define LAYOUTFILE <AI/Ctrl/App/App.lay>
#include <CtrlCore/lay.h>


#include "TaskCtrl.h"
#include "OmniCtrl.h"
#include "Playground.h"
#include "ProjectWizard.h"
#include "Notepad.h"

#endif
