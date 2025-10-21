#ifndef _Eon_Ecs_EasingSystem_h_
#define _Eon_Ecs_EasingSystem_h_

	
class Easing :
	public Component
{
	
public:
	ECS_COMPONENT_CTOR(Easing)
    vec3 target_position = { 0,0,0 };
    quat target_orientation = Identity<quat>();
    float position_easing_factor = 0;
    float orientation_easing_factor = 0;
    
    void operator=(const Easing& e) {
        target_position = e.target_position;
        target_orientation = e.target_orientation;
        position_easing_factor = e.position_easing_factor;
        orientation_easing_factor = e.orientation_easing_factor;
    }
    
	void Visit(Vis& v) override;
    bool Initialize(const WorldState&) override;
    void Uninitialize() override;
    
};

using EasingPtr = Ptr<Easing>;




#endif
