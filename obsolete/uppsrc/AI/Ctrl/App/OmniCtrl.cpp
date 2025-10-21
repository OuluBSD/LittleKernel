#include "App.h"
#include <ide/ide.h>

NAMESPACE_UPP


	
OmniCtrl::OmniCtrl() {
#ifdef flagAUDIO
	dev.owner = this;
	TabCtrl::Add(detailed.SizePos(), "Detailed");
	TabCtrl::Add(dev.SizePos(), "Devices");
#endif
}

void OmniCtrl::Data() {
	int tab = Get();
	int i = 0;
#ifdef flagAUDIO
	if (i++ == tab)	detailed.Data();
	if (i++ == tab)	dev.Data();
#endif
}

#ifdef flagAUDIO
void OmniCtrl::SetSoundThread(Ptr<SoundThreadBase> thrd) {
	dev.SetSoundThread(thrd);
	detailed.SetSoundThread(thrd);
	PostCallback(THISBACK(Data));
}
#endif

#ifdef flagAUDIO
OmniDeviceIO::OmniDeviceIO() {
	auto& form = snd.form;
	auto& thrd = snd.thrd;
	auto& meter = snd.meter;
	
	meter.c = this;
	CtrlLayout(form);
	
	PageCtrl::Add(form.SizePos(), "Sound").Height(320);
	form.volume.Add(meter.SizePos());
	form.autostart.Set(TheIde()->autostart_audio_src);
	
	OnFinish(0);
	form.refresh <<= THISBACK(PopulateSrc);
	form.autostart.WhenAction = [this]{TheIde()->autostart_audio_src = snd.form.autostart.Get();};
	
	Ptr<Ctrl> p = this;
	auto* ide = TheIde();
	
	// Silence detection time limit
	form.time_slide.MinMax(2,500);
	form.time_slide.SetData(ide->audio_timelimit * 100);
	form.time_val.SetData(ide->audio_timelimit);
	form.time_slide.WhenSlideFinish = [this]{
		auto& form = snd.form; auto& thrd = snd.thrd;
		double t = (double)form.time_slide.GetData() * 0.01;
		TheIde()->audio_timelimit = t;
		form.time_val.SetData(t);
		if (thrd) thrd->silence_timelimit = t;
	};
	form.time_val.WhenEnter = [this]{
		auto& form = snd.form; auto& thrd = snd.thrd;
		double t = form.time_val.GetData();
		TheIde()->audio_timelimit = t;
		form.time_slide.SetData(t * 100);
		if (thrd) thrd->silence_timelimit = t;
	};
	
	// Silence detection volume treshold
	form.volume_slide.MinMax(1,100);
	form.volume_slide.SetData(ide->audio_volumetreshold * 100);
	form.volume_val.SetData(ide->audio_volumetreshold);
	form.volume_slide.WhenSlideFinish =[this]{
		auto& form = snd.form; auto& thrd = snd.thrd;
		double t = (double)form.volume_slide.GetData() * 0.01;
		TheIde()->audio_volumetreshold = t;
		form.volume_val.SetData(t);
		if (thrd) thrd->silence_treshold = t;
	};
	form.volume_val.WhenEnter = [this]{
		auto& form = snd.form; auto& thrd = snd.thrd;
		double t = form.volume_val.GetData();
		TheIde()->audio_volumetreshold = t;
		form.volume_slide.SetData(t * 100);
		if (thrd) thrd->silence_treshold = t;
	};
	
	PostCallback(THISBACK(PopulateSrc));
}

void OmniDeviceIO::Data() {
	PopulateSrc();
}

void OmniDeviceIO::SetSoundThread(Ptr<SoundThreadBase> t) {
	snd.thrd = t;
	Data();
}




OmniDetailedCtrl::OmniDetailedCtrl() {
	Add(hsplit.SizePos());
	CtrlLayout(waveform);
	hsplit.Horz();
	for(int i = 0; i < 3; i++)
		hsplit << split[i];
	split[0].Vert() << discussions;
	split[1].Vert() << messages;
	split[2].Vert() << phrases << waveform;
	
	waveform.waveform.Add(wavectrl.SizePos());
	
	discussions.AddColumn("#");
	discussions.AddColumn("Discussion");
	discussions.AddIndex("IDX");
	discussions.WhenAction = THISBACK(DataDiscussion);
	messages.AddColumn("#");
	messages.AddColumn("Message");
	messages.AddIndex("IDX");
	messages.WhenAction = THISBACK(DataMessage);
	phrases.AddColumn("#");
	phrases.AddColumn("Phrase");
	phrases.AddIndex("IDX");
	phrases.WhenAction = THISBACK(DataPhrase);
	
	
}

