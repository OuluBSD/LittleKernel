// This file have been generated automatically.
// DO NOT MODIFY THIS FILE!

#ifndef _IHal_IHal_h_
#define _IHal_IHal_h_

#include <Eon/Eon.h>
#include <api/Graphics/Graphics.h>

NAMESPACE_UPP

#define HAL_CLS_LIST(x) \
	HAL_CLS(AudioSinkDevice, x) \
	HAL_CLS(CenterVideoSinkDevice, x) \
	HAL_CLS(CenterFboSinkDevice, x) \
	HAL_CLS(OglVideoSinkDevice, x) \
	HAL_CLS(D12VideoSinkDevice, x) \
	HAL_CLS(ContextBase, x) \
	HAL_CLS(EventsBase, x) \
	HAL_CLS(UppEventsBase, x) \
	HAL_CLS(UppOglDevice, x) \

#define HAL_VNDR_LIST \
	HAL_VNDR(HalSdl) \
	HAL_VNDR(HalHolo) \

#define HAL_CLS(x, v) struct v##x;
#define HAL_VNDR(x) HAL_CLS_LIST(x)
HAL_VNDR_LIST
#undef HAL_VNDR
#undef HAL_CLS

#if defined flagSDL2
struct HalSdl {
	struct NativeAudioSinkDevice;
	struct NativeCenterVideoSinkDevice;
	struct NativeCenterFboSinkDevice;
	#if defined flagOGL
	struct NativeOglVideoSinkDevice;
	#endif
	#if defined flagDX12
	struct NativeD12VideoSinkDevice;
	#endif
	struct NativeContextBase;
	struct NativeEventsBase;
	#ifdef flagGUI
	struct NativeUppEventsBase;
	#endif
	#if defined flagOGL
	struct NativeUppOglDevice;
	#endif
	
	struct Thread {
		
	};
	
	static Thread& Local() {thread_local static Thread t; return t;}
	
	#include "IfaceFuncs.inl"
	
};
#endif
#if (defined flagUWP && defined flagDX12)
struct HalHolo {
	struct NativeAudioSinkDevice;
	struct NativeCenterVideoSinkDevice;
	struct NativeCenterFboSinkDevice;
	#if defined flagOGL
	struct NativeOglVideoSinkDevice;
	#endif
	#if defined flagDX12
	struct NativeD12VideoSinkDevice;
	#endif
	struct NativeContextBase;
	struct NativeEventsBase;
	#if defined flagGUI
	struct NativeUppEventsBase;
	#endif
	#if defined flagOGL
	struct NativeUppOglDevice;
	#endif
	
	struct Thread {
		
	};
	
	static Thread& Local() {thread_local static Thread t; return t;}
	
	#include "IfaceFuncs.inl"
	
};
#endif

struct HalAudioSinkDevice : public Atom {
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalAudioSinkDevice() {}
};

struct HalCenterVideoSinkDevice : public Atom {
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalCenterVideoSinkDevice() {}
};

struct HalCenterFboSinkDevice : public Atom {
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalCenterFboSinkDevice() {}
};

#if defined flagOGL
struct HalOglVideoSinkDevice : public Atom {
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalOglVideoSinkDevice() {}
};
#endif

#if defined flagDX12
struct HalD12VideoSinkDevice : public Atom {
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalD12VideoSinkDevice() {}
};
#endif

struct HalContextBase : public Atom {
	//RTTI_DECL1(HalContextBase, Atom)
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalContextBase() {}
};

struct HalEventsBase : public Atom {
	//RTTI_DECL1(HalEventsBase, Atom)
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalEventsBase() {}
};

#if defined flagGUI
struct HalUppEventsBase : public Atom {
	//RTTI_DECL1(HalUppEventsBase, Atom)
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalUppEventsBase() {}
};
#endif

#if defined flagOGL
struct HalUppOglDevice : public Atom {
	//RTTI_DECL1(HalUppOglDevice, Atom)
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~HalUppOglDevice() {}
};
#endif


