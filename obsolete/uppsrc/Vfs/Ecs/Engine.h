#ifndef _Vfs_Ecs_Engine_h_
#define _Vfs_Ecs_Engine_h_


class Engine;
class MachineVerifier;


class System :
	public VfsValueExt,
	public Destroyable,
	public Enableable
{
public:
    System();
    System(VfsValue& m);
    virtual ~System();

    virtual TypeCls GetTypeCls() const = 0;
	virtual void Visit(Vis& vis) {}
    virtual bool Arg(String key, Value value) {return true;}
	
	Engine& GetEngine() const;
};

#define SYS_CTOR(x) \
	CLASSTYPE(x) \
	x(VfsValue& m) : System(m) {}
#define SYS_CTOR_(x) \
	CLASSTYPE(x) \
	x(VfsValue& m) : System(m)
#define SYS_DEF_VISIT void Visit(Vis& vis) override {vis.VisitT<System>("System",*this);}
#define SYS_DEF_VISIT_(x) void Visit(Vis& vis) override {x; vis.VisitT<System>("System",*this);}
#define SYS_DEF_VISIT_H void Visit(Vis& vis) override;
#define SYS_DEF_VISIT_I(cls, x) void cls::Visit(Vis& vis) {x; vis.VisitT<System>("System",*this);}

#define REGISTER_SYSTEM_ATOM(x, eon, cat) VfsValueExtFactory::Register<x>(#x, VFSEXT_SYSTEM_ATOM, eon, cat);
#define REGISTER_SYSTEM_ECS(x, eon, cat) VfsValueExtFactory::Register<x>(#x, VFSEXT_SYSTEM_ECS, eon, cat);
#define REGISTER_COMPONENT(x, eon, cat) VfsValueExtFactory::Register<x>(#x, VFSEXT_COMPONENT, eon, cat);
#define REGISTER_COMPONENT_(x, str, eon, cat) VfsValueExtFactory::Register<x>(str, VFSEXT_COMPONENT, eon, cat);
#define REGISTER_EXCHANGE(x, dev, val) VfsValueExtFactory::RegisterExchange<x>(#x, dev, val);

class Engine : public VfsValueExt
{
	int64 ticks = 0;
	RunningFlagSingle main_thrd;
	
public:
	int64 GetTicks() const {return ticks;}
	
	
    template<typename SystemT>
    Ptr<SystemT> Get() {
        auto system = TryGet<SystemT>();
        ASSERT(system);
        return system;
    }

    template<typename SystemT>
    Ptr<SystemT> TryGet()
    {
        CXX2A_STATIC_ASSERT(IsSystem<SystemT>::value, "T should derive from System");
        return val.Find<SystemT>();
    }

    template<typename SystemT>
    Ptr<SystemT> Find()
    {
        CXX2A_STATIC_ASSERT(IsSystem<SystemT>::value, "T should derive from System");
        auto v = this->val.template FindAll<SystemT>();
        return v.IsEmpty() ? 0 : v[0];
    }
	
    template<typename SystemT, typename... Args>
    Ptr<SystemT> Add(Args&&... args)
    {
        CXX2A_STATIC_ASSERT(IsSystem<SystemT>::value, "T should derive from System");
        VfsValue& sub = val.Add();
        SystemT* syst = new SystemT(sub, args...);
		sub.ext = syst;
		sub.type_hash = syst->GetTypeHash();
        return syst;
    }

    template<typename SystemT, typename... Args>
    Ptr<SystemT> GetAdd(Args&&... args) {
        auto sys = val.Find<SystemT>();
        if (sys)
            return sys;
        return Add<SystemT>(args...);
    }
    
    template<typename SystemT, typename... Args>
    Ptr<SystemT> FindAdd(Args&&... args) {return GetAdd<SystemT, Args...>(args...);}
    
    
    template<typename SystemT>
    void Remove()
    {
        CXX2A_STATIC_ASSERT(IsSystem<SystemT>::value, "T should derive from System");

        ASSERT(is_initialized && is_started);
        for(int i = 0; i < val.sub.GetCount(); i++) {
			auto& n = val.sub[i];
			if (n.ext) {
				SystemT* t = CastPtr<SystemT>(&*n.ext);
				if (t) {
					val.sub.Remove(i--);
				}
			}
        }
    }

	CLASSTYPE(Engine)
    Engine(VfsValue& n);
    virtual ~Engine();

    bool IsStarted() const;

    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
	void Visit(Vis& vis) override;
    
	bool StartLoad(String app_name, String override_eon_file="", ValueMap extra_args=ValueMap(), const char* extra_str=0);
	bool StartMain(String script_content, String script_file, ValueMap args, bool dbg_ref_visits=false, uint64 dbg_ref=0);
    void Suspend();
    void Resume();
    void StartMainLoop();
    void MainLoop();
    void DieFast() {Start(); Update(0); Stop();}
	void Clear() {ticks=0; is_started=0; is_initialized=0; is_suspended=0; /*systems.Clear();*/}
	void ClearCallbacks();
	
