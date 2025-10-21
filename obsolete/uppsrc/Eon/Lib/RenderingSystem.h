#ifndef _EonLib_RenderingSystem_h_
#define _EonLib_RenderingSystem_h_

#if 0

class RenderingSystem :
	public Parallel::System
{
	
protected:
	
	#ifdef flagSDL2
	Parallel::BufferT<SdlSwGfx>* sdl_sw_buf = 0;
	#ifdef flagOGL
	Parallel::BufferT<SdlOglGfx>* sdl_ogl_buf = 0;
	#endif
	#endif
	
	GfxDataState*				state = 0;
	double						time = 0;
	bool						is_dummy = false;
	
	// calibration
	Vector<Gu::GuboManager*> gubo_scopes;
	
protected:
    bool Initialize(const WorldState&) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    
    
    
public:
	using Base = System;
	ATOM_CTOR_(RenderingSystem, Base)
    SYS_CTOR(RenderingSystem)
	SYS_DEF_VISIT
	
	ModelRef GetAddModelFile(String path);
	
	void CalibrationEvent(CtrlEvent& ev);
	
	
	#ifdef flagSDL2
	void Attach(String key, Parallel::BufferT<SdlSwGfx>* b);
	#ifdef flagOGL
	void Attach(String key, Parallel::BufferT<SdlOglGfx>* b);
	void Attach(Gu::GuboManager* b);
	#endif
	#endif
	
};

using RenderingSystemRef = Ptr<RenderingSystem>;


#endif
#endif
