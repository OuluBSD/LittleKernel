#ifndef _EonLib_Gui_h_
#define _EonLib_Gui_h_


#ifdef flagGUI
struct DefaultGuiAppComponent :
	public Component,
	public BinderIfaceVideo,
	public BinderIfaceEvents
{
	//RTTI_DECL2(DefaultGuiAppComponent, ComponentT, BinderIfaceVideo)
	
	Point prev_mouse;
	
	//WindowSystemRef wins;
	Geom2DComponentPtr cw;
	TransformRef trans;
	Transform2DRef trans2;
	
	
	DefaultGuiAppComponent();
	void operator=(const DefaultGuiAppComponent& t) {Panic("Can't copy DefaultGuiAppComponent");}
	
	void Serialize(Stream& e) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	void Visit(Vis& v) override;
	void Update(double dt) override;
	bool Render(Draw& d) override;
	bool RenderProg(DrawCommand*& begin, DrawCommand*& end) override;
	void Dispatch(const CtrlEvent& state) override;
	bool Arg(const String& key, const String& value) override;
	
	void DrawObj(GfxStateDraw& fb, bool use_texture);
	void StateStartup(GfxDataState& s);
	
	
};
#endif


#endif
