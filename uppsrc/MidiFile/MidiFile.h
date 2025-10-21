#ifndef _MidiFile_MidiFile_h_
#define _MidiFile_MidiFile_h_

#include <Core/Core.h>
#include <Core/ProcessUtil/ProcessUtil.h>

#define NAMESPACE_MIDI_NAME MidiIO
#define NAMESPACE_MIDI_BEGIN namespace  Upp { namespace  NAMESPACE_MIDI_NAME {
#define NAMESPACE_MIDI_END }}

NAMESPACE_MIDI_BEGIN

#include "Message.h"
#include "Event.h"
#include "EventList.h"
#include "File.h"
#include "Binasc.h"
#include "Utils.h"

NAMESPACE_MIDI_END

#endif
