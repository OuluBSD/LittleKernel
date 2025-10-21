#include "Script.h"

NAMESPACE_UPP
namespace Eon {


ScriptStateLoader::ScriptStateLoader(ScriptChainLoader& parent, int id, Eon::StateDeclaration& def):
	Base(parent, id, def)
{
	
}

String ScriptStateLoader::GetTreeString(int indent) {
	String s;
	s.Cat('\t', indent);
	s << "state " << id.ToString() << "\n";
	return s;
}

bool ScriptStateLoader::Load() {
	id = def.id;
	
	Eon::Id parent_id = parent.GetDeepId();
	LOG(id.ToString());
	LOG(parent_id.ToString());
	
	ScriptLoader& loader = GetLoader();
	
	VfsValue* l = loader.ResolveLoop(parent_id);
	
	EnvState& env = l->Add<EnvState>(id.ToString());
	
	// ready
	
	LOG("ScriptStateLoader::Load: loaded path: " << parent_id.ToString() << "  +  " << id.ToString());
	
	return true;
}

bool ScriptStateLoader::PostInitialize() {
	return true;
}


}
END_UPP_NAMESPACE
