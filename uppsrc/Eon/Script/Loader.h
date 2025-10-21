#ifndef _Eon_Script_Loader_h_
#define _Eon_Script_Loader_h_



namespace Eon {
using namespace Upp;

class Action;
class ScriptLoopLoader;
class ScriptStateLoader;
class ScriptSystemLoader;
class ScriptTopChainLoader;
class ScriptChainLoader;
class ScriptMachineLoader;
class ScriptWorldLoader;
class ScriptLoader;

void GetAtomActions(const WorldState& src, Vector<Eon::Action>& acts);



template <class ParserDef, class LoaderParent>
class ScriptLoaderBase {
	
	
protected:
	
	int							iter = 0;
	
	
public:
	virtual ~ScriptLoaderBase() {}
	
	LoaderParent&				parent;
	ParserDef&					def;
	String						err_str;
	int							id = -1;
	
	ScriptLoaderBase(LoaderParent& parent, int id, ParserDef& def) : parent(parent), id(id), def(def){}
	int					GetId() const {return id;}
	ScriptLoader&		GetLoader() {return parent.GetLoader();}
	String				GetErrorString() const {return err_str;}
	void				AddError(const FileLocation& loc, String msg) {parent.AddError(loc, msg);}
	
	virtual Eon::Id	GetDeepId() const {Eon::Id id = parent.GetDeepId(); id.Append(def.id); return id;}
	
	virtual bool		Load() = 0;
	virtual void		Visit(Vis& vis) = 0;
	virtual String		GetTreeString(int indent) = 0;
	virtual void		GetLoops(Vector<ScriptLoopLoader*>& v) = 0;
	virtual void		GetStates(Vector<ScriptStateLoader*>& v) {Panic("not implemented");}
	
};





class ScriptLoopLoader : public ScriptLoaderBase<Eon::LoopDefinition, ScriptChainLoader> {
	
protected:
	
	// Implement found loop
	struct AddedAtom {
		AtomBasePtr		a;
		LinkBasePtr		l;
		IfaceConnTuple	iface;
	};
	
	Array<AddedAtom>		added_atoms;
	
public:
	using Base = ScriptLoaderBase<Eon::LoopDefinition, ScriptChainLoader>;
	
	
protected:
	friend class ScriptLoader;
	friend class ScriptConnectionSolver;
	friend class ::Upp::MachineVerifier;
	
	struct SideLink : Moveable<SideLink> {
		ScriptLoopLoader*	link = 0;
		ValDevCls			vd;
		bool				is_user_conditional = false;
		bool				is_user_stmt = false;
		bool				is_required = false;
	};
	
	struct AtomSideLinks : Moveable<AtomSideLinks> {
		Vector<SideLink>	src_side_conns;
		Vector<SideLink>	sink_side_conns;
		AtomTypeCls			type;
		
	};
	
	Array<AtomBasePtr>				atoms;
	Vector<AtomSideLinks>			atom_links;
	
	
	void SetSideSourceConnected(const AtomTypeCls& type, int ch_i, ScriptLoopLoader& sink);
	void SetSideSinkConnected(const AtomTypeCls& type, int ch_i, ScriptLoopLoader& src);
	bool IsAllSidesConnected() const;
	bool IsTopSidesConnected() const;
	
	
public:
	ScriptLoopLoader(ScriptChainLoader& parent, int id, Eon::LoopDefinition& def);
	
	
	void		Forward();
	void		InitSegments();
	void		SearchNewSegment();
	void		PruneSegmentGoals();
	void		DumpLoop();
	
	
	bool		Start();
	bool		Parse();
	bool		PostInitialize();
	void		UpdateLoopLimits();
	void		UndoLoad();
	int			GetAtomLinkCount() const {return atom_links.GetCount();}
	static AtomBasePtr AddAtomTypeCls(VfsValue& val, AtomTypeCls cls);
	static LinkBasePtr AddLinkTypeCls(VfsValue& val, LinkTypeCls cls);
	static bool MakeLink(VfsValue& val, AtomBasePtr src_atom, AtomBasePtr dst_atom);
	
