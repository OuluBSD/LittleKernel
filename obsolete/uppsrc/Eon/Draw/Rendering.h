#ifndef _Eon_Draw_Rendering_h_
#define _Eon_Draw_Rendering_h_


struct GfxStateDraw;

struct RendererBase :
	public BinderIfaceVideo
{
	ModelLoader loader;
	String obj;
	TimeStop ts;
	double phase_time = 3.0;
	int iter = 0;
	int frame = 0;
	int phase = 0;
	int phases = 2;
	bool use_pbr = false;
	Size sz;
	
	bool have_skybox = false;
	String skybox_diffuse, skybox_irradiance;
	
	RendererBase();
	void operator=(const RendererBase& t) {Panic("Can't copy RendererBase");}
	void Visit(Vis& v) override {v VISN(loader);}
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Render(Draw& draw) override;
	bool Arg(const String& key, const String& value) override;
	
	void DrawObj(GfxStateDraw& fb, bool use_texture);
	
};


#endif
