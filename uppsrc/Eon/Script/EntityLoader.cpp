#include "Script.h"

NAMESPACE_UPP
namespace Eon {

ScriptEntityLoader::ScriptEntityLoader(ScriptPoolLoader& parent, int id, Eon::EntityDefinition& def) :
	Base(parent, id, def) {
	
	for (Eon::ComponentDefinition& sys : def.comps) {
		comps.Add(new ScriptComponentLoader(*this, comps.GetCount(), sys));
	}
	
}

bool ScriptEntityLoader::Load() {
	TODO
	return false;
}


}
END_UPP_NAMESPACE
