#ifndef _AudioCtrl_AudioView_h_
#define _AudioCtrl_AudioView_h_

#include <SoftAudio/SoftAudio.h>
#include <LocalCtrl/LocalCtrl.h>

NAMESPACE_UPP
using namespace UPP;

class AudioCtrl;
class AudioViewCtrl;

class AudioWindowCtrl : public Ctrl {
	
protected:
	friend class AudioViewCtrl;
	AudioViewCtrl* viewctrl;
	int offset;
	int window;
	int channel;
	
public:
	//RTTI_DECL1(AudioWindowCtrl, Ctrl)
	AudioWindowCtrl();
	
};

class AudioPlotCtrl : public AudioWindowCtrl {
	
	
public:
	AudioPlotCtrl();
	
	virtual void Paint(Draw& d);
	
};


class AudioHeatmapCtrl : public AudioWindowCtrl {
	
	Image render;
	
public:
	AudioHeatmapCtrl();
	
	virtual void Paint(Draw& d);
	
};

class AudioViewCtrl : public Ctrl {
	
protected:
	friend class AudioPlotCtrl;
	friend class AudioHeatmapCtrl;
	
	Record* file = NULL;
	
	Splitter plot_split, hmap_split;
	Array<AudioPlotCtrl> plots;
	Array<AudioHeatmapCtrl> hmaps;
	
public:
	//RTTI_DECL1(AudioViewCtrl, Ctrl)
	typedef AudioViewCtrl CLASSNAME;
	AudioViewCtrl();
	
	void RefreshData();
	void SetView(int type);
	
};

END_UPP_NAMESPACE

#endif
