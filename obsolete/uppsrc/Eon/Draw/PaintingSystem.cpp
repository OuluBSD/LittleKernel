#include "Draw.h"


NAMESPACE_UPP


bool PaintingInteractionSystemBase::Initialize(const WorldState& ws) {
	ws_at_init = ws;
	
	if (!InteractionListener::Initialize(GetEngine(), this))
		return false;
	
	tb = GetEngine().val.Find<ToolboxSystemBase>();
	if (!tb) {
		LOG("PaintingInteractionSystemBase: error: ToolboxSystemBase required");
		return false;
	}
	return true;
}

void PaintingInteractionSystemBase::Uninitialize() {
	InteractionListener::Uninitialize(GetEngine(), this);
	tb = 0;
}

bool PaintingInteractionSystemBase::Start() {
	GetEngine().Get<ToolboxSystemBase>()->AddToolSystem(this);
	return true;
}

void PaintingInteractionSystemBase::Stop() {
	GetEngine().Get<ToolboxSystemBase>()->RemoveToolSystem(this);
}

bool PaintingInteractionSystemBase::Arg(String key, Value value) {
	if (key == "debug.model")
		dbg_model = value.ToString() == "true";
	return true;
}

VfsValue* PaintingInteractionSystemBase::GetPool() const {
	VfsValue* v = &*val.owner;
	while (v && v->type_hash != 0)
		v = v->owner;
	ASSERT(v);
	return v;
}

String PaintingInteractionSystemBase::GetInstructions() const {
	return "Press and hold trigger to paint.\n\n"
	       "Touch and press touchpad to choose brush color.\n\n"
	       "Hold grasp button to move strokes around. While holding grasp tilt thumbstick forward/backward to translate strokes.\n\n"
	       "Push thumbstick down to delete strokes.\n\n";
}

String PaintingInteractionSystemBase::GetDisplayName() const {
	return "Painting";
}

EntityPtr PaintingInteractionSystemBase::CreateToolSelector() const {
	TODO
	#if 0
	auto selector = GetEngine().Get<EntityStore>()->GetRoot()->Create<ToolSelectorPrefab>();
	selector->Get<ModelComponent>()->SetPrefabModel("PaintBrush");
	selector->Get<Transform>()->data.orientation = AxisAngleQuat({ 1, 0, 0 }, M_PIf / 1.5f);
	selector->Get<ToolSelectorKey>()->type = GetType();
	return selector;
	#endif
	return 0;
}

void PaintingInteractionSystemBase::Attach(PaintComponentPtr paint) {
	VectorFindAdd(comps, paint);
	
	TODO
	#if 0
	EntityStorePtr es = GetEngine().Get<EntityStore>();
	EntityPtr entity = paint->GetEntity();
	const auto& selected_color = colors[0];
	
	#if 0
	auto paint_brush =
		!dbg_model
			? es->GetRoot()->Create<PaintBrush>()
			: es->GetRoot()->Create<DummyToolModel>();
	paint_brush->Get<ModelComponent>()->color = selected_color;
	#else
	EntityPtr paint_brush = paint->GetEntity();
	#endif
	
	
	auto touchpad_indicator = es->GetRoot()->Create<StaticSphere>();
	touchpad_indicator->Get<Transform>()->size = { 0.005f, 0.005f, 0.005f };
	touchpad_indicator->Get<ModelComponent>()->color = Colors::Gray;
	paint->selected_color = selected_color;
	paint->paint_brush = paint_brush;
	paint->touchpad_indicator = touchpad_indicator;
	
	for (auto color : colors) {
		auto color_picker = es->GetRoot()->Create<StaticSphere>();
		color_picker->Get<Transform>()->size = { 0.01f, 0.01f, 0.01f };
		color_picker->Get<ModelComponent>()->color = color;
		paint->clr_pick_objects.Add(color_picker);
	}
	
	paint->beam = es->GetRoot()->Create<StaticCube>();
	paint->beam->Get<Transform>()->size = { 0.005f, 0.005f, 10.0f };
	paint->beam->Get<ModelComponent>()->color = Colors::Aquamarine;
	
	#endif
}

void PaintingInteractionSystemBase::Detach(PaintComponentPtr c) {
	VectorRemoveKey(comps, c);
}

