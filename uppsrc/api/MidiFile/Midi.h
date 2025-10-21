#ifndef _AtomLocal_Midi_h_
#define _AtomLocal_Midi_h_


NAMESPACE_UPP


class MidiFileReaderAtom : public Atom
{
	bool close_machine = false;
	
	String last_error;
	Vector<int> track_i;
	double song_dt = 0;
	bool split_channels = false;
	int drum_side_ch = -1;
	bool use_global_time = false;
	
	MidiIO::File file;
	MidiIO::MidiFrame tmp;
	
public:
	using Atom::Atom;
	COPY_PANIC(MidiFileReaderAtom);
	CLASSTYPE(MidiFileReaderAtom);
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	MidiFileReaderAtom(VfsValue& n);
	
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	void Update(double dt) override;
	bool IsReady(PacketIO& io) override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	
	bool OpenFilePath(String path);
	void Clear();
	void CollectTrackEvents(int i);
	void DumpMidiFile();
	bool IsEnd() const;
	
	String GetLastError() const {return last_error;}
	
	
	Callback WhenError;
	
};

class MidiNullAtom : public Atom
{
	bool verbose = false;
	
public:
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	CLASSTYPE(MidiNullAtom);
	
	MidiNullAtom(VfsValue& n);
	
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	bool IsReady(PacketIO& io) override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	
};


END_UPP_NAMESPACE


#endif
