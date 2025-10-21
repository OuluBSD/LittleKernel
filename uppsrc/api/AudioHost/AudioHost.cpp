#include "AudioHost.h"

NAMESPACE_UPP


INITBLOCK_(AudioHost) {
	SerialLoaderFactory::Register<SerialMidiEonLoader>(".mid");
}


END_UPP_NAMESPACE
