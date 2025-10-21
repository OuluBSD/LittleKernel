#ifndef _Eon_Draw_EonDraw_h_
#define _Eon_Draw_EonDraw_h_

#include <Draw/Draw.h>
#include <Draw/Extensions/Extensions.h>
#include <Eon/Ecs/Ecs.h>
//#include <api/Graphics/Graphics.h>
#include <api/Hal/Hal.h>

#ifdef flagMSC
	#pragma warning( disable : 4250 ) // C4250: useless Inheritance via dominance warning
#endif

NAMESPACE_UPP

#include "GEnums.h"
#include "ProgPainter.h"
#include "ProgDraw.h"
#include "Rendering.h"
#include "Camera.h"
#include "Model.h"
#include "ModelCache.h"
#include "RenderingSystem.h"
#include "EventSystem.h"
#include "Prefab.h"
#include "PaintStrokeSystem.h"
#include "ToolSystem.h"
#include "ToolboxSystem.h"
#include "PaintingSystem.h"
#include "ShootingSystem.h"
#include "ThrowingSystem.h"
#include "Prefabs.h"

END_UPP_NAMESPACE

#endif
