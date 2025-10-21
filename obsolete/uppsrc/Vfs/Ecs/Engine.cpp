#include "Ecs.h"

// Keeping 3 separate Engine files for historical git-log reasons

NAMESPACE_UPP

Engine::Engine(VfsValue& n) : VfsValueExt(n) {
	DBG_CONSTRUCT
}

Engine::~Engine() {
	ASSERT_(!is_initialized && !is_started, "Engine should be in a clean state upon destruction");
	DBG_DESTRUCT
}


void Engine::ClearCallbacks() {
	WhenEnterUpdate.Clear();
	WhenLeaveUpdate.Clear();
	WhenEnterSystemUpdate.Clear();
	WhenLeaveSystemUpdate.Clear();
	WhenGuiProgram.Clear();
	WhenUserProgram.Clear();
	WhenInitialize.Clear();
	WhenPreFirstUpdate.Clear();
	WhenPostInitialize.Clear();
	WhenBoot.Clear();
	WhenUserInitialize.Clear();
}

bool Engine::Start() {
	ASSERT_(!is_initialized && !is_started, "Shouldn't call Start if we already started");
	
	WhenInitialize(*this);
	
	is_looping_systems = true;
	
	RunCallbacks();
	
	Vector<VfsValueExt*> post_init_exts;
	auto systems = val.FindAll<System>();
	for (auto it : systems) {
		if (it->IsInitialized())
			continue;
		if (!it->Initialize(ws)) {
			LOG("Could not initialize system " << it->GetTypeCls().GetName());
			return false;
		}
		it->SetInitialized();
		post_init_exts << it;
	}
	
	RunCallbacks();
	
	// Post-initialization is like 2nd round of init (when there's need for a two-phase init)
	for (auto it : post_init_exts) {
		if (!it->PostInitialize()) {
			LOG("Could not post-initialize system " << it->GetTypeCls().GetName());
			return false;
		}
	}
	
	RunCallbacks();
	
	is_initialized = true;
	
	for (auto it : systems) {
		if (!it->Start()) {
			SetFailed("System could not start: " + it->GetTypeName());
			return false;
		}
	}
	
	RunCallbacks();
	
	is_looping_systems = false;
	
	is_started = true;
	
	return true;
}

void Engine::Update(double dt) {
	ASSERT_(is_started, "Shouldn't call Update if we haven't been started");
	
	if (!ticks)
		WhenPreFirstUpdate(*this);
	
	WhenEnterUpdate();
	
	if (dt <= 0.0) {
		WhenLeaveUpdate();
		return;
	}
	if (dt > 0.5)
		dt = 0.5;
		
	if (is_suspended) {
		WhenLeaveUpdate();
		return;
	}
	
	RunCallbacks();
	
	for (auto& c : update_list) {
		if (c)
			c->Update(dt);
	}
	
	for (auto it : ~NameUpdaters()) {
		int i = named_update_lists.Find(it.key);
		if (i < 0)
			continue;
		auto& list = named_update_lists[i];
		for (auto& e : list) {
			if (e)
				it.value(*e);
		}
	}
	
	RunCallbacks();
	
	is_looping_systems = true;
	
	auto systems = val.FindAll<System>();
	for (System* b : systems) {
		WhenEnterSystemUpdate(*b);
		
		b->Update(dt);
		
		WhenLeaveSystemUpdate();
	}
	
	RunCallbacks();
	
	is_looping_systems = false;
	
	++ticks;
	
	WhenLeaveUpdate();
}

void Engine::Stop() {
	if (!is_started)
		return;
	
	main_thrd.Stop();
	
	is_started = false;
	
	DBG_BEGIN_UNREF_CHECK
	
	is_looping_systems = true;
	
	
	RunCallbacks();
	
	// Stop components
	for (VfsValue::IteratorDeep v = val.BeginDeep(); v; v++) {
		Component* comp = v->ext ? CastPtr<Component>(&*v->ext) : 0;
		if (comp)
			comp->Stop();
	}
	
	RunCallbacks();
	
	// Stop atoms
	for (VfsValue::IteratorDeep v = val.BeginDeep(); v; v++) {
		AtomBase* atom = v->ext ? CastPtr<AtomBase>(&*v->ext) : 0;
		if (atom)
			atom->Stop();
	}
	
	RunCallbacks();
	
	// Stop systems
	auto systems = val.FindAll<System>();
	for (auto it = systems.End()-1; it != systems.Begin()-1; --it) {
		(*it)->Stop();
	}
	
	is_initialized = false;
	
	RunCallbacks();
	
	for (auto it = systems.End()-1; it != systems.Begin()-1; --it) {
		if ((*it)->IsInitialized()) {
			(*it)->Uninitialize();
			(*it)->SetInitialized(false);
		}
	}
	
	is_looping_systems = false;
}

