#include "Draw.h"


NAMESPACE_UPP


VfsValue* ThrowingInteractionSystemBase::GetPool() const {
	return val.FindOwnerNull();
}

void ThrowingInteractionSystemBase::Visit(Vis& v) {
	VIS_THIS(ToolSys);
}

bool ThrowingInteractionSystemBase::Initialize(const WorldState& ws) {
	ball_holding_distance = 0.5f;
	
	if (!InteractionListener::Initialize(GetEngine(), this))
		return false;
	
	return true;
}

void ThrowingInteractionSystemBase::Uninitialize() {
	InteractionListener::Uninitialize(GetEngine(), this);
}

void ThrowingInteractionSystemBase::Attach(ThrowingComponentPtr c) {
	ArrayFindAdd(comps, c);
}

void ThrowingInteractionSystemBase::Detach(ThrowingComponentPtr c) {
	ArrayRemoveKey(comps, c);
}

String ThrowingInteractionSystemBase::GetInstructions() const {
	return "Press and hold trigger to spawn a baseball.\n\n"
	       "Release trigger to throw the baseball.";
}

String ThrowingInteractionSystemBase::GetDisplayName() const {
	return "Throwing";
}

EntityPtr ThrowingInteractionSystemBase::CreateToolSelector() const {
	auto selector = CreatePrefab<ToolSelectorPrefab>(*GetPool(), ws_at_init);
	selector->Get<ModelComponent>().SetPrefabModel("Baseball");
	selector->Get<ToolSelectorKey>().type = GetTypeCls();
	return selector;
}

void ThrowingInteractionSystemBase::Update(double dt) {
	for (ThrowingComponentPtr& throwing : comps) {
		EntityPtr entity = throwing->GetEntity();
		TransformPtr trans = entity->Find<Transform>();
		if (!trans)
			continue;
		
		if (throwing->ball_object) {
			if (!throwing->IsEnabled()) {
				throwing->ball_object->Destroy();
			}
			else {
				vec3 fwd_dir = trans->GetForwardDirection();
				TransformPtr ball_transform = throwing->ball_object->Find<Transform>();
				ball_transform->data.position = trans->data.position + fwd_dir * ball_holding_distance;
				ball_transform->data.orientation = trans->data.orientation;
				
				if (ball_transform->size[0] < 1.0f) {
					ball_transform->size += vec3( 2.0f * (float)dt );
				}
			}
		}
	}
}

void ThrowingInteractionSystemBase::OnControllerUpdated(const GeomEvent& e) {
	// pass
}

void ThrowingInteractionSystemBase::OnControllerPressed(const GeomEvent& e) {
	if (e.type == EVENT_HOLO_PRESSED && e.value == ControllerMatrix::TRIGGER) {
		for (ThrowingComponentPtr& throwing : comps) {
			if (!throwing->IsEnabled()) continue;
			
			throwing->ball_object = CreatePrefab<Baseball>(*GetPool(), ws_at_init);
			throwing->ball_object->Get<Transform>().size = vec3{ throwing->scale };
			throwing->ball_object->Get<RigidBody>().SetEnabled(false);
			throwing->ball_object->Get<PhysicsBody>().BindDefault();
		}
	}
}

void ThrowingInteractionSystemBase::OnControllerReleased(const GeomEvent& e) {
	if (e.type == EVENT_HOLO_RELEASED && e.value == ControllerMatrix::TRIGGER) {
		ASSERT(e.ctrl);
		const ControllerState& source_state = e.GetState();
		const ControllerMatrix& source_props = source_state.props;
		const ControllerSource& source = source_state.GetSource();
		
		for (ThrowingComponentPtr& throwing : comps) {
			if (!throwing->IsEnabled()) continue;
			
			EntityPtr entity = throwing->GetEntity();
			TransformPtr trans = entity->Find<Transform>();
			
			if (throwing->ball_object) {
				// We no longer need to keep a reference to the thrown ball.
				EntityPtr ball = throwing->ball_object;
				throwing->ball_object = 0;
				
				// If the controller has no motion, release the ball with no initial velocity.
				RigidBodyPtr rb = ball->Find<RigidBody>();
				rb->SetEnabled(true);
				rb->velocity = {};
				rb->angular_velocity = {};
				
				TransformPtr ball_trans = ball->Find<Transform>();
				
				vec3 position = trans->data.position;
				vec3 fwd_dir = trans->GetForwardDirection();
				
				
				vec3 velocity;
				vec3 grasp_angular_velocity;
				source.GetVelocity(velocity.data);
				source.GetAngularVelocity(grasp_angular_velocity.data);
				
				#if 1
				LOG("ThrowingInteractionSystemBase::OnControllerReleased: " <<
					"velocity: " << velocity.ToString() << ", "
					"grasp_angular_velocity: " << grasp_angular_velocity.ToString() << ")");
				#endif
				
				if (!grasp_angular_velocity.IsNull()) {
					const vec3 ball_position = position + (fwd_dir * ball_holding_distance);
					const vec3 ball_velocity = GetVelocityNearSourceLocation(position, velocity, grasp_angular_velocity, ball_position);
					
					if (!ball_velocity.IsNull()) {
						ball_trans->data.position = ball_position;
						ball_trans->data.orientation = trans->data.orientation;
						rb->velocity = ball_velocity;
						rb->angular_velocity = grasp_angular_velocity;
						
					}
				}
			}
		}
	}
}

void ThrowingInteractionSystemBase::Register() {
	// pass
}

void ThrowingInteractionSystemBase::Unregister() {
	// pass
}

void ThrowingInteractionSystemBase::Activate(EntityPtr entity) {
	// pass
}

void ThrowingInteractionSystemBase::Deactivate(EntityPtr entity) {
	// pass
}














void ThrowingComponent::Visit(Vis& v) {
	VIS_THIS(CustomToolComponent);
	
	_VIS_(distance_from_pointer)
	 VIS_(scale);
	
	v & ball_object;
}

bool ThrowingComponent::Initialize(const WorldState& ws) {
	ToolComponentPtr tool = GetEntity()->val.Find<ToolComponent>();
	if (tool)
		tool->AddTool(this);
	
	Ptr<ThrowingInteractionSystemBase> sys = GetEngine().TryGet<ThrowingInteractionSystemBase>();
	if (sys)
		sys-> Attach(this);
	
	return true;
}

void ThrowingComponent::Uninitialize() {
	ToolComponentPtr tool = GetEntity()->val.Find<ToolComponent>();
	if (tool)
		tool->RemoveTool(this);
	
	Ptr<ThrowingInteractionSystemBase> sys = GetEngine().TryGet<ThrowingInteractionSystemBase>();
	if (sys)
		sys->Detach(this);
}

void ThrowingComponent::SetEnabled(bool enable) {
	Enableable::SetEnabled(enable);
	
	if (ball_object) {
		ball_object->SetEnabled(enable);
	}
}

void ThrowingComponent::Destroy() {
	Destroyable::Destroy();
	
	if (ball_object) {
		ball_object->Destroy();
	}
}

bool ThrowingComponent::LoadModel(ModelComponent& mdl) {
	mdl.Clear();
	return true;
}


END_UPP_NAMESPACE
