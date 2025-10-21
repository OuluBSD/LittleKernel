#include "AudioCtrl.h"

NAMESPACE_UPP



INITBLOCK_(VideoSourceCtrl) {
	MakeInterfaceCtrlFactory<VideoSource, VideoSourceCtrl>();
}



VideoSourceCtrl::VideoSourceCtrl() {
	Add(audio.SizePos());
	
}

void VideoSourceCtrl::SetInterface(ComponentPtr c, ExchangeProviderBasePtr b) {
	
}



END_UPP_NAMESPACE