	bool		Load() override;
	void		Visit(Vis& vis) override {vis || atoms;}
	String		GetTreeString(int indent) override;
	void		GetLoops(Vector<ScriptLoopLoader*>& v) override;
};

class ScriptChainLoader : public ScriptLoaderBase<Eon::ChainDefinition, ScriptTopChainLoader> {
	
public:
	using Base = ScriptLoaderBase<Eon::ChainDefinition, ScriptTopChainLoader>;
	
public:
	Array<ScriptLoopLoader>			loops;
	Array<ScriptStateLoader>		states;
	
	
	ScriptChainLoader(ScriptTopChainLoader& parent, int id, Eon::ChainDefinition& def);
	
	void		Forward();
	void		MakeOptionLinkVector();
	void		FindAcceptedLinks();
	void		LinkPlanner();
	void		Linker();
	
	void		Visit(Vis& vis) override {vis || loops || states;}
	String		GetTreeString(int indent) override;
	void		GetLoops(Vector<ScriptLoopLoader*>& v) override;
	void		GetStates(Vector<ScriptStateLoader*>& v) override;
	Eon::Id	GetDeepId() const override;
	bool		Load() override;
	
};


class ScriptTopChainLoader : public ScriptLoaderBase<Eon::ChainDefinition, ScriptMachineLoader> {
public:
	using Base = ScriptLoaderBase<Eon::ChainDefinition, ScriptMachineLoader>;
	
public:
	enum {NORMAL, SPLITTED_CHAIN, SPLITTED_LOOPS};
	
	Array<ScriptChainLoader>		chains;
	Array<ScriptTopChainLoader>		subchains;
	ScriptTopChainLoader*			chain_parent;
	bool							use_subchains = false;
	
	
	ScriptTopChainLoader(int mode, ScriptMachineLoader& parent, ScriptTopChainLoader* chain_parent, int id, Eon::ChainDefinition& def);
	
	void		Visit(Vis& vis) override {vis || subchains || chains;}
	String		GetTreeString(int indent) override;
	void		GetLoops(Vector<ScriptLoopLoader*>& v) override;
	void		GetStates(Vector<ScriptStateLoader*>& v) override;
	bool		Load() override;
	
};

class ScriptStateLoader : public ScriptLoaderBase<Eon::StateDeclaration, ScriptChainLoader> {
	
protected:
	Eon::Id		id;
	
public:
	using Base = ScriptLoaderBase<Eon::StateDeclaration, ScriptChainLoader>;
	
public:
	
	
	ScriptStateLoader(ScriptChainLoader& parent, int id, Eon::StateDeclaration& def);
	void		Visit(Vis& vis) override {}
	void		GetLoops(Vector<ScriptLoopLoader*>& v) override {}
	void		GetStates(Vector<ScriptStateLoader*>& v) override {}
	String		GetTreeString(int indent) override;
	bool		Load() override;
	void		Forward() {}
	bool		PostInitialize();
	
};

class ScriptMachineLoader : public ScriptLoaderBase<Eon::MachineDefinition, ScriptSystemLoader> {
public:
	using Base = ScriptLoaderBase<Eon::MachineDefinition, ScriptSystemLoader>;
	
public:
	Array<ScriptTopChainLoader>		chains;
	
	
	ScriptMachineLoader(ScriptSystemLoader& parent, int id, Eon::MachineDefinition& def);
	void		Visit(Vis& vis) override {vis || chains;}
	bool		Load() override;
	String		GetTreeString(int indent) override;
	void		GetLoops(Vector<ScriptLoopLoader*>& v) override;
	void		GetStates(Vector<ScriptStateLoader*>& v) override;
	
};

class ScriptSystemLoader : public ScriptLoaderBase<Eon::GlobalScope, ScriptLoader> {
public:
	using Base = ScriptLoaderBase<Eon::GlobalScope, ScriptLoader>;
	
public:
	Array<ScriptMachineLoader>		machs;
	Array<ScriptWorldLoader>		worlds;
	
	
	ScriptSystemLoader(ScriptLoader& parent, int id, Eon::GlobalScope& glob);
	
