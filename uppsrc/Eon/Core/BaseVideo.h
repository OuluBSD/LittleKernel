#ifndef _Eon_Core_BaseVideo_h_
#define _Eon_Core_BaseVideo_h_

#if defined flagSCREEN




struct BufferWriterBase {
	
};


class DebugVideoGenerator {
	using T = byte;
	
	Vector<T> frame;
	int frame_part_size = 0;
	
	
public:
	typedef DebugVideoGenerator CLASSNAME;
	DebugVideoGenerator();
	
	
	void Play(int frame_offset, PacketValue& p);
	void Play(int frame_offset, BufferWriterBase& buf);
	void GenerateNoise(const VideoFormat& fmt);
	void GenerateSine(const VideoFormat& fmt);
	uint64 GetMaxOffset() const {return frame.GetCount();}
	
	
};

class VideoGenBase :
	public Atom
{
	DebugVideoGenerator		gen;
	ValueFormat				fmt;
	String					last_error;
	int						mode = 0;
	int						preset_i = -1;
	
	
	enum {
		MODE_NONE,
		MODE_TRACK_NUM,
	};
	
	void GenerateStereoSine(const VideoFormat& fmt);
	
public:
	VideoGenBase(VfsValue& n);
	
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this);}
	
	void SetPreset(int i) {preset_i = i;}
	String GetLastError() const {return last_error;}
	
	
};




#endif
#endif
