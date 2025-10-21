#ifndef _EonLib_OdePrefab_h_
#define _EonLib_OdePrefab_h_

#if 0
#ifdef flagODE


struct StaticGroundPlane : public OdeObject, public Component {
	//RTTI_DECL2(StaticGroundPlane, OdeObject, Component)
	
	typedef StaticGroundPlane CLASSNAME;
	
	using Parent = Entity;
	
	void operator=(const StaticGroundPlane& ) {Panic("Not implemented");}
	COMP_DEF_VISIT
	
	void OnAttach() override {
		OdeObject::OnAttach();
		
		geom = dCreatePlane(GetSpace()->GetSpaceId(), 0, 1, 0, 0);
		
		is_override_phys_geom = true;
		override_geom = Identity<mat4>();
		
		ModelBuilder mb;
		mb	.AddPlane(vec3(-50, 0, -50), vec2(100, 100))
			.SetMaterial(DefaultMaterial());
		loader = mb.AsModel();
	}
	
	String ToString() override {return "StaticGroundPlane";}
};

using StaticGroundPlaneRef = Ptr<StaticGroundPlane>;

struct StaticGroundPlanePrefab :
	EntityPrefab<Transform, Renderable, StaticGroundPlane>
{
    static Components Make(Entity& e)
    {
        auto components = EntityPrefab::Make(e);
		auto ground = components.Get<StaticGroundPlanePtr>();
		
		components.Get<TransformPtr>()->position[1] = -5.0;
		components.Get<RenderablePtr>()->cb.Add(ground->GetRefreshCallback());
		
		OdeSystemPtr w = e.GetEngine().Get<OdeSystem>();
		OdeSystem& ow = CastPtr<OdeSystem>(*w);
		StaticGroundPlanePtr plane = components.Get<StaticGroundPlanePtr>();
		ASSERT(plane);
		ow.OdeNode::Attach(*plane);
		
        return components;
    }
};


#endif
#endif
#endif