	void		Visit(Vis& vis) override {vis || machs || worlds;}
	String		GetTreeString(int indent=0) override;
	void		GetLoops(Vector<ScriptLoopLoader*>& v) override;
	void		GetStates(Vector<ScriptStateLoader*>& v) override;
	bool		Load() override;
	bool		LoadEcs();
	Eon::Id		GetDeepId() const override {return Eon::Id();}
	
	void		Dump() {LOG(GetTreeString(0));}
	
};



class ScriptLoader :
	public System,
	public ErrorSource
{
protected:
	friend class ScriptLoopLoader;
	friend class ScriptChainLoader;
	friend class ScriptTopChainLoader;
	friend class ScriptMachineLoader;
	static int loop_counter;
	
	One<ScriptSystemLoader> loader;
	
	int tmp_side_id_counter = 0;
	
	Vector<String> post_load_file;
	Vector<String> post_load_string;
	//LoopStorePtr es;
	//SpaceStorePtr ss;
    Eon::CompilationUnit cunit;
    bool collect_errors = false;
    bool eager_build_chains = false;
    Array<One<ChainContext>> built_chains;
	
	bool GetPathId(Eon::Id& script_id, AstNode* from, AstNode* to);
	
public:
	CLASSTYPE(ScriptLoader);
	ScriptLoader(VfsValue& m);
	void Visit(Vis& v) override;

	void PostLoadFile(const String& path) {post_load_file << path;}
    void PostLoadString(const String& s) {post_load_string << s;}
    void SetEagerChainBuild(bool b=true) { eager_build_chains = b; }
	
	ScriptLoader&	GetLoader() {return *this;}
	int&			GetSideIdCounter() {return tmp_side_id_counter;}
	int				NewConnectionId() {return tmp_side_id_counter++;}
	void			AddError(const FileLocation& loc, String msg) {ErrorSource::AddError(loc, msg);}
	bool			LoadAst(AstNode* root);
	// Non-AST path: build a chain directly from a prepared definition using Core contexts
	bool			BuildChain(const Eon::ChainDefinition& chain);
	
	
protected:
	
	~ScriptLoader();
    bool		Initialize(const WorldState& ws) override;
    bool		PostInitialize() override;
    void		Update(double dt) override;
    void		Uninitialize() override;
    
    void		LogMessage(ProcMsg msg);
    void		Cleanup();
    bool		DoPostLoad();
	bool		LoadFile(String path);
	bool		Load(const String& content, const String& filepath="temp");
	bool		ConnectSides(ScriptLoopLoader& loop0, ScriptLoopLoader& loop1);
	bool		ImplementScript();
	
	bool		LoadCompilationUnit(AstNode* root);
	bool		LoadGlobalScope(Eon::GlobalScope& glob, AstNode* root);
	bool		LoadChain(Eon::ChainDefinition& chain, AstNode* root);
	bool		LoadMachine(Eon::MachineDefinition& mach, AstNode* root);
	bool		LoadWorld(Eon::WorldDefinition& def, AstNode* n);
	bool		LoadDriver(Eon::DriverDefinition& def, AstNode* n);
	bool		LoadTopChain(Eon::ChainDefinition& def, AstNode* n);
	bool		LoadEcsSystem(Eon::EcsSysDefinition& def, AstNode* n);
	bool		LoadPool(Eon::PoolDefinition& def, AstNode* n);
	bool		LoadTopPool(Eon::PoolDefinition& def, AstNode* n);
	bool		LoadState(Eon::StateDeclaration& def, AstNode* n);
	bool		LoadEntity(Eon::EntityDefinition& def, AstNode* n);
	bool		LoadComponent(Eon::ComponentDefinition& def, AstNode* n);
	bool		LoadArguments(ArrayMap<String, Value>& args, AstNode* n);
	
	
	
protected:
	friend class Eon::ScriptStateLoader;
	
	VfsValue*	ResolveLoop(Eon::Id& id);
	
	
public:
	
	
	Callback1<System&>	WhenEnterScriptLoad;
	Callback				WhenLeaveScriptLoad;
	
	Eon::Id	GetDeepId() const {return Eon::Id();}
	
};

using ScriptLoaderPtr = Ptr<ScriptLoader>;


}

#endif