void PaintingInteractionSystemBase::Register() {
	TODO
	#if 0
	// add MotionControllerComponent to new entities (see PaintBrush)
	// note: duplicate PaintingInteractionSystemBase::Register
	
	ToolSys::Register(entities);
	auto es = GetEngine().Get<EntityStore>();
	
	for (auto& entity : m_entities) {
		const auto& selected_color = colors[0];
		auto paint_brush = es->GetRoot()->Create<PaintBrush>();
		paint_brush->Get<PbrRenderable>()->color = selected_color;
		paint_brush->Get<MotionControllerComponent>()->req_hand = entity->Get<MotionControllerComponent>()->req_hand;
		auto touchpad_indicator = es->GetRoot()->Create<StaticSphere>();
		touchpad_indicator->Get<Transform>()->size = { 0.005f, 0.005f, 0.005f };
		touchpad_indicator->Get<PbrRenderable>()->color = Colors::Gray;
		PaintComponentPtr paint = entity->Get<PaintComponent>();
		paint->selected_color = selected_color;
		paint->paint_brush = paint_brush;
		paint->touchpad_indicator = touchpad_indicator;
		
		for (auto color : colors) {
			auto color_picker = es->GetRoot()->Create<StaticSphere>();
			color_picker->Get<Transform>()->size = { 0.01f, 0.01f, 0.01f };
			color_picker->Get<PbrRenderable>()->color = color;
			paint->clr_pick_objects.Add(color_picker);
		}
		
		paint->beam = es->GetRoot()->Create<StaticCube>();
		paint->beam->Get<Transform>()->size = { 0.005f, 0.005f, 10.0f };
		paint->beam->Get<PbrRenderable>()->color = Colors::Aquamarine;
		paint->SetEnabled(false);
	}
	
	GetEngine().Get<SpatialInteractionSystem>()->AddListener(this);
	#endif
}

void PaintingInteractionSystemBase::Unregister() {
	
}

void PaintingInteractionSystemBase::Activate(EntityPtr entity) {
	
	// Stop rendering the controller
	auto* a = entity->val.Find<ModelComponent>();
	if (a)
		a->SetEnabled(false);
}

void PaintingInteractionSystemBase::Deactivate(EntityPtr entity) {
	auto* a = entity->val.Find<ModelComponent>();
	if (!a) return;
	a->SetEnabled(true);
	
	PaintComponentPtr paint = entity->val.Find<PaintComponent>();
	if (!paint) return;
	
	// Copy out the strokes from the component so they can persist in the world.
	if (paint->stroke_in_progress) {
		MemSwap(paint->strokes.Add(), paint->stroke_in_progress);
	}
	
	if (paint->strokes.GetCount()) {
		MemSwap(persistent_strokes.Add(), paint->strokes);
	}
	
}

void PaintingInteractionSystemBase::ClearStrokes() {
	TODO
	#if 0
	// Destroy all the paint strokes currently active
	for (auto& enabled_entity : GetEnabledEntities()) {
		auto entity = enabled_entity.Get<EntityPtr>();
		auto paint = enabled_entity.Get<PaintComponentPtr>();
		
		for (auto& stroke : paint->strokes) {
			stroke->Destroy();
		}
		
		paint->strokes.Clear();
		
		if (paint->stroke_in_progress) {
			paint->stroke_in_progress->Destroy();
			paint->stroke_in_progress.Clear();
		}
	}
	
	// Destroy all the persistent strokes
	for (auto& stroke_group : persistent_strokes) {
		for (auto& stroke : stroke_group) {
			stroke->Destroy();
		}
	}
	
	persistent_strokes.Clear();
	#endif
}

void PaintingInteractionSystemBase::OnControllerPressed(const GeomEvent& e) {
	#if 0
	if (args.PressKind() == SpatialInteractionPressKind::Thumbstick) {
		ClearStrokes();
	}
	else
		OnControllerUpdated(e);
	#endif
}

void PaintingInteractionSystemBase::OnControllerReleased(const GeomEvent& e) {
	OnControllerUpdated(e);
}

