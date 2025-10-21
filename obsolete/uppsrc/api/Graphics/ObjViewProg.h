#ifndef _IGraphics_ObjViewProg_h_
#define _IGraphics_ObjViewProg_h_


NAMESPACE_UPP


struct ObjViewProg :
	public BinderIfaceVideo
{
	//RTTI_DECL1(ObjViewProg, BinderIfaceVideo)
	
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
	
	ObjViewProg();
	void operator=(const ObjViewProg& t) {Panic("Can't copy ObjViewProgT");}
	void Visit(Vis& v) override {v VISN(loader);}
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	bool Render(Draw& draw) override;
	bool Arg(const String& key, const String& value) override;
	
	void DrawObj(GfxStateDraw& fb, bool use_texture);
	
};


END_UPP_NAMESPACE


#endif