template <class Hal> struct HalAudioSinkDeviceT : HalAudioSinkDevice {
	HalAudioSinkDeviceT(VfsValue& n) : HalAudioSinkDevice(n) {}
	using CLASSNAME = HalAudioSinkDeviceT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	//RTTI_DECL1(CLASSNAME, HalAudioSinkDevice)
	void Visit(Vis& v) override {
		if (dev) Hal::AudioSinkDevice_Visit(*dev, *this, v);
		VIS_THIS(HalAudioSinkDevice);
	}
	typename Hal::NativeAudioSinkDevice* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::AudioSinkDevice_Create(dev))
			return false;
		if (!Hal::AudioSinkDevice_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::AudioSinkDevice_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::AudioSinkDevice_Start(*dev, *this);
	}
	void Stop() override {
		Hal::AudioSinkDevice_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::AudioSinkDevice_Uninitialize(*dev, *this);
		Hal::AudioSinkDevice_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::AudioSinkDevice_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::AudioSinkDevice_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::AudioSinkDevice_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::AudioSinkDevice_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::AudioSinkDevice_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::AudioSinkDevice_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::AudioSinkDevice_DetachContext(*dev, *this, a);
	}
};
template <class Hal> struct HalCenterVideoSinkDeviceT : HalCenterVideoSinkDevice {
	HalCenterVideoSinkDeviceT(VfsValue& n) : HalCenterVideoSinkDevice(n) {}
	using CLASSNAME = HalCenterVideoSinkDeviceT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::CenterVideoSinkDevice_Visit(*dev, *this, v);
		VIS_THIS(HalCenterVideoSinkDevice);
	}
	typename Hal::NativeCenterVideoSinkDevice* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::CenterVideoSinkDevice_Create(dev))
			return false;
		if (!Hal::CenterVideoSinkDevice_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::CenterVideoSinkDevice_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::CenterVideoSinkDevice_Start(*dev, *this);
	}
	void Stop() override {
		Hal::CenterVideoSinkDevice_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::CenterVideoSinkDevice_Uninitialize(*dev, *this);
		Hal::CenterVideoSinkDevice_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::CenterVideoSinkDevice_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::CenterVideoSinkDevice_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::CenterVideoSinkDevice_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::CenterVideoSinkDevice_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::CenterVideoSinkDevice_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::CenterVideoSinkDevice_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::CenterVideoSinkDevice_DetachContext(*dev, *this, a);
	}
};
template <class Hal> struct HalCenterFboSinkDeviceT : HalCenterFboSinkDevice {
	HalCenterFboSinkDeviceT(VfsValue& n) : HalCenterFboSinkDevice(n) {}
	using CLASSNAME = HalCenterFboSinkDeviceT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::CenterFboSinkDevice_Visit(*dev, *this, v);
		VIS_THIS(HalCenterFboSinkDevice);
	}
	typename Hal::NativeCenterFboSinkDevice* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::CenterFboSinkDevice_Create(dev))
			return false;
		if (!Hal::CenterFboSinkDevice_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::CenterFboSinkDevice_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::CenterFboSinkDevice_Start(*dev, *this);
	}
	void Stop() override {
		Hal::CenterFboSinkDevice_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::CenterFboSinkDevice_Uninitialize(*dev, *this);
		Hal::CenterFboSinkDevice_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::CenterFboSinkDevice_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::CenterFboSinkDevice_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::CenterFboSinkDevice_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::CenterFboSinkDevice_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::CenterFboSinkDevice_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::CenterFboSinkDevice_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::CenterFboSinkDevice_DetachContext(*dev, *this, a);
	}
};
#if defined flagOGL
template <class Hal> struct HalOglVideoSinkDeviceT : HalOglVideoSinkDevice {
	HalOglVideoSinkDeviceT(VfsValue& n) : HalOglVideoSinkDevice(n) {}
	using CLASSNAME = HalOglVideoSinkDeviceT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::OglVideoSinkDevice_Visit(*dev, *this, v);
		VIS_THIS(HalOglVideoSinkDevice);
	}
	typename Hal::NativeOglVideoSinkDevice* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::OglVideoSinkDevice_Create(dev))
			return false;
		if (!Hal::OglVideoSinkDevice_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::OglVideoSinkDevice_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::OglVideoSinkDevice_Start(*dev, *this);
	}
	void Stop() override {
		Hal::OglVideoSinkDevice_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::OglVideoSinkDevice_Uninitialize(*dev, *this);
		Hal::OglVideoSinkDevice_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::OglVideoSinkDevice_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::OglVideoSinkDevice_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::OglVideoSinkDevice_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::OglVideoSinkDevice_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::OglVideoSinkDevice_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::OglVideoSinkDevice_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::OglVideoSinkDevice_DetachContext(*dev, *this, a);
	}
};
#endif
#if defined flagDX12
template <class Hal> struct HalD12VideoSinkDeviceT : HalD12VideoSinkDevice {
	HalD12VideoSinkDeviceT(VfsValue& n) : HalD12VideoSinkDevice(n) {}
	using CLASSNAME = HalD12VideoSinkDeviceT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::D12VideoSinkDevice_Visit(*dev, *this, v);
		VIS_THIS(HalD12VideoSinkDevice);
	}
	typename Hal::NativeD12VideoSinkDevice* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::D12VideoSinkDevice_Create(dev))
			return false;
		if (!Hal::D12VideoSinkDevice_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::D12VideoSinkDevice_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::D12VideoSinkDevice_Start(*dev, *this);
	}
	void Stop() override {
		Hal::D12VideoSinkDevice_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::D12VideoSinkDevice_Uninitialize(*dev, *this);
		Hal::D12VideoSinkDevice_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::D12VideoSinkDevice_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::D12VideoSinkDevice_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::D12VideoSinkDevice_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::D12VideoSinkDevice_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::D12VideoSinkDevice_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::D12VideoSinkDevice_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::D12VideoSinkDevice_DetachContext(*dev, *this, a);
	}
};
#endif
template <class Hal> struct HalContextBaseT : HalContextBase {
	HalContextBaseT(VfsValue& n) : HalContextBase(n) {}
	using CLASSNAME = HalContextBaseT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::ContextBase_Visit(*dev, *this, v);
		VIS_THIS(HalContextBase);
	}
	typename Hal::NativeContextBase* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::ContextBase_Create(dev))
			return false;
		if (!Hal::ContextBase_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::ContextBase_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::ContextBase_Start(*dev, *this);
	}
	void Stop() override {
		Hal::ContextBase_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::ContextBase_Uninitialize(*dev, *this);
		Hal::ContextBase_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::ContextBase_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::ContextBase_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::ContextBase_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::ContextBase_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::ContextBase_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::ContextBase_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::ContextBase_DetachContext(*dev, *this, a);
	}
};
template <class Hal> struct HalEventsBaseT : HalEventsBase {
	HalEventsBaseT(VfsValue& n) : HalEventsBase(n) {}
	using CLASSNAME = HalEventsBaseT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::EventsBase_Visit(*dev, *this, v);
		VIS_THIS(HalEventsBase);
	}
	typename Hal::NativeEventsBase* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::EventsBase_Create(dev))
			return false;
		if (!Hal::EventsBase_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::EventsBase_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::EventsBase_Start(*dev, *this);
	}
	void Stop() override {
		Hal::EventsBase_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::EventsBase_Uninitialize(*dev, *this);
		Hal::EventsBase_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::EventsBase_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::EventsBase_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::EventsBase_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::EventsBase_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::EventsBase_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::EventsBase_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::EventsBase_DetachContext(*dev, *this, a);
	}
};
#if defined flagGUI
template <class Hal> struct HalUppEventsBaseT : HalUppEventsBase {
	HalUppEventsBaseT(VfsValue& n) : HalUppEventsBase(n) {}
	using CLASSNAME = HalUppEventsBaseT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::UppEventsBase_Visit(*dev, *this, v);
		VIS_THIS(HalUppEventsBase);
	}
	typename Hal::NativeUppEventsBase* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::UppEventsBase_Create(dev))
			return false;
		if (!Hal::UppEventsBase_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::UppEventsBase_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::UppEventsBase_Start(*dev, *this);
	}
	void Stop() override {
		Hal::UppEventsBase_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::UppEventsBase_Uninitialize(*dev, *this);
		Hal::UppEventsBase_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::UppEventsBase_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::UppEventsBase_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::UppEventsBase_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::UppEventsBase_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::UppEventsBase_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::UppEventsBase_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::UppEventsBase_DetachContext(*dev, *this, a);
	}
};

