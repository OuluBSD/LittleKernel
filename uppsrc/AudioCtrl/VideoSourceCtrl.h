#ifndef _AudioCtrl_VideoSourceCtrl_h_
#define _AudioCtrl_VideoSourceCtrl_h_

NAMESPACE_UPP

class VideoSourceCtrl : public InterfaceCtrl {
	AudioSourceCtrl audio;
	
public:
	typedef VideoSourceCtrl CLASSNAME;
	VideoSourceCtrl();
	
	void SetInterface(ComponentPtr c, ExchangeProviderBasePtr b) override;
	
};

END_UPP_NAMESPACE

#endif
