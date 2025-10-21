#ifndef _IGraphics_Types_h_
#define _IGraphics_Types_h_



NAMESPACE_UPP


#define GFX_CLS_LIST(g) \
	GFX_CLS(VertexShaderArgs, g) \
	GFX_CLS(FragmentShaderArgs, g) \
	GFX_CLS(DataObject, g) \
	GFX_CLS(Compiler, g) \
	GFX_CLS(Linker, g) \
	GFX_CLS(ShaderPipeline, g) \
	GFX_CLS(Renderer, g) \
	GFX_CLS(StateDraw, g) \
	GFX_CLS(Buffer, g) \
	GFX_CLS(BufferStage, g) \


#if defined flagOGL && defined flagEMSCRIPTEN
#define GFX_RENDSYS_LIST \
	GFX_RSYS(SdlOgl) \

#elif defined flagOGL && defined flagPOSIX
#define GFX_RENDSYS_LIST \
	GFX_RSYS(SdlOgl) \
	GFX_RSYS(X11Ogl) \
	GFX_RSYS(X11Sw) \

#elif defined flagOGL && defined flagWIN32
#define GFX_RENDSYS_LIST \
	GFX_RSYS(SdlSw) \
	GFX_RSYS(SdlOgl) \

#elif defined flagSDL2
#define GFX_RENDSYS_LIST \
	GFX_RSYS(SdlSw) \

#elif defined flagPOSIX
#define GFX_RENDSYS_LIST \
	GFX_RSYS(X11Sw) \

#elif defined flagDX11
#define GFX_RENDSYS_LIST \
	GFX_RSYS(WinD11) \

#else
	#define GFX_RENDSYS_LIST
	#if defined flagCLANG || defined flagGCC
		#warning no render system in IGraphics
	#endif
#endif


#define GFX_CLS(x, g) template <class Gfx> struct x##T;
GFX_CLS_LIST(_)
#undef GFX_CLS

#define GFX_RSYS(x) struct x##Gfx;
GFX_RENDSYS_LIST
#undef GFX_RSYS


struct GfxFramebuffer;
struct GfxBuffer;
template <class Gfx> struct FramebufferT;



template <class Gfx>
struct SwGfxT {
	using NativeColorBufferPtr		= ByteImage*;
	using NativeColorBufferConstPtr	= ConstByteImage*;
	using NativeDepthBufferPtr		= DepthImage*;
	using NativeDepthBufferConstPtr	= ConstDepthImage*;
	
	using FramebufferBase	= GfxFramebuffer;
	
	
	struct Thread {
		SoftRend rend;
		SoftCompiler comp;
		SoftShader* shdr = 0;
		SoftProgram* prog = 0;
		SoftFramebuffer* fb = 0;
		SoftFramebuffer* ctx_default_fb = 0;
		DepthImage* depth = 0;
		SoftPipeline* pipe = 0;
		SoftVertexArray* vao = 0;
		
		struct Tex {
			NativeColorBufferConstPtr r = 0;
			NativeColorBufferPtr rw = 0;
			
			NativeColorBufferConstPtr GetReadTexture() const {return r ? r : rw;}
		};
		Tex texture[CHANNEL_COUNT];
		int active_texture = -1;
		
		Tex& T() {
			ASSERT(active_texture >= 0 && active_texture < CHANNEL_COUNT);
			return texture[active_texture];
		}
	};
	
	// not here: using NativeTexture = uint32;
	using NativeShaderPtr = SoftShader*;
	using NativeFrameBufferPtr = SoftFramebuffer*;
	using NativeFrameBufferConstPtr = const SoftFramebuffer*;
	using NativeVertexArray = SoftVertexArray;
	using NativeVertexBuffer = SoftVertexBuffer;
	using NativeElementBuffer = SoftElementBuffer;
	using NativeProgram = SoftProgram;
	using NativePipeline = SoftPipeline;
	using ValFormat = VideoFormat;
	using Impl = EmptyClass;
	
	static const GVar::GfxType Type = GVar::SW;
	
	#include "TypeFuncList.inl"
	
	static VideoFormat& GetFormat(ValueFormat& fmt) {return fmt.vid;}
	
	static Thread& Local();
	static SoftRend& Rend();
	
	static NativeColorBufferConstPtr GetLocalTexture(int ch);
	
	static const bool is_builtin_shader = true;
	
};

#ifdef flagOGL

struct OglFramebufferBase;
struct OglBufferBase;
	
template <class Gfx>
struct OglGfxT {
	static const bool is_builtin_shader = false;
	