void Engine::Suspend() {
	is_suspended = true;
}

void Engine::Resume() {
	is_suspended = false;
}

bool Engine::IsStarted() const {
	return is_started;
}

void Engine::SystemStartup(TypeCls type_id, System* system) {
	if (system->IsInitialized() || (system->Initialize(ws) && system->PostInitialize())) {
		
		system->SetInitialized();
		
		RTLOG("Engine::SystemStartup: added system to already running engine: " << system->GetTypeCls().GetName());
		
		bool has_already = false;
		auto systems = val.FindAll<System>();
		for (auto r : systems)
			if (&*r == system)
				has_already = true;
		if (!has_already) {
			Panic("error"); // previously system was added to systems list here, but now it has owner already
		}
		system->Start();
	}
	else {
		RTLOG("Engine::SystemStartup: error: could not initialize system in already running engine: " << system->GetTypeCls().GetName());
		delete system;
	}
}

void Engine::Visit(Vis& vis) {
	vis && update_list;
	vis && named_update_lists;
}

Ptr<System> Engine::Add(TypeCls type, bool startup)
{
	int i = VfsValueExtFactory::FindTypeClsFactory(type);
	ASSERT(i >= 0);
	if (i < 0)
		return 0;
	hash_t type_hash = VfsValueExtFactory::List()[i].type_hash;
	VfsValue& new_val = val.Add(type.GetName(), type_hash);
	System* syst = new_val.FindExt<System>();
	ASSERT(syst);
	
	if (syst && startup && is_started)
		SystemStartup(type, syst);
	
    return syst;
}

Ptr<System> Engine::GetAdd(String id, bool startup) {
    int i = VfsValueExtFactory::EonToType().Find(id);
    if (i < 0)
        return Ptr<System>();
    TypeCls type = VfsValueExtFactory::EonToType()[i];
    auto v = val.FindAll(type);
    if (v.GetCount())
        return v[0]->ext ? CastPtr<System>(&*v[0]->ext) : 0;
    return Add(type, startup);
}






VectorMap<String,Event<VfsValueExt&>>& Engine::NameUpdaters() {
	static VectorMap<String,Event<VfsValueExt&>> m;
	return m;
}


void Engine::AddUpdated(VfsValueExt* c) {
	VfsValueExtPtr p = c;
	VectorFindAdd(update_list, p);
}

void Engine::RemoveUpdated( VfsValueExt* c) {
	VfsValueExtPtr p = c;
	VectorRemoveKey(update_list, p);
}

void Engine::AddUpdated(String name, VfsValueExt* c) {
	VfsValueExtPtr p = c;
	VectorFindAdd(named_update_lists.GetAdd(name), p);
}

void Engine::RemoveUpdated(String name, VfsValueExt* c) {
	VfsValueExtPtr p = c;
	VectorRemoveKey(named_update_lists.GetAdd(name), p);
}

void Engine::AddNameUpdater(String name, Event<VfsValueExt&> update_fn) {
	ASSERT(NameUpdaters().Find(name) < 0);
	NameUpdaters().GetAdd(name) = update_fn;
}

Engine& Engine::Setup(String name, Engine* e, bool in_thrd) {
	auto& root = MetaEnv().root;
	Engine& eng = e ? *e : root.GetAdd<Engine>("eng");
	if (eng.IsRunning())
		return eng;
	
	eng.WhenPreFirstUpdate << callback(DefaultStartup);
	eng.StartLoad(name);
	
	if (in_thrd)
		eng.StartMainLoop();
	
	return eng;
}

void Engine::Uninstall(bool clear_root, Engine* e) {
	auto& root = MetaEnv().root;
	Engine& eng = e ? *e : root.GetAdd<Engine>("eng");
	
	eng.Stop();
	eng.Clear();
	
	auto& rm = clear_root ? root : eng.val;
	rm.StopDeep();
	rm.ClearDependenciesDeep();
	rm.UninitializeDeep();
	rm.ClearExtDeep();
	rm.sub.Clear();
}

void Engine::PostCallback(Event<> cb) {
	StaticEngine& s = GetStatic();
	s.lock.Enter();
	s.callbacks << cb;
	s.lock.Leave();
}

Engine::StaticEngine& Engine::GetStatic() {
	static StaticEngine s;
	return s;
}

void Engine::RunCallbacks() {
	StaticEngine& s = GetStatic();
	if (s.callbacks.IsEmpty())
		return;
	Vector<Event<>> callbacks;
	s.lock.Enter();
	Swap(callbacks, s.callbacks);
	s.lock.Leave();
	for (auto& cb : callbacks)
		cb();
}

END_UPP_NAMESPACE

