#include "Script.h"

NAMESPACE_UPP
namespace Eon {

ScriptPoolLoader::ScriptPoolLoader(ScriptWorldLoader& parent, ScriptPoolLoader* chain_parent, int id, Eon::PoolDefinition& def) :
	Base(parent, id, def),
	chain_parent(chain_parent)
{
	
	for (Eon::EntityDefinition& e : def.ents) {
		entities.Add(new ScriptEntityLoader(*this, entities.GetCount(), e));
	}
	
	for (Eon::PoolDefinition& p : def.pools) {
		pools.Add(new ScriptPoolLoader(parent, this, pools.GetCount(), p));
	}
	
}

bool ScriptPoolLoader::Load() {
	TODO
	return false;
}


}
END_UPP_NAMESPACE
