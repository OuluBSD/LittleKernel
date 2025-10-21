#ifndef _AudioCore_Fluidsynth_h_
#define _AudioCore_Fluidsynth_h_

#if HAVE_FLUIDSYNTH

#include <fluidsynth.h>


NAMESPACE_UPP


class Fluidsynth {
	fluid_settings_t* settings = 0;
	fluid_synth_t* synth = 0;
	fluid_audio_driver_t* adriver = 0;
	int sfont_id = -1;
	bool sf_loaded = false;
	
public:
	typedef Fluidsynth CLASSNAME;
	Fluidsynth();
	~Fluidsynth();
	
	void Init();
	void Clear();
	
	bool LoadSoundfontFile(String path);
	
	void ConfigureTrack(const MidiIO::File& file, int track_i);
	
};


END_UPP_NAMESPACE

#endif
#endif
