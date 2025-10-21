#include "Script.h"

NAMESPACE_UPP
namespace Eon {

ScriptComponentLoader::ScriptComponentLoader(ScriptEntityLoader& parent, int id, Eon::ComponentDefinition& def) :
	Base(parent, id, def) {
	
}

bool ScriptComponentLoader::Load() {
	TODO
	return false;
}


}
END_UPP_NAMESPACE
