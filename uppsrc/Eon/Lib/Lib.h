#ifndef _EonLib_EonLib_h_
#define _EonLib_EonLib_h_

#include <Eon/Draw/Draw.h>

#include <api/MidiFile/MidiFile.h>
#include <api/MidiHw/MidiHw.h>
#include <api/Synth/Synth.h>
#include <api/Effect/Effect.h>
#include <api/Audio/Audio.h>
#include <api/Hal/Hal.h>
#include <api/Physics/Physics.h>

#ifdef flagSCREEN
	#include <api/Graphics/Graphics.h>
#endif

#ifdef flagVR
	#include <api/Holograph/Holograph.h>
#endif

#include <api/Screen/Screen.h>
#include <api/Media/Media.h>
#include <api/Volumetric/Volumetric.h>
#include <api/Camera/Camera.h>
#include <api/AudioFileOut/AudioFileOut.h>
#include <api/Holograph/Holograph.h>

#include <Eon/Draw/Draw.h>


#define BIG_NUMBER 100000000

NAMESPACE_UPP

#include "Handle.h"
#include "GeneratedAudio.h"
#include "GeneratedHandle.h"
#include "GeneratedMinimal.h"
#include "GeneratedVR.h"
#include "RenderingSystem.h"
#include "Absolute2D.h"
#include "Geom2DComponent.h"
#include "Gui.h"
#include "EcsPhysics.h"
#include "OdePrefab.h"
#include "TPrefab.h"

END_UPP_NAMESPACE

#endif

