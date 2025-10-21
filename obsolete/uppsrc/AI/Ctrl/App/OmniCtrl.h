#ifndef _AICtrl_OmniCtrl_h_
#define _AICtrl_OmniCtrl_h_

NAMESPACE_UPP

class OmniCtrl;

#ifdef flagAUDIO

struct OmniDetailedCtrl : Ctrl {
	Splitter hsplit;
	ArrayCtrl discussions, messages, phrases;
	Splitter split[3];

	WithSoundDaemonClip<Ctrl> waveform;
	WaveformCtrl wavectrl;
	Ptr<SoundThreadBase> thrd;

	typedef OmniDetailedCtrl CLASSNAME;
	OmniDetailedCtrl();
	void SetSoundThread(Ptr<SoundThreadBase> thrd);
	void Data();
	void DataManager();
	void DataDiscussion();
	void DataMessage();
	void DataPhrase();
	void ClearWaveform();
};


struct OmniDeviceIO : PageCtrl {
	struct VolumeMeterCtrl : Ctrl {
		OmniDeviceIO* c = 0;
		VolumeMeterCtrl();
		void Paint(Draw& d) override;
	};
	
	struct {
		WithSoundDaemon<Ctrl> form;
		VolumeMeterCtrl meter;
		Ptr<SoundThreadBase> thrd;
		TimeCallback tc;
	} snd;
	
	OmniCtrl* owner = 0;
	
	typedef OmniDeviceIO CLASSNAME;
	OmniDeviceIO();
	void InitSound();
	void Data();
	void SetSoundThread(Ptr<SoundThreadBase> thrd);
	void PopulateSrc();
	void OnFinish(void*);
	void EnableMeter();
	void DisableMeter();
	void OnStart();
	void Stop();
	void OnRecord();
	void OnError(String s);
};

#endif

class OmniCtrl : public TabCtrl {
#ifdef flagAUDIO
	OmniDetailedCtrl detailed;
	OmniDeviceIO dev;
#endif
	
public:
	typedef OmniCtrl CLASSNAME;
	OmniCtrl();
	
	void Data();
#ifdef flagAUDIO
	void SetSoundThread(Ptr<SoundThreadBase> thrd);
#endif
};


END_UPP_NAMESPACE

#endif
 
