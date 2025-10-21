#include "Script.h"


NAMESPACE_UPP

namespace Eon {
bool ScriptLoopLoader::MakeLink(VfsValue& val, AtomBasePtr src_atom, AtomBasePtr dst_atom) {
	// This is for primary link (src_ch==0 to sink_ch== 0) only...
	InterfaceSourcePtr src = src_atom->GetSource();
	ExchangeSourceProviderPtr src_ep = CastPtr<ExchangeSourceProvider>(&*src);
	if (!src_ep) {
		LOG("ScriptLoopLoader::MakeLink: error: internal error (no src_ep)");
		return false;
	}
	
	auto sink = dst_atom->GetSink();
	Ptr<ExchangeSinkProvider> sinkT = dst_atom->GetSinkT<ExchangeSinkProvider>();
	ASSERT(src && sink && sinkT);
	if (!src || !sink || !sinkT)
		return false;
	
	int src_ch = 0;
	int sink_ch = 0;
	
	
	ValueFormat src_fmt = src->GetSourceValue(src_ch).GetFormat();
	ValueFormat sink_fmt = sink->GetValue(sink_ch).GetFormat();
	if (src_fmt.vd != sink_fmt.vd) {
		LOG("error: sink and source device-value-class mismatch: src(" + src_fmt.vd.ToString() + "), sink(" + sink_fmt.vd.ToString() + ")");
		return false;
	}
	
	ASSERT(src_atom != dst_atom);
	ASSERT(src_atom->GetLink() != dst_atom->GetLink()); // "stupid" but important
	//ASSERT(src	->AsAtomBase()->val.HasOwnerDeep(val));
	//ASSERT(sink	->AsAtomBase()->val.HasOwnerDeep(val));
	CookiePtr src_cookie, sink_cookie;
	
	if (src->Accept(sinkT, src_cookie, sink_cookie)) {
		
		// Create exchange-point object
		auto& sdmap = VfsValueExtFactory::IfaceLinkDataMap();
		int i = sdmap.Find(src_fmt.vd);
		if (i < 0) {
			LOG("error: no exchange-point class set for type " + src_fmt.vd.ToString());
			ASSERT(0);
			return false;
		}
		const auto& src_d = sdmap[i];
		if (src_d.vd != src_fmt.vd) {
			ASSERT(0);
			LOG("internal error: unexpected sink class type");
			return false;
		}
		
		String name = src_d.name;
		hash_t type_hash = src_d.type_hash;
		ASSERT(type_hash);
		
		//ASSERT(val.type_hash && val.owner && !val.owner->type_hash);
		auto space = val.FindOwnerNull();
		ASSERT(type_hash && space);
		
		VfsValue& link_val = space->Add(name, type_hash);
		if (!link_val.ext) {
			LOG("error: VfsValue has no any ext unexpectedly");
			ASSERT(0);
			return false;
		}
		ExchangePointPtr ep = link_val.FindExt<ExchangePoint>();
		if (!ep) {
			LOG("error: VfsValue has no LinkBase ext unexpectedly");
			ASSERT(0);
			return false;
		}
		RTLOG("Loop::Link(...): created " << ep->GetDynamicName() << " at " << HexStr(ep->GetHashValue()));
		RTLOG("                 src-atom: " << HexStr(src_atom->GetTypeCls().GetHashValue()));
		RTLOG("                 src-link: " << HexStr(src_atom->GetLink()->GetTypeCls().GetHashValue()));
		RTLOG("                 dst-atom: " << HexStr(dst_atom->GetTypeCls().GetHashValue()));
		RTLOG("                 dst-link: " << HexStr(dst_atom->GetLink()->GetTypeCls().GetHashValue()));
		src->Link(ep, sinkT, src_cookie, sink_cookie);
		ep->Init(space);
		ep->Set(src_ep, sinkT, src_cookie, sink_cookie);
		src_atom->GetLink()->SetPrimarySink(dst_atom->GetLink());
		dst_atom->GetLink()->SetPrimarySource(src_atom->GetLink());
		
		return true;
	}
	return false;
}
}

END_UPP_NAMESPACE