#if defined flagOGL
template <class Hal> struct HalUppOglDeviceT : HalUppOglDevice {
	HalUppOglDeviceT(VfsValue& n) : HalUppOglDevice(n) {}
	using CLASSNAME = HalUppOglDeviceT<Hal>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Hal::UppOglDevice_Visit(*dev, *this, v);
		VIS_THIS(HalUppOglDevice);
	}
	typename Hal::NativeUppOglDevice* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Hal::UppOglDevice_Create(dev))
			return false;
		if (!Hal::UppOglDevice_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Hal::UppOglDevice_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Hal::UppOglDevice_Start(*dev, *this);
	}
	void Stop() override {
		Hal::UppOglDevice_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Hal::UppOglDevice_Uninitialize(*dev, *this);
		Hal::UppOglDevice_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Hal::UppOglDevice_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool Recv(int sink_ch, const Packet& in) override {
		return Hal::UppOglDevice_Recv(*dev, *this, sink_ch, in);
	}
	void Finalize(RealtimeSourceConfig& cfg) override {
		return Hal::UppOglDevice_Finalize(*dev, *this, cfg);
	}
	void Update(double dt) override {
		return Hal::UppOglDevice_Update(*dev, *this, dt);
	}
	bool IsReady(PacketIO& io) override {
		return Hal::UppOglDevice_IsReady(*dev, *this, io);
	}
	bool AttachContext(AtomBase& a) override {
		return Hal::UppOglDevice_AttachContext(*dev, *this, a);
	}
	void DetachContext(AtomBase& a) override {
		Hal::UppOglDevice_DetachContext(*dev, *this, a);
	}
};
#endif
#endif
#if defined flagSDL2
using SdlAudioSinkDevice = HalAudioSinkDeviceT<HalSdl>;
using SdlCenterVideoSinkDevice = HalCenterVideoSinkDeviceT<HalSdl>;
using SdlCenterFboSinkDevice = HalCenterFboSinkDeviceT<HalSdl>;
#if defined flagOGL
using SdlOglVideoSinkDevice = HalOglVideoSinkDeviceT<HalSdl>;
#endif
#if defined flagDX12
using SdlD12VideoSinkDevice = HalD12VideoSinkDeviceT<HalSdl>;
#endif
using SdlContextBase = HalContextBaseT<HalSdl>;
using SdlEventsBase = HalEventsBaseT<HalSdl>;
#if defined flagGUI
using SdlUppEventsBase = HalUppEventsBaseT<HalSdl>;
#if defined flagOGL
using SdlUppOglDevice = HalUppOglDeviceT<HalSdl>;
#endif
#endif
#endif
#if (defined flagUWP && defined flagDX12)
using HoloAudioSinkDevice = HalAudioSinkDeviceT<HalHolo>;
using HoloCenterVideoSinkDevice = HalCenterVideoSinkDeviceT<HalHolo>;
using HoloCenterFboSinkDevice = HalCenterFboSinkDeviceT<HalHolo>;
#if defined flagOGL
using HoloOglVideoSinkDevice = HalOglVideoSinkDeviceT<HalHolo>;
#endif
#if defined flagDX12
using HoloD12VideoSinkDevice = HalD12VideoSinkDeviceT<HalHolo>;
#endif
using HoloContextBase = HalContextBaseT<HalHolo>;
using HoloEventsBase = HalEventsBaseT<HalHolo>;
using HoloUppEventsBase = HalUppEventsBaseT<HalHolo>;
#if defined flagOGL
using HoloUppOglDevice = HalUppOglDeviceT<HalHolo>;
#endif
#endif

END_UPP_NAMESPACE

#endif
