#ifndef _Sound_SoundThread_h_
#define _Sound_SoundThread_h_

NAMESPACE_UPP

class SoundDaemon;
struct SoundDiscussionManager;
struct SoundPhrase;
struct SoundMessage;
struct SoundDiscussion;

struct SoundThreadBase : Pte<SoundThreadBase> {
	SoundDaemon& owner;
	Sound snd;
	bool running = false, stopped = true;
	double meter_duration = 0.1;
	double time_duration = 0;
	double silence_duration = 0;
	bool is_recording = false;
	StreamParameters param;
	Ptr<SoundDiscussionManager> mgr;
	Ptr<SoundDiscussion> discussion;
	Ptr<SoundMessage> msg;
	Ptr<SoundPhrase> phrase;
	
	// User params
	double silence_treshold = 0.01;
	double silence_timelimit = 1.0;
	
	Event<void*> WhenFinished;
	Event<String> WhenError;
	
	typedef SoundThreadBase CLASSNAME;
	SoundThreadBase(SoundDaemon& owner);
	virtual ~SoundThreadBase();
	bool IsRunning() const;
	bool IsOpen() const;
	void OnFinish(void*);
	const Sound& GetSound() const {return snd;}
	void SetDevice(SoundDevice dev, int channels);
	void Start();
	void Stop();
	void SetNotRunning();
	void Abort();
	void Wait();
	void Attach(SoundDiscussionManager& m);
	void CheckEnd(StreamCallbackArgs& args);
	void Detach();
	virtual SampleFormat GetSampleFormat() const = 0;
	virtual void ClearData() = 0;
	virtual void* GetDataPtr() = 0;
	virtual double GetVolume() const = 0;
	virtual double GetPeakValue() const = 0;
	virtual void RecordCallback(StreamCallbackArgs& args) = 0;
};

template <class Sample>
struct SoundThread : SoundThreadBase {
	using Clip = SoundClip<Sample>;
	Clip current;
	Clip meter;
	int meter_index = 0;
	
	SoundThread(SoundDaemon& owner) : SoundThreadBase(owner) {}
	SampleFormat GetSampleFormat() const override {return ::UPP::template GetSampleFormat<Sample>();}
	void ClearData() override {current.Clear(); meter.Clear();}
	void* GetDataPtr() override {return &current;}
	double GetVolume() const override;
	double GetPeakValue() const override;
	void RecordCallback(StreamCallbackArgs& args) override;
	
	Event<Clip> WhenClipBegin, WhenClipEnd;
};

END_UPP_NAMESPACE

#endif
