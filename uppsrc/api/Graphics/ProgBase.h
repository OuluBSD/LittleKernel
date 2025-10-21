#ifndef _IGraphics_ProgBase_h_
#define _IGraphics_ProgBase_h_

NAMESPACE_UPP


template <class Gfx>
struct FboProgAtomT :
	public BufferBaseT<Gfx>
{
	using DataState = DataStateT<Gfx>;
	using ModelState = ModelStateT<Gfx>;
	using Material = MaterialT<Gfx>;
	using NativeColorBufferPtr = typename Gfx::NativeColorBufferPtr;
	
    FramebufferT<Gfx> fb;
    Size sz;
	DataState data;
	bool dbg_info = 1;
	int dbg_win_id;
	double resize_multiplier = 0.01;
	bool write_ecs = false;
	
	struct Window : Moveable<Window> {
		DrawCommandImageRenderer rend;
		NativeColorBufferPtr tex;
		int tex_id;
		bool inited = false;
	};
	VectorMap<hash_t, Window> windows;
	
	DrawCommand* ProcessWindow(DrawCommand* cmd);
	void ProcessWindowCommands(DrawCommand* begin, DrawCommand* end);
	
public:
	using BufferBase = BufferBaseT<Gfx>;
	using CLASSNAME = FboProgAtomT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	FboProgAtomT(VfsValue& n) : BufferBase(n) {}
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	bool IsReady(PacketIO& io) override;
	void Finalize(RealtimeSourceConfig& cfg) override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void Visit(Vis& v) override {}
	
};


#if defined flagX11
using X11SwFboProgBase = FboProgAtomT<X11SwGfx>;

#ifdef flagOGL
using X11OglFboProgBase = FboProgAtomT<X11OglGfx>;
#endif

#endif

#if defined flagOGL && defined flagSDL2
using SdlOglFboProgBase = FboProgAtomT<SdlOglGfx>;
#endif

END_UPP_NAMESPACE

#endif
