#ifndef _Eon_Ecs_EcsCommonComponents_h_
#define _Eon_Ecs_EcsCommonComponents_h_


class RigidBody : public Component {
	
public:
	ECS_COMPONENT_CTOR(RigidBody)
	vec3 velocity;
	vec3 acceleration;
	vec3 angular_velocity;
	vec3 angular_acceleration;
	
	float damping_factor;
	
	
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	
    void operator=(const RigidBody& r) {
        velocity = r.velocity;
        acceleration = r.acceleration;
        angular_velocity = r.angular_velocity;
        angular_acceleration = r.angular_acceleration;
        damping_factor = r.damping_factor;
    }
    
	
};

using RigidBodyPtr = Ptr<RigidBody>;







class TextRenderable :
	public Component
{
	
public:
	ECS_COMPONENT_CTOR(TextRenderable)
    String				text = "";
    double				font_size = 60.0;
    
	void Visit(Vis& v) override {
		_VIS_(text)
		 VIS_(font_size);
		VISIT_COMPONENT
	}
    
    
};



#endif
