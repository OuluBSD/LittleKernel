#include "Draw.h"


NAMESPACE_UPP


bool Renderable::Initialize(const WorldState& ws) {
	Engine& e = GetEngine();
	RenderingSystemPtr rend = e.TryGet<RenderingSystem>();
	if (rend)
		rend->AddRenderable(this);
	
	return true;
}

void Renderable::Uninitialize() {
	Engine& e = GetEngine();
	RenderingSystemPtr rend = e.TryGet<RenderingSystem>();
	if (rend)
		rend->RemoveRenderable(this);
}

void Renderable::Visit(Vis& v) {
	TODO//e % color % offset % alpha_multiplier;
}



void RenderingSystem::AddViewable(ViewablePtr v) {
	ASSERT(v);
	VectorFindAdd(views, v);
}

void RenderingSystem::RemoveViewable(ViewablePtr v) {
	ASSERT(v);
	VectorRemoveKey(views, v);
}

void RenderingSystem::AddRenderable(RenderablePtr b) {
	ASSERT(b);
	VectorFindAdd(rends, b);
}

void RenderingSystem::RemoveRenderable(RenderablePtr b) {
	ASSERT(b);
	VectorRemoveKey(rends, b);
}

void RenderingSystem::AddModel(ModelComponentPtr m) {
	ASSERT(m);
	VectorFindAdd(models, m);
}

void RenderingSystem::RemoveModel(ModelComponentPtr m) {
	ASSERT(m);
	VectorRemoveKey(models, m);
}

void RenderingSystem::AddCamera(CameraBase& c) {
	VectorFindAdd(cams, &c);
}

void RenderingSystem::RemoveCamera(CameraBase& c) {
	VectorRemoveKey(cams, &c);
}


void RenderingSystem::Update(double dt) {
	
}

void RenderingSystem::Render(GfxDataState& state) {
	for (ModelComponentPtr& m : models) {
		
		m->Load(state);
		
	}
	
	for (CameraBase* cb : cams) {
		if (calib.is_enabled) {
			cb->calib = calib;
			cb->UpdateCalibration();
		}
		
		cb->Load(state);
	}
}

bool RenderingSystem::Initialize(const WorldState& ws) {
	return true;
}

bool RenderingSystem::Start() {
	return true;
}

void RenderingSystem::Stop() {
	
}

void RenderingSystem::Uninitialize() {
	ASSERT_(rends.IsEmpty(), "RenderingSystem must be added to Engine before EntityStore");
	
}

bool RenderingSystem::Arg(String key, Value value) {
	
	if (key == "dummy")
		is_dummy = value.ToString() == "true";
	
	return true;
}

void RenderingSystem::CalibrationEvent(GeomEvent& ev) {
	
	if (ev.type == EVENT_HOLO_CALIB) {
		calib.is_enabled = true;
		
		switch (ev.n) {
			case HOLO_CALIB_FOV:		calib.fov += ev.fvalue; break;
			case HOLO_CALIB_SCALE:		calib.scale += ev.fvalue; break;
			case HOLO_CALIB_EYE_DIST:	calib.eye_dist += ev.fvalue; break;
			case HOLO_CALIB_X:			calib.position[0] += ev.fvalue; break;
			case HOLO_CALIB_Y:			calib.position[1] += ev.fvalue; break;
			case HOLO_CALIB_Z:			calib.position[2] += ev.fvalue; break;
			case HOLO_CALIB_YAW:		calib.axes[0] += ev.fvalue; break;
			case HOLO_CALIB_PITCH:		calib.axes[1] += ev.fvalue; break;
			case HOLO_CALIB_ROLL:		calib.axes[2] += ev.fvalue; break;
			default: Panic("invalid holographic calibration subtype");
		}
		
		calib.Dump();
	}
	
}



END_UPP_NAMESPACE
