#include "Video.h"


NAMESPACE_UPP


VideoSourceFileCtrl::VideoSourceFileCtrl() {
	CtrlLayout(*this);
	
	this->browse.WhenAction = [this]{
		VideoSourceFile& comp = this->GetExt<VideoSourceFile>();
		ValueMap map = comp.val.value;
		String path = map("path");
		String dir = path.Is() ? GetFileDirectory(path) : GetHomeDirectory();
		FileSelNative sel;
		sel.ActiveDir(dir);
		if (path.Is())
			sel.Set(GetFileName(path));
		if (sel.ExecuteOpen("Select video file")) {
			map = comp.val.value;
			map.Set("path", sel.Get());
			comp.val.value = map;
			PostCallback(THISBACK(UpdateData));
		}
	};
	
	this->update.WhenAction = THISBACK(UpdateData);
	
}

void VideoSourceFileCtrl::UpdateData() {
	VideoSourceFile& comp = this->GetExt<VideoSourceFile>();
	ValueMap map = comp.val.value;
	String path = map("path");
	if (!FileExists(path)) {
		PromptOK("File doesn't exist: " + path);
		return;
	}
	
	String cmd = "ffprobe -v quiet -print_format json -show_format -show_streams -print_format json ";
	cmd += "\"" + path + "\"";
	
	String out;
	int ret = Sys(cmd, out);
	LOG(out);
	if (ret) {
		PromptOK("Error executing: " + cmd);
		return;
	}
	this->full_json = out;
	
	int video_stream_count = 0;
	int audio_stream_count = 0;
	Value val = ParseJSON(out);
	this->last_value = val;
	this->video_sample_str.Clear();
	this->video_codec_str.Clear();
	this->audio_codec_str.Clear();
	this->audio_sample_str.Clear();
	Value streams = val("streams");
	for(int i = 0; i < streams.GetCount(); i++) {
		Value stream = streams[i];
		String codec_name = stream("codec_name");
		String codec_type = stream("codec_type");
		String bit_rate = stream("bit_rate");
		String time_base = stream("time_base");
		double time_base_d = FractionDbl(time_base);
		double duration = stream("duration_ts");
		double seconds = duration * time_base_d;
		if (codec_type == "video") {
			this->video_codec_str = stream("codec_long_name");
			Size sz(stream("width"), stream("height"));
			Size aspect = GetAspectRatio(sz);
			String pix_fmt = stream("pix_fmt");
			String frame_rate = stream("r_frame_rate");
			map("frame_rate") = frame_rate;
			if (++video_stream_count == 1) {
				this->video_sample_str = Format("%d x %d, aspect %d:%d, %s, fps %s", sz.cx, sz.cy, aspect.cx, aspect.cy, pix_fmt, frame_rate);
			}
		}
		else if (codec_type == "audio") {
			this->audio_codec_str = stream("codec_long_name");
			int channels = stream("channels");
			String sample_fmt = stream("sample_fmt");
			String sample_rate = stream("sample_rate");
			if (sample_fmt == "fltp") sample_fmt = "32bit float";
			if (++audio_stream_count == 1) {
				this->audio_sample_str = Format("%s, samplerate %s", sample_fmt, sample_rate);
			}
		}
	}
	Value format = val("format");
	{
		String format_name = format("format_name");
		String format_long_name = format("format_long_name");
		double seconds = ScanDouble((String)format("duration"));
		map.Set("duration", seconds);
		this->duration_str = GetDurationString(seconds);
		this->size_str = GetSizeString(StrInt64((String)format("size")));
		this->container_str = format_long_name;
		if (!format_name.IsEmpty())
			container_str += " (" + format_name + ")";
		
		Value tags = format("tags");
		{
			String major_brand = tags("major_brand");
			String minor_version = tags("minor_version");
			String compatible_brands = tags("compatible_brands");
			String encoder = tags("encoder");
			if (!major_brand.IsEmpty())
				container_str += ", Major: " + major_brand;
			if (!minor_version.IsEmpty())
				container_str += ", Minor-version: " + minor_version;
			if (!compatible_brands.IsEmpty())
				container_str += ", Compatible-brands: " + compatible_brands;
			if (!encoder.IsEmpty())
				container_str += ", Encoder: " + encoder;
		}
	}
	
	comp.val.value = map;
	
	PostCallback(THISBACK(Data));
}

void VideoSourceFileCtrl::Data() {
	VideoSourceFile& comp = this->GetExt<VideoSourceFile>();
	ValueMap map = comp.val.value;
	String path = map("path");
	if (!path.IsEmpty() && this->path.GetData().IsNull())
		UpdateData();
	this->path.SetData(path);
	this->container.SetData(container_str);
	this->length.SetData(duration_str);
	this->size.SetData(size_str);
	this->video_codec.SetData(video_codec_str);
	this->video_sample.SetData(video_sample_str);
	this->audio_codec.SetData(audio_codec_str);
	this->audio_sample.SetData(audio_sample_str);
	this->json.SetData(full_json);
}

void VideoSourceFileCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(VideoSourceFile, VideoSourceFileCtrl)





RangeCtrl::RangeCtrl() {
	
}

int RangeCtrl::GetTimePos(double f, int width) const {
	if (f <= 0.0) return 0;
	if (f >= 1.0) return width;
	int x = (int)((width * f) + 0.5);
	return x;
}

double RangeCtrl::GetPos(int x, int width) const {
	x = min(width-1, max(0,x));
	double f = (double)x / (double)width;
	return f;
}

void RangeCtrl::Paint(Draw& d) {
	Color bg = GrayColor(256-32);
	Color accent = GrayColor(32);
	Color sel_clr = Color(119, 129, 173);
	Color begin_clr = LtGreen();
	Color end_clr = LtRed();
	Size sz = GetSize();
	d.DrawRect(sz, bg);
	
	/* range */ {
		int left = GetTimePos(this->range_begin, sz.cx);
		int right = GetTimePos(this->range_end, sz.cx);
		if (left < right) {
			int width = right - left;
			d.DrawRect(left, 1, width, sz.cy-2, sel_clr);
			d.DrawLine(left, 0, left, sz.cy-1, 1, begin_clr);
			d.DrawLine(right, 0, right, sz.cy-1, 1, end_clr);
		}
	}
	
	/* cursor */ {
		Color clr = Black();
		int x = GetTimePos(this->cursor, sz.cx);
		if (x > 0)
			d.DrawLine(x-1, 0, x-1, sz.cy-1, 1, bg);
		d.DrawLine(x, 0, x, sz.cy-1, 1, accent);
		if (x < sz.cx-1)
			d.DrawLine(x+1, 0, x+1, sz.cy-1, 1, bg);
	}
	
	d.DrawLine(0, 0, sz.cx-1, 0, 1, accent);
	d.DrawLine(0, sz.cy-1, sz.cx-1, sz.cy-1, 1, accent);
	d.DrawLine(0, 0, 0, sz.cy-1, 1, accent);
	d.DrawLine(sz.cx-1, 0, sz.cx-1, sz.cy-1, 1, accent);
	
}

void RangeCtrl::LeftDown(Point p, dword keyflags) {
	Size sz = GetSize();
	double f = GetPos(p.x, sz.cx);
	if (keyflags & K_CTRL) {
		is_selecting = 1;
		SetCapture();
		range_begin = f;
		WhenSetBegin(f);
	}
	else if (keyflags & K_ALT) {
		is_selecting = 2;
		SetCapture();
		range_end = f;
		WhenSetEnd(f);
	}
	else {
		is_selecting = 3;
		SetCapture();
		cursor = f;
		WhenTime(f);
	}
	Refresh();
}

void RangeCtrl::LeftUp(Point p, dword keyflags) {
	if (is_selecting) {
		Size sz = GetSize();
		double f = GetPos(p.x, sz.cx);
		if (is_selecting == 1) {
			range_begin = f;
			WhenSetBegin(f);
		}
		else if (is_selecting == 2) {
			range_end = f;
			WhenSetEnd(f);
		}
		else if (is_selecting == 3) {
			cursor = f;
			WhenTime(f);
		}
		is_selecting = 0;
		ReleaseCapture();
		Refresh();
	}
}

void RangeCtrl::MouseMove(Point p, dword keyflags) {
	if (is_selecting) {
		Size sz = GetSize();
		double f = GetPos(p.x, sz.cx);
		if (is_selecting == 1) {
			range_begin = f;
			WhenSetBegin(f);
		}
		else if (is_selecting == 2) {
			range_end = f;
			WhenSetEnd(f);
		}
		else if (is_selecting == 3) {
			cursor = f;
			WhenTime(f);
		}
		Refresh();
	}
}

void RangeCtrl::MouseLeave() {
	
}

void RangeCtrl::Set(double cursor, double range_begin, double range_end) {
	this->cursor = cursor;
	this->range_begin = range_begin;
	this->range_end = range_end;
	this->Refresh();
}

void RangeCtrl::SetCursor(double d) {
	this->cursor = d;
	this->Refresh();
	WhenTime(d);
}



VideoSourceFileRangeCtrl::VideoSourceFileRangeCtrl() {
	CtrlLayout(header);
	
	Add(header.HSizePos().TopPos(0,70));
	Add(viewer.HSizePos().VSizePos(70,50));
	Add(range.HSizePos().BottomPos(0,50));
	
	range.WhenTime     = THISBACK1(Set, 0);
	range.WhenSetBegin = THISBACK1(Set, 1);
	range.WhenSetEnd   = THISBACK1(Set, 2);
	
	header.play_all.WhenAction = THISBACK1(Play, 0);
	header.play_range.WhenAction = THISBACK1(Play, 1);
}

