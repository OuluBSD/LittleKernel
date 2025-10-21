#ifndef _Eon_Draw_ToolboxSystem_h_
#define _Eon_Draw_ToolboxSystem_h_


class ToolSystemBase;
class PlayerHandComponent;


class CustomToolComponent :
	public Component {
	
public:
	ECS_COMPONENT_CTOR(CustomToolComponent)
	virtual bool LoadModel(ModelComponent&) = 0;
	
	void Visit(Vis& v) override {}
	
};

using CustomToolComponentPtr = Ptr<CustomToolComponent>;



class ToolComponent : public Component {
	
public:
	ECS_COMPONENT_CTOR(ToolComponent)
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	
	void SwitchNext();
	void SwitchOff();
	void AddTool(CustomToolComponentPtr cb);
	void RemoveTool(CustomToolComponentPtr cb);
	void RefreshModel();
	
	String title;
	String description;
	TypeCls tool_type { AsVoidTypeCls() };
	
	CustomToolComponentPtr active_tool;
	Array<CustomToolComponentPtr> tools;
	PlayerHandComponent* active_hand = 0;
	
};

using ToolComponentPtr = Ptr<ToolComponent>;


class ToolboxSystemBase :
	public System,
	public InteractionListener
{
	
public:
	ECS_SYS_DEF_VISIT
	ECS_SYS_CTOR(ToolboxSystemBase);
	
	using Parent = Engine;
	
	
	void AddToolSystem(ToolSystemBasePtr system);
	void RemoveToolSystem(ToolSystemBasePtr system);
	
	void Attach(ToolComponentPtr tool);
	void Detach(ToolComponentPtr tool);
	
	const Array<ToolComponentPtr>& GetTools() const {return tools;}
	
protected:
	// System
	
	System* GetSystem() override {return this;}
	bool Start() override;
	void Update(double dt) override;
	void Stop() override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	
private:
	Array<ToolComponentPtr> tools;
	TypeMap<ToolSystemBasePtr> selectors;
	TypeMap<EntityPtr> selector_objects;
	
	bool test_tool_changer{ false };
	bool show_toolbox{ false };
	int active_tool_idx = -1;
	
	
protected:
	void SwitchToolType(EntityPtr entity, const TypeId& new_type);
	void OnControllerPressed(const GeomEvent& e) override;
	
};

using ToolboxSystemBasePtr = Ptr<ToolboxSystemBase>;


template<typename T, typename ToolComponent>
inline bool ToolSystemBaseT<T,ToolComponent>::Start() {
	Engine& m = GetEngine();
	m.Get<ToolboxSystemBase>()->AddToolSystem(this);
	return true;
}

template<typename T, typename ToolComponent>
inline void ToolSystemBaseT<T,ToolComponent>::Stop() {
	Engine& m = GetEngine();
	m.Get<ToolboxSystemBase>()->RemoveToolSystem(this);
}


#endif
