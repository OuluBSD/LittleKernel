#pragma once

#if 1


// ToolSystemBase
// Base abstract class for all ToolSystems
class ToolSystemBase : public System {
public:
	using System::System;
	
	void Visit(Vis& vis) override {}
	
	virtual String GetInstructions() const = 0;
	virtual String GetDisplayName() const = 0;
	
	virtual EntityPtr CreateToolSelector() const = 0;
	
	virtual void Register() = 0;
	virtual void Unregister() = 0;
	virtual void Activate(EntityPtr entity) = 0;
	virtual void Deactivate(EntityPtr entity) = 0;
};

using ToolSystemBasePtr = Ptr<ToolSystemBase>;

class ToolSelectorKey :
	public Component {
	
public:
	ECS_COMPONENT_CTOR(ToolSelectorKey)
	
	TypeCls type { AsVoidTypeCls() };
	
	void Visit(Vis& v) override;
	
};

struct ToolSelectorPrefab :
	EntityPrefab <
	Transform,
	ModelComponent,
	ToolSelectorKey,
	RigidBody,
	Easing
	> {
	
	static Components Make(Entity& e, const WorldState& ws) {
		Components components = EntityPrefab::Make(e, ws);
		RigidBodyPtr rb = components.Get<RigidBodyPtr>();
		EasingPtr ea = components.Get<EasingPtr>();
		rb->angular_velocity = { 0.0f, -3.0f, 0.0f }; // Spin in place
		rb->damping_factor = 1.0f;
		ea->position_easing_factor = 0.1f;
		return components;
	}
};

template<typename T, typename ToolComponent>
class ToolSystemBaseT :
	public ToolSystemBase,
	public InteractionListener
{
	
public:
	typedef ToolSystemBaseT<T, ToolComponent> CLASSNAME;
	ToolSystemBaseT(VfsValue& n) : ToolSystemBase(n) {}
	void Visit(Vis& v) override {VIS_THIS(ToolSystemBase); /*vis && m_entities;*/}
	TypeCls GetTypeCls() const override {return AsTypeCls<T>();}
	
protected:
	// System
	bool Start() override;
	void Stop() override;
};


#endif
