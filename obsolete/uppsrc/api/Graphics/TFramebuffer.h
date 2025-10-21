#ifndef _IGraphics_TFramebuffer_h_
#define _IGraphics_TFramebuffer_h_

NAMESPACE_UPP


template <class Gfx> struct InputStateT;


template <class Gfx>
struct FramebufferT : Gfx::FramebufferBase {
	using Base = FramebufferT<Gfx>;
	using NativeColorBufferPtr = typename Gfx::NativeColorBufferPtr;
	using NativeDepthBufferPtr = typename Gfx::NativeDepthBufferPtr;
	using NativeFrameBufferPtr = typename Gfx::NativeFrameBufferPtr;
	using NativeFrameBufferConstPtr = typename Gfx::NativeFrameBufferConstPtr;
	using SystemFrameBufferPtr = typename Gfx::SystemFrameBufferPtr;
	//RTTI_DECL1(FramebufferT, GfxFramebuffer)
	
	NativeColorBufferPtr	color_buf[2];
	NativeDepthBufferPtr	depth_buf[2];
	NativeFrameBufferPtr	frame_buf[2];
	bool locked = false;
	
	
	
	
	
	FramebufferT() {
		for(int i = 0; i < 2; i++) {
			Gfx::ClearColorBufferPtr(color_buf[i]);
			Gfx::ClearDepthBufferPtr(depth_buf[i]);
			Gfx::ClearFramebufferPtr(frame_buf[i]);
		}
	}
	
	NativeFrameBufferConstPtr GetReadFramebuffer() const;
	NativeColorBufferPtr GetActiveColorBuffer();
	
	void Init(NativeColorBufferPtr clr, int w, int h, int stride);
	void Init(NativeFrameBufferPtr fbo, NativeColorBufferPtr clr, int w, int h, int stride);
	
	bool Create(int w, int h, int channels=3) override {TODO}
	void Enter() override {ASSERT(!locked); locked = true;}
	void Leave() override {ASSERT(locked);  locked = false;}
	byte* GetIterator(int x, int y) override {Panic("Not usable: OglFramebuffer::GetIterator"); return 0;}
	void DrawFill(const byte* mem, int sz, int pitch) override;
	
	//void Bind();
	//void Clear();
	//void Render();
	
	
	void SetWindowFbo(bool b=true);
	
};


template <class Gfx>
struct InputStateT : GfxInputState {
	//RTTI_DECL1(InputStateT, GfxInputState)
	using Base = InputStateT<Gfx>;
	using BufferStage = BufferStageT<Gfx>;
	
	const BufferStage* stage = 0;
	
	void Clear() {
		this->GfxInputState::Clear();
		stage = 0;
	}
	
};


END_UPP_NAMESPACE

#endif
