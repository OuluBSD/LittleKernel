#ifndef _portaudio_Sound_h_
#define _portaudio_Sound_h_

NAMESPACE_UPP
class ASound {
protected:
	mutable int err;
	PaStream* stream = 0;
	
	double samplerate = 0;
	dword fpb = 0;
	StreamFlags flags;
	
public:
	ASound();
	ASound(const ASound& s):err(s.err),samplerate(s.samplerate),
	                        fpb(s.fpb),flags(s.flags){stream=s.stream;}
	
protected:
	void OpenStream(PaStreamCallback* cb, void* data,
	                const StreamParameters& inparam, const StreamParameters& outparam);
	void OpenDefaultStream(PaStreamCallback* cb, void* data, int inchannels,int outchannels,SampleFormat format);

public:
	ASound& SetSampleRate(double sr)           {samplerate=sr; return *this;}
	ASound& SetFramesPerBuffer(dword frames)   {fpb=frames;    return *this;}
	ASound& SetFlags(StreamFlags f)            {flags=f;       return *this;}
	double GetSampleRate() const;
	dword GetFramesPerBuffer() const      {return fpb;}
	StreamFlags GetFlags() const          {return flags;}

	void Start()               {err = Pa_StartStream(stream); CHECK_ERR;}
	void Stop()                {err = Pa_StopStream(stream); CHECK_ERR;}
	void Abort()               {err = Pa_AbortStream(stream); CHECK_ERR;}
	void Close();

	bool IsStopped() const;
	bool IsActive() const;
	bool IsOpen() const                 {return (stream != NULL);}
	bool IsError() const                {return err!=paNoError;};
	void ClearError() const             {err=paNoError;}
	int  GetErrorCode() const           {return err;}
	String GetError()const              {return Pa_GetErrorText(err);}

	double GetInputLatency() const;
	double GetOutputLatency() const;
	double Time() const                 {return Pa_GetStreamTime(stream);}
	double CpuLoad() const              {return Pa_GetStreamCpuLoad(stream);}
};

class Sound : public ASound {
	StreamCallbackData scallback;
public:
	Callback1<StreamCallbackArgs&> WhenAction;
	Callback1<void*> WhenFinished;

	Sound(){}
	Sound(const Sound &){}

	void Open(void* data,const StreamParameters& inparam,const StreamParameters& outparam);
	void Open(const StreamParameters& inparam,const StreamParameters& outparam);
	void OpenDefault(void* data, int inchannels=0,int outchannels=2,SampleFormat format=SND_FLOAT32);
	void OpenDefault(int inchannels=0, int outchannels=2,SampleFormat format=SND_FLOAT32);

	void operator<<=(Callback1<StreamCallbackArgs&> cb)    {WhenAction = cb;}

private:
	void SetFinishCallback();
};

class SoundStream : public ASound {
public:
	SoundStream(){};
	SoundStream(const SoundStream& s){stream=s.stream;}

	void Open(const StreamParameters& inparam,const StreamParameters& outparam);
	void OpenDefault(int inchannels=0, int outchannels=2, SampleFormat format=SND_FLOAT32);

	//template <class T>
	void Write(const void* data,unsigned int frames){
		err=Pa_WriteStream(stream,data,frames);
		CHECK_ERR;
	}
	//template <class T>
	void Read(void* data,unsigned int frames){
		err=Pa_ReadStream(stream,data,frames);
		CHECK_ERR;
	}

	int ReadAvailable();
	int WriteAvailable();
};

class SoundAPI : public Moveable<SoundAPI> {
public:
	int index, type;
	const char* name;
	int deviceCount;
	PaDeviceIndex defaultInputDevice;
	PaDeviceIndex defaultOutputDevice;
	SoundAPI():index(Null),name("Null"){};
	SoundAPI(const Nuller&):index(Null),name("Null"){}
	SoundAPI(int n);
	bool IsNullInstance()const           {return IsNull(index);}
	String ToString()const;
	operator int()const                  {return index;}
	operator const PaHostApiInfo*()const {return Pa_GetHostApiInfo(index);}
};

class SoundDevice : public Moveable<SoundDevice> {
public:
	int index;
	const char* name;
	int API;
	int InputChannels, OutputChannels;
	double LowInputLatency,LowOutputLatency,HighInputLatency,HighOutputLatency;
	double SampleRate;
	
	SoundDevice():index(Null),name("Null"){};
	SoundDevice(const Nuller&):index(Null),name("Null"){}
	SoundDevice(int n);
	SoundDevice(const SoundDevice& sd) {*this = sd;}
	void operator=(const SoundDevice& sd) {
		index = sd.index;
		name = sd.name;
		API = sd.API;
		InputChannels = sd.InputChannels;
		OutputChannels = sd.OutputChannels;
		LowInputLatency = sd.LowInputLatency;
		LowOutputLatency = sd.LowOutputLatency;
		HighInputLatency = sd.HighInputLatency;
		HighOutputLatency = sd.HighOutputLatency;
		SampleRate = sd.SampleRate;
	}
	hash_t GetHashValue() const {
		CombineHash ch;
		ch	.Do(index).Do(String(name)).Do(API).Do(InputChannels)
			.Do(OutputChannels).Do(LowInputLatency).Do(LowOutputLatency)
			.Do(HighInputLatency).Do(HighOutputLatency).Do(SampleRate);
		return ch;
	}
	bool IsNullInstance()const            {return IsNull(index);}
	String ToString()const;
	operator int()const                   {return index;}
	operator const PaDeviceInfo*()const   {return Pa_GetDeviceInfo(index);}
};

class SoundSystem {
	mutable int err;
	static bool exists;
public:
	SoundSystem();
	~SoundSystem();

	const Vector<SoundDevice> GetDevices()const;
	const Vector<SoundAPI>    GetAPIs()const;

	const SoundDevice GetDefaultInput()const;
	const SoundDevice GetDefaultOutput()const;
	const SoundAPI    GetDefaultAPI()const;
	
	int GetCount()const;
	int GetAPICount()const;
	const SoundDevice Get(int n)const;
	const SoundAPI    GetAPI(int n)const;
	const SoundDevice operator[](int n)const   {return Get(n);}
	const SoundAPI    operator()(int n)const   {return GetAPI(n);}

	bool IsError() const       {return err<paNoError;};
	void ClearError() const    {err=paNoError;}
	int  GetErrorCode() const  {return err;}
	String GetError()const     {return Pa_GetErrorText(err);}

	static bool Exists()       {return exists;}

	String ToString()const;
};

SoundSystem& SoundSys();

END_UPP_NAMESPACE

#endif
