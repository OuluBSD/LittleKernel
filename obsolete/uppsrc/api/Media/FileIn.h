#ifndef _IMedia_FileIn_h_
#define _IMedia_FileIn_h_

NAMESPACE_UPP



template <class Backend> struct FileInputT;



template <class Backend>
struct AudioFrameQueueT :
	AudioInputFrameT<Backend>
{
	off32_gen	gen;
	
protected:
	using FileInput = FileInputT<Backend>;
	friend struct FileInputT<Backend>;
	FileInput* owner = 0;
	
public:
	using Base = AudioInputFrameT<Backend>;
	using AVFrame = typename Backend::AVFrame;
	using AVSampleFormat = typename Backend::AVSampleFormat;
	//RTTI_DECL1(AudioFrameQueueT, Base)
	
	AudioFrameQueueT() {}
	
	void				FillAudioBuffer(double time_pos, AVFrame* frame);
	void				Visit(Vis& v) {}
	
	void				Close() override;
	void				FillBuffer() override;
	
	int64				GetCurrentOffset() const {return gen.GetCurrent().value;}
	
};




template <class Backend>
struct VideoFrameQueueT :
	VideoInputFrameT<Backend>
{
	using AVFrame = typename Backend::AVFrame;
	using AVCodecContextPtr = typename Backend::AVCodecContextPtr;
	using ImgConvContextPtr = typename Backend::ImgConvContextPtr;
	using Frame = typename Backend::Frame;
	using Recycler = Upp::Recycler<Frame,true>;
	using Pool = RecyclerPool<Frame,true>;
	
	struct SwsContext*		img_convert_ctx = 0;
	LinkedList<Recycler>	frames;
	Pool					pool;
	int						min_buf_samples = MIN_AUDIO_BUFFER_SAMPLES;
	off32_gen				gen;
	
protected:
	using FileInput = FileInputT<Backend>;
	friend struct FileInputT<Backend>;
	FileInput* owner = 0;
	
public:
	using Base = VideoInputFrameT<Backend>;
	//RTTI_DECL1(VideoFrameQueueT, Base)
	~VideoFrameQueueT() {Clear();}
	
	void				Visit(Vis& v) {}
	void				Init(AVCodecContextPtr& ctx);
	
	void				Close() override;
	void				Clear() override;
	void				FillBuffer() override;
	
	void				Process(double time_pos, AVFrame* frame, bool vflip=true);
	void				DropFrames(int i);
	
};



template <class Backend>
struct FileChannelT
{
	
protected:
	#ifdef flagMSC
	using FileInput = typename FileInputT<Backend>;
	#else
	using FileInput = struct FileInputT<Backend>;
	#endif
	using AVFrame = typename Backend::AVFrame;
	using AVCodecContextPtr = typename Backend::AVCodecContextPtr;
	using AVFormatContext = typename Backend::AVFormatContext;
	using AVCodecParserContextPtr = typename Backend::AVCodecParserContextPtr;
	using AVCodec = typename Backend::AVCodec;
	using AVPacket = typename Backend::AVPacket;
	using AVStream = typename Backend::AVStream;
	using AVCodecParameters = typename Backend::AVCodecParameters;
	using AVDictionary = typename Backend::AVDictionary;
	friend FileInput;
	
	AVFormatContext* file_fmt_ctx = NULL;
	AVCodecContextPtr codec_ctx = NULL;
	AVCodecParserContextPtr parser = NULL;
	AVFrame* frame = NULL;
	AVCodec codec = NULL;
	double frame_pos_time = 0;
	int stream_i = -1;
	bool is_open = false;
	String errstr;
	
	
	int DecodePacket(AVPacket& pkt, int *got_frame);
	
public:
	~FileChannelT() {Clear();}
	bool IsOpen() const {return is_open;}
	
	void Clear();
	void ClearDevice();
	
	bool OpenVideo(AVFormatContext* file_fmt_ctx, ValueFormat& fmt);
	bool OpenAudio(AVFormatContext* file_fmt_ctx, ValueFormat& fmt);
	bool OpenDevice();
	
	bool ReadFrame(AVPacket& pkt);
	double GetSeconds() const {return frame_pos_time;}
	
};



template <class Backend>
struct FileInputT :
	PacketBufferParent
{
private:
	using AudioFrameQueue = typename Backend::AudioFrameQueue;
	using VideoFrameQueue = typename Backend::VideoFrameQueue;
	using FileChannel = typename Backend::FileChannel;
	using AVFormatContext = typename Backend::AVFormatContext;
	using AVPacket = typename Backend::AVPacket;
	using AVCodecParameters = typename Backend::AVCodecParameters;
	
	AudioFrameQueue aframe;
	VideoFrameQueue vframe;
	
	bool has_audio;
	bool has_video;
	bool is_dev_open;
	String path;
	String errstr;
	FileChannel v;
	FileChannel a;
	AVFormatContext* file_fmt_ctx = NULL;
	AVPacket* pkt = 0;
	bool is_eof = false;
	bool pkt_ref = false;
	
	bool HasMediaOpen() const {return has_video || has_audio;}
	void ClearDevice();
	void ClearPacketData();
	void ClearPacket();
	void InitPacket();
	bool IsFrameLoaded() const {return pkt_ref;}
	bool ReadFrame();
	bool ProcessVideoFrame();
	bool ProcessAudioFrame();
	
	
public:
	//RTTI_DECL_R0(FileInputT)
	FileInputT();
	~FileInputT() {Clear();}
	
	bool						IsEof() const;
	void						Visit(Vis& v) {v VISN(aframe) VISN(vframe);}
	void						Clear();
	double						GetSeconds() const;
	bool						IsAudioOpen() const;
	
	void						FillVideoBuffer();
	void						FillAudioBuffer();
	AudioFrameQueue&			GetAudio() {return aframe;}
	VideoFrameQueue&			GetVideo() {return vframe;}
	
	bool						IsOpenAudio() const {return has_audio;}
	bool						IsOpenVideo() const {return has_video;}
	bool						IsOpen() const;
	bool						Open();
	void						Close();
	bool						OpenFile(String path);
	
	String	GetLastError() const;
	
	void	DropVideoFrames(int frames);
	String						GetPath() const;
	
	
	Callback WhenStopped;
	
};



END_UPP_NAMESPACE

#endif
