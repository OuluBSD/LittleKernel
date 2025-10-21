#include "Script.h"

NAMESPACE_UPP
namespace Eon {


ScriptWorldLoader::ScriptWorldLoader(ScriptSystemLoader& parent, int id, Eon::WorldDefinition& def) :
	Base(parent, id, def)
{
	
	for (Eon::EcsSysDefinition& sys : def.systems) {
		systems.Add(new ScriptEcsSystemLoader(*this, systems.GetCount(), sys));
	}
	
	for (Eon::PoolDefinition& pool : def.pools) {
		pools.Add(new ScriptPoolLoader(*this, 0, pools.GetCount(), pool));
	}
	
}

bool ScriptWorldLoader::Load() {
	TODO
	return false;
}

String ScriptWorldLoader::GetTreeString(int indent) {
	String s;
	s.Cat('\t', indent);
	s << "Engine " << id;
	s.Cat('\n');
	for (ScriptEcsSystemLoader& loader : systems) {
		s << loader.GetTreeString(indent+1);
	}
	for (ScriptPoolLoader& loader : pools) {
		s << loader.GetTreeString(indent+1);
	}
	
	return s;
}


ExtScriptEcsLoaderBase* __ecs_script_loader;



}
END_UPP_NAMESPACE
