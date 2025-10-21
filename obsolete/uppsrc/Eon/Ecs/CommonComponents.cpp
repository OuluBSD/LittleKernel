#include "Ecs.h"


NAMESPACE_UPP


const vec3 EarthGravity = { 0, -9.8f, 0 };


bool Transform::Initialize(const WorldState& ws) {
	data.position = zero<vec3>();
	size = one<vec3>();
	data.mode = TransformMatrix::MODE_LOOKAT; // use direction & up instead of orientation
	data.orientation = Identity<quat>();
	data.direction = VEC_FWD; // "look at" alternative to quaternion
	data.up = VEC_UP; // "look at" alternative to quaternion
	
	Ptr<WorldLogicSystem> sys = GetEngine().TryGet<WorldLogicSystem>();
	if (sys)
		sys->Attach(this);
	return true;
}

void Transform::Uninitialize() {
	Ptr<WorldLogicSystem> sys = GetEngine().TryGet<WorldLogicSystem>();
	if (sys)
		sys->Detach(this);
}

void Transform::operator=(const Transform& t) {
	data = t.data;
    size = t.size;
}

void Transform::Visit(Vis& v) {
	v
	 VISN(data)
	 VISN(size)
	 VISN(relative_position)
	 VISN(anchor_position)
	 VISN(anchor_orientation)
	 VIS_(verbose);
	
	VISIT_COMPONENT
}

mat4 Transform::GetMatrix() const {
	return Translate(data.position) * QuatMat(data.orientation) * Scale(size);
}

vec3 Transform::GetForwardDirection() const {
	return data.GetForwardDirection();
}

bool Transform::Arg(String key, Value value) {
	if (key == "x")
		data.position[0] = value;
	else if (key == "y")
		data.position[1] = value;
	else if (key == "z")
		data.position[2] = value;
	else if (key == "cx")
		size[0] = value;
	else if (key == "cy")
		size[1] = value;
	else if (key == "cz")
		size[2] = value;
	else
		return false;
	
	return true;
}

String Transform::ToString() const {
	String s;
	s << "pos" << data.position.ToString() << ", size" << size.ToString() << ", orient" << data.orientation.ToString();
	return s;
}







void Transform2D::Visit(Vis& v) {
	v
	 VISN(position)
	 VISN(size);
	
	VISIT_COMPONENT
}

void Transform2D::operator=(const Transform2D& t) {
    position = t.position;
    size = t.size;
}


END_UPP_NAMESPACE
