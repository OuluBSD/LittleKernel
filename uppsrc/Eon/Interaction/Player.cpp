#include "Interaction.h"
#include <Eon/Ecs/Ecs.h>

NAMESPACE_UPP


void PlayerHandComponent::Visit(Vis& v) {
	_VIS_(is_simulated)
	 VIS_(attach_ctrl_model)
	 VIS_(req_hand);
	
	v & body & source & location;
	
	VISIT_COMPONENT;
}

bool PlayerHandComponent::Initialize(const WorldState& ws) {
	return true;
}

void PlayerHandComponent::Uninitialize() {
	
}

bool PlayerHandComponent::IsSource(const ControllerSource& rhs) const {
	TODO
	return false;
}

bool PlayerHandComponent::Arg(String key, Value value) {
	if (key == "hand") {
		if (value.ToString() == "left")
			req_hand = PlayerHandedness::LeftHand;
		else if (value.ToString() == "right")
			req_hand = PlayerHandedness::RightHand;
		else
			return false;
	}
	else if (key == "body") {
		TODO
		#if 0
		EntityStorePtr es = GetEngine().TryGet<EntityStore>();
		if (!es)
			return false;
		EntityPtr e = es->FindEntity(value);
		if (e) {
			PlayerBodyComponentPtr bc = e->Find<PlayerBodyComponent>();
			if (bc && bc->SetHand((PlayerHandedness)req_hand, this)) {
				body = bc;
			}
		}
		else {
			LOG("PlayerHandComponent::Arg: could not find entity: " + value.ToString());
			return false;
		}
		#endif
	}
	else if (key == "simulated")
		is_simulated = value.ToString() == "true";
	
	return true;
}










void PlayerHeadComponent::Visit(Vis& v) {
	v & body;
	VISIT_COMPONENT
}

bool PlayerHeadComponent::Initialize(const WorldState& ws) {
	return true;
}

void PlayerHeadComponent::Uninitialize() {
	
}

bool PlayerHeadComponent::Arg(String key, Value value) {
	if (key == "body") {
		TODO
		#if 0
		EntityPtr e = es->FindEntity(value);
		if (e) {
			PlayerBodyComponentPtr bc = e->Find<PlayerBodyComponent>();
			if (bc && bc->SetHead(this)) {
				body = bc;
			}
		}
		else {
			LOG("PlayerHeadComponent::Arg: could not find entity: " + value.ToString());
			return false;
		}
		#endif
	}
	return true;
}







void PlayerBodyComponent::Visit(Vis& v) {
	_VIS_(height);
	v & hands[0] & hands[1] & head;
	VISIT_COMPONENT
}

bool PlayerBodyComponent::Initialize(const WorldState& ws) {
	PlayerBodySystem* sys = val.FindOwnerWith<PlayerBodySystem>();
	ASSERT(sys);
	if (sys) sys->Attach(this);
	return true;
}

void PlayerBodyComponent::Uninitialize() {
	PlayerBodySystem* sys = val.FindOwnerWith<PlayerBodySystem>();
	ASSERT(sys);
	if (sys) sys->Detach(this);
}

bool PlayerBodyComponent::Arg(String key, Value value) {
	if (key == "height")
		height = (float)(double)value;
	
	return true;
}

bool PlayerBodyComponent::SetHand(PlayerHandedness hand, PlayerHandComponentPtr comp) {
	if (hand == PlayerHandedness::LeftHand) {
		if (hands[0])
			return false;
		hands[0] = comp;
	}
	else if (hand == PlayerHandedness::RightHand) {
		if (hands[1])
			return false;
		hands[1] = comp;
	}
	else return false;
	
	return true;
}

bool PlayerBodyComponent::SetHead(PlayerHeadComponentPtr head) {
	if (this->head)
		return false;
	this->head = head;
	
	return true;
}






bool PlayerBodySystem::Initialize(const WorldState& ws) {
	if (!InteractionListener::Initialize(GetEngine(), this))
		return false;
	
	return true;
}

void PlayerBodySystem::Uninitialize() {
	InteractionListener::Uninitialize(GetEngine(), this);
	
}