void PaintingInteractionSystemBase::OnControllerUpdated(const GeomEvent& e) {
	const bool dbg_log = 0;
	
	for (PaintComponentPtr& paint : comps) {
		if (!paint->IsEnabled()) continue;
		
		EntityPtr entity = paint->GetEntity();
		
		bool new_stroke_started = false;
		Ptr<Model> paint_brush_model = paint->paint_brush->Get<ModelComponent>().GetModel();
		
		if (paint_brush_model && !paint->has_brush_tip_offset) {
			Optional<NodeIndex> touch_node = paint_brush_model->FindFirstNode("PaintTip");
			
			if (touch_node) {
				// Calcluate paint tip offset from holding pose
				// we use offset as it does not rely on the current transform of the model
				// we initialize it once as the value will not change
				mat4 brush_tip_world_transform = paint_brush_model->GetNodeWorldTransform(touch_node.value());
				mat4 paint_brush_world_transform = paint_brush_model->GetNode(ModelNode::RootIdx).GetTransform();
				
				paint->has_brush_tip_offset = true;
				paint->brush_tip_offset_from_holding_pose =
				        brush_tip_world_transform *
				        paint_brush_world_transform.GetInverse();
				
				if (dbg_log) {
					vec3 pos = paint->brush_tip_offset_from_holding_pose.GetTranslation();
					LOG("PaintingInteractionSystemBase::OnControllerUpdated: initial pos " << pos.ToString());
				}
			}
			else {
				TODO
			}
		}
		
		Ptr<ToolComponent> tool = entity->Find<ToolComponent>();
		ASSERT(tool);
		
		Ptr<PlayerHandComponent> controller = tool->active_hand;
		if (!controller)
			continue;
		
		{
			const ControllerMatrix& controller_properties = *e.ctrl;
			const auto& ctrl = controller_properties.ctrl[1];
			paint->touchpad_x   = ctrl.GetTouchpadX();
			paint->touchpad_y   = ctrl.GetTouchpadY();
			paint->thumbstick_x = ctrl.GetThumbstickX();
			paint->thumbstick_y = ctrl.GetThumbstickY();
			
			if (paint->cur_state == PaintComponent::State::Idle) {
				if (ctrl.IsSelectPressed()) {
					paint->cur_state = PaintComponent::State::Painting;
					new_stroke_started = true;
				}
				else if (ctrl.IsGrasped()) {
					paint->cur_state = PaintComponent::State::Manipulating;
				}
				else if (ctrl.IsTouchpadTouched()) {
					paint->cur_state = PaintComponent::State::ColorSelection;
				}
			}
			else if (paint->cur_state == PaintComponent::State::Painting) {
				if (ctrl.IsSelectPressed() == false) {
					paint->cur_state = PaintComponent::State::Idle;
				}
			}
			else if (paint->cur_state == PaintComponent::State::Manipulating) {
				if (ctrl.IsGrasped() == false) {
					paint->cur_state = PaintComponent::State::Idle;
					paint->prev_manip_loc = nullptr;
				}
			}
			else if (paint->cur_state == PaintComponent::State::ColorSelection) {
				if (paint->wait_touchpad_release == false) {
					if (ctrl.IsTouchpadPressed()) {
						paint->wait_touchpad_release = true;
						paint->selected_color = SelectColor(paint->touchpad_x, paint->touchpad_y);
						
						//SendContinuousBuzzForDuration(source_state.GetSource(), 100_ms);
					}
				}
				
				if (ctrl.IsTouchpadPressed() == false) {
					paint->wait_touchpad_release = false;
				}
				
				if (ctrl.IsTouchpadTouched() == false) {
					paint->cur_state = PaintComponent::State::Idle;
				}
			}
			
			if (paint->cur_state == PaintComponent::State::Painting) {
				// Start new stroke
				if (new_stroke_started) {
					paint->stroke_in_progress = CreatePrefab<PaintStroke>(*GetPool(), ws_at_init);
					ModelComponentPtr m = paint->stroke_in_progress->Find<ModelComponent>();
					m->color = paint->selected_color;
					paint->strokes.Add(paint->stroke_in_progress);
				}
				
				TransformPtr trans = entity->Find<Transform>();
				if (trans) {
					if (paint->stroke_in_progress) {
						mat4 location = trans->GetMatrix();
						mat4 paint_to_world;
						if (paint->has_brush_tip_offset) {
							paint_to_world =
							        paint->brush_tip_offset_from_holding_pose *
							        location;
						}
						else {
							paint_to_world = location;
						}
						
						if (dbg_log) {
							vec3 pos = paint_to_world.GetTranslation();
							LOG("PaintingInteractionSystemBase::OnControllerUpdated: pos " << pos.ToString());
						}
						
						paint->stroke_in_progress
							->Get<PaintStrokeComponent>()
								.AddPoint(RemoveScale(paint_to_world), paint_tip_thickness);
					}
				}
			}
		}
	}
}


