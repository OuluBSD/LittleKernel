#ifndef _IGraphics_TBufferField_h_
#define _IGraphics_TBufferField_h_

NAMESPACE_UPP


template <class Gfx>
struct GfxBufferFieldT {
	
public:
	using Buffer = BufferT<Gfx>;
	using DataState = DataStateT<Gfx>;
	using NativeFrameBufferPtr = typename Gfx::NativeFrameBufferPtr;
	using NativeColorBufferPtr = typename Gfx::NativeColorBufferPtr;
	
	Buffer buf;
	DataState data;
	NativeFrameBufferPtr fb = 0;
	NativeColorBufferPtr clr = 0;
	bool add_data_states = false;
	
public:
	using GfxBufferField = GfxBufferFieldT<Gfx>;
	//RTTI_DECL0(GfxBufferFieldT);
	
	GfxBufferFieldT();
	void Visit(Vis& v) {v VISN(buf);}
	
	void ClearPtr() {buf.ClearPtr();}
	bool Initialize(AtomBase& a, const WorldState& ws);
	bool PostInitialize();
	bool ImageInitialize(bool is_win_fbo, Size screen_sz);
	
	Buffer& GetBuffer() {return buf;}
	bool AcceptsOrder() const {return buf.AcceptsOrders();}
	void Update(double dt) {buf.Update(dt);}
	
	bool IsAudio() const {return buf.IsAudio();}
	
	NativeFrameBufferPtr GetFrame() const {return fb;}
	NativeColorBufferPtr GetColor() const {return clr;}
	
};


END_UPP_NAMESPACE

#endif
