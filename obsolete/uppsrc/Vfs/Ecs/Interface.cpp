#include "Ecs.h"


NAMESPACE_UPP


AtomTypeCls InterfaceBase::GetAtomType() const {
	InterfaceBase& b = const_cast<InterfaceBase&>(*this);
	return b.AsAtomBase()->GetType();
}



bool DefaultExchangePoint::IsPacketStuck() {
	return false;
}

void DefaultExchangePoint::ForwardExchange(FwdScope& fwd) {
	DefaultInterfaceSinkPtr sink = dynamic_cast<DefaultInterfaceSink*>(&*this->sink);
	ASSERT(sink);
	fwd.AddNext(sink->AsAtomBase()->GetLink()->GetPacketForwarder());
}

void DefaultExchangePoint::ForwardSetup(FwdScope& fwd) {
	DefaultInterfaceSinkPtr sink = dynamic_cast<DefaultInterfaceSink*>(&*this->sink);
	ASSERT(sink);
	
	int ch_i = 0;
	
	ValueBase& to_val = sink->GetValue(ch_i);
	ValueFormat to_fmt = to_val.GetFormat();
	if (!to_fmt.IsValid()) {
		ValDevTuple t = sink->GetSinkCls();
		ASSERT(t.IsValid());
		to_fmt = GetDefaultFormat(t[0].vd);
		ASSERT(to_fmt.IsValid());
		RTLOG("DefaultExchangePoint::ForwardSetup: fixing sink fmt to: " << to_fmt.ToString());
		to_val.SetFormat(to_fmt);
	}
}

void DefaultExchangePoint::ForwardAtom(FwdScope& fwd) {
	const int src_ch_i = 0;
	const int sink_ch_i = 0;
	
	WhenEnterValExPtForward(*this);
	
	RTLOG("DefaultExchangePoint::Forward: (" << HexStrPtr(this) << ") begin");
	DefaultInterfaceSourcePtr src = dynamic_cast<DefaultInterfaceSource*>(&*this->src);
	DefaultInterfaceSinkPtr sink = dynamic_cast<DefaultInterfaceSink*>(&*this->sink);
	ASSERT(src);
	ASSERT(sink);
	
	
	Ex ex(this);
	ValueBase& src_value = src->GetSourceValue(src_ch_i);
	int src_sz = src_value.GetQueueSize();
	
	if (src_sz) {
		ValueBase& sink_value = sink->GetValue(sink_ch_i);
		bool sink_full = sink_value.IsQueueFull();
		
		if (!sink_full) {RTLOG("ExchangePoint::Forward: exchanging");}
		else {RTLOG("ExchangePoint::Forward: sink full");}
		
		int iter = 0;
		while (src_sz && !sink_full) {
			
			ex.Set(src_value, fwd.Cfg());
			sink_value.Exchange(ex);
			
			if (ex.IsFail()) {
				RTLOG("error: ExchangePoint::Forward: exchange failed");
				fwd.SetFailed();
				break;
			}
			
			src_sz = src_value.GetQueueSize();
			sink_full = sink_value.IsQueueFull();
			++iter;
			if (src_sz && !sink_full) {
				RTLOG("ExchangePoint::Forward: going to iter " << iter << ", sz=" << src_sz << ", sink_full=" << (int)sink_full);
			}
		}
	}
	else {
		RTLOG("ExchangePoint::Forward: empty source");
	}
	
	fwd.AddNext(sink->AsAtomBase()->GetLink());
	
	WhenLeaveValExPtForward();
}

void DefaultExchangePoint::Init(VfsValue* mexpt) {
	ASSERT(mexpt);
	
	#if HAVE_VALSYSTEM
	USING_VALDEVCORE(ValSystem)
	this->conn = conn;
	if (conn) {
		SpacePtr loop = GetConnectorBaseSpace(conn);
		Engine& mach = GetSpaceMachine(loop);
		Ptr<ValSystem> sys = mach.Get<ValSystem>();
		ASSERT(sys);
		if (sys)
			sys->Add(AsPtr<ExchangePoint>());
	}
	#endif
}

void DefaultExchangePoint::Deinit() {
	#if HAVE_VALSYSTEM
	USING_VALDEVCORE(ValSystem)
	if (conn) {
		SpacePtr loop = GetConnectorBaseSpace(conn);
		Engine& mach = GetSpaceMachine(loop);
		Ptr<ValSystem> sys = mach.Get<ValSystem>();
		ASSERT(sys);
		if (sys)
			sys->Remove(AsPtr<ExchangePoint>());
		conn = 0;
	}
	#endif
}



bool DefaultInterfaceSink::Initialize(const WorldState& ws) {
	AtomBase* ab = AsAtomBase();
	AtomTypeCls type = ab->GetType();
	ASSERT(type.IsValid());
	
	SetContainerCount(type.iface.sink.GetCount());
	for(int i = 0; i < type.iface.sink.GetCount(); i++)
		InitializeContainer(i, type.iface.sink[i].vd);
	
	return true;
}

bool DefaultInterfaceSource::Initialize(const WorldState& ws) {
	AtomBase* ab = AsAtomBase();
	AtomTypeCls type = ab->GetType();
	ASSERT(type.IsValid());
	
	SetContainerCount(type.iface.src.GetCount());
	for(int i = 0; i < type.iface.src.GetCount(); i++)
		InitializeContainer(i, type.iface.src[i].vd);
	
	return true;
}


END_UPP_NAMESPACE
