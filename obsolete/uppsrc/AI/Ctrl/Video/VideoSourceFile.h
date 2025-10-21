#ifndef _AI_Ctrl_VideoSourceFile_h_
#define _AI_Ctrl_VideoSourceFile_h_

NAMESPACE_UPP

class VideoSourceFileCtrl : public WithVideoSourceFile<ComponentCtrl> {
	String duration;
	String container_str;
	String duration_str;
	String video_sample_str;
	String video_codec_str;
	String audio_sample_str;
	String audio_codec_str;
	String size_str;
	String full_json;
	Value last_value;
	
public:
	typedef VideoSourceFileCtrl CLASSNAME;
	VideoSourceFileCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void UpdateData();
	
};

INITIALIZE(VideoSourceFileCtrl)

class RangeCtrl : public Ctrl {
	double range_begin = 0, range_end = 1;
	double cursor = 0;
	int GetTimePos(double t, int width) const;
	double GetPos(int x, int width) const;
	int is_selecting = 0;
	int last_border = -1;
public:
	typedef RangeCtrl CLASSNAME;
	RangeCtrl();
	void Paint(Draw& d) override;
	void LeftDown(Point p, dword keyflags) override;
	void LeftUp(Point p, dword keyflags) override;
	void MouseMove(Point p, dword keyflags) override;
	void MouseLeave() override;
	void Set(double cursor, double range_begin, double range_end);
	void SetCursor(double d);
	Event<double> WhenSetBegin, WhenSetEnd, WhenTime;
};

class VideoSourceFileRangeCtrl : public AiComponentCtrl {
	WithVideoSourceFileRange<Ctrl> header;
	ImageViewerCtrl viewer;
	RangeCtrl range;
	Vector<Ptr<VideoSourceFile>> files;
	Vector<String> file_paths;
	double duration = 0;
	double frame_rate = 1;
	void GetImage(double time, Event<Image> cb);
	void SetViewer(Image img);
public:
	typedef VideoSourceFileRangeCtrl CLASSNAME;
	VideoSourceFileRangeCtrl();
	
	void Data() override;
	void DataFile();
	bool UpdateSources();
	void ToolMenu(Bar& bar) override;
	void Set(double t, int src);
	void Play(bool range);
	VideoSourceFile* GetFile();
	String GetFilePath();
};

INITIALIZE(VideoSourceFileRangeCtrl)

END_UPP_NAMESPACE

#endif
 
