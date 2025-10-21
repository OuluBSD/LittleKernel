#ifndef _EonLib_GeneratedAudio_h_
#define _EonLib_GeneratedAudio_h_

// This file is generated. Do not modify this file.


class MidiFileReaderPipe : public MidiFileReaderAtom {

public:
	ATOM_CTOR_(MidiFileReaderPipe, MidiFileReaderAtom)
	//ATOMTYPE(MidiFileReaderPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class MidiFileReader : public MidiFileReaderAtom {

public:
	ATOM_CTOR_(MidiFileReader, MidiFileReaderAtom)
	//ATOMTYPE(MidiFileReader)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class MidiFileReader16 : public MidiFileReaderAtom {

public:
	ATOM_CTOR_(MidiFileReader16, MidiFileReaderAtom)
	//ATOMTYPE(MidiFileReader16)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class MidiNullSink : public MidiNullAtom {

public:
	ATOM_CTOR_(MidiNullSink, MidiNullAtom)
	//ATOMTYPE(MidiNullSink)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if (defined flagFLUIDLITE) || (defined flagFLUIDSYNTH)
class FluidsynthPipe : public FluidsynthInstrument {

public:
	ATOM_CTOR_(FluidsynthPipe, FluidsynthInstrument)
	//ATOMTYPE(FluidsynthPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class SoftInstrumentPipe : public SoftInstrument {

public:
	ATOM_CTOR_(SoftInstrumentPipe, SoftInstrument)
	//ATOMTYPE(SoftInstrumentPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class FmSynthPipe : public FmSynthInstrument {

public:
	ATOM_CTOR_(FmSynthPipe, FmSynthInstrument)
	//ATOMTYPE(FmSynthPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if defined flagLV2
class LV2InstrumentPipe : public LV2Instrument {

public:
	ATOM_CTOR_(LV2InstrumentPipe, LV2Instrument)
	//ATOMTYPE(LV2InstrumentPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class CoreSynthPipe : public CoreSynthInstrument {

public:
	ATOM_CTOR_(CoreSynthPipe, CoreSynthInstrument)
	//ATOMTYPE(CoreSynthPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class CoreDrummerPipe : public CoreDrummerInstrument {

public:
	ATOM_CTOR_(CoreDrummerPipe, CoreDrummerInstrument)
	//ATOMTYPE(CoreDrummerPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class CoreEffectPipe : public AudioCoreEffect {

public:
	ATOM_CTOR_(CoreEffectPipe, AudioCoreEffect)
	//ATOMTYPE(CoreEffectPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class CoreEffectAtom : public AudioCoreEffect {

public:
	ATOM_CTOR_(CoreEffectAtom, AudioCoreEffect)
	//ATOMTYPE(CoreEffectAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if defined flagLV2
class LV2EffectPipe : public LV2Effect {

public:
	ATOM_CTOR_(LV2EffectPipe, LV2Effect)
	//ATOMTYPE(LV2EffectPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagBUILTIN_PORTMIDI) || (defined flagPORTMIDI)
class PortmidiPipe : public PortmidiSource {

public:
	ATOM_CTOR_(PortmidiPipe, PortmidiSource)
	//ATOMTYPE(PortmidiPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagBUILTIN_PORTMIDI) || (defined flagPORTMIDI)
class PortmidiSend : public PortmidiSource {

public:
	ATOM_CTOR_(PortmidiSend, PortmidiSource)
	//ATOMTYPE(PortmidiSend)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class CoreAudioFileOut : public CoreAudioSink {

public:
	ATOM_CTOR_(CoreAudioFileOut, CoreAudioSink)
	//ATOMTYPE(CoreAudioFileOut)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};


#endif
