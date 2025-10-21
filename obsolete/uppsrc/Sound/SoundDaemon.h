#ifndef _Sound_SoundDaemon_h_
#define _Sound_SoundDaemon_h_

NAMESPACE_UPP

class SoundDaemon {
public:
	
private:
	ArrayMap<hash_t, SoundThreadBase> thrds;
	
public:
	typedef SoundDaemon CLASSNAME;
	SoundDaemon();
	~SoundDaemon();
	
	static SoundDaemon& Static();
	
	SoundThreadBase& GetAddThread(SoundDevice dev, SampleFormat sample, int channels);
	
	template <class T>
	SoundThread<T>& GetAddThread(SoundDevice dev, int channels) {
		SampleFormat sample = GetSampleFormat<T>();
		CombineHash ch;
		ch.Do(dev);
		ch.Do((int)sample);
		ch.Do(channels);
		hash_t h = ch;
		int i = thrds.Find(h);
		if (i >= 0)
			return dynamic_cast<SoundThread<T>&>(thrds[i]);
		auto* p = new SoundThread<T>(*this);
		p->SetDevice(dev, channels);
		thrds.Add(h, p);
		return *p;
	}

};

END_UPP_NAMESPACE

#endif
