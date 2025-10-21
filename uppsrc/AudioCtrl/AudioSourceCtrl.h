#ifndef _AudioCtrl_AudioSourceCtrl_h_
#define _AudioCtrl_AudioSourceCtrl_h_

NAMESPACE_UPP

class AudioSourceCtrl : public InterfaceCtrl {
	StaticRect s;
	
public:
	typedef AudioSourceCtrl CLASSNAME;
	AudioSourceCtrl();
	
	void SetInterface(ComponentPtr c, ExchangeProviderBasePtr b) override;
	
};

END_UPP_NAMESPACE

#endif