void PaintingInteractionSystemBase::Update(double dt) {
	ASSERT(tb);
	if (!tb) return;
	
	for (PaintComponentPtr& paint : comps) {
		if (!paint->IsEnabled()) continue;
		
		EntityPtr entity = paint->GetEntity();
		
		ToolComponentPtr tool = paint->GetEntity()->val.Find<ToolComponent>();
		if (!tool)
			continue;
		
		PlayerHandComponent* controller = tool->active_hand;
		if (!controller)
			continue;
		
		ASSERT(paint->beam);
		paint->beam->Get<ModelComponent>().SetEnabled(paint->cur_state == PaintComponent::State::Manipulating);
		
		// Set properties required for rendering
		paint->touchpad_indicator->Get<ModelComponent>().SetEnabled(paint->cur_state == PaintComponent::State::ColorSelection);
		
		for (auto& go : paint->clr_pick_objects) {
			go->Get<ModelComponent>().SetEnabled(paint->cur_state == PaintComponent::State::ColorSelection);
		}
		
		const bool show_controller = paint->cur_state == PaintComponent::State::Manipulating;
		paint->paint_brush->Get<ModelComponent>().SetEnabled(!show_controller);
		
		if (auto location = controller->location) {
			const vec3 position = location->GetPosition();
			const quat orientation = location->GetOrientation();
			const vec4 paint_tip_color = paint->cur_state == PaintComponent::State::ColorSelection ? SelectColor(paint->touchpad_x, paint->touchpad_y) : paint->selected_color;
			paint->paint_brush->Get<ModelComponent>().color = paint_tip_color;
			
			if (paint->cur_state == PaintComponent::State::Manipulating) {
				// Update the paint strokes based on the change in location
				if (paint->prev_manip_loc) {
					const vec3 previous_position = paint->prev_manip_loc->GetPosition();
					const quat previous_orientation = paint->prev_manip_loc->GetOrientation();
					const quat orientation_delta = orientation * Inverse(previous_orientation);
					const mat4 manipulation_transform = Translate(-previous_position) * QuatMat(orientation_delta) * Translate(position);
					
					for (auto stroke : paint->strokes) {
						TODO
					}
				}
				
				paint->prev_manip_loc = location;
				// Move the paint strokes based on manipulation changes
				constexpr double ThumbstickMovementThresholdPercent = 0.2f; // Deadzone to prevent slight thumbstick movement
				constexpr float MovementSpeedInMetersPerSecond = 2.5f;
				
				if (const auto& pointer_pose = location->GetHandPose()) {
					const vec3 position = pointer_pose->GetPosition();
					const vec3 forward = pointer_pose->GetForwardDirection();
					
					if (abs(paint->thumbstick_y) > ThumbstickMovementThresholdPercent) {
						const vec3 forward_movement = forward * paint->thumbstick_y * MovementSpeedInMetersPerSecond * (float)dt;
						
						// Move all paintings along beam path
						for (auto& stroke : paint->strokes) {
							stroke->Get<Transform>().data.position += forward_movement;
						}
					}
					
					paint->beam->Get<Transform>().data.position =
					        position +
					        forward * (paint->beam->Get<Transform>().size[2] * 0.5f);
					paint->beam->Get<Transform>().data.orientation = pointer_pose->GetOrientation();
				}
			}
			else if (paint->cur_state == PaintComponent::State::ColorSelection) {
				constexpr float colorpicker_diameter = 0.025f;
				constexpr float colorpicker_height = 0.015f;
				const mat4 paint_brush_to_world = paint->paint_brush->Get<Transform>().GetMatrix();
				const vec3 touchpad_indicator_on_paint_brush = { paint->touchpad_x * colorpicker_diameter, colorpicker_height, paint->touchpad_y* colorpicker_diameter * -1 };
				const vec3 touchpad_indicator_in_world = VectorTransform(touchpad_indicator_on_paint_brush, paint_brush_to_world);
				paint->touchpad_indicator->Get<Transform>().data.position = touchpad_indicator_in_world;
				// Color picker plane defined as slightly above the touchpad with the same orientation as the touchpad
				const int num_colors = static_cast<int>(paint->clr_pick_objects.GetCount());
				auto iter = paint->clr_pick_objects.begin();
				
				for (int i = 0; i < num_colors; ++i, ++iter) {
					const float angle = (static_cast<float>(i * -1 - 1) / static_cast<float>(num_colors)) * (2 * M_PIf) - M_PIf;
					const float next_angle = (static_cast<float>((i + 1) * -1 - 1) / static_cast<float>(num_colors)) * (2 * M_PIf) - M_PIf;
					const float angle_delta = (next_angle - angle) / 2; // Want color icon to appear in the middle of the segment, not the start.
					const float final_angle = angle - angle_delta;
					const vec3 color_indicator_on_paint_brush = { std::cos(final_angle)* colorpicker_diameter, colorpicker_height, std::sin(final_angle)* colorpicker_diameter };
					const vec3 color_indicator_in_world = VectorTransform(color_indicator_on_paint_brush, paint_brush_to_world);
					iter()->Get<Transform>().data.position = color_indicator_in_world;
				}
			}
		}
	}
}

