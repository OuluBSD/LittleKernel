#ifndef _AI_Imaging_ImgAspectFixer_h_
#define _AI_Imaging_ImgAspectFixer_h_

NAMESPACE_UPP

class AspectFixer;

class ImageViewerCtrl : public Ctrl {
	Image img;
	
public:
	typedef ImageViewerCtrl CLASSNAME;
	ImageViewerCtrl();
	
	void Paint(Draw& d) override;
	void SetImage(const Image& i);
	void Clear();
	void Menu(Bar& menu);
	void RightDown(Point p, dword keyflags) override;

};

class ImageAspectFixerTool : public Ctrl {
	Splitter vsplit;
	WithImageAspectForm<Ctrl> form;
	ImageViewerCtrl from, to;
	String path;
	Image src_image;
	AspectFixer* af_ptr = 0;
	
	Vector<String> queue;
	
	void OpenFile(String path);
	
	Event<> WhenReady;
	
public:
	typedef ImageAspectFixerTool CLASSNAME;
	ImageAspectFixerTool();
	
	void Data();
	void ToolMenu(Bar& bar);
	
	void OnQueueSelect();
	void Do(int fn);
	void PostDo(int fn) {PostCallback(THISBACK1(Do, fn));}
	void QueueProcess(int cursor);
	
};

//INITIALIZE(ImageAspectFixerTool)

END_UPP_NAMESPACE

#endif
 
