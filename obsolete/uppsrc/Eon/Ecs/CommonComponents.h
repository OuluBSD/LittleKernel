#ifndef _Eon_Ecs_CommonComponents_h_
#define _Eon_Ecs_CommonComponents_h_


extern const vec3 EarthGravity;


class Transform : public Component {
	
public:
	ECS_COMPONENT_CTOR(Transform)
	TransformMatrix data;
	vec3 size;
	vec3 relative_position;
	vec3 anchor_position;
	quat anchor_orientation;
	bool verbose = false;
	
	void operator=(const Transform& t);
	mat4 GetMatrix() const;
	vec3 GetForwardDirection() const;
	
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	String ToString() const override;
	
};

typedef Ptr<Transform> TransformPtr;


void CopyTransformPos(EntityPtr from, EntityPtr to);

class Transform2D : public Component {
	
public:
	ECS_COMPONENT_CTOR(Transform2D)
	vec2 position = zero<vec2>();
	vec2 size = one<vec2>();
	
	void operator=(const Transform2D& t);
	
	void Visit(Vis& v) override;
	
};


#endif
