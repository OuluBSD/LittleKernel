#ifndef _Sound_SoundClip_h_
#define _Sound_SoundClip_h_

NAMESPACE_UPP

struct SoundClipBase {
	virtual ~SoundClipBase() {}
	
	virtual bool IsUpdating() const = 0;
	virtual bool Set(const SoundClipBase& b) = 0;
	virtual SoundClipBase* Clone() const = 0;
	virtual int GetSampleRate() const = 0;
	virtual int GetChannels() const = 0;
	virtual SampleFormat GetFormat() const = 0;
	virtual int GetCount() const = 0;
	virtual void GetValues(int begin, int end, Vector<double>& values) = 0;
	
	double GetDuration() const;
};

template <class Sample>
struct SoundClip : Moveable<SoundClip<Sample>>, SoundClipBase {
	struct Data {
		Vector<Sample> data;
		double samplerate = 0;
		int channels = 0;
		bool updating = false;
		Atomic refs;
		Data() {refs = 0;}
		void SetFormat(int samplerate, int channels) {this->channels = channels; this->samplerate = samplerate;}
	};
	Data* data = 0;
	
	SoundClip() {}
	SoundClip(const SoundClip& c) {*this = c;}
	SoundClip(SoundClip&& c) {data = c.data; c.data = 0;}
	void Clear() {if (data) {data->refs-=1; if (data->refs <= 0) delete data; data = 0;}}
	void Create() {Clear(); data = new Data(); data->refs++;}
	void operator=(const SoundClip& c) {Clear(); data = c.data; if (data) data->refs++;}
	bool IsEmpty() const {return !data || data->data.IsEmpty();}
	bool IsNull() const {return !data;}
	Sample* Begin() {return data ? data->data.Begin() : 0;}
	Sample* End() {return data ? data->data.End() : 0;}
	const Sample* Begin() const {return data ? data->data.Begin() : 0;}
	const Sample* End() const {return data ? data->data.End() : 0;}
	bool Read(Vector<float>& out);
	bool Write(Vector<float>& out);
	static SampleFormat GetSampleFormat() {return ::UPP::GetSampleFormat<Sample>();}
	
	bool Set(const SoundClipBase& b) override {
		const SoundClip<Sample>* c = dynamic_cast<const SoundClip<Sample>*>(&b);
		if (c)
			*this = *c;
		return c != 0;
	}
	bool IsUpdating() const override {return data ? data->updating : false;}
	SoundClipBase* Clone() const override {return new SoundClip<Sample>(*this);}
	int GetSampleRate() const override {return data ? data->samplerate : 0;}
	int GetChannels() const override {return data ? data->channels : 0;}
	SampleFormat GetFormat() const override {return GetSampleFormat();}
	int GetCount() const override {return data ? data->data.GetCount() : 0;}
	void GetValues(int begin, int end, Vector<double>& values) override {
		if (!data) {
			values.SetCount(0);
			return;
		}
		begin = max(0, begin);
		end = min(end, data->data.GetCount());
		int count = end - begin;
		values.SetCount(count);
		double* out = values.Begin();
		double* out_end = values.End();
		const Sample* in = data->data.Begin();
		while (out != out_end)
			*out++ = SampleToDouble<Sample>(*in++);
	}
};

END_UPP_NAMESPACE

#endif