vec4 PaintingInteractionSystemBase::SelectColor(double x, double y) {
	if (x == 0 && y == 0) {
		return colors.Top();
	}
	
	constexpr double min = -M_PI;
	constexpr double max = +M_PI;
	double angle = std::atan2(y, x);
	int index = static_cast<int>(std::round((angle - min) / (max - min) * (colors.GetCount() - 1)));
	return colors[index];
}












void PaintComponent::Visit(Vis& v) {
	VIS_THIS(CustomToolComponent);
	
	TODO
	#if 0
	CustomToolComponent::Etherize(e);
	
	e % selected_color
	  % cur_state
	  % touchpad_x
	  % touchpad_y
	  % thumbstick_x
	  % thumbstick_y
	  % wait_touchpad_release
	  % brush_tip_offset_from_holding_pose
	  % has_brush_tip_offset;
	
	EtherizeRef(e, touchpad_indicator);
	EtherizeRef(e, stroke_in_progress);
	EtherizeRef(e, paint_brush);
	EtherizeRef(e, beam);
	EtherizeRefContainer(e, clr_pick_objects);
	EtherizeRefContainer(e, strokes);
	
	#endif
}

bool PaintComponent::Initialize(const WorldState& ws) {
	ToolComponentPtr tool = GetEntity()->val.Find<ToolComponent>();
	if (tool)
		tool->AddTool(this);
	
	Ptr<PaintingInteractionSystemBase> sys = GetEngine().TryGet<PaintingInteractionSystemBase>();
	if (sys)
		sys-> Attach(this);
	
	return true;
}

void PaintComponent::Uninitialize() {
	ToolComponentPtr tool = GetEntity()->val.Find<ToolComponent>();
	if (tool)
		tool->RemoveTool(this);
	
	Ptr<PaintingInteractionSystemBase> sys = GetEngine().TryGet<PaintingInteractionSystemBase>();
	if (sys)
		sys->Detach(this);
}

void PaintComponent::SetEnabled(bool enable) {
	Enableable::SetEnabled(enable);
	
	for (auto& color_picker : clr_pick_objects) {
		color_picker->SetEnabled(enable);
	}
	
	if (touchpad_indicator) {
		touchpad_indicator->SetEnabled(enable);
	}
	
	if (paint_brush) {
		paint_brush->SetEnabled(enable);
	}
	
	if (beam) {
		beam->SetEnabled(enable);
	}
}

void PaintComponent::Destroy() {
	Destroyable::Destroy();
	
	for (auto& color_picker : clr_pick_objects) {
		color_picker->Destroy();
	}
	
	if (touchpad_indicator) {
		touchpad_indicator->Destroy();
	}
	
	if (paint_brush) {
		paint_brush->Destroy();
	}
	
	if (beam) {
		beam->Destroy();
	}
}

bool PaintComponent::LoadModel(ModelComponent& mdl) {
	ModelCachePtr sys = GetEngine().val.Find<ModelCache>();
	if (!sys)
		return false;
	
	String path = KnownModelNames::GetPath(KnownModelNames::PaintBrush);
	ModelPtr m = sys->GetAddModelFile(path);
	mdl.SetModelMatrix(Identity<mat4>());
	mdl.SetModel(m);
	return true;
}


END_UPP_NAMESPACE

