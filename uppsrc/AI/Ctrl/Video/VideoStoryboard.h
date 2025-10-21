#ifndef _AI_ImgCtrl_VideoStoryboard_h_
#define _AI_ImgCtrl_VideoStoryboard_h_

NAMESPACE_UPP


class VideoStoryboardCtrl : public AiComponentCtrl {
	Splitter split, vsplit0, vsplit1;
	ArrayCtrl list;
	ImageViewerCtrl img[4];
	
public:
	typedef VideoStoryboardCtrl CLASSNAME;
	VideoStoryboardCtrl();
	
	void Data() override;
	void DataLine();
	void OnValueChange();
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	
	
};

INITIALIZE(VideoStoryboardCtrl)


END_UPP_NAMESPACE

#endif
 
