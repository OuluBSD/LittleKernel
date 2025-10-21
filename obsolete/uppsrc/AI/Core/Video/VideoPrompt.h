#ifndef _AI_Core_VideoPrompt_h_
#define _AI_Core_VideoPrompt_h_





struct VideoPromptMaker : Component
{
	
	COMPONENT_CONSTRUCTOR(VideoPromptMaker)
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1);
	}
	
};

INITIALIZE(VideoPromptMaker)




#endif
