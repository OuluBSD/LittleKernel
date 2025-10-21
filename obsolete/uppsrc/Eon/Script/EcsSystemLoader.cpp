#include "Script.h"

NAMESPACE_UPP
namespace Eon {

ScriptEcsSystemLoader::ScriptEcsSystemLoader(ScriptWorldLoader& parent, int id, Eon::EcsSysDefinition& def) :
	Base(parent, id, def) {
	
}

bool ScriptEcsSystemLoader::Load() {
	TODO
	return false;
}

}
END_UPP_NAMESPACE
