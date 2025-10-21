#include "Eon00.h"

NAMESPACE_UPP

void Run00bAudioGen(Engine& eng, int method) {
	auto sys = eng.GetAdd<Eon::ScriptLoader>();
	sys->SetEagerChainBuild(true);
	
	switch(method) {
	case 1:
	case 2:
		LOG(Format("warning: Run00bAudioGen: method %d not implemented yet", method));
	case 0:
		sys->PostLoadFile(GetDataFile("00b_audio_gen.eon"));
		break;
	default:
		throw Exc(Format("Run00bAudioGen: unknown method %d", method));
	}
}

END_UPP_NAMESPACE
