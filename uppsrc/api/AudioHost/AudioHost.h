#ifndef _AudioHost_AudioHost_h_
#define _AudioHost_AudioHost_h_

/*

	This packages is used for loading midi files and making lv2-instrument and effect system
	for playing them with better than regular midi sound quality.
	
	Note: not every midi file is expected to sound good, because of different mixing and
		  different sound-sample characteristics than what composer had.
	
*/

#ifdef flagLV2
#include <plugin/lilv/lilv.h>
#include <plugin/lilv/lilvmm.hpp>
#include <plugin/lilv/lilv_config.h>
#include "lv2/lv2plug.in/ns/ext/event/event.h"
#include "lv2/lv2plug.in/ns/ext/uri-map/uri-map.h"
#include "lv2/lv2plug.in/ns/ext/event/event-helpers.h"
#endif

#include <Core/Core.h>
#include <Eon/Eon.h>
#include <MidiFile/MidiFile.h>

#include "Host.h"
#include "Loader.h"
#include "Util.h"


#endif
