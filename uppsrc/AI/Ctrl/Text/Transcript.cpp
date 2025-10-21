#include "Text.h"
#include <ide/ide.h>


NAMESPACE_UPP


TranscriptProofreadCtrl::TranscriptProofreadCtrl() {
	CtrlLayout(*this);
	
	start.WhenAction = THISBACK(Start);
	play.WhenAction = THISBACK(PlaySelected);
	
	ai.WhenAction = [this] {
		int ai_idx = this->ai.GetIndex();
		auto& comp = GetExt<COMPNAME>();
		comp.val.value("ai-idx") = ai_idx;
	};
	
	misspelled.WhenAction = [this] {
		auto& comp = GetExt<COMPNAME>();
		comp.val.value("misspelled") = this->misspelled.GetData();
	};
	
	lines.AddColumn("Begin");
	lines.AddColumn("End");
	lines.AddColumn("Text");
	lines.AddIndex("IDX");
	lines.ColumnWidths("1 1 3");
	lines.WhenBar = [this](Bar& b) {
		if (lines.IsCursor())
			b.Add("Play", THISBACK1(PlaySingle, lines.Get("IDX")));
	};
}

void TranscriptProofreadCtrl::Data() {
	auto& comp = GetExt<TranscriptProofread>();
	SetAiProviders(this->ai, comp.val.value("ai-idx"));
	
	finder.UpdateSources(*this, transcripts, THISBACK(DataFile));
	DataFile();
}

void TranscriptProofreadCtrl::DataFile() {
	int idx = transcripts.GetIndex();
	if (idx < 0 || idx >= this->transcripts.GetCount())
		return;
	COMPNAME& comp = GetExt<COMPNAME>();
	auto& vidfile = *finder.file_ptrs[idx];
	comp.val.value("text")        = vidfile.val.value("text");
	comp.val.value("path")        = vidfile.val.value("path");
	comp.val.value("duration")    = vidfile.val.value("duration");
	comp.val.value("frame_rate")  = vidfile.val.value("frame_rate");
	comp.val.value("vidpath")     = vidfile.val.value("path");
	comp.val.value("range_begin") = vidfile.val.value("range_begin");
	comp.val.value("range_end")   = vidfile.val.value("range_end");
	
	this->misspelled.SetData(comp.val.value("misspelled"));
	
	String text = comp.val.value("proofread");
	if (text.IsEmpty()) {
		this->lines.Clear();
	}
	else {
		ValueMap selected = comp.val.value("selected");
		comp.val.value("selected") = selected; // realize ValueArray
		
		TranscriptResponse r;
		LoadFromJson(r,text);
		opt.SetCount(r.segments.GetCount());
		for(int i = 0; i < r.segments.GetCount(); i++) {
			auto& segment = r.segments[i];
			lines.Set(i, 0, GetDurationString(segment.start));
			lines.Set(i, 1, GetDurationString(segment.end));
			lines.Set(i, 2, segment.text);
			bool b = selected.Find(i) >= 0;
			auto& o = opt[i];
			o.SetLabel(segment.text);
			lines.SetCtrl(i, 2, o);
			o.Set(b);
			o.WhenAction = [this,i,&o]{
				COMPNAME& comp = GetExt<COMPNAME>();
				ValueMap selected = comp.val.value("selected");
				if (o.Get())
					selected.GetAdd((Value)i) = true;
				else
					selected.RemoveKey((Value)i);
				comp.val.value("selected") = selected;
			};
			lines.Set(i, "IDX", i);
		}
	}
}

