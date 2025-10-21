#include <AI/Ctrl/Ctrl.h>
#include <ide/ide.h>

NAMESPACE_UPP

AudioTranscriptCtrl::AudioTranscriptCtrl() {
	CtrlLayout(*this);
	make_audio.WhenAction = THISBACK1(MakeAudio, Event<>());
	start.WhenAction = THISBACK(Start);
	ai.WhenAction = [this] {
		int ai_idx = this->ai.GetIndex();
		AudioTranscript& comp = GetExt<AudioTranscript>();
		comp.val.value("ai-idx") = ai_idx;
	};
	language.WhenAction = [this] {
		AudioTranscript& comp = GetExt<AudioTranscript>();
		comp.val.value("language") = this->language.GetData();
	};
	lines.AddColumn("Begin");
	lines.AddColumn("End");
	lines.AddColumn("Text");
	lines.AddIndex("IDX");
	lines.ColumnWidths("1 1 3");
}

void AudioTranscriptCtrl::MakeAudio(Event<> cb_ready) {
	if (vidpath.IsEmpty())
		return;
	
	PostCallback([this]{this->make_audio.Disable();});
	
	Thread::Start([this,cb_ready]{
		CombineHash ch;
		ch << range_begin << range_end;
		hash_t h = ch;
		String dir = ConfigFile("cache");
		RealizeDirectory(dir);
		mp3path = AppendFileName(dir, GetFileTitle(vidpath) + "-" + IntStr(h & 0xFFFFFF) + ".mp3");
		double range_len = range_end - range_begin;
		String cmd =
			"ffmpeg -y -i \"" + vidpath + "\" "
			"-ss " + DblStr(range_begin) + " -t " + DblStr(range_len) + " "
			"-vn -acodec mp3 -ar 44100 -ac 1 -ab 128k "
			"-filter:a \"speechnorm=e=12.5:r=0.0001:l=1\" "
			"\"" + mp3path + "\"";
		//DLOG(cmd);
		String out;
		if (!Sys(cmd, out))
			PostCallback([this]{this->status.SetLabel("Wrote audio file: " + mp3path);});
		else
			PostCallback([this]{this->status.SetLabel("Failed to extract audio");});
		PostCallback([this]{this->make_audio.Enable();});
		PostCallback(cb_ready);
	});
}

void AudioTranscriptCtrl::Start() {
	PostCallback([this]{this->start.Disable();});
	Event<> fn = [this]{
		ts.Reset();
		String mp3 = LoadFile(mp3path);
		if (mp3.IsEmpty())
			return;
		AudioTranscript& comp = GetExt<AudioTranscript>();
		TaskMgr& m = AiTaskManager();
		TranscriptionArgs args;
		int idx = ai.GetIndex();
		args.file = mp3path;
		args.ai_provider_idx = this->ai.GetKey(idx);
		args.language = comp.val.value("language");
		PostCallback([this]{this->status.SetLabel("Making transcript of audio file: " + this->mp3path);});
		m.GetTranscription(args, [this](String s) {
			AudioTranscript& comp = GetExt<AudioTranscript>();
			comp.val.value("text") = s;
			//DLOG(s);
			PostCallback([this,s]{
				this->status.SetLabel("Transcription was completed in " + ts.ToString());
				this->start.Enable();
				DataFile();
			});
		});
	};
	if (mp3path.IsEmpty())
		MakeAudio(fn);
	else
		fn();
}

void SetAiProviders(DropList& ai, int ai_idx) {
	ai.Clear();
	auto& ai_mgr = AiManager();
	for(int i = 0; i < ai_mgr.GetCount(); i++) {
		auto& prov = ai_mgr[i];
		if (prov.IsFeatureAudioToText())
			ai.Add(i, ai_mgr[i].name + " (" + ai_mgr[i].GetTypeString() + ")");
	}
	if (ai.GetCount()) {
		if (ai_idx >= 0 && ai_idx < ai.GetCount())
			ai.SetIndex(ai_idx);
		else
			ai.SetIndex(0);
	}
}

void AudioTranscriptCtrl::Data() {
	AudioTranscript& comp = GetExt<AudioTranscript>();
	SetAiProviders(this->ai, comp.val.value("ai-idx"));
	
	finder.UpdateSources(*this, files, THISBACK(DataFile));
	DataFile();
}

void AudioTranscriptCtrl::DataFile() {
	AudioTranscript& comp = GetExt<AudioTranscript>();
	int idx = files.GetIndex();
	if (idx < 0 || idx >= this->files.GetCount())
		return;
	auto& vidfile = *finder.file_ptrs[idx];
	
	this->duration    = vidfile.val.value("duration");
	this->frame_rate  = FractionDbl((String)vidfile.val.value("frame_rate"));
	this->vidpath     = vidfile.val.value("path");
	this->range_begin = vidfile.val.value("range_begin");
	this->range_end   = vidfile.val.value("range_end");
	
	comp.val.value("path")        = vidfile.val.value("path");
	comp.val.value("duration")    = vidfile.val.value("duration");
	comp.val.value("frame_rate")  = vidfile.val.value("frame_rate");
	comp.val.value("vidpath")     = vidfile.val.value("path");
	comp.val.value("range_begin") = vidfile.val.value("range_begin");
	comp.val.value("range_end")   = vidfile.val.value("range_end");
	
	this->language.SetData(comp.val.value("language"));
	
	String text = comp.val.value("text");
	if (text.IsEmpty()) {
		this->lines.Clear();
	}
	else {
		LoadFromJson(r,text);
		this->line_editors.SetCount(r.segments.GetCount());
		for(int i = 0; i < r.segments.GetCount(); i++) {
			auto& segment = r.segments[i];
			lines.Set(i, 0, GetDurationString(segment.start));
			lines.Set(i, 1, GetDurationString(segment.end));
			lines.Set(i, 2, segment.text);
			lines.Set(i, "IDX", i);
			EditString& es = line_editors[i];
			es.SetData(segment.text);
			lines.SetCtrl(i, 2, es);
			es.WhenAction = [this,&es,&segment]{
				segment.text = es.GetData();
				tc.Set(500, THISBACK(SaveTextChanges));
			};
		}
	}
}

void AudioTranscriptCtrl::SaveTextChanges() {
	AudioTranscript& comp = GetExt<AudioTranscript>();
	String text = StoreAsJson(r, false);
	//LOG(text);
	comp.val.value("text") = text;
}

void AudioTranscriptCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(AudioTranscript, AudioTranscriptCtrl)





ScriptSpeechCtrl::ScriptSpeechCtrl() {
	
}

void ScriptSpeechCtrl::Data() {
	
}

void ScriptSpeechCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(ScriptSpeech, ScriptSpeechCtrl)



END_UPP_NAMESPACE