void OmniDetailedCtrl::SetSoundThread(Ptr<SoundThreadBase> t) {
	if (thrd)
		thrd->Detach();
	thrd = t;
	
	#if 0
	OmniThread& aidm = OmniThread::Single();
	aidm.WhenDiscussionBegin = [this](SoundDiscussion& d) {PostCallback(THISBACK(DataManager));};
	aidm.WhenMessageBegin = [this](SoundMessage& m) {PostCallback(THISBACK(DataDiscussion));};
	aidm.WhenPhraseBegin = [this](SoundPhrase& p) {PostCallback(THISBACK(DataMessage));};
	thrd->Attach(aidm);
	#endif
}

void OmniDetailedCtrl::Data() {
	
	DataManager();
}

void OmniDetailedCtrl::DataManager() {
	#if 0
	OmniThread& dm = OmniThread::Single();
	
	for(int i = 0; i < dm.discussions.GetCount(); i++) {
		const SoundDiscussion& sd = dm.discussions[i];
		discussions.Set(i, 0, i);
		discussions.Set(i, "IDX", i);
	}
	discussions.SetCount(dm.discussions.GetCount());
	if (!discussions.IsCursor() && discussions.GetCount())
		discussions.SetCursor(0);
	#endif
}

void OmniDetailedCtrl::DataDiscussion() {
	#if 0
	OmniThread& dm = OmniThread::Single();
	
	if (!discussions.IsCursor()) {
		messages.Clear();
		phrases.Clear();
		ClearWaveform();
		return;
	}
	int dis_i = discussions.Get("IDX");
	const SoundDiscussion& sd = dm.discussions[dis_i];
	
	for(int i = 0; i < sd.messages.GetCount(); i++) {
		const auto& it = sd.messages[i];
		messages.Set(i, 0, i);
		messages.Set(i, "IDX", i);
	}
	messages.SetCount(sd.messages.GetCount());
	if (!messages.IsCursor() && messages.GetCount())
		messages.SetCursor(0);
	#endif
}

void OmniDetailedCtrl::DataMessage() {
	#if 0
	OmniThread& dm = OmniThread::Single();
	
	if (!discussions.IsCursor() || !messages.IsCursor()) {
		phrases.Clear();
		ClearWaveform();
		return;
	}
	
	int dis_i = discussions.Get("IDX");
	const SoundDiscussion& sd = dm.discussions[dis_i];
	int msg_i = messages.Get("IDX");
	const SoundMessage& sm = sd.messages[msg_i];
	
	int prev_c = phrases.GetCount();
	int c = sm.phrases.GetCount();
	for(int i = 0; i < c; i++) {
		const auto& it = sm.phrases[i];
		phrases.Set(i, 0, i);
		phrases.Set(i, "IDX", i);
	}
	phrases.SetCount(c);
	if (c && prev_c == c-1 && sm.phrases[c-1].current && sm.phrases[c-1].current->IsUpdating())
		phrases.SetCursor(c-1);
	else if (!phrases.IsCursor() && phrases.GetCount())
		phrases.SetCursor(0);
	#endif
}

void OmniDetailedCtrl::DataPhrase() {
	#if 0
	OmniThread& dm = OmniThread::Single();
	
	if (!phrases.IsCursor()) {
		ClearWaveform();
		return;
	}
	
	int dis_i = discussions.Get("IDX");
	const SoundDiscussion& sd = dm.discussions[dis_i];
	int msg_i = messages.Get("IDX");
	const SoundMessage& sm = sd.messages[msg_i];
	int phrase_i = phrases.Get("IDX");
	const SoundPhrase& sp = sm.phrases[phrase_i];
	
	if (!sp.current) {
		ClearWaveform();
		return;
	}
	
	const SoundClipBase& clip = *sp.current;
	waveform.duration.SetData(Format("%f.1 seconds", clip.GetDuration()));
	waveform.channels.SetData(IntStr(clip.GetChannels()));
	waveform.samplerate.SetData(IntStr(clip.GetSampleRate()));
	waveform.sampleformat.SetData(GetSampleFormatString(clip.GetFormat()));
	wavectrl.SetClip(clip);
	wavectrl.Refresh();
	#endif
}