	using NativeTexture = GLuint;
	using NativeShaderPtr = GLuint;
	using NativeColorBufferPtr = GLuint;
	using NativeColorBufferConstPtr = GLuint;
	using NativeDepthBufferPtr = GLuint;
	using NativeDepthBufferConstPtr = GLuint;
	using NativeFrameBufferPtr = GLuint;
	using NativeFrameBufferConstPtr = GLuint;
	using NativeBuffer = GLuint;
	using SystemFrameBufferPtr = NativeFrameBufferPtr;
	using NativeVertexArray = GLuint;
	using NativeVertexBuffer = GLuint;
	using NativeElementBuffer = GLuint;
	using NativeProgram = GLuint;
	using NativePipeline = GLuint;
	using ValFormat = FboFormat;
	using Impl = EmptyClass;
	
	using FramebufferBase = OglFramebufferBase;
	using BufferBase = OglBufferBase;
	
	static const GVar::GfxType Type = GVar::OGL;
	
	#include "TypeFuncList.inl"
	
	static FboFormat& GetFormat(ValueFormat& fmt);
	
};
#endif








#if defined flagX11
struct X11Gfx {
	using NativeDisplay			= ::Display*;
	using NativeWindow			= ::Window;
	using NativeRenderer		= void*;
	using NativeRendererInfo	= void*;
	
	static Size GetWindowSize(NativeWindow& win);
	static bool CreateWindowAndRenderer(Size screen_sz, dword flags, NativeWindow& win, NativeRenderer& rend);
	static void SetTitle(NativeDisplay& display, NativeWindow& win, String title);
	static void SetWindowFullscreen(NativeWindow& win, bool b=true);
	static void DestroyRenderer(NativeRenderer& rend);
	static void ClearRendererPtr(NativeRenderer& rend) {rend = 0;}
	static void DestroyWindow(NativeWindow& win);
	static void MaximizeWindow(NativeWindow& win);
	static void RestoreWindow(NativeWindow& win);
	static void SetWindowPosition(NativeWindow& win, Point pt);
	static void SetWindowSize(NativeWindow& win, Size sz);
	
	
};

struct X11SwGfx : SwGfxT<X11SwGfx>, X11Gfx {
	using SystemFrameBufferPtr = SoftFramebuffer*;
	using NativeGLContext = void*;
	using BufferBase = GfxBuffer;
	
	using NativeTexture = SystemFrameBufferPtr;
	using NativeSurface = NativeColorBufferPtr;
	
	#define GFX_CLS(x, g) using x = x##T<g##Gfx>;
	GFX_CLS_LIST(X11Sw)
	#undef GFX_CLS
	
	
	static void DeleteContext(NativeGLContext& ctx);
	static void ActivateNextFrame(NativeDisplay& d, NativeWindow& w, NativeRenderer& r, NativeColorBufferPtr color_buf);
	
	static bool LockTextureToSurface(NativeTexture& tex, Rect r, NativeSurface& surf);
	static void UnlockTextureToSurface(NativeTexture& tex);
	static void QueryTexture(NativeTexture& tex, uint32& fmt, int& access, int& w, int& h);
	
	static int GetBytesPerPixel(NativeSurface& surf);
	static int GetPitch(NativeSurface& surf);
	static byte* GetData(NativeSurface& surf);
	
};

#ifdef flagOGL
struct X11OglGfx : OglGfxT<X11OglGfx>, X11Gfx {
	#define GFX_CLS(x, g) using x = x##T<g##Gfx>;
	GFX_CLS_LIST(X11Ogl)
	#undef GFX_CLS
	
	using NativeGLContext = ::GLXContext;
	using SoftShaderLibrary = DummySoftShaderLibrary;
	
	static void DeleteContext(NativeGLContext& ctx);
	static void ActivateNextFrame(NativeDisplay& d, NativeWindow& w, NativeRenderer& r, NativeColorBufferPtr color_buf);
	
};
#endif
#endif

#if defined flagX11
	#define X11SW_GFXTYPE GFXTYPE(X11Sw)
	#define X11SW_EXCPLICIT_INITIALIZE_CLASS(x) template struct x <X11SwGfx>;
#else
	#define X11SW_GFXTYPE
	#define X11SW_EXCPLICIT_INITIALIZE_CLASS(x)
#endif

#if defined flagX11 && defined flagOGL
	#define X11OGL_GFXTYPE GFXTYPE(X11Ogl)
	#define X11OGL_EXCPLICIT_INITIALIZE_CLASS(x) template struct x <X11OglGfx>;
