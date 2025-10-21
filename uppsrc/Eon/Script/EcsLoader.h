#ifndef _Eon_Script_EcsLoader_h_
#define _Eon_Script_EcsLoader_h_


namespace Eon {


class ScriptWorldLoader;
class ScriptPoolLoader;
class ScriptEntityLoader;


template <class ParserDef, class LoaderParent>
class EcsLoaderBase {
	
public:
	virtual ~EcsLoaderBase() {}
	
	LoaderParent&				parent;
	ParserDef&					def;
	String						err_str;
	int							id = -1;
	
	EcsLoaderBase(LoaderParent& parent, int id, ParserDef& def) : parent(parent), id(id), def(def){}
	
	virtual bool		Load() = 0;
	virtual void		Visit(Vis& vis) = 0;
	virtual String		GetTreeString(int indent) {TODO; return String();}
	void				SetError(String s) {err_str = s;}
	String				GetErrorString() const {return err_str;}
	
};


class ScriptEcsSystemLoader : public EcsLoaderBase<Eon::EcsSysDefinition, ScriptWorldLoader> {
public:
	using Base = EcsLoaderBase<Eon::EcsSysDefinition, ScriptWorldLoader>;
	
	ArrayMap<String, Value> args;
	
public:
	
	
	ScriptEcsSystemLoader(ScriptWorldLoader& parent, int id, Eon::EcsSysDefinition& def);
	void		Visit(Vis& vis) override {}
	bool		Load() override;
	
};

class ScriptComponentLoader : public EcsLoaderBase<Eon::ComponentDefinition, ScriptEntityLoader> {
public:
	using Base = EcsLoaderBase<Eon::ComponentDefinition, ScriptEntityLoader>;
	
public:
	
	
	ScriptComponentLoader(ScriptEntityLoader& parent, int id, Eon::ComponentDefinition& def);
	void		Visit(Vis& vis) override {}
	bool		Load() override;
	
};

class ScriptEntityLoader : public EcsLoaderBase<Eon::EntityDefinition, ScriptPoolLoader> {
public:
	using Base = EcsLoaderBase<Eon::EntityDefinition, ScriptPoolLoader>;
	
public:
	Array<ScriptComponentLoader>	comps;
	
	
	ScriptEntityLoader(ScriptPoolLoader& parent, int id, Eon::EntityDefinition& def);
	void		Visit(Vis& vis) override {vis || comps;}
	bool		Load() override;
	
};

class ScriptPoolLoader : public EcsLoaderBase<Eon::PoolDefinition, ScriptWorldLoader> {
public:
	using Base = EcsLoaderBase<Eon::PoolDefinition, ScriptWorldLoader>;
	
public:
	
	Array<ScriptEntityLoader>		entities;
	Array<ScriptPoolLoader>			pools;
	ScriptPoolLoader*				chain_parent;
	bool							use_subpools = false;
	
	
	ScriptPoolLoader(ScriptWorldLoader& parent, ScriptPoolLoader* chain_parent, int id, Eon::PoolDefinition& def);
	
	void		Visit(Vis& vis) override {vis || entities || pools;}
	bool		Load() override;
	
};

class ScriptWorldLoader : public EcsLoaderBase<Eon::WorldDefinition, ScriptSystemLoader> {
public:
	using Base = EcsLoaderBase<Eon::WorldDefinition, ScriptSystemLoader>;
	
public:
	Array<ScriptEcsSystemLoader>	systems;
	Array<ScriptPoolLoader>			pools;
	
	
	ScriptWorldLoader(ScriptSystemLoader& parent, int id, Eon::WorldDefinition& def);
	void		Visit(Vis& vis) override {vis || systems || pools;}
	String		GetTreeString(int indent) override;
	bool		Load() override;
	
};


struct ExtScriptEcsLoaderBase {
	String err_str;
	
	virtual ~ExtScriptEcsLoaderBase() {}
	
	virtual bool Load(ScriptWorldLoader& l) = 0;
	virtual void Clear() {}
	
	void SetError(String s) {err_str = s;}
	
	String GetErrorString() const {return err_str;}
	
};


extern ExtScriptEcsLoaderBase* __ecs_script_loader;


}


#endif
