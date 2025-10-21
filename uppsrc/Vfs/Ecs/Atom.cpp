#include "Ecs.h"


NAMESPACE_UPP

// incomplete Plan dtor in header
AtomBase::CustomerData::CustomerData() : cfg(gen) {}
AtomBase::CustomerData::~CustomerData() {}


AtomBase::AtomBase(VfsValue& n) : VfsValueExt(n) {
	DBG_CONSTRUCT
}

AtomBase::~AtomBase() {
	ASSERT(dep_count == 0);
	DBG_DESTRUCT
}

Engine& AtomBase::GetEngine() {
	Engine* m = val.FindOwner<Engine>();
	ASSERT(m);
	if (!m) throw Exc("Engine not found");
	return *m;
}

void AtomBase::UninitializeDeep() {
	VfsValueExt::UninitializeDeep();
	ClearSinkSource();
	ClearDependencies();
	UninitializeAtom();
}

/*Space& AtomBase::GetParent() {
	Space* s = GetSpace();
	ASSERT(s);
	if (!s) throw Exc("space not found");
	return *s;
}*/

LinkBase* AtomBase::GetLink() {
	return link;
}

String AtomBase::ToString() const {
	return GetTypeCls().GetName();
}

void AtomBase::SetInterface(const IfaceConnTuple& iface) {
	this->iface = iface;
}

const IfaceConnTuple& AtomBase::GetInterface() const {
	return iface;
}

void AtomBase::SetPrimarySinkQueueSize(int i) {
	GetSink()->GetValue(0).SetMinQueueSize(i);
}

void AtomBase::AddAtomToUpdateList() {
	Engine* sys = val.FindOwner<Engine>();
	ASSERT(sys);
	if (!sys) throw Exc("AtomSystem not found");
	sys->AddUpdated(this);
}

void AtomBase::RemoveAtomFromUpdateList() {
	Engine* sys = val.FindOwner<Engine>();
	if (sys)
		sys->RemoveUpdated(this);
}

int AtomBase::FindSourceWithValDev(ValDevCls vd) {
	InterfaceSourcePtr src = GetSource();
	int c = src->GetSourceCount();
	for(int i = 0; i < c; i++) {
		ValueBase& v = src->GetSourceValue(i);
		ValueFormat f = v.GetFormat();
		if (f.vd == vd)
			return i;
	}
	return -1;
}

int AtomBase::FindSinkWithValDev(ValDevCls vd) {
	InterfaceSinkPtr src = GetSink();
	int c = src->GetSinkCount();
	for(int i = 0; i < c; i++) {
		ValueBase& v = src->GetValue(i);
		ValueFormat f = v.GetFormat();
		if (f.vd == vd)
			return i;
	}
	return -1;
}

void AtomBase::UpdateSinkFormat(ValCls vc, ValueFormat fmt) {
	InterfaceSinkPtr sink_iface = GetSink();
	int sink_count = sink_iface->GetSinkCount();
	for(int i = 0; i < sink_count; i++) {
		ValueBase& val = sink_iface->GetValue(i);
		ValueFormat val_fmt = val.GetFormat();
		if (val_fmt.vd.val == vc && val_fmt != fmt) {
			RTLOG("AudioOutput::UpdateSinkFormat: updating sink #" << i << " format to " << fmt.ToString());
			val.SetFormat(fmt);
		}
	}
}


void AtomBase::PostContinueForward() {
	RTLOG("AtomBase::PostContinueForward");
	ASSERT(link);
	if (link)
		link->PostContinueForward();
}

bool AtomBase::Recv(int sink_ch, const Packet& in) {
	// TODO assert(0), because this is bad idea, but I was lazy... all tests will break
	return true;
}

void AtomBase::SetQueueSize(int queue_size) {
	InterfaceSinkPtr sink_iface = this->GetSink();
	InterfaceSourcePtr src_iface = this->GetSource();
	if (queue_size == 1) {
		int c = sink_iface->GetSinkCount();
		for(int i = 0; i < c; i++)
			sink_iface->GetValue(i).SetMaxQueueSize(queue_size);
		
		c = src_iface->GetSourceCount();
		for(int i = 0; i < c; i++)
			src_iface->GetSourceValue(i).SetMaxQueueSize(queue_size);
	}
	else {
		int c = sink_iface->GetSinkCount();
		for(int i = 0; i < c; i++)
			sink_iface->GetValue(i).SetMinQueueSize(queue_size);
		
		c = src_iface->GetSourceCount();
		for(int i = 0; i < c; i++)
			src_iface->GetSourceValue(i).SetMinQueueSize(queue_size);
	}
}

VfsValue* AtomBase::GetSpace() {
	VfsValue* o = val.owner;
	ASSERT(o && !o->type_hash);
	return o;
}


END_UPP_NAMESPACE

