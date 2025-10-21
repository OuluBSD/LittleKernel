#include "Script.h"

NAMESPACE_UPP
namespace Eon {


ScriptChainLoader::ScriptChainLoader(ScriptTopChainLoader& parent, int id, Eon::ChainDefinition& def) :
	Base(parent, id, def)
{
	
	for (Eon::LoopDefinition& loop : def.loops) {
		ScriptLoopLoader& loader = loops.Add(new ScriptLoopLoader(*this, loops.GetCount(), loop));
	}
	
	for (Eon::StateDeclaration& state : def.states) {
		ScriptStateLoader& loader = states.Add(new ScriptStateLoader(*this, states.GetCount(), state));
	}
	
}

bool ScriptChainLoader::Load() {
    for (ScriptLoopLoader& l : loops)
        if (!l.Load())
            return false;
    return true;
}

String ScriptChainLoader::GetTreeString(int indent) {
	String s;
	s.Cat('\t', indent);
	s << "Chain " << id;
	s.Cat('\n');
	for (ScriptLoopLoader& loader : loops) {
		s << loader.GetTreeString(indent+1);
	}
	
	return s;
}

void ScriptChainLoader::GetLoops(Vector<ScriptLoopLoader*>& v) {
	for (ScriptLoopLoader& loader : loops) {
		v.Add(&loader);
	}
}

void ScriptChainLoader::GetStates(Vector<ScriptStateLoader*>& v) {
	for (ScriptStateLoader& state : states) {
		v.Add(&state);
	}
}

Eon::Id ScriptChainLoader::GetDeepId() const {
	return parent.GetDeepId(); // chain & topchain has same id
}


}
END_UPP_NAMESPACE
