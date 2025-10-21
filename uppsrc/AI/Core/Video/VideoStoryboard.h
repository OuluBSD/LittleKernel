#ifndef _AI_Core_VideoStoryboard_h_
#define _AI_Core_VideoStoryboard_h_





struct VideoStoryboard : Component
{
	
	COMPONENT_CONSTRUCTOR(VideoStoryboard)
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1);
	}
	
};

INITIALIZE(VideoStoryboard)




#endif
