#include "Script.h"

NAMESPACE_UPP
namespace Eon {


ScriptMachineLoader::ScriptMachineLoader(ScriptSystemLoader& parent, int id, Eon::MachineDefinition& def) :
	Base(parent, id, def)
{
	
	for (Eon::ChainDefinition& chain : def.chains) {
		ScriptTopChainLoader& loader = chains.Add(new ScriptTopChainLoader(0, *this, 0, chains.GetCount(), chain));
	}
	
}

bool ScriptMachineLoader::Load() {
    for (ScriptTopChainLoader& tc : chains)
        if (!tc.Load())
            return false;
    return true;
}

String ScriptMachineLoader::GetTreeString(int indent) {
	String s;
	s.Cat('\t', indent);
	s << "Machine " << id;
	s.Cat('\n');
	for (ScriptTopChainLoader& loader : chains) {
		s << loader.GetTreeString(indent+1);
	}
	
	return s;
}

void ScriptMachineLoader::GetLoops(Vector<ScriptLoopLoader*>& v) {
	for (ScriptTopChainLoader& loader : chains) {
		loader.GetLoops(v);
	}
}

void ScriptMachineLoader::GetStates(Vector<ScriptStateLoader*>& v) {
	for (ScriptTopChainLoader& loader : chains) {
		loader.GetStates(v);
	}
}


}
END_UPP_NAMESPACE
