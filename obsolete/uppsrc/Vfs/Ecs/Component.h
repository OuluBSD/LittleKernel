#ifndef _Vfs_Ecs_Component_h_
#define _Vfs_Ecs_Component_h_


struct DatasetPtrs;
class Engine;


struct DatasetProvider {
	virtual void GetDataset(DatasetPtrs& p) const = 0;
};

struct Component :
	VfsValueExt,
	Destroyable,
	Enableable,
	DatasetProvider
{
	Component(VfsValue& owner) : VfsValueExt(owner) {}
	
	void GetDataset(DatasetPtrs& p) const override;
	String ToString() const override;
	
	Engine& GetEngine();
	Entity* GetEntity();
	virtual void Visit(Vis& vis) override;
	
	void AddToUpdateList();
	void RemoveFromUpdateList();
	template <class S> void AddToSystem();
	template <class S> void RemoveFromSystem();
	
	
	template <class T> T& Get() {
		ASSERT(val.owner);
		if (!val.owner) throw Exc("no owner");
		T* o = val.owner->Find<T>();
		ASSERT(o);
		if (!o) throw Exc("no T in owner");
		return *o;
	}
	
};

using ComponentPtr = Ptr<Component>;


#define ECS_COMPONENT_CTOR_(x) \
	CLASSTYPE(x) \
	x(VfsValue& e) : Component(e)
#define ECS_COMPONENT_CTOR(x) ECS_COMPONENT_CTOR_(x) {}

#define VISIT_COMPONENT


#endif
