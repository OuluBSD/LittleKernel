#include "Ecs.h"

NAMESPACE_UPP


ExchangeBase::ExchangeBase() {
	//DBG_CONSTRUCT
}

ExchangeBase::~ExchangeBase() {
	//DBG_DESTRUCT
}
	






#ifdef flagDEBUG
bool ExchangeSourceProvider::print_debug = true;
#else
bool ExchangeSourceProvider::print_debug = false;
#endif



void ExchangeSourceProvider::Link(ExchangePoint* expt, SinkProv sink, Cookie& src_c, Cookie& sink_c) {
	ASSERT(expt);
	if (print_debug) {
		String s;
		
		String cfg0 = GetConfigString();
		if (!cfg0.IsEmpty()) cfg0 = "<" + cfg0 + ">";
		
		String cfg1 = sink->GetConfigString();
		if (!cfg1.IsEmpty()) cfg1 = "<" + cfg1 + ">";
		
		TypeCls this_type = GetTypeCls();
		TypeCls sink_type = sink->GetTypeCls();
		
		s << this_type.GetName() <<
			cfg0 << "(" << HexStr(this_type.GetHashValue()) << ") linked to " <<
			sink_type.GetName() <<
			cfg1 << "(" << HexStr(sink_type.GetHashValue()) << ")";
		LOG(s);
	}
	base.SetLink(expt, sink);
	sink->base.SetLink(expt, this);
	OnLink(sink, src_c, sink_c);
	sink->OnLink(this, src_c, sink_c);
}











ExchangeSinkProvider::ExchangeSinkProvider() {
	DBG_CONSTRUCT
}

ExchangeSinkProvider::~ExchangeSinkProvider() {
	DBG_DESTRUCT
}







ExchangeSourceProvider::ExchangeSourceProvider() {
	DBG_CONSTRUCT
}

ExchangeSourceProvider::~ExchangeSourceProvider() {
	DBG_DESTRUCT
}








ExchangeSideSinkProvider::ExchangeSideSinkProvider() {
	DBG_CONSTRUCT
}

ExchangeSideSinkProvider::~ExchangeSideSinkProvider() {
	DBG_DESTRUCT
}







ExchangeSideSourceProvider::ExchangeSideSourceProvider() {
	DBG_CONSTRUCT
}

ExchangeSideSourceProvider::~ExchangeSideSourceProvider() {
	DBG_DESTRUCT
}








void FwdScope::Clear() {
	cur = 0;
	for(int i = 0; i < QUEUE_SIZE; i++)
		next[i] = 0;
	read_i = 0;
	write_i = 0;
	cfg = 0;
	first = 0;
	is_failed = false;
	is_break = false;
	is_once = false;
	is_looped = false;
}

String FwdScope::GetFlagString() const {
	String s;
	s << "fail=" << AsString(is_failed);
	s << ", break=" << AsString(is_break);
	s << ", once=" << AsString(is_once);
	return s;
}

void FwdScope::ForwardWeak() {
	if (cur) {
		int pos = read_i-1;
		if (!cur->IsPacketStuck()) {
			RTLOG("FwdScope::ForwardWeak: fwd " << pos << " (at " << cur->GetDynamicName() << ", " << cur->GetSecondaryName() << "; " << HexStrPtr(cur->GetSecondaryPtr()) << ")");
			cur->ForwardSetup(*this);
			cur->ForwardAtom(*this);
		}
		else {
			RTLOG("FwdScope::ForwardWeak: skip " << pos << " (at " << cur->GetDynamicName() << ", " << cur->GetSecondaryName() << "; " << HexStrPtr(cur->GetSecondaryPtr()) << ")");
		}
		cur->ForwardExchange(*this);
		
		if (cur->IsLoopComplete(*this))
			LoopComplete();
	}
}

void FwdScope::Forward() {
	if (cur) {
		int pos = read_i-1;
		RTLOG("FwdScope::Forward: " << pos << " (at " << cur->GetDynamicName() << ", " << cur->GetSecondaryName() << "; " << HexStrPtr(cur->GetSecondaryPtr()) << ")");
		cur->ForwardSetup(*this);
		cur->ForwardAtom(*this);
		cur->ForwardExchange(*this);
		
		if (cur->IsLoopComplete(*this))
			LoopComplete();
	}
}

void FwdScope::ForwardAddNext() {
	if (cur) {
		cur->ForwardExchange(*this);
		
		if (cur->IsLoopComplete(*this))
			LoopComplete();
	}
}

void FwdScope::AddNext(PacketForwarder* cb) {
	if (cb) {
		ASSERT_(cb != cur, "Duplicate forward is not allowed");
		if (!first)
			first = cb;
		//else if (cb == first)
		//	return;
		int prev_write_i = write_i > 0 ? write_i - 1 : QUEUE_SIZE-1;
		if (next[prev_write_i] == cb)
			return;
		ASSERT(!next[write_i]);
		next[write_i] = cb;
		write_i = (write_i + 1) % QUEUE_SIZE;
	}
}

void FwdScope::ActivateNext() {
	cur = next[read_i];
	next[read_i] = 0;
	read_i = (read_i + 1) % QUEUE_SIZE;
}











ExchangePoint::ExchangePoint(VfsValue& n) : PacketForwarder(n) {
	DBG_CONSTRUCT
}

ExchangePoint::~ExchangePoint() {
	DBG_DESTRUCT
}

void ExchangePoint::Clear() {
	src = 0;
	sink = 0;
	src_cookie = 0;
	sink_cookie = 0;
}

void ExchangePoint::Set(ExchangeSourceProviderPtr src, ExchangeSinkProviderPtr sink) {
	Clear();
	this->src	= src;
	this->sink	= sink;
	ExchangePointPtr thisref = this;
	src->SetSink(thisref, sink);
	sink->SetSource(thisref, src);
}

void ExchangePoint::Set(ExchangeSourceProviderPtr src, ExchangeSinkProviderPtr sink, CookiePtr sink_cookie, CookiePtr src_cookie) {
	Clear();
	this->src_cookie	= src_cookie;
	this->sink_cookie	= sink_cookie;
	this->src	= src;
	this->sink	= sink;
	ASSERT(src->IsSink(sink));
	ASSERT(sink->IsSource(src));
}


END_UPP_NAMESPACE
