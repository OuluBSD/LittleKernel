#ifndef _EonLib_TPrefab_h_
#define _EonLib_TPrefab_h_


template <class Fys>
struct StaticGroundPlane :
	public Component,
	public Fys::Object
{
	using Object = typename Fys::Object;
	//RTTI_DECL2(StaticGroundPlane, Object, Component)
	
	typedef StaticGroundPlane CLASSNAME;
	
	using Parent = Entity;
	
	void operator=(const StaticGroundPlane& ) {Panic("Not implemented");}
	COMP_DEF_VISIT
	
	void OnAttach() override {
		Object::OnAttach();
		
		Fys::SetGeomModelPlane(this->geom, this->GetSystem()->GetSpace().GetNative());
		
		this->is_override_phys_geom = true;
		this->override_geom = Identity<mat4>();
		
		TODO
		ModelBuilder mb;
		mb	.AddPlane(vec3(-50, 0, -50), vec2(100, 100))
			/*.SetMaterial(DefaultMaterial())*/
			;
		this->loader = mb.AsModel();
	}
	
	String ToString() const override {return "StaticGroundPlane";}
};

template <class Fys>
struct StaticGroundPlanePrefab :
	EntityPrefab<Transform, Renderable, StaticGroundPlane<Fys>>
{
	using GroundPlane = StaticGroundPlane<Fys>;
	using Prefab = EntityPrefab<Transform, Renderable, GroundPlane>;
	using Components = typename Prefab::Components;
	using System = typename Fys::System;
	
    static Components Make(Entity& e)
    {
		Engine* mach = e.val.FindOwner<Engine>();
		if (!mach) return Components();
        auto components = Prefab::Make(e);
		auto ground = components.template Get<Ptr<GroundPlane>>();
		
		components.template Get<TransformPtr>()->position[1] = -5.0;
		components.template Get<RenderablePtr>()->cb.Add(ground->GetRefreshCallback());
		
		Ptr<System> w = mach->val.Find<System>();
		Ptr<GroundPlane> plane = components.template Get<Ptr<GroundPlane>>();
		ASSERT(plane);
		w->Attach(*plane);
		
        return components;
    }
};

template <class Fys>
struct StaticBox : public Fys::Object {
	using Object = typename Fys::Object;
	double width = 1.0, height = 1.0, length = 1.0;
	
	ATOM_CTOR_(StaticBox, Object)
	StaticBox& Set(double w, double h, double l) {width=w; height=h; length=l; return *this;}
	
	void OnAttach() override;
	
	String ToString() override {return "StaticBox";}
};


#endif
