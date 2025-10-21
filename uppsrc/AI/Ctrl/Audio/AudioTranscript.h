#ifndef _AI_Ctrl_AudioTranscript_h_
#define _AI_Ctrl_AudioTranscript_h_

NAMESPACE_UPP

void SetAiProviders(DropList& ai, int ai_idx);

template <class K, class T>
struct RangeFinder {
	Vector<Ptr<T>> file_ptrs;
	Vector<String> file_paths;
	
	bool UpdateSources(ComponentCtrl& c, DropList& files, Event<> DataFileCb) {
		file_ptrs.Clear();
		file_paths.Clear();
		K& comp = c.GetExt<K>();
		if (!comp.val.owner)
			return false;
		String sel_path = comp.val.value("path");
		files.Clear();
		files.WhenAction.Clear();
		file_ptrs = comp.val.owner->template FindAll<T>();
		int cursor = 0;
		for(int i = 0; i < file_ptrs.GetCount(); i++) {
			auto& file = *file_ptrs[i];
			ValueMap map = file.val.value;
			String path = map("path");
			double range_begin = file.val.value("range_begin");
			double range_end = file.val.value("range_end");
			double total = range_end - range_begin;
			String total_str = GetDurationString(total);
			file_paths.Add(path);
			String title = Format("%s (%.2f - %.2f, %s)", GetFileName(path), range_begin, range_end, total_str);
			files.Add(title);
			if (sel_path == path)
				cursor = i;
		}
		bool change_file = false;
		if (cursor >= 0 && cursor < files.GetCount()) {
			files.SetIndex(cursor);
		}
		files.WhenAction = DataFileCb;
		return change_file;
	}

};

class AudioTranscriptCtrl : public WithAudioTranscript<ComponentCtrl> {
	typedef AudioTranscript COMPNAME;
	RangeFinder<COMPNAME, VideoSourceFileRange> finder;
	Array<EditString> line_editors;
	TranscriptResponse r;
	TimeCallback tc;
	double duration = 0;
	double frame_rate = 1;
	double range_begin = 0;
	double range_end = 0;
	String vidpath;
	String mp3path;
	TimeStop ts;
	
	void MakeAudio(Event<> cb_ready);
	void Start();
	void SaveTextChanges();
public:
	typedef AudioTranscriptCtrl CLASSNAME;
	AudioTranscriptCtrl();
	
	void Data() override;
	void DataFile();
	void ToolMenu(Bar& bar) override;
	
};

INITIALIZE(AudioTranscriptCtrl)

class ScriptSpeechCtrl : public WithScriptSpeech<ComponentCtrl> {
	
public:
	typedef ScriptSpeechCtrl CLASSNAME;
	ScriptSpeechCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	
	
};

INITIALIZE(ScriptSpeechCtrl)

END_UPP_NAMESPACE

#endif
 
