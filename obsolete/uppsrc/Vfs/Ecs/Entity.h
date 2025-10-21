#ifndef _Vfs_Ecs_Entity_h_
#define _Vfs_Ecs_Entity_h_

#define COMPONENT_CONSTRUCTOR_(x) CLASSTYPE(x) x(VfsValue& n) : Component(n)
#define COMPONENT_CONSTRUCTOR(x) COMPONENT_CONSTRUCTOR_(x) {}
#define COMPONENT_OVERRIDE_TODO void Visit(Vis& s) override {TODO}
#define METANODE_EXT_CONSTRUCTOR_(x) CLASSTYPE(x) x(VfsValue& n) : VfsValueExt(n)
#define METANODE_EXT_CONSTRUCTOR(x) METANODE_EXT_CONSTRUCTOR_(x) {}

#define COMPONENT_STUB_HEADER(type) \
struct type : Component \
{ \
	COMPONENT_CONSTRUCTOR(type) \
	void Visit(Vis& v) override {v.Ver(1)(1);} \
}; \
INITIALIZE(type)

#define POLYVALUE_STUB_HEADER(type) COMPONENT_STUB_HEADER(type)

using EntityId				= int64;

struct Entity :
	VfsValueExt,
	Destroyable,
	Enableable
{
	CLASSTYPE(Entity);
	Entity(VfsValue& v);
	void Clear() {data.Clear();}
	void Visit(Vis& v) override;
	int GetGender() const;
	EntityData* FindData(const VfsPath& path);
	
	bool operator()(const Entity& a, const Entity& b) const {
		return a.data.Get("order", Value()) < b.data.Get("order", Value());
	}
	
	void SetPrefab(String s)        {prefab = s;}
	void SetCreated(int64 i)		{created = i;}
	void SetChanged(int64 i)		{changed = i;}
	String GetPrefab() const		{return prefab;}
	String GetName() const override	{return val.id;}
	String ToString() const override {return IntStr64(idx) + " " + prefab + (val.id.GetCount() ? ": " + val.id : String());}
	ComponentPtr CreateEon(String id);
	
	template<typename... ComponentTs>
	RTuple<Ptr<ComponentTs>...> CreateComponents(const WorldState& ws) {
		static_assert(AllComponents<ComponentTs...>::value, "Ts should all be a component");
		
		auto tuple = RTuple<Ptr<ComponentTs>...> {{
				Add0<ComponentTs>(ws)
			}
			...
		};
		tuple.ForEach([this,&ws](auto& comp) {comp->Initialize(ws);});
		return tuple;
	}
	
	template <class T> T* Find() {return val.Find<T>();}
	template <class T> T& Get() {
		T* o = val.Find<T>();
		ASSERT(o);
		if (!o) throw Exc("no T in owner");
		return *o;
	}
	
public:
	void                Initialize(String prefab);
	bool				InitializeComponents(const WorldState& ws);
	void				UninitializeComponents();
	void				ClearComponents();
	void                SetIdx(EntityId i) {idx = i;}
	static EntityId     GetNextIdx();
	
protected:
	friend class EntityInfoCtrl;
	friend class VNodeComponentCtrl;
	friend struct MetaEnvironment;
	friend struct VirtualNode;
	friend struct IdeMetaEnvironment;
	
	VectorMap<String, Value>		data;
	ArrayMap<VfsPath, EntityData>	objs;
	String							prefab;
	EntityId						idx = -1;
	int64							created = 0;
	int64							changed = 0;
	
	Value& Data(const String& key) {return data.GetAdd(key);}
	
private:
	template<typename T> void Remove0();
	template<typename T> Ptr<T> Add0(const WorldState& ws);
	
};

using EntityPtr = Ptr<Entity>;

INITIALIZE(Entity);


#define INITIALIZE_VALUECOMPONENT(x) \
struct x : VfsValueExt { \
	CLASSTYPE(x) \
	x(VfsValue& n) : VfsValueExt(n) {} \
	void Visit(Vis& v) override {} \
}; \
using x##Ptr = Ptr<x>; \
INITIALIZE(x)






template<typename... ComponentTs>
struct EntityPrefab {
	static_assert(AllComponents<ComponentTs...>::value, "All components should derive from Component");
	
	using Components = RTuple<Ptr<ComponentTs>...>;
	
	static String GetComponentNames() {
		return RTuple<Ptr<ComponentTs>...>::GetTypeNames();
	}
	
	static Components Make(Entity& e, const WorldState& ws) {
		return e.CreateComponents<ComponentTs...>(ws);
	}
};

template<typename PrefabT>
EntityPtr CreatePrefab(VfsValue& val, const WorldState& ws) {
	static_assert(RTupleAllComponents<typename PrefabT::Components>::value, "Prefab should have a list of Components");
	
	Entity& e = val.Add<Entity>();
	e.SetIdx(Entity::GetNextIdx());
	PrefabT::Make(e, ws);
	e.Initialize(PrefabT::GetComponentNames());
	
	return &e;
}



COMPONENT_STUB_HEADER(Context)
COMPONENT_STUB_HEADER(PkgEnv)
COMPONENT_STUB_HEADER(DbRef)
COMPONENT_STUB_HEADER(VirtualIOScript)
COMPONENT_STUB_HEADER(VirtualIOScriptProofread)
COMPONENT_STUB_HEADER(VirtualIOScriptLine)
COMPONENT_STUB_HEADER(VirtualIOScriptSub)


#endif
