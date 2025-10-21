#include "Sound.h"

NAMESPACE_UPP

SoundThreadBase::SoundThreadBase(SoundDaemon& owner) : owner(owner) {
	snd.WhenFinished = THISBACK(OnFinish);
	
}

SoundThreadBase::~SoundThreadBase() {
	if (snd.IsOpen())
		snd.Abort();
}

void SoundThreadBase::Start() {
	ASSERT(param.device >= 0);
	if (snd.IsOpen()) {
		snd.Abort();
		running = false;
		stopped = true;
	}
	time_duration = 0;
	silence_duration = 0;
	is_recording = false;
	ClearData();
	
	snd <<= THISBACK(RecordCallback);
	
	void* userdata = GetDataPtr();
	snd.Open(&userdata, param, Null);
	if(snd.IsError()){
		LOG(snd.GetError());
		WhenError(snd.GetError());
		return;
	}
	snd.Start();
	if(snd.IsError()){
		LOG(snd.GetError());
		WhenError(snd.GetError());
		return;
	}
	running = true;
	stopped = false;
}

void SoundThreadBase::SetDevice(SoundDevice dev, int channels) {
	SampleFormat fmt = GetSampleFormat();
	this->param = StreamParameters(dev,channels,fmt,dev.LowInputLatency);
}

bool SoundThreadBase::IsRunning() const {
	return running;
}

bool SoundThreadBase::IsOpen() const {
	return snd.IsOpen();
}

void SoundThreadBase::OnFinish(void* p) {
	running = false;
	stopped = true;
	WhenFinished(p);
}

void SoundThreadBase::Stop() {
	running = false;
	while (!stopped)
		Sleep(10);
}

void SoundThreadBase::SetNotRunning() {
	running = false;
}

void SoundThreadBase::Abort() {
	if (snd.IsOpen())
		snd.Abort();
}

void SoundThreadBase::Wait() {
	while (!stopped)
		Sleep(10);
}

void SoundThreadBase::Attach(SoundDiscussionManager& m) {
	m.thrd = this;
	mgr = &m;
	discussion = &mgr->Add();
	msg = &discussion->Add();
	phrase = 0;
}

void SoundThreadBase::Detach() {
	if (mgr && mgr->thrd == this)
		mgr->thrd = 0;
	mgr = 0;
	discussion = 0;
	msg = 0;
	phrase = 0;
}


END_UPP_NAMESPACE
