// This file have been generated automatically.
// DO NOT MODIFY THIS FILE!

#ifndef _IVolumetric_IVolumetric_h_
#define _IVolumetric_IVolumetric_h_

#include <Eon/Eon.h>

NAMESPACE_UPP

#define VOL_CLS_LIST(x) \
	VOL_CLS(StaticSource, x) \

#define VOL_VNDR_LIST \
	VOL_VNDR(VolRawByte) \

#define VOL_CLS(x, v) struct v##x;
#define VOL_VNDR(x) VOL_CLS_LIST(x)
VOL_VNDR_LIST
#undef VOL_VNDR
#undef VOL_CLS

struct VolRawByte {
	struct NativeStaticSource;
	
	struct Thread {
		
	};
	
	static Thread& Local() {thread_local static Thread t; return t;}
	
	#include "IfaceFuncs.inl"
	
};

struct VolStaticSource : public Atom {
	//RTTI_DECL1(VolStaticSource, Atom)
	using Atom::Atom;
	void Visit(Vis& v) override {VIS_THIS(Atom);}
	
	virtual ~VolStaticSource() {}
};


template <class Vol> struct VolumetricStaticSourceT : VolStaticSource {
	VolumetricStaticSourceT(VfsValue& n) : VolStaticSource(n) {}
	using CLASSNAME = VolumetricStaticSourceT<Vol>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	void Visit(Vis& v) override {
		if (dev) Vol::StaticSource_Visit(*dev, *this, v);
		VIS_THIS(VolStaticSource);
	}
	typename Vol::NativeStaticSource* dev = 0;
	bool Initialize(const WorldState& ws) override {
		if (!Vol::StaticSource_Create(dev))
			return false;
		if (!Vol::StaticSource_Initialize(*dev, *this, ws))
			return false;
		return true;
	}
	bool PostInitialize() override {
		if (!Vol::StaticSource_PostInitialize(*dev, *this))
			return false;
		return true;
	}
	bool Start() override {
		return Vol::StaticSource_Start(*dev, *this);
	}
	void Stop() override {
		Vol::StaticSource_Stop(*dev, *this);
	}
	void Uninitialize() override {
		ASSERT(this->GetDependencyCount() == 0);
		Vol::StaticSource_Uninitialize(*dev, *this);
		Vol::StaticSource_Destroy(dev);
	}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {
		if (!Vol::StaticSource_Send(*dev, *this, cfg, out, src_ch))
			return false;
		return true;
	}
	bool IsReady(PacketIO& io) override {
		return Vol::StaticSource_IsReady(*dev, *this, io);
	}
};

using RawByteStaticSource = VolumetricStaticSourceT<VolRawByte>;

END_UPP_NAMESPACE

#endif
