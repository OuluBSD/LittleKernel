#ifndef _Sound_Sound_h_
#define _Sound_Sound_h_

#include <Core/Core.h>

#if !defined flagSYS_PORTAUDIO
	#include <plugin/portaudio/portaudio.h>
#else
	#include <portaudio.h>
#endif


#include "Types.h"
#include "SoundClip.h"
#include "SoundSystem.h"
#include "SoundThread.h"
#include "Discussion.h"
#include "SoundDaemon.h"

#endif
