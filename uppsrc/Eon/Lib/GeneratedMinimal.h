#ifndef _EonLib_GeneratedMinimal_h_
#define _EonLib_GeneratedMinimal_h_

// This file is generated. Do not modify this file.

class CenterCustomer : public CustomerBase {

public:
	ATOM_CTOR_(CenterCustomer, CustomerBase)
	//ATOMTYPE(CenterCustomer)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class TestRealtimeSrc : public RollingValueBase {

public:
	ATOM_CTOR_(TestRealtimeSrc, RollingValueBase)
	//ATOMTYPE(TestRealtimeSrc)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class TestRealtimeSink : public VoidSinkBase {

public:
	ATOM_CTOR_(TestRealtimeSink, VoidSinkBase)
	//ATOMTYPE(TestRealtimeSink)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class TestPollerSink : public VoidPollerSinkBase {

public:
	ATOM_CTOR_(TestPollerSink, VoidPollerSinkBase)
	//ATOMTYPE(TestPollerSink)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if (!defined flagSYS_PORTAUDIO) || (defined flagPORTAUDIO)
class PortaudioSink : public PortaudioSinkDevice {

public:
	ATOM_CTOR_(PortaudioSink, PortaudioSinkDevice)
	//ATOMTYPE(PortaudioSink)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagFFMPEG
class AudioDecoderSrc : public FfmpegSourceDevice {

public:
	ATOM_CTOR_(AudioDecoderSrc, FfmpegSourceDevice)
	//ATOMTYPE(AudioDecoderSrc)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class AudioDbgSrc : public AudioGenBase {

public:
	ATOM_CTOR_(AudioDbgSrc, AudioGenBase)
	//ATOMTYPE(AudioDbgSrc)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class AudioSplitter : public VoidBase {

public:
	ATOM_CTOR_(AudioSplitter, VoidBase)
	//ATOMTYPE(AudioSplitter)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class AudioSplitterUser : public VoidBase {

public:
	ATOM_CTOR_(AudioSplitterUser, VoidBase)
	//ATOMTYPE(AudioSplitterUser)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class AudioJoiner : public VoidBase {

public:
	ATOM_CTOR_(AudioJoiner, VoidBase)
	//ATOMTYPE(AudioJoiner)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class AudioJoinerUser : public VoidBase {

public:
	ATOM_CTOR_(AudioJoinerUser, VoidBase)
	//ATOMTYPE(AudioJoinerUser)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class AudioJoiner2User : public VoidBase {

public:
	ATOM_CTOR_(AudioJoiner2User, VoidBase)
	//ATOMTYPE(AudioJoiner2User)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

class AudioMixer16 : public AudioMixerBase {

public:
	ATOM_CTOR_(AudioMixer16, AudioMixerBase)
	//ATOMTYPE(AudioMixer16)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if defined flagSCREEN
class VideoDbgSrc : public VideoGenBase {

public:
	ATOM_CTOR_(VideoDbgSrc, VideoGenBase)
	//ATOMTYPE(VideoDbgSrc)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagOPENCV && defined flagLINUX)
class WebcamPipe : public V4L2OpenCVCamera {

public:
	ATOM_CTOR_(WebcamPipe, V4L2OpenCVCamera)
	//ATOMTYPE(WebcamPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagOPENCV && defined flagLINUX)
class WebcamAtom : public V4L2OpenCVCamera {

public:
	ATOM_CTOR_(WebcamAtom, V4L2OpenCVCamera)
	//ATOMTYPE(WebcamAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagFFMPEG
class AudioLoaderAtom : public FfmpegSourceDevice {

public:
	ATOM_CTOR_(AudioLoaderAtom, FfmpegSourceDevice)
	//ATOMTYPE(AudioLoaderAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagFFMPEG
class VideoLoaderAtom : public FfmpegSourceDevice {

public:
	ATOM_CTOR_(VideoLoaderAtom, FfmpegSourceDevice)
	//ATOMTYPE(VideoLoaderAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSCREEN
class EventStatePipe : public EventStateBase {

public:
	ATOM_CTOR_(EventStatePipe, EventStateBase)
	//ATOMTYPE(EventStatePipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwFboProg : public X11SwFboBase {

public:
	ATOM_CTOR_(X11SwFboProg, X11SwFboBase)
	//ATOMTYPE(X11SwFboProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglFboProg : public X11OglFboBase {

public:
	ATOM_CTOR_(X11OglFboProg, X11OglFboBase)
	//ATOMTYPE(X11OglFboProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSDL2
class SdlSwFboProg : public SdlCenterFboSinkDevice {

public:
	ATOM_CTOR_(SdlSwFboProg, SdlCenterFboSinkDevice)
	//ATOMTYPE(SdlSwFboProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglFboProg : public SdlOglFboBase {

public:
	ATOM_CTOR_(SdlOglFboProg, SdlOglFboBase)
	//ATOMTYPE(SdlOglFboProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwShaderPipe : public X11SwShaderBase {

public:
	ATOM_CTOR_(X11SwShaderPipe, X11SwShaderBase)
	//ATOMTYPE(X11SwShaderPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglShaderPipe : public X11OglShaderBase {

public:
	ATOM_CTOR_(X11OglShaderPipe, X11OglShaderBase)
	//ATOMTYPE(X11OglShaderPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglShaderPipe : public SdlOglShaderBase {

public:
	ATOM_CTOR_(SdlOglShaderPipe, SdlOglShaderBase)
	//ATOMTYPE(SdlOglShaderPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwFboAtomPipe : public X11SwSinkDevice {

public:
	ATOM_CTOR_(X11SwFboAtomPipe, X11SwSinkDevice)
	//ATOMTYPE(X11SwFboAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglFboAtomPipe : public X11OglSinkDevice {

public:
	ATOM_CTOR_(X11OglFboAtomPipe, X11OglSinkDevice)
	//ATOMTYPE(X11OglFboAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSDL2
class SdlSwFboAtomPipe : public SdlCenterFboSinkDevice {

public:
	ATOM_CTOR_(SdlSwFboAtomPipe, SdlCenterFboSinkDevice)
	//ATOMTYPE(SdlSwFboAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSDL2 && defined flagOGL)
class SdlOglFboAtomPipe : public SdlOglVideoSinkDevice {

public:
	ATOM_CTOR_(SdlOglFboAtomPipe, SdlOglVideoSinkDevice)
	//ATOMTYPE(SdlOglFboAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSDL2 && defined flagOGL)
class SdlOglProgAtomPipe : public SdlOglVideoSinkDevice {

public:
	ATOM_CTOR_(SdlOglProgAtomPipe, SdlOglVideoSinkDevice)
	//ATOMTYPE(SdlOglProgAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11ContextAtom : public X11Context {

public:
	ATOM_CTOR_(X11ContextAtom, X11Context)
	//ATOMTYPE(X11ContextAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwContextAtom : public X11SwContext {

public:
	ATOM_CTOR_(X11SwContextAtom, X11SwContext)
	//ATOMTYPE(X11SwContextAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglContextAtom : public X11OglContext {

public:
	ATOM_CTOR_(X11OglContextAtom, X11OglContext)
	//ATOMTYPE(X11OglContextAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11EventAtomPipe : public X11EventsBase {

public:
	ATOM_CTOR_(X11EventAtomPipe, X11EventsBase)
	//ATOMTYPE(X11EventAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwEventAtomPipe : public X11SwEventsBase {

public:
	ATOM_CTOR_(X11SwEventAtomPipe, X11SwEventsBase)
	//ATOMTYPE(X11SwEventAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglEventAtomPipe : public X11OglEventsBase {

public:
	ATOM_CTOR_(X11OglEventAtomPipe, X11OglEventsBase)
	//ATOMTYPE(X11OglEventAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagWIN32 && defined flagSCREEN && !defined flagUWP)
class WinContextAtom : public WinContext {

public:
	ATOM_CTOR_(WinContextAtom, WinContext)
	//ATOMTYPE(WinContextAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagWIN32 && defined flagSCREEN && !defined flagUWP)
class WinVideoAtomPipe : public WinSinkDevice {

public:
	ATOM_CTOR_(WinVideoAtomPipe, WinSinkDevice)
	//ATOMTYPE(WinVideoAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class DxCustomer : public CustomerBase {

public:
	ATOM_CTOR_(DxCustomer, CustomerBase)
	//ATOMTYPE(DxCustomer)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if (defined flagWIN32 && defined flagSCREEN && defined flagDX11)
class WinD11ContextAtom : public WinD11Context {

public:
	ATOM_CTOR_(WinD11ContextAtom, WinD11Context)
	//ATOMTYPE(WinD11ContextAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagWIN32 && defined flagDX11)
class WinD11FboProg : public WinD11FboBase {

public:
	ATOM_CTOR_(WinD11FboProg, WinD11FboBase)
	//ATOMTYPE(WinD11FboProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagWIN32 && defined flagSCREEN && defined flagDX11)
class WinD11FboAtomPipe : public WinD11SinkDevice {

public:
	ATOM_CTOR_(WinD11FboAtomPipe, WinD11SinkDevice)
	//ATOMTYPE(WinD11FboAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagWIN32 && defined flagSCREEN && defined flagDX11)
class WinD11FboAtomSA : public WinD11SinkDevice {

public:
	ATOM_CTOR_(WinD11FboAtomSA, WinD11SinkDevice)
	//ATOMTYPE(WinD11FboAtomSA)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class OglCustomer : public CustomerBase {

public:
	ATOM_CTOR_(OglCustomer, CustomerBase)
	//ATOMTYPE(OglCustomer)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if defined flagSDL2
class SdlContextAtom : public SdlContextBase {

public:
	ATOM_CTOR_(SdlContextAtom, SdlContextBase)
	//ATOMTYPE(SdlContextAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSDL2
class SdlEventAtomPipe : public SdlEventsBase {

public:
	ATOM_CTOR_(SdlEventAtomPipe, SdlEventsBase)
	//ATOMTYPE(SdlEventAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class TestEventSrcPipe : public TestEventSrcBase {

public:
	ATOM_CTOR_(TestEventSrcPipe, TestEventSrcBase)
	//ATOMTYPE(TestEventSrcPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglImageLoader : public SdlOglImageBase {

public:
	ATOM_CTOR_(SdlOglImageLoader, SdlOglImageBase)
	//ATOMTYPE(SdlOglImageLoader)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

class VolumeLoaderAtom : public RawByteStaticSource {

public:
	ATOM_CTOR_(VolumeLoaderAtom, RawByteStaticSource)
	//ATOMTYPE(VolumeLoaderAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};

#if (defined flagX11 && defined flagSCREEN)
class X11VideoAtomPipe : public X11SinkDevice {

public:
	ATOM_CTOR_(X11VideoAtomPipe, X11SinkDevice)
	//ATOMTYPE(X11VideoAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglVideoAtomPipe : public X11OglSinkDevice {

public:
	ATOM_CTOR_(X11OglVideoAtomPipe, X11OglSinkDevice)
	//ATOMTYPE(X11OglVideoAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglFboAtomSA : public X11OglSinkDevice {

public:
	ATOM_CTOR_(X11OglFboAtomSA, X11OglSinkDevice)
	//ATOMTYPE(X11OglFboAtomSA)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwVideoAtomPipe : public X11SwSinkDevice {

public:
	ATOM_CTOR_(X11SwVideoAtomPipe, X11SwSinkDevice)
	//ATOMTYPE(X11SwVideoAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwFboAtomSA : public X11SwSinkDevice {

public:
	ATOM_CTOR_(X11SwFboAtomSA, X11SwSinkDevice)
	//ATOMTYPE(X11SwFboAtomSA)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSDL2 && defined flagOGL)
class SdlOglFboAtomSA : public SdlOglVideoSinkDevice {

public:
	ATOM_CTOR_(SdlOglFboAtomSA, SdlOglVideoSinkDevice)
	//ATOMTYPE(SdlOglFboAtomSA)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSDL2 && defined flagOGL && defined flagGUI)
class SdlUppOglDeviceSA : public SdlUppOglDevice {

public:
	ATOM_CTOR_(SdlUppOglDeviceSA, SdlUppOglDevice)
	//ATOMTYPE(SdlUppOglDeviceSA)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSDL2 && defined flagOGL)
class SdlOglFboPipe : public SdlOglVideoSinkDevice {

public:
	ATOM_CTOR_(SdlOglFboPipe, SdlOglVideoSinkDevice)
	//ATOMTYPE(SdlOglFboPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSDL2 && defined flagOGL)
class SdlOglFboAtom : public SdlOglVideoSinkDevice {

public:
	ATOM_CTOR_(SdlOglFboAtom, SdlOglVideoSinkDevice)
	//ATOMTYPE(SdlOglFboAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSDL2 && defined flagGUI)
class SdlUppEventsBasePipe : public SdlUppEventsBase {

public:
	ATOM_CTOR_(SdlUppEventsBasePipe, SdlUppEventsBase)
	//ATOMTYPE(SdlUppEventsBasePipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSDL2
class SdlVideoAtomPipe : public SdlCenterVideoSinkDevice {

public:
	ATOM_CTOR_(SdlVideoAtomPipe, SdlCenterVideoSinkDevice)
	//ATOMTYPE(SdlVideoAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSDL2
class SdlProgAtomPipe : public SdlCenterVideoSinkDevice {

public:
	ATOM_CTOR_(SdlProgAtomPipe, SdlCenterVideoSinkDevice)
	//ATOMTYPE(SdlProgAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11ProgAtomPipe : public X11SinkDevice {

public:
	ATOM_CTOR_(X11ProgAtomPipe, X11SinkDevice)
	//ATOMTYPE(X11ProgAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN)
class X11SwFboGuiProg : public X11SwFboProgBase {

public:
	ATOM_CTOR_(X11SwFboGuiProg, X11SwFboProgBase)
	//ATOMTYPE(X11SwFboGuiProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagX11 && defined flagSCREEN && defined flagOGL)
class X11OglFboGuiProg : public X11OglFboProgBase {

public:
	ATOM_CTOR_(X11OglFboGuiProg, X11OglFboProgBase)
	//ATOMTYPE(X11OglFboGuiProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglFboGuiProg : public SdlOglFboProgBase {

public:
	ATOM_CTOR_(SdlOglFboGuiProg, SdlOglFboProgBase)
	//ATOMTYPE(SdlOglFboGuiProg)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSDL2
class SdlVideoAtom : public SdlCenterVideoSinkDevice {

public:
	ATOM_CTOR_(SdlVideoAtom, SdlCenterVideoSinkDevice)
	//ATOMTYPE(SdlVideoAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSDL2
class SdlAudioAtom : public SdlAudioSinkDevice {

public:
	ATOM_CTOR_(SdlAudioAtom, SdlAudioSinkDevice)
	//ATOMTYPE(SdlAudioAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglShaderAtom : public SdlOglShaderBase {

public:
	ATOM_CTOR_(SdlOglShaderAtom, SdlOglShaderBase)
	//ATOMTYPE(SdlOglShaderAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglShaderAtomSA : public SdlOglShaderBase {

public:
	ATOM_CTOR_(SdlOglShaderAtomSA, SdlOglShaderBase)
	//ATOMTYPE(SdlOglShaderAtomSA)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglTextureSource : public SdlOglTextureBase {

public:
	ATOM_CTOR_(SdlOglTextureSource, SdlOglTextureBase)
	//ATOMTYPE(SdlOglTextureSource)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglVolumeSource : public SdlOglTextureBase {

public:
	ATOM_CTOR_(SdlOglVolumeSource, SdlOglTextureBase)
	//ATOMTYPE(SdlOglVolumeSource)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglAudioSink : public SdlOglFboReaderBase {

public:
	ATOM_CTOR_(SdlOglAudioSink, SdlOglFboReaderBase)
	//ATOMTYPE(SdlOglAudioSink)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglKeyboardSource : public SdlOglKeyboardBase {

public:
	ATOM_CTOR_(SdlOglKeyboardSource, SdlOglKeyboardBase)
	//ATOMTYPE(SdlOglKeyboardSource)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagSCREEN && defined flagSDL2 && defined flagOGL)
class SdlOglAudioSource : public SdlOglAudioBase {

public:
	ATOM_CTOR_(SdlOglAudioSource, SdlOglAudioBase)
	//ATOMTYPE(SdlOglAudioSource)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#endif
