#include "AudioCtrl.h"

NAMESPACE_UPP


INITBLOCK_(AudioSourceCtrl) {
	MakeInterfaceCtrlFactory<AudioSource, AudioSourceCtrl>();
}



AudioSourceCtrl::AudioSourceCtrl() {
	Add(s.SizePos());
	s.Background(Color(176, 237, 255));
}

void AudioSourceCtrl::SetInterface(ComponentPtr c, ExchangeProviderBasePtr b) {
	
}



END_UPP_NAMESPACE
