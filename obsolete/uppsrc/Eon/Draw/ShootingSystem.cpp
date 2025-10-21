#include "Draw.h"


NAMESPACE_UPP


bool ShootingInteractionSystemBase::Initialize(const WorldState& ws) {
	if (!InteractionListener::Initialize(GetEngine(), this))
		return false;
	
	return true;
}

void ShootingInteractionSystemBase::Uninitialize() {
	InteractionListener::Uninitialize(GetEngine(), this);
}

void ShootingInteractionSystemBase::Attach(ShootingComponentPtr c) {
	ArrayFindAdd(comps, c);
	
	// The "barrel_to_ctrl" is to transform from the tip of the barrel to the location of the controller
	const mat4 barrel_to_ctrl = Translate(vec3(0.0f, 0.0675f, 0.0f)) * XRotation(ConvertToRadians(-10));
	
	Ptr<Entity> entity = c->GetEntity();
	entity->Get<ShootingComponent>().barrel_to_ctrl = barrel_to_ctrl;
}

void ShootingInteractionSystemBase::Detach(ShootingComponentPtr c) {
	ArrayRemoveKey(comps, c);
}

String ShootingInteractionSystemBase::GetInstructions() const {
	return "Pull the trigger to fire the gun.\n\n"
	       "You can feel controller vibrate for each bullet.\n\n";
}

String ShootingInteractionSystemBase::GetDisplayName() const {
	return "Shooting";
}

EntityPtr ShootingInteractionSystemBase::CreateToolSelector() const {
	auto selector = CreatePrefab<ToolSelectorPrefab>(*GetPool(), ws_at_init);
	selector->Get<ModelComponent>().SetPrefabModel("Gun");
	selector->Get<ToolSelectorKey>().type = GetTypeCls();
	return selector;
}

void ShootingInteractionSystemBase::Register() {
	
}

void ShootingInteractionSystemBase::Unregister() {
	
}

void ShootingInteractionSystemBase::Activate(EntityPtr entity) {
	entity->Get<ModelComponent>().SetEnabled(false);
}

void ShootingInteractionSystemBase::Deactivate(EntityPtr entity) {
	entity->Get<ModelComponent>().SetEnabled(true);
}

void ShootingInteractionSystemBase::OnControllerReleased(const GeomEvent& e) {
	// pass
}

void ShootingInteractionSystemBase::OnControllerPressed(const GeomEvent& e) {
	
	if (e.type == EVENT_HOLO_PRESSED && e.value == ControllerMatrix::TRIGGER) {
		for (ShootingComponentPtr& shooting : comps) {
			if (!shooting->IsEnabled())
				continue;
			EntityPtr entity = shooting->GetEntity();
			TransformPtr trans = entity->Find<Transform>();
			if (!trans)
				continue;
			
			const mat4 trans_mat = trans->GetMatrix();
			const mat4 barrel_to_world =
				trans_mat * shooting->barrel_to_ctrl
				;
				
			vec3 position = Position(barrel_to_world);
			quat orientation = MatQuat(barrel_to_world);
			vec3 forward = Forward(barrel_to_world);
			forward.Normalize();
			vec3 bullet_velocity = forward * shooting->bullet_speed;
			
			#if 0
			LOG("ShootingInteractionSystemBase::OnControllerPressed: "
				"position: " << position.ToString() << ", "
				"orientation: " << orientation.ToString() << ", "
				"forward: " << forward.ToString() << ", "
				"bullet_velocity: " << bullet_velocity.ToString());
			#endif
			
			// Create bullet and send it on it's merry way
			EntityPtr bullet = CreatePrefab<Bullet>(*GetPool(), ws_at_init);
			TransformPtr bullet_trans = bullet->Find<Transform>();
			RigidBodyPtr bullet_rbody = bullet->Find<RigidBody>();
			PhysicsBodyPtr bullet_pbody = bullet->Find<PhysicsBody>();
			bullet_trans->data.position = position;
			bullet_trans->data.orientation = orientation;
			bullet_rbody->velocity = bullet_velocity;
			bullet_pbody->BindDefault();
				
			//SendContinuousBuzzForDuration(args.State().Source(), 125_ms);
		}
	}
}

void ShootingInteractionSystemBase::OnControllerUpdated(const GeomEvent& e) {
	for (ShootingComponentPtr& shooting : comps) {
		if (!shooting->IsEnabled()) continue;
		
		EntityPtr entity = shooting->GetEntity();
		ModelComponentPtr model = entity->Find<ModelComponent>();
		
		// Show the controllers while we're holding grasp, to help show how the model relates to the real world object
		ASSERT(e.ctrl);
		bool should_render_controller = e.ctrl->ctrl[1].IsGrasped();
		model->color[3] = should_render_controller ? 0.25f : 1.0f;
	}
}

VfsValue* ShootingInteractionSystemBase::GetPool() const {
	VfsValue* o = val.owner;
	while (o && o->type_hash)
		o = o->owner;
	return o;
}















void ShootingComponent::Visit(Vis& v) {
	TODO
	#if 0
	CustomToolComponent::Etherize(e);
	
	e % bullet_speed
	  % barrel_to_ctrl;
	#endif
	VIS_THIS(CustomToolComponent);
}

bool ShootingComponent::Initialize(const WorldState& ws) {
	ToolComponentPtr tool = GetEntity()->val.Find<ToolComponent>();
	if (tool)
		tool->AddTool(this);
	
	Ptr<ShootingInteractionSystemBase> sys = GetEngine().TryGet<ShootingInteractionSystemBase>();
	if (sys)
		sys-> Attach(this);
	
	return true;
}

void ShootingComponent::Uninitialize() {
	ToolComponentPtr tool = GetEntity()->val.Find<ToolComponent>();
	if (tool)
		tool->RemoveTool(this);
	
	Ptr<ShootingInteractionSystemBase> sys = GetEngine().TryGet<ShootingInteractionSystemBase>();
	if (sys)
		sys->Detach(this);
}

bool ShootingComponent::LoadModel(ModelComponent& mdl) {
	ModelCachePtr sys = GetEngine().val.Find<ModelCache>();
	if (!sys)
		return false;
	
	String path = KnownModelNames::GetPath(KnownModelNames::Gun);
	ModelPtr m = sys->GetAddModelFile(path);
	mdl.SetModelMatrix(YRotation(M_PIf));
	mdl.SetModel(m);
	
	if (0) {
		Ptr<ModelComponent> m = GetEntity()->val.Find<ModelComponent>();
		ASSERT(m);
		if (m) {
			m->SetRotation(ConvertToRadians(180), ConvertToRadians(70), 0.0f);
			m->SetTranslation(vec3(0, 0.05f, 0.0f));
		}
	}
	
	return true;
}


END_UPP_NAMESPACE
