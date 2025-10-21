#pragma once

#if 1


class PaintComponent :
	public CustomToolComponent {
	
public:
	CLASSTYPE(PaintComponent)
	PaintComponent(VfsValue& e) : CustomToolComponent(e) {}
	
	#undef Idle
	#undef Painting
	#undef ColorSelection
	#undef State
	typedef enum : byte  {
		Idle,
		Painting,
		Manipulating,
		ColorSelection
	} State;
	
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	void SetEnabled(bool enable) override;
	void Destroy() override;
	bool LoadModel(ModelComponent&) override;
	
	
	EntityPtr touchpad_indicator;
	EntityPtr stroke_in_progress;
	EntityPtr paint_brush;
	EntityPtr beam;
	LinkedList<EntityPtr> clr_pick_objects;
	LinkedList<EntityPtr> strokes;
	HandSourceLocation* prev_manip_loc = 0;
	
	vec4 selected_color = Colors::White;
	byte cur_state { State::Idle };
	
	float touchpad_x = 0.0f;
	float touchpad_y = 0.0f;
	
	float thumbstick_x = 0.0f;
	float thumbstick_y = 0.0f;
	
	bool wait_touchpad_release = false;
	
	mat4 brush_tip_offset_from_holding_pose;
	bool has_brush_tip_offset = false;
	
};

using PaintComponentPtr = Ptr<PaintComponent>;


class PaintingInteractionSystemBase :
	public ToolSystemBaseT<PaintingInteractionSystemBase, PaintComponent>
{
public:
	using ToolSys = ToolSystemBaseT<PaintingInteractionSystemBase, PaintComponent>;
	CLASSTYPE(PaintingInteractionSystemBase);
	PaintingInteractionSystemBase(VfsValue& m) : ToolSys(m) {}
	void Visit(Vis& v) override {VIS_THIS(ToolSys) ^ persistent_strokes;}
	
	using Parent = Engine;
	
	static constexpr float paint_tip_thickness = 0.008f;
	static constexpr const char* POOL_NAME = "painting";
	
	VfsValue* GetPool() const;
	
	void Attach(PaintComponentPtr c);
	void Detach(PaintComponentPtr c);
	
protected:
	// System
	System* GetSystem() override {return this;}
	bool Initialize(const WorldState&) override;
	bool Start() override;
	void Update(double dt) override;
	void Stop() override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	
	// ToolSystemBase
	String GetInstructions() const override;
	String GetDisplayName() const override;
	EntityPtr CreateToolSelector() const override;
	
	void Register() override;
	void Unregister() override;
	void Activate(EntityPtr entity) override;
	void Deactivate(EntityPtr entity) override;
	
	// ISpatialInteractionListener
	void OnControllerPressed(const GeomEvent& e) override;
	void OnControllerUpdated(const GeomEvent& e) override;
	void OnControllerReleased(const GeomEvent& e) override;
	
private:
	vec4 SelectColor(double x, double y);
	void ClearStrokes();
	
	SimpleFixedArray<vec4, 10> colors = {
		Colors::Red,
		Colors::Chocolate,
		Colors::Yellow,
		Colors::Lime,
		Colors::Cyan,
		Colors::Blue,
		Colors::MediumPurple,
		Colors::White,
		Colors::DimGray,
		Colors::Black
	};
	
	ToolboxSystemBasePtr tb;
	Vector<PaintComponentPtr> comps;
	LinkedList<LinkedList<EntityPtr>> persistent_strokes;
	bool dbg_model = false;
	WorldState ws_at_init;
	
};


#endif
