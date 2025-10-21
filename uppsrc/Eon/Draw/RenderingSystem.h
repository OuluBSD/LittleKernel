#ifndef _Eon_Draw_RenderingSystem_h_
#define _Eon_Draw_RenderingSystem_h_


class VirtualGui;


class RenderingSystem :
	public System
{
	
protected:
	friend class VirtualGui;
	
	#ifdef flagSDL2
	BufferT<SdlSwGfx>* sdl_sw_buf = 0;
	#ifdef flagOGL
	BufferT<SdlOglGfx>* sdl_ogl_buf = 0;
	#endif
	#endif
	Vector<RenderablePtr>		rends;
	Vector<ViewablePtr>			views;
	Vector<ModelComponentPtr>	models;
	Vector<CameraBase*>			cams;
	
	double						time = 0;
	bool						is_dummy = false;
	
	// calibration
	CalibrationData				calib;
	
	//ArrayMap<String, ModelLoader> model_cache;
	
protected:
    bool Initialize(const WorldState&) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    bool Arg(String key, Value value) override;
    
    
    
public:
	using Base = System;
    ECS_SYS_CTOR(RenderingSystem)
	ECS_SYS_DEF_VISIT
	
	//ModelPtr GetAddModelFile(String path);
	
	void Render(GfxDataState& s);
	
	void AddViewable(ViewablePtr v);
	void AddRenderable(RenderablePtr b);
	void AddModel(ModelComponentPtr m);
	void AddCamera(CameraBase& c);
	
	void RemoveViewable(ViewablePtr v);
	void RemoveRenderable(RenderablePtr b);
	void RemoveModel(ModelComponentPtr m);
	void RemoveCamera(CameraBase& c);
	
	void CalibrationEvent(GeomEvent& ev);
	
	
	#ifdef flagSDL2
	void Attach(String key, BufferT<SdlSwGfx>* b);
	#ifdef flagOGL
	void Attach(String key, BufferT<SdlOglGfx>* b);
	#endif
	#endif
	
};

using RenderingSystemPtr = Ptr<RenderingSystem>;


#endif
