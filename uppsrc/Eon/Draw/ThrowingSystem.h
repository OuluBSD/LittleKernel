#ifndef _Eon_Draw_ThrowingSystem_h_
#define _Eon_Draw_ThrowingSystem_h_


class ThrowingComponent :
	public CustomToolComponent {
	
public:
	CLASSTYPE(ThrowingComponent)
	ThrowingComponent(VfsValue& e) : CustomToolComponent(e) {}
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	void SetEnabled(bool enable) override;
	void Destroy() override;
	bool LoadModel(ModelComponent&) override;
	
	EntityPtr ball_object;
	
	float distance_from_pointer = 0.05f;
	float scale = 0.25f;
};

using ThrowingComponentPtr = Ptr<ThrowingComponent>;


// ThrowingInteractionSystem
// This ToolSystem manages the Throwing tool which allows you to throw baseballs in 3D scene
class ThrowingInteractionSystemBase :
	public ToolSystemBaseT<ThrowingInteractionSystemBase, ThrowingComponent>
{
	Array<ThrowingComponentPtr> comps;
	WorldState ws_at_init;
	
public:
	using ToolSys = ToolSystemBaseT<ThrowingInteractionSystemBase, ThrowingComponent>;
	CLASSTYPE(ThrowingInteractionSystemBase);
	ThrowingInteractionSystemBase(VfsValue& m) : ToolSys(m) {}
	void Visit(Vis& vis) override;
	
	using Parent = Engine;
	float ball_holding_distance;
	static constexpr const char* POOL_NAME = "throwing";
	
	void Attach(ThrowingComponentPtr c);
	void Detach(ThrowingComponentPtr c);
	VfsValue* GetPool() const;
	
	
protected:
	// System
	System* GetSystem() override {return this;}
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	void Update(double dt) override;
	void OnControllerPressed(const GeomEvent& e) override;
	void OnControllerUpdated(const GeomEvent& e) override;
	void OnControllerReleased(const GeomEvent& e) override;
	
	// IInteractionModeSystem
	String GetInstructions() const override;
	String GetDisplayName() const override;
	EntityPtr CreateToolSelector() const override;
	void Register() override;
	void Unregister() override;
	void Activate(EntityPtr entity) override;
	void Deactivate(EntityPtr entity) override;
	
};


#endif
