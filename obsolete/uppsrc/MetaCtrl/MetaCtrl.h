#ifndef _MetaCtrl_MetaCtrl_h_
#define _MetaCtrl_MetaCtrl_h_

#include <Vfs/Ecs/Ecs.h>
#include <Vfs/Analysis/Analysis.h>
#include <Vfs/Solver/Solver.h>
#include <CtrlLib/CtrlLib.h>
#ifdef flagAUDIO
	#include <Sound/Sound.h>
	#include <SoundCtrl/SoundCtrl.h>
#endif
#include <CodeEditor/CodeEditor.h>

#ifndef flagGUI
#error GUI flag is required
#endif

#define IMAGECLASS MetaImgs
#define IMAGEFILE <MetaCtrl/Images.iml>
#include <Draw/iml_header.h>

NAMESPACE_UPP

#define LAYOUTFILE <MetaCtrl/MetaCtrl.lay>
#include <CtrlCore/lay.h>

#include "Common.h"
#include "Node.h"
#include "ProcessCtrl.h"
#include "TextDesigner.h"
#include "Entity.h"

END_UPP_NAMESPACE

#endif