void TranscriptProofreadCtrl::Start() {
	PostCallback([this]{this->start.Disable();});
	Event<> fn = [this]{
		ts.Reset();
		COMPNAME& comp = GetExt<COMPNAME>();
		TaskMgr& m = AiTaskManager();
		TaskArgs args;
		args.fn = FN_TRANSCRIPT_PROOFREAD_1;
		int idx = ai.GetIndex();
		args.params("ai_provider_idx") = this->ai.GetKey(idx);
		args.params("misspelled") = comp.val.value("misspelled");
		args.params("text") = comp.val.value("text");
		PostCallback([this,&comp]{this->status.SetLabel("Making proofread of transcript of: " + (String)comp.val.value("path"));});
		m.GetJson(args, [this](String s) {
			COMPNAME& comp = GetExt<COMPNAME>();
			s = "- #" + s;
			//DLOG(s);
			
			String text = comp.val.value("text");
			TranscriptResponse r;
			LoadFromJson(r, text);
			
			Index<int> modified_lines;
			Vector<String> lines = Split(s, "\n");
			for (String& l : lines) {
				if (l.Left(3) != "- #") continue;
				l = TrimLeft(l.Mid(3));
				int line = ScanInt(l);
				int i = l.Find(" ");
				if (i < 0) continue;
				l = l.Mid(i+1);
				if (line >= 0 && line < r.segments.GetCount()) {
					auto& src_segment = r.segments[line];
					src_segment.text = TrimBoth(l);
					modified_lines.FindAdd(line);
				}
			}
			Vector<int> rm_list;
			// Combine time-range of removed segments
			for(int i = 0; i < r.segments.GetCount(); i++) {
				if (modified_lines.Find(i) < 0)
					rm_list << i;
				else if (modified_lines.Find(i) >= 0) {
					auto& to_segment = r.segments[i];
					to_segment.tokens.Clear();
					if (i+1 < r.segments.GetCount()) {
						i++;
						for(; i < r.segments.GetCount(); i++) {
							if (modified_lines.Find(i) >= 0) {
								i--;
								break;
							}
							auto& from_segment = r.segments[i];
							rm_list << i;
							to_segment.end = from_segment.end;
						}
					}
				}
			}
			if (!rm_list.IsEmpty())
				r.segments.Remove(rm_list);
			
			r.text = "";
			for(int i = 0; i < r.segments.GetCount(); i++) {
				if (i) text << " ";
				r.text << r.segments[i].text;
			}
			
			comp.val.value("proofread") = StoreAsJson(r);
			
			PostCallback([this,s]{
				this->status.SetLabel("Proofread was completed in " + ts.ToString());
				this->start.Enable();
				DataFile();
			});
		});
	};
	fn(); // I left this event-call-pattern for code duplication, where wrapper with callback-param is needed
}

void TranscriptProofreadCtrl::PlaySelected() {
	if (!playing) {
		PostCallback([this]{this->play.SetLabel("Stop");});
		Thread::Start([this]{
			COMPNAME& comp = GetExt<COMPNAME>();
			ValueMap selected = comp.val.value("selected");
			Vector<int> segs;
			for(int i = 0; i < selected.GetCount(); i++) {
				int idx = selected.GetKey(i);
				segs << idx;
			}
			Sort(segs, StdLess<int>());
			
			String path = comp.val.value("path");
			String text = comp.val.value("proofread");
			TranscriptResponse r;
			LoadFromJson(r, text);
			for(int i = 0; i < segs.GetCount() && playing; i++) {
				int seg_i = segs[i];
				if (seg_i >= 0 && seg_i < r.segments.GetCount()) {
					const auto& seg = r.segments[seg_i];
					
					String cmd = "cmd /c mpv";
					cmd << " --start=" + DblStr(seg.start) + " --end=" + DblStr(seg.end);
					cmd += " \"" + path + "\"";
					String out;
					Sys(cmd, out);
				}
			}
			PostCallback([this]{this->play.SetLabel("Play");});
			playing = false;
		});
	}
	else {
		PostCallback([this]{this->play.SetLabel("Play");});
	}
	
	playing = !playing;
}

void TranscriptProofreadCtrl::PlaySingle(int seg_i) {
	COMPNAME& comp = GetExt<COMPNAME>();
	String path = comp.val.value("path");
	String text = comp.val.value("proofread");
	TranscriptResponse r;
	LoadFromJson(r, text);
	if (seg_i >= 0 && seg_i < r.segments.GetCount()) {
		const auto& seg = r.segments[seg_i];
		String cmd = "cmd /c mpv";
		cmd << " --start=" + DblStr(seg.start) + " --end=" + DblStr(seg.end);
		cmd += " \"" + path + "\"";
		String out;
		Sys(cmd, out);
	}
}

void TranscriptProofreadCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(TranscriptProofread, TranscriptProofreadCtrl)





StorylineConversionCtrl::StorylineConversionCtrl() {
	
}

void StorylineConversionCtrl::Data() {
	
}

void StorylineConversionCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(StorylineConversion, StorylineConversionCtrl)






StorylineScriptCtrl::StorylineScriptCtrl() {
	
}

void StorylineScriptCtrl::Data() {
	
}

void StorylineScriptCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(StorylineScript, StorylineScriptCtrl)



ScriptConversionCtrl::ScriptConversionCtrl() {
	
}

void ScriptConversionCtrl::Data() {
	
}

void ScriptConversionCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(ScriptConversion, ScriptConversionCtrl)



END_UPP_NAMESPACE