#else
	#define X11OGL_GFXTYPE
	#define X11OGL_EXCPLICIT_INITIALIZE_CLASS(x)
#endif



// used only for FramebufferT direct video
struct CpuGfx {
	using FramebufferBase	= GfxFramebuffer;
	
	using NativeColorBufferPtr	= void*;
	using NativeDepthBufferPtr	= void*;
	using NativeFrameBufferPtr	= void*;
	using NativeFrameBufferConstPtr	= void*;
	using SystemFrameBufferPtr	= void*;
	
};

#ifdef flagSDL2
struct SdlGfx {
	using NativeDisplay			= void*;
	using NativeWindow			= SDL_Window*;
	using NativeRenderer		= SDL_Renderer*;
	using NativeRendererInfo	= SDL_RendererInfo;
	using NativeGLContext		= SDL_GLContext;
	
	static Size GetWindowSize(NativeWindow& win);
	static bool CreateWindowAndRenderer(Size screen_sz, dword flags, NativeWindow& win, NativeRenderer& rend);
	static void SetTitle(NativeDisplay& display, NativeWindow& win, String title);
	static void SetWindowFullscreen(NativeWindow& win, bool b=true);
	static void DestroyRenderer(NativeRenderer& rend);
	static void ClearRendererPtr(NativeRenderer& rend) {rend = 0;}
	static void DestroyWindow(NativeWindow& win);
	static void DeleteContext(NativeGLContext& ctx);
	static void MaximizeWindow(NativeWindow& win);
	static void RestoreWindow(NativeWindow& win);
	static void SetWindowPosition(NativeWindow& win, Point pt);
	static void SetWindowSize(NativeWindow& win, Size sz);
	
	
	
};


// used only for FramebufferT direct video
struct SdlCpuGfx : CpuGfx, SdlGfx {
	using SystemFrameBufferPtr = SDL_Texture*;
	
	using NativeTexture = SDL_Texture*;
	using NativeSurface = SDL_Surface*;
	using NativeColorBufferPtr = NativeTexture;
	using NativeColorBufferConstPtr = NativeTexture;
	
	#define GFX_CLS(x, g) using x = x##T<g##Gfx>;
	GFX_CLS_LIST(SdlCpu)
	#undef GFX_CLS
	
	static void SetContextDefaultFramebuffer(NativeFrameBufferPtr fb) {}
	static void ActivateNextFrame(NativeDisplay& d, NativeWindow& w, NativeRenderer& r, NativeColorBufferPtr color_buf);
	
	static int GetBytesPerPixel(NativeSurface& surf);
	static int GetPitch(NativeSurface& surf);
	static byte* GetData(NativeSurface& surf);
	
	static bool LockTextureToSurface(NativeTexture& tex, Rect r, NativeSurface& surf);
	static void QueryTexture(NativeTexture& tex, uint32& fmt, int& access, int& w, int& h);
	static void UnlockTextureToSurface(NativeTexture& tex);
	
	static void ClearFramebufferPtr(NativeTexture& fb) {fb = 0;}
	static void ClearFramebufferPtr(NativeFrameBufferPtr& fb) {fb = 0;}
	static void ClearColorBufferPtr(NativeColorBufferPtr& b) {b = 0;}
	static void ClearDepthBufferPtr(NativeDepthBufferPtr& b) {b = 0;}
	
};

struct SdlSwGfx : SwGfxT<SdlSwGfx>, SdlGfx {
	using SystemFrameBufferPtr = SDL_Texture*;
	
	using NativeTexture = SDL_Texture*;
	using NativeSurface = SDL_Surface*;
	
	#define GFX_CLS(x, g) using x = x##T<g##Gfx>;
	GFX_CLS_LIST(SdlSw)
	#undef GFX_CLS
	
	static void ActivateNextFrame(NativeDisplay& d, NativeWindow& w, NativeRenderer& r, NativeColorBufferPtr color_buf);
	
	static int GetBytesPerPixel(NativeSurface& surf);
	static int GetPitch(NativeSurface& surf);
	static byte* GetData(NativeSurface& surf);
	
	static bool LockTextureToSurface(SoftFramebuffer* tex, Rect r, NativeSurface& surf);
	static void QueryTexture(SoftFramebuffer* tex, uint32& fmt, int& access, int& w, int& h);
	static void UnlockTextureToSurface(SoftFramebuffer* tex);
	
	static void ClearFramebufferPtr(NativeTexture& fb);
	static void ClearFramebufferPtr(NativeFrameBufferPtr& fb);
	
};