void VideoSourceFileRangeCtrl::Set(double t, int src) {
	VideoSourceFileRange& comp = GetExt<VideoSourceFileRange>();
	ASSERT(t >= 0.0 && t <= 1.0);
	if (src == 0) {
		double seconds = duration * t;
		GetImage(seconds,THISBACK(SetViewer));
	}
	else if (src == 1) {
		comp.val.value("range_begin") = duration * t;
		range.SetCursor(t);
	}
	else if (src == 2) {
		comp.val.value("range_end") = duration * t;
		range.SetCursor(t);
	}
}

void VideoSourceFileRangeCtrl::GetImage(double time, Event<Image> cb) {
	int frame = (int)(time * frame_rate + 0.5);
	String img_path = ConfigFile("temp.png");
	DeleteFile(img_path);
	String cmd =
		"ffmpeg -y -i "
		"\"" + GetFilePath() + "\" " +
		"-vf \"select=eq(n\\," + IntStr(frame) + ")\" -vframes 1 \"" + img_path + "\"";
	//DLOG(cmd);
	String out;
	Sys(cmd, out);
	Image img = StreamRaster::LoadFileAny(img_path);
	cb(img);
}

void VideoSourceFileRangeCtrl::SetViewer(Image img) {
	viewer.SetImage(img);
}

void VideoSourceFileRangeCtrl::Play(bool range) {
	String path = GetFilePath();
	if (path.IsEmpty() || !FileExists(path))
		return;
	String cmd = "cmd /c start mpv";
	if (range) {
		VideoSourceFileRange& comp = GetExt<VideoSourceFileRange>();
		double range_begin = comp.val.value("range_begin");
		double range_end = comp.val.value("range_end");
		if (range_begin < range_end) {
			cmd << " --start=" + DblStr(range_begin) + " --end=" + DblStr(range_end);
		}
	}
	cmd += " \"" + path + "\"";
	Thread::Start([this,cmd]{
		String out;
		Sys(cmd, out);
	});
}

VideoSourceFile* VideoSourceFileRangeCtrl::GetFile() {
	int idx = header.files.GetIndex();
	if (idx >= 0 && idx < this->files.GetCount())
		return files[idx];
	return 0;
}

String VideoSourceFileRangeCtrl::GetFilePath() {
	int idx = header.files.GetIndex();
	if (idx >= 0 && idx < this->file_paths.GetCount())
		return file_paths[idx];
	return "";
}

bool VideoSourceFileRangeCtrl::UpdateSources() {
	files.Clear();
	file_paths.Clear();
	VideoSourceFileRange& comp = GetExt<VideoSourceFileRange>();
	if (!comp.val.owner)
		return false;
	String sel_path = comp.val.value("path");
	header.files.Clear();
	header.files.WhenAction.Clear();
	files = comp.val.owner->FindAll<VideoSourceFile>();
	int cursor = 0;
	for(int i = 0; i < files.GetCount(); i++) {
		auto& comp = *files[i];
		ValueMap map = comp.val.value;
		String path = map("path");
		file_paths.Add(path);
		String name = GetFileName(path);
		header.files.Add(name);
		if (sel_path == path)
			cursor = i;
	}
	
	bool change_file = false;
	if (cursor >= 0 && cursor < header.files.GetCount()) {
		header.files.SetIndex(cursor);
	}
	header.files.WhenAction = THISBACK(DataFile);
	return change_file;
}

void VideoSourceFileRangeCtrl::Data() {
	UpdateSources();
	DataFile();
}

void VideoSourceFileRangeCtrl::DataFile() {
	int idx = header.files.GetIndex();
	if (idx < 0 || idx >= this->files.GetCount())
		return;
	auto& vidfile = *files[idx];
	String vidpath = file_paths[idx];
	this->duration = vidfile.val.value("duration");
	this->frame_rate = FractionDbl((String)vidfile.val.value("frame_rate"));
	header.duration.SetData(GetDurationString(duration));
	
	VideoSourceFileRange& comp = GetExt<VideoSourceFileRange>();
	String path = comp.val.value("path");
	double range_begin = 0, range_end = 0;
	if (path == vidpath) {
		range_begin = comp.val.value("range_begin");
		range_begin = min(duration, max(0.0, range_begin));
		range_end = comp.val.value("range_end");
		range_end = min(duration, max(0.0, range_end));
	}
	else {
		comp.val.value("path") = vidpath;
		comp.val.value("range_begin") = 0;
		comp.val.value("range_end") = this->duration;
		comp.val.value("duration") = vidfile.val.value("duration");
		comp.val.value("frame_rate") = vidfile.val.value("frame_rate");
	}
	if (!range_end) range_end = duration;
	
	header.range_begin.SetData(GetDurationString(range_begin));
	header.range_end.SetData(GetDurationString(range_end));
	
	double f_begin = range_begin / duration;
	double f_end = range_end / duration;
	range.Set(f_begin, f_begin, f_end);
	
	Set(f_begin, 0);
}

void VideoSourceFileRangeCtrl::ToolMenu(Bar& bar) {
	
}

INITIALIZER_COMPONENT_CTRL(VideoSourceFileRange, VideoSourceFileRangeCtrl)




END_UPP_NAMESPACE