void OmniDetailedCtrl::ClearWaveform() {
	waveform.duration.SetData(Value());
	waveform.channels.SetData(Value());
	waveform.samplerate.SetData(Value());
	waveform.sampleformat.SetData(Value());
	wavectrl.Clear();
}

void OmniDeviceIO::PopulateSrc() {
	auto& form = snd.form;
	auto& thrd = snd.thrd;
	const SoundSystem& s=SoundSys();
	form.src.Clear();
	form.src->SetRoot(Null,-1,"Use default input device");
	for(int i=0;i<s.GetAPICount();i++){
		SoundAPI a=s.GetAPI(i);
		form.src->Add(0,MetaImgs::API(),a.index,a.name + String(" (default input device)"),true);
	}
	for(int i=0;i<s.GetCount();i++){
		SoundDevice d=s[i];
		if(d.InputChannels)
			form.src->Add(form.src->Find(d.API),MetaImgs::Device(),d.index+1024,d.name,false);
	}
	form.src->OpenDeep(0);
	
	Ide* ide = TheIde();
	form.src <<= ide->audio_src;
}

void OmniDeviceIO::OnRecord() {
	auto& form = snd.form;
	auto& thrd = snd.thrd;
	form.error.Clear();
	
	SoundDevice dev;
	int n=~form.src;
	if(n==-1)
		dev=SoundSys().GetDefaultInput();
	else if(n<1024)
		dev=SoundSys().GetAPI(n).defaultInputDevice;
	else
		dev=SoundSys().GetDevices()[n-1024];
	if (IsNull(dev)){
		form.error <<= "[1 Error: No default input device.";
		return;
	}
	
	Ide* ide = TheIde();
	ide->audio_src = n;
	
	SoundDaemon& sd = SoundDaemon::Static();
	hash_t stream_hash = dev.GetHashValue();
	
	if (owner)
		owner->SetSoundThread(&sd.template GetAddThread<uint8>(dev, 1));
	else
		SetSoundThread(&sd.template GetAddThread<uint8>(dev, 1));
	
	thrd->silence_timelimit = TheIde()->audio_timelimit;
	thrd->silence_treshold = TheIde()->audio_volumetreshold;
	
	Ptr<Ctrl> p = this;
	thrd->WhenFinished = [this,p](void* arg){if (p) OnFinish(arg);};
	
	thrd->Start();
	
	OnStart();
}

void OmniDeviceIO::OnStart() {
	auto& form = snd.form;
	auto& thrd = snd.thrd;
	form.rec.SetImage(Image());
	form.rec.SetLabel(t_("Stop"));
	form.rec.WhenAction = THISBACK(Stop);
	EnableMeter();
}

void OmniDeviceIO::Stop() {
	auto& thrd = snd.thrd;
	if (thrd) {
		thrd->Stop();
		OnFinish(0);
	}
}

void OmniDeviceIO::EnableMeter() {
	snd.tc.Set(-100, [this]{snd.meter.Refresh();});
}

void OmniDeviceIO::DisableMeter() {
	snd.tc.Kill();
}

void OmniDeviceIO::OnError(String s) {
	snd.form.error <<= "[1 Error: " + s;
	DisableMeter();
}

void OmniDeviceIO::OnFinish(void*) {
	auto& form = snd.form;
	GuiLock __;
	form.rec.SetImage(MetaImgs::Record());
	form.rec.SetLabel(t_("Record"));
	form.rec.WhenAction = THISBACK(OnRecord);
	DisableMeter();
}

OmniDeviceIO::VolumeMeterCtrl::VolumeMeterCtrl() {
	SetFrame(InsetFrame());
}

void OmniDeviceIO::VolumeMeterCtrl::Paint(Draw& d) {
	Size sz = GetSize();
	d.DrawRect(sz, White());
	if (c && c->snd.thrd) {
		double vol = c->snd.thrd->GetVolume();
		int h = (int)(sz.cy * vol);
		Color clr = Blue();
		d.DrawRect(RectC(0,sz.cy-h,sz.cx,h), clr);
	}
}
#endif

END_UPP_NAMESPACE