#ifdef flagOGL
struct SdlOglGfx : OglGfxT<SdlOglGfx>, SdlGfx {
	using NativeSurface = void*;
	using SoftShaderLibrary = DummySoftShaderLibrary;
	
	#define GFX_CLS(x, g) using x = x##T<g##Gfx>;
	GFX_CLS_LIST(SdlOgl)
	#undef GFX_CLS
	
	static void ActivateNextFrame(NativeDisplay& d, NativeWindow& w, NativeRenderer& r, NativeColorBufferPtr color_buf);
	
	static bool LockTextureToSurface(NativeTexture& tex, Rect r, NativeSurface& surf);
	static void QueryTexture(NativeTexture& tex, uint32& fmt, int& access, int& w, int& h);
	
};

	#define SDLOGL_GFXTYPE GFXTYPE(SdlOgl)
	#define SDLOGL_EXCPLICIT_INITIALIZE_CLASS(x) template struct x <SdlOglGfx>;
#else
	#define SDLOGL_GFXTYPE
	#define SDLOGL_EXCPLICIT_INITIALIZE_CLASS(x)
#endif

#define SDL_GFXTYPE \
	SDLOGL_GFXTYPE \
	GFXTYPE(SdlSw)

#define SDLVIDEO_EXCPLICIT_INITIALIZE_CLASS(x) \
	template struct x <SdlCpuGfx>;

#define SDLSW_EXCPLICIT_INITIALIZE_CLASS(x) \
	template struct x <SdlSwGfx>;

#define SDL_EXCPLICIT_INITIALIZE_CLASS(x) \
	SDLSW_EXCPLICIT_INITIALIZE_CLASS(x) \
	SDLOGL_EXCPLICIT_INITIALIZE_CLASS(x) \
	

#else

#define SDL_GFXTYPE
#define SDL_EXCPLICIT_INITIALIZE_CLASS(x)
#define SDLSW_EXCPLICIT_INITIALIZE_CLASS(x)
#define SDLOGL_EXCPLICIT_INITIALIZE_CLASS(x)
#define SDLVIDEO_EXCPLICIT_INITIALIZE_CLASS(x)

#endif












#if defined flagDX11
using namespace Microsoft::WRL;

struct Dx11FramebufferBase;
struct Dx11BufferBase;
struct D11Shader;

struct D11GfxImpl {
    D3D11_TEXTURE2D_DESC			m_bbDesc;
    ComPtr<ID3D11RenderTargetView>	m_pRenderTarget;
    
    
};

template <class Gfx>
struct D11GfxT {
	
	static const bool is_builtin_shader = false;
	
	using NativeDevicePtr			= ComPtr<ID3D11Device>;
	using NativeDeviceContextPtr	= ID3D11DeviceContext3 *;
	using NativeShaderResourcesPtr	= ComPtr<ID3D11ShaderResourceView>;
	using NativeBlendStatePtr		= ComPtr<ID3D11BlendState>;
	using NativeSamplerStatePtr		= ComPtr <ID3D11SamplerState>;
	using NativeBufferPtr			= ComPtr <ID3D11Buffer>;
	using NativeInputLayoutPtr		= ComPtr <ID3D11InputLayout>;
	using NativeVertexShaderPtr		= ComPtr <ID3D11VertexShader>;
	using NativeGeometryShaderPtr	= ComPtr <ID3D11GeometryShader>;
	using NativePixelShaderPtr		= ComPtr <ID3D11PixelShader>;
	using FramebufferBase			= Dx11FramebufferBase;
	using BufferBase				= Dx11BufferBase;
	
    using NativeTexture				=       ComPtr<ID3D11Texture2D>;
    using NativeShaderPtr             =       D11Shader*;
    using NativeColorBufferPtr		=       ComPtr<ID3D11Texture2D>;
    using NativeColorBufferConstPtr	= const ComPtr<ID3D11Texture2D>;
    using NativeDepthBufferPtr		=       ComPtr<ID3D11DepthStencilView>;
    using NativeDepthBufferConstPtr	= const ComPtr<ID3D11DepthStencilView>;
    using NativeFrameBufferPtr		=       ComPtr<IDXGISwapChain>;
    using NativeFrameBufferConstPtr	= const ComPtr<IDXGISwapChain>;
    using NativeBuffer = uint32;
    using SystemFrameBufferPtr		= NativeFrameBufferPtr;
    using NativeVertexArray = uint32;
    using NativeVertexBuffer = uint32;
    using NativeElementBuffer = uint32;
    using NativeProgram = uint32;
    using NativePipeline = uint32;
    using ValFormat = Serial::FboFormat;
    using Impl = D11GfxImpl;

