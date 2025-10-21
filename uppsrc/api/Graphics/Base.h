#ifndef _IGraphics_Base_h_
#define _IGraphics_Base_h_

NAMESPACE_UPP


template <class Gfx>
struct BufferBaseT :
	public Atom
{
	
protected:
	using Buffer = BufferT<Gfx>;
	
	RealtimeSourceConfig* last_cfg = 0;
	
public:
	using Atom::Atom;
	using BufferBase = BufferBaseT<Gfx>;
	//RTTI_DECL1(BufferBaseT, Atom);
	
	void Visit(Vis& v) override {v VISN(bf); VIS_THIS(Atom);}
	void Update(double dt) override {bf.Update(dt);}
	RealtimeSourceConfig* GetConfig() override {return last_cfg;}
	
	Buffer& GetBuffer() {return bf.GetBuffer();}
	
	
	GfxBufferFieldT<Gfx> bf;
};


template <class Gfx>
struct ShaderBaseT :
	public BufferBaseT<Gfx>
{
	
public:
	using ShaderBase = ShaderBaseT<Gfx>;
	using BufferBase = BufferBaseT<Gfx>;
	using CLASSNAME = ShaderBaseT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	
	ShaderBaseT(VfsValue& n) : BufferBase(n) {}
	
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	bool Start() override;
	void Uninitialize() override;
	bool IsReady(PacketIO& io) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	bool Recv(int sink_ch, const Packet& in) override;
	void Finalize(RealtimeSourceConfig& cfg) override;
	void Visit(Vis& v) override {VIS_THIS(BufferBase);}
	
	
};


template <class Gfx>
struct TextureBaseT :
	public BufferBaseT<Gfx>
{
	using Filter = GVar::Filter;
	using Wrap = GVar::Wrap;
	
	bool			loading_cubemap = false;
	Filter			filter = GVar::FILTER_LINEAR;
	Wrap			wrap = GVar::WRAP_REPEAT;
	Array<Packet>	cubemap;
	
public:
	using BufferBase = BufferBaseT<Gfx>;
	using CLASSNAME = TextureBaseT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	
	TextureBaseT(VfsValue& n) : BufferBase(n) {}
	
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	bool IsReady(PacketIO& io) override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void Visit(Vis& v) override;
	bool NegotiateSinkFormat(LinkBase& link, int sink_ch, const ValueFormat& new_fmt) override;
	
	
};



template <class Gfx>
struct FboReaderBaseT :
	public BufferBaseT<Gfx>
{
	using Buffer = BufferT<Gfx>;
	using BufferStage = BufferStageT<Gfx>;
	
	BufferStage* src_buf = 0;
	
public:
	using BufferBase = BufferBaseT<Gfx>;
	using NativeFrameBufferConstPtr = typename Gfx::NativeFrameBufferConstPtr;
	using CLASSNAME = FboReaderBaseT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	
	FboReaderBaseT(VfsValue& n) : BufferBase(n) {}
	
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	bool IsReady(PacketIO& io) override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	bool NegotiateSinkFormat(LinkBase& link, int sink_ch, const ValueFormat& new_fmt) override;
	void Visit(Vis& v) override;
	
};




template <class Gfx>
struct KeyboardBaseT :
	public BufferBaseT<Gfx>
{
	String			target;
	EnvStatePtr		state;
	
	
public:
	using BufferBase = BufferBaseT<Gfx>;
	using CLASSNAME = KeyboardBaseT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	KeyboardBaseT(VfsValue& n) : BufferBase(n) {}
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	bool IsReady(PacketIO& io) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void Visit(Vis& v) override {v & state;}
	
};


template <class Gfx>
struct AudioBaseT :
	public BufferBaseT<Gfx>
{
	
public:
	using BufferBase = BufferBaseT<Gfx>;
	using CLASSNAME = AudioBaseT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	AudioBaseT(VfsValue& n) : BufferBase(n) {}
	
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	bool IsReady(PacketIO& io) override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	bool NegotiateSinkFormat(LinkBase& link, int sink_ch, const ValueFormat& new_fmt) override;
	void Visit(Vis& v) override {}
	
};

#define GFXTYPE(x) \
	using x##ShaderBase = ShaderBaseT<x##Gfx>; \
	using x##TextureBase = TextureBaseT<x##Gfx>; \
	using x##FboReaderBase = FboReaderBaseT<x##Gfx>; \
	using x##KeyboardBase = KeyboardBaseT<x##Gfx>; \
	using x##AudioBase = AudioBaseT<x##Gfx>;
GFXTYPE_LIST
#undef GFXTYPE


END_UPP_NAMESPACE

#endif
