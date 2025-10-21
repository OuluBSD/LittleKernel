#ifndef _AI_Ctrl_Base_Base_h_
#define _AI_Ctrl_Base_Base_h_

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

#define IMAGECLASS AIImgs
#define IMAGEFILE <AI/Ctrl/Base/AIImgs.iml>
#include <Draw/iml_header.h>

#include "Fn.h"
#include "VfsProgram.h"

#endif

