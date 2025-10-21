#ifndef _AudioCore_Midi_h_
#define _AudioCore_Midi_h_

NAMESPACE_UPP


class MidiFrame {
	
public:
	Vector<const MidiIO::Event*> midi;
	
	void Reset() {midi.SetCount(0);}
};


END_UPP_NAMESPACE

#endif
