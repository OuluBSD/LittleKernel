#ifndef _IMedia_MediaAtomBase_h_
#define _IMedia_MediaAtomBase_h_


NAMESPACE_UPP


template <class Backend>
struct MediaAtomBaseT :
	Atom
{
private:
	using FileInput = FileInputT<Backend>;
	using MediaStreamThread = typename Backend::MediaStreamThread;
	using VideoInputFramePtr = typename Backend::VideoInputFramePtr;
	using AudioInputFramePtr = typename Backend::AudioInputFramePtr;
	
	typedef enum {
		INVALID_MODE,
		AUDIO_ONLY,
		VIDEO_ONLY,
		AUDIOVIDEO,
	} Mode;
	
	FileInput			file_in;
	Mode				mode = INVALID_MODE;
	
	String last_error;
	MediaStreamThread vi;
	Size def_cap_sz;
	double time = 0;
	int audio_ch = -1, video_ch = -1;
	int def_cap_fps = 25;
	bool stops_machine = false;
	bool vflip = false;
	bool video_packet_ready = false, audio_packet_ready = false;
	String filepath;
	
	
	bool RealizeAudioFormat();
	bool RealizeVideoFormat();
	
public:
	typedef MediaAtomBaseT CLASSNAME;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	MediaAtomBaseT(VfsValue& n);
	
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	//void Forward(FwdScope& fwd) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	bool IsReady(PacketIO& io) override;
	bool PostInitialize() override;
	void Update(double dt) override;
	
	void Visit(Vis& v) override {v VISN(file_in); VIS_THIS(Atom);}
	bool LoadFileAny(String path);
	void OnError();
	void OnStop();
	
	
	void SetError(String s);
	String GetLastError() const {return last_error;}
	
	MediaStreamThread& GetInput() {return vi;}
	
	
	Callback WhenStopped;
	
	
};



#ifdef flagFFMPEG
using FfmpegSourceDevice = MediaAtomBaseT<FfmpegMedia>;
using FfmpegSinkDevice = MediaAtomBaseT<FfmpegMedia>;
#endif

END_UPP_NAMESPACE

#endif
