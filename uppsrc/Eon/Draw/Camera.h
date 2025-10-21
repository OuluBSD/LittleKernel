#ifndef _Eon_Draw_Camera_h_
#define _Eon_Draw_Camera_h_


struct GfxShader;

class Viewable :
	public Component
{
	
public:
	ECS_COMPONENT_CTOR(Viewable)
	void Visit(Vis& v) override {}
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	
	void operator=(const Viewable& c) {}
	
	
	Callback1<GfxShader&> cb;
	
};

using ViewablePtr = Ptr<Viewable>;


class Viewport : public Component {
public:
	ECS_COMPONENT_CTOR(Viewport)
	vec3 target = zero<vec3>();
	double fov = M_PI/2;
	double angle = 0;
	
	
	vec3 GetTarget() const {return target;}
	void SetTraget(const vec3& v) {target = v;}
	
	void Visit(Vis& v) override {v VISN(target) VIS_(fov) VIS_(angle);}
	bool Arg(String key, Value value) override;
	
	void operator=(const Viewport& vp) {
		target = vp.target;
	}
	
};

using ViewportPtr = Ptr<Viewport>;

struct CameraBase : Pte<CameraBase>
{
	bool use_stereo = false;
	CalibrationData calib;
	
	mat4 view_stereo[2];
	mat4 proj_stereo[2];
	mat4 mvp_stereo[2];
	
	
	virtual bool Load(GfxDataState& state) = 0;
	virtual void UpdateCalibration() = 0;
	void Visit(Vis& v);
	
};

using CameraBasePtr = Ptr<CameraBase>;

class ChaseCam :
	public Component,
	public CameraBase
{
	TransformPtr trans;
	TransformPtr target;
	ViewablePtr viewable;
	ViewportPtr vport;
	
	mat4 view;
	mat4 projection;
	mat4 port;
	mat4 port_stereo;
	vec2 viewport_sz;
	
	bool test_log = false;
	double time = 0;
	double phase_time = 1.5;
	float fov = 110;
	float used_fov = 0;
	
	
	float GetUsedFov();
	
public:
	ECS_COMPONENT_CTOR(ChaseCam)
	
	void Visit(Vis& v) override {VIS_THIS(Component); v & target & viewable & vport;}
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	void Update(double dt) override;
	bool Arg(String key, Value value) override;
	bool Load(GfxDataState& state) override;
	void UpdateCalibration() override;
	
	void CheckUpdateProjection();
	void UpdateProjection();
	void UpdateView();
	void SetViewportSize(Size sz);
	void SetTarget(TransformPtr tgt) {target = tgt;}
	
	void operator=(const ChaseCam& vp) {
		target = vp.target;
	}
};

using ChaseCamPtr = Ptr<ChaseCam>;


struct CameraPrefab : EntityPrefab<Transform, Viewport, Viewable>
{
    static Components Make(Entity& e, const WorldState& ws)
    {
        auto components = EntityPrefab::Make(e, ws);
		
		TransformPtr t = components.Get<TransformPtr>();
		t->data.mode = TransformMatrix::MODE_POSITION;
		t->data.position[2] = 10.0;
		t->data.position[1] = 3.0;
		t->data.position[0] = 0.0;
		
        return components;
    }
};


#endif
