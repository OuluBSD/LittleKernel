#ifndef _SoftSynth_SoftSynth_h_
#define _SoftSynth_SoftSynth_h_

#include <Core/Core.h>
#include <MidiFile/MidiFile.h>
#undef Status

#define NAMESPACE_SOFTSYNTH_BEGIN NAMESPACE_UPP namespace SoftSynth {
#define NAMESPACE_SOFTSYNTH_END   END_UPP_NAMESPACE }


#if CPU_ARM
	#define SYNTH_NEON_ASM 1
#endif


#include "Common.h"
#include "Fm.h"


#endif