void PlayerBodySystem::Update(double dt) {
	for (PlayerBodyComponentPtr& b : bodies) {
		EntityPtr e = b->GetEntity();
		TransformPtr trans = e->Find<Transform>();
		if (!trans)
			continue;
		
		float eyes_from_height = 0.15f; // guesstimate, that height of eyes is 15cm from your total height
		float hand_from_height = 0.4f; // guesstimate
		
		vec3 body_feet_pos = trans->data.position;
		vec3 head_direction = VEC_FWD;
		vec3 head_up = VEC_UP;
		vec3 axes(0,0,0);
		TransformMatrix tm;
		
		if (b->head) {
			vec3 head_rel_pos(0, b->height - eyes_from_height, 0);
			vec3 head_pos = body_feet_pos + head_rel_pos;
			ASSERT(head_pos[1] > 0.0f);
			
			TransformPtr head_trans = b->head->GetEntity()->val.Find<Transform>();
			if (head_trans) {
				TransformMatrix& t = head_trans->data;
				head_trans->anchor_position = head_pos;
				t.position = head_trans->anchor_position + head_trans->relative_position;
				head_pos = t.position,
				tm = t;
				
				axes = t.axes;
				head_up = t.up;
				head_direction = head_trans->GetForwardDirection();
				ASSERT(t.up.GetLength() > 0);
			}
		}
		
		for(int i = 0; i < 2; i++) {
			if (b->hands[i]) {
				TransformPtr hand_trans = b->hands[i]->GetEntity()->val.Find<Transform>();
				if (hand_trans) {
					if (b->hands[i]->is_simulated) {
						hand_trans->data = tm;
						float horz_deg = (i == 1 ? -1.f : +1.f) * 30;
						CameraObject(
							tm.position, head_direction, head_up,
							DEG2RADf(horz_deg), DEG2RADf(-30), 0.3f,
							hand_trans->data.position);
							
					}
					else {
						hand_trans->anchor_position = tm.position;
						hand_trans->anchor_orientation = tm.orientation;
						
					}
				}
			}
		}
		
	}
}

void PlayerBodySystem::RefreshComponentsForSource(const HandLocationSource& source) {
	
	TODO
	
}

void PlayerBodySystem::Attach(PlayerBodyComponentPtr h) {
	VectorFindAdd(bodies, h);
}

void PlayerBodySystem::Detach(PlayerBodyComponentPtr h) {
	VectorRemoveKey(bodies, h);
}

void PlayerBodySystem::OnControllerDetected(const GeomEvent& e) {
	// pass
}

void PlayerBodySystem::OnControllerLost(const GeomEvent& e) {
	// pass
}

void PlayerBodySystem::OnControllerPressed(const GeomEvent& e) {
	// pass
}

void PlayerBodySystem::OnControllerReleased(const GeomEvent& e) {
	// pass
}

void PlayerBodySystem::OnControllerUpdated(const GeomEvent& e) {
	if (e.type == EVENT_HOLO_LOOK) {
		for (PlayerBodyComponentPtr& b : bodies) {
			if (b->head && e.trans) {
				TransformPtr trans = b->head->GetEntity()->val.Find<Transform>();
				if (trans) {
					trans->data = *e.trans;
					trans->relative_position = e.trans->position;
					trans->data.position = trans->anchor_position + trans->relative_position;
				}
				/*auto cam = b->head->GetEntity()->val.Find<CameraBase>();
				if (e.cam->mode == TransformMatrix::MODE_LOOKAT) {
					
				}
				else if (e.cam->mode == TransformMatrix::MODE_AXES) {
					trans->
				}
				else if (e.cam->mode == TransformMatrix::MODE_QUATERNION) {
					
				}
				else if (e.cam->mode == TransformMatrix::MODE_VIEW && e.cam->is_stereo) {
					if (cam) {
						cam->cam = e.cam;
					}
				}
				else TODO*/
					
				#if 0
				TransformPtr trans = b->head->GetEntity()->val.Find<Transform>();
				if (trans) {
					if (!e.spatial->use_lookat) {
						trans->use_lookat = false;
						COPY4(trans->orientation, e.spatial->orient);
					}
					else {
						trans->use_lookat = true;
						COPY3(trans->direction, e.spatial->direction);
						trans->up = VEC_UP;
					}
				}
				#endif
			}
		}
	}
	else if (e.type == EVENT_HOLO_MOVE_FAR_RELATIVE) {
		for (PlayerBodyComponentPtr& b : bodies) {
			TransformPtr trans = b->GetEntity()->val.Find<Transform>();
			if (trans) {
				trans->data.position += e.trans->position;
			}
		}
	}
	else if (e.type == EVENT_HOLO_MOVE_CONTROLLER) {
		PlayerBodyComponentPtr vr_body;
		if (!bodies.IsEmpty()) vr_body = bodies[0];
		
		if (vr_body && e.ctrl) {
			const ControllerMatrix& cm = *e.ctrl;
			
			for(int i = 0; i < 2; i++) {
				if (vr_body->hands[i] && cm.ctrl[i].is_enabled) {
					PlayerHandComponent& hand = *vr_body->hands[i];
					const ControllerMatrix::Ctrl& ctrl = cm.ctrl[i];
					
					TransformPtr trans = hand.GetEntity()->val.Find<Transform>();
					if (trans) {
						trans->data = ctrl.trans;
						
						mat4 arot = QuatMat(trans->anchor_orientation);
						mat4 trot = QuatMat(trans->data.orientation);
						mat4 crot = arot * trot;
						trans->data.orientation = MatQuat(crot);
						trans->data.FillFromOrientation();
						
						vec3 new_position = (arot * trans->data.position.Embed()).Splice();
						trans->data.position = trans->anchor_position + new_position;
						trans->data.mode = TransformMatrix::MODE_QUATERNION;
						
					}
				}
			}
		}
	}
	else if (e.type == EVENT_HOLO_UPDATED) {
		
	}
	else TODO
}


END_UPP_NAMESPACE
