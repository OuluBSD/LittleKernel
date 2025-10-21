#ifndef _Eon_Script_EonLoader_h_
#define _Eon_Script_EonLoader_h_

namespace Eon {

struct ExtScriptEcsLoader : Eon::ExtScriptEcsLoaderBase {
	
	
	
	bool Load(ScriptWorldLoader& l) override;
	bool Load(ScriptEcsSystemLoader& l, System& sys);
	bool Load(ScriptPoolLoader& l, VfsValue& pool);
	bool Load(ScriptEntityLoader& l, Entity& ent);
	bool Load(ScriptComponentLoader& l, Component& ent);
	
};

}

#endif