    bool IsRunning() const {return main_thrd.IsRunning();}
	void SetNotRunning() {main_thrd.SetNotRunning();}
	void SetFailed(String msg="") {is_failed = true; fail_msg = msg;}
	void WarnDeveloper(String msg);
	void StopRunner();
	void MainLoop(bool (*fn)(void*), void* arg);
	//void Main(String script_content, String script_file, Value args, MachineVerifier* ver, bool dbg_ref_visits=false, uint64 dbg_ref=0);
	
	Ptr<System> Add(TypeCls type, bool startup=true);
	Ptr<System> GetAdd(String id, bool startup=true);
    
    Val& GetRootPool();
    Val& GetRootSpace();
    Val& GetRootLoop();
    
	bool CommandLineInitializer(bool skip_eon_file);
    
public:
	Event<> WhenEnterUpdate;
	Event<System&> WhenEnterSystemUpdate;
	
	Event<> WhenLeaveUpdate;
	Event<> WhenLeaveSystemUpdate;
	
	Event<Engine&> WhenGuiProgram;
	Event<Engine&> WhenInitialize;
	Event<Engine&> WhenPreFirstUpdate;
	Event<Engine&> WhenPostInitialize;
	Event<Engine&> WhenBoot;
	Event<Engine&> WhenUserInitialize;
	Event<Engine&> WhenUserProgram;
	
	//EntitySystem& GetEntitySystem() {ASSERT(sys); return *sys;}
	
	
private:
    //using SystemCollection = ArrayMap<TypeCls, System>;
    //SystemCollection systems;
	
	bool is_started = false;
    bool is_initialized = false;
    bool is_suspended = false;
    bool is_looping_systems = false;
    bool is_failed = false;
    String fail_msg;
	ValueMap eon_params;
	String eon_script;
	String eon_file;
	uint64 break_addr = 0;
	Index<String> last_warnings;
	double warning_age = 0;
	WorldState ws;
    
    int FindSystem(TypeCls type_id);// {return systems.Find(type_id);}
    void Add(TypeCls type_id, System* system, bool startup=true);
    void Remove(TypeCls typeId);
    
    Vector<VfsValueExtPtr> update_list;
    ArrayMap<String,Vector<VfsValueExtPtr>> named_update_lists;
    
    static VectorMap<String,Event<VfsValueExt&>>& NameUpdaters();
    
public:
    MachineVerifier* mver = 0;
	
	// Moved AtomSystem here
    void AddUpdated(VfsValueExt* c);
    void RemoveUpdated(VfsValueExt* c);
    void AddUpdated(String name, VfsValueExt* c);
    void RemoveUpdated(String name, VfsValueExt* c);
	
	static void AddNameUpdater(String name, Event<VfsValueExt&> update_fn);
	static Engine& Setup(String name, Engine* e=0, bool in_thrd=false);
	static void Uninstall(bool clear_root=true, Engine* e=0);
	static void PostCallback(Event<> cb);
	
private:
	Vector<AtomBasePtr> updated;
	
	struct StaticEngine {
		Vector<Event<>> callbacks;
		Mutex lock;
	};
	static StaticEngine& GetStatic();
	static void RunCallbacks();
protected:
	friend struct Eon::ExtScriptEcsLoader;
	
    void SystemStartup(TypeCls type_id, System* system);
    
    
};

using EnginePtr = Ptr<Engine>;

template <class S>
inline void Component::AddToSystem() {
	S* sys = GetEngine().Get<S>();
	ASSERT(sys);
	if (sys)
		sys->Add(*this);
}

template <class S>
inline void Component::RemoveFromSystem() {
	S* sys = GetEngine().Get<S>();
	ASSERT(sys);
	if (sys)
		sys->Remove(*this);
}

template<typename T>
void Entity::Remove0() {
	for(int i = 0; i < val.sub.GetCount(); i++) {
		auto& n = val.sub[i];
		if (n.ext) {
			T* t = CastPtr<T>(&*n.ext);
			if (t) {
				val.sub.Remove(i--);
			}
		}
    }
}

template<typename T>
Ptr<T> Entity::Add0(const WorldState& ws) {
	VfsValue& sub = val.Add();
    T* comp = new T(sub);
	sub.ext = comp;
	sub.type_hash = comp->GetTypeHash();
    if (!comp->Initialize(ws))
        return 0;
	comp->SetInitialized();
	ASSERT(comp->GetEntity());
	return comp;
}


void MachineEcsInit(Engine&); // todo rename


template <class S>
inline void AtomBase::AddToSystem() {
	auto eng = val.FindOwner<Engine>();
	ASSERT(eng);
	if (!eng) return;
	Ptr<S> sys = eng->Get<S>();
	ASSERT(sys);
	if (sys)
		sys->Add(*this);
}

template <class S>
inline void AtomBase::RemoveFromSystem() {
	auto eng = val.FindOwner<Engine>();
	ASSERT(eng);
	if (!eng) return;
	Ptr<S> sys = eng->Get<S>();
	ASSERT(sys);
	if (sys)
		sys->Remove(*this);
}


#endif
