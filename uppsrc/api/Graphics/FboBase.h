#ifndef _IGraphics_FboBase_h_
#define _IGraphics_FboBase_h_

NAMESPACE_UPP


class GfxAtomBase : public Atom {
	
	
public:
	using Atom::Atom;
	GfxAtomBase(VfsValue& n) : Atom(n) {}
	
};


template <class Gfx>
struct FboAtomT :
	GfxAtomBase
{
	using StateDraw = StateDrawT<Gfx>;
	using Framebuffer = FramebufferT<Gfx>;
	using DataState = DataStateT<Gfx>;
	using PipelineState = PipelineStateT<Gfx>;
	using ProgramState = ProgramStateT<Gfx>;
	
	Vector<BinderIfaceVideo*> binders;
	One<BinderIfaceVideo>	own_binder;
	String					target;
	EnvStatePtr				state;
	int						prev_iter = -1;
	ValDevCls				src_type;
	One<ImagePainter>		id;
	StateDraw				accel_sd;
	Framebuffer				cpu_fb;
	bool					draw_mem = false;
	RealtimeSourceConfig*	last_cfg = 0;
	String					program;
	DataState				data;
	One<BinderIfaceVideo>	prog;
	WorldState				ws_at_init;
	
	static FboAtomT*	latest;
	
public:
	using CLASSNAME = FboAtomT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	FboAtomT(VfsValue& n);
	
	bool			Initialize(const WorldState& ws) override;
	bool			PostInitialize() override;
	void			Uninitialize() override;
	bool			IsReady(PacketIO& io) override;
	bool			Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void			Visit(Vis& v) override {v & state; v VISN(data); VIS_THIS(Atom);}
	bool			Recv(int sink_ch, const Packet& in) override;
	void			Finalize(RealtimeSourceConfig& cfg) override;
	RealtimeSourceConfig* GetConfig() override {return last_cfg;}
	
	void AddBinder(BinderIfaceVideo* iface);
	void RemoveBinder(BinderIfaceVideo* iface);
	
	static Callback1<FboAtomT<Gfx>*>	WhenInitialize;
	static FboAtomT& Latest();
	
};


#if defined flagX11
using X11SwFboBase = FboAtomT<X11SwGfx>;

#ifdef flagOGL
using X11OglFboBase = FboAtomT<X11OglGfx>;
#endif

#endif

#if defined flagOGL && defined flagSDL2
using SdlOglFboBase = FboAtomT<SdlOglGfx>;
#endif


#if defined flagDX11 && defined flagWIN32
using WinD11FboBase = FboAtomT<WinD11Gfx>;
#endif


END_UPP_NAMESPACE

#endif
