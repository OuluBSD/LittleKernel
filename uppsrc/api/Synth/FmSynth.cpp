#include "Synth.h"

#if 1
NAMESPACE_UPP

struct SynFmSynth::NativeInstrument {
    SoftSynth::FmSynth instrument;
    int sample_rate;
};





bool SynFmSynth::Instrument_Create(NativeInstrument*& dev) {
	dev = new NativeInstrument;
	return true;
}

void SynFmSynth::Instrument_Destroy(NativeInstrument*& dev) {
	delete dev;
}

void SynFmSynth::Instrument_Visit(NativeInstrument& dev, AtomBase&, Visitor& vis) {
	
}

bool SynFmSynth::Instrument_Initialize(NativeInstrument& dev, AtomBase& a, const WorldState& ws) {
	dev.sample_rate = ws.GetInt(".samplerate", 1024);
	
	#if 1
	String filepath = RealizeShareFile(ws.GetString(".filepath"));
	if (!FileExists(filepath)) {
		LOG("SynFmSynth::Instrument_Initialize: file doesn't exist: " << filepath);
		return false;
	}
	String content = LoadFile(filepath);
	if (content.IsEmpty()) {
		LOG("SynFmSynth::Instrument_Initialize: got empty file: " << filepath);
		return false;
	}
	
	const char* data = content.Begin();
	int len = content.GetCount();
	
	SoftSynth::FmSynth::PresetMetadata metadata;
	SoftSynth::Status s = dev.instrument.LoadPreset(metadata, data, len);
	if (s != SoftSynth::STATUS_OK) {
		LOG("SynFmSynth::Instrument_Initialize: FmSynth failed with code " << IntStr((int)s));
		return false;
	}
	
	#elif 0
	dev.instrument.LoadTest();
	#else
	dev.instrument.Reset();
	#endif
	
	ISourcePtr src = a.GetSource();
	int c = src->GetSourceCount();
	ValueBase& v = src->GetSourceValue(c-1);
	ValueFormat fmt = v.GetFormat();
	if (!fmt.IsAudio())
		return false;
	
	AudioFormat& afmt = fmt;
	afmt.SetType(BinarySample::FLT_LE);
	afmt.SetSampleRate(dev.sample_rate);
	dev.instrument.Init((float)afmt.freq);
	
	v.SetFormat(fmt);
	
	return true;
}

bool SynFmSynth::Instrument_PostInitialize(NativeInstrument& dev, AtomBase& a) {
	return true;
}

bool SynFmSynth::Instrument_Start(NativeInstrument& dev, AtomBase& a) {
	return true;
}

void SynFmSynth::Instrument_Stop(NativeInstrument& dev, AtomBase& a) {
	
}

void SynFmSynth::Instrument_Uninitialize(NativeInstrument& dev, AtomBase& a) {
	
}

bool SynFmSynth::Instrument_Send(NativeInstrument& dev, AtomBase& a, RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) {
	ValueFormat fmt = out.GetFormat();
	if (fmt.IsAudio()) {
		AudioFormat& afmt = fmt;
		int sr = afmt.GetSampleRate();
		int ch = afmt.GetSize();
		ASSERT(ch == 2);
		ASSERT(afmt.IsSampleFloat());
		
		Vector<byte>& d = out.Data();
		d.SetCount(afmt.GetFrameSize(), 0);
		dev.instrument.RenderInterleaved((float*)(byte*)d.Begin(), sr);
		
		#if HAVE_PACKETTIMING
		out.SetBeginTime();
		#endif
		
	}
	return true;
}

bool SynFmSynth::Instrument_Recv(NativeInstrument& dev, AtomBase& a, int sink_ch, const Packet& in) {
	ValueFormat fmt = in->GetFormat();
	if (fmt.IsMidi()) {
		const Vector<byte>& data = in->Data();
		int count = data.GetCount() / sizeof(MidiIO::Event);
		
		const MidiIO::Event* ev  = (const MidiIO::Event*)(const byte*)data.Begin();
		const MidiIO::Event* end = ev + count;
		
		while (ev != end) {
			dev.instrument.HandleEvent(*ev);
			ev++;
		}
	}
	else if (fmt.IsOrder()) {
		// pass
	}
	else return false;
	
	return true;
}

void SynFmSynth::Instrument_Finalize(NativeInstrument& dev, AtomBase& a, RealtimeSourceConfig& cfg) {
	
}

bool SynFmSynth::Instrument_IsReady(NativeInstrument& dev, AtomBase& a, PacketIO& io) {
	// Primary sink is required always (continuous audio) so ignore midi input, which is mixed
	// to primary occasionally.
	return (io.active_sink_mask & 0x1) && io.full_src_mask == 0;
}





END_UPP_NAMESPACE
#endif