	static const GVar::GfxType Type = GVar::DX;
	
	
	#include "TypeFuncList.inl"
	
	static Serial::FboFormat& GetFormat(ValueFormat& fmt);
	
};

struct DxGfx {
	using NativeDisplay			= ComPtr<ID3D11Device>;
	using NativeWindow			= HWND;
	using NativeRenderer		= ComPtr<ID3D11DeviceContext>;
	using NativeRendererInfo	= void*;
	using NativeGLContext		= void*;
	
	static Size GetWindowSize(NativeWindow& win);
	static bool CreateWindowAndRenderer(Size screen_sz, dword flags, NativeWindow& win, NativeRenderer& rend);
	static void SetTitle(NativeDisplay& display, NativeWindow& win, String title);
	static void SetWindowFullscreen(NativeWindow& win, bool b=true);
	static void DestroyRenderer(NativeRenderer& rend);
	static void ClearRendererPtr(NativeRenderer& rend);
	static void DestroyWindow(NativeWindow& win);
	static void DeleteContext(NativeGLContext& ctx);
	static void MaximizeWindow(NativeWindow& win);
	static void RestoreWindow(NativeWindow& win);
	static void SetWindowPosition(NativeWindow& win, Point pt);
	static void SetWindowSize(NativeWindow& win, Size sz);
	
	
	
};


struct WinD11Gfx : D11GfxT<WinD11Gfx>, DxGfx {
	using NativeSurface = void*;
	using SoftShaderLibrary = DummySoftShaderLibrary;
	using NativeColorBufferPtr = typename D11GfxT<WinD11Gfx>::NativeColorBufferPtr;
	
	#define GFX_CLS(x, g) using x = x##T<g##Gfx>;
	GFX_CLS_LIST(WinD11)
	#undef GFX_CLS
	
	static void ActivateNextFrame(NativeDisplay& d, NativeWindow& w, NativeRenderer& r, NativeColorBufferPtr color_buf);
	
};

#define WINDX_GFXTYPE GFXTYPE(WinD11)
#define WINDX_EXCPLICIT_INITIALIZE_CLASS(x) template struct x <WinD11Gfx>;

#define WIN_GFXTYPE \
	WINDX_GFXTYPE

#define WIN_EXCPLICIT_INITIALIZE_CLASS(x) \
	WINDX_EXCPLICIT_INITIALIZE_CLASS(x) \
	

#else

#define WINDX_GFXTYPE
#define WINDX_EXCPLICIT_INITIALIZE_CLASS(x)
#define WIN_GFXTYPE
#define WINDX_EXCPLICIT_INITIALIZE_CLASS(x)
#define WIN_EXCPLICIT_INITIALIZE_CLASS(x)

#endif














#define GFXTYPE_LIST \
	 \
	X11SW_GFXTYPE \
	X11OGL_GFXTYPE \
	SDL_GFXTYPE \
	WIN_GFXTYPE \


#define GFX_EXCPLICIT_INITIALIZE_CLASS(x) \
	 \
	X11SW_EXCPLICIT_INITIALIZE_CLASS(x) \
	X11OGL_EXCPLICIT_INITIALIZE_CLASS(x) \
	SDL_EXCPLICIT_INITIALIZE_CLASS(x) \
	WIN_EXCPLICIT_INITIALIZE_CLASS(x) \


#define GFX3D_EXCPLICIT_INITIALIZE_CLASS(x) \
	 \
	X11SW_EXCPLICIT_INITIALIZE_CLASS(x) \
	X11OGL_EXCPLICIT_INITIALIZE_CLASS(x) \
	SDLSW_EXCPLICIT_INITIALIZE_CLASS(x) \
	SDLOGL_EXCPLICIT_INITIALIZE_CLASS(x) \
	WINDX_EXCPLICIT_INITIALIZE_CLASS(x) \


#define SOFTREND_EXCPLICIT_INITIALIZE_CLASS(x) \
	 \
	SDLSW_EXCPLICIT_INITIALIZE_CLASS(x) \
	X11SW_EXCPLICIT_INITIALIZE_CLASS(x) \


#define GFXVIDEO_EXCPLICIT_INITIALIZE_CLASS(x) \
	 \
	SDLVIDEO_EXCPLICIT_INITIALIZE_CLASS(x) \


END_UPP_NAMESPACE

#endif
