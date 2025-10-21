#include "EcsFoundation.h"

NAMESPACE_UPP


String GetAtomRoleString(AtomRole t) {
	switch (t) {
		#define ATOM_ROLE(x) case x: return #x;
		ATOM_ROLE_LIST
		
		#undef ATOM_ROLE
		default:			return "invalid";
	}
}

String GetValClsName(ValCls t) {
	switch (t) {
		#define IFACE(x) case x: return ToLower(String(#x));
		IFACE_LIST
		#undef IFACE
		default: return "invalid";
	}
}

String GetDevClsName(DevCls t) {
	switch (t) {
		case CENTER:	return "center";
		case NET:		return "net";
		case OGL:		return "ogl";
		case DX:		return "dx";
		case INVALID_DEV:
		default: return "invalid";
	}
}

ValDevCls::ValDevCls() {}
ValDevCls::ValDevCls(DevCls d, ValCls v)
	: val(v), dev(d)
{
}
ValDevCls::ValDevCls(ValCls v, DevCls d)
	: val(v), dev(d)
{
}
ValDevCls::ValDevCls(const ValDevCls& v)
	: val(v.val), dev(v.dev)
{
}
void ValDevCls::Clear()
{
	val = INVALID_VAL;
	dev = INVALID_DEV;
}
bool ValDevCls::IsValid() const { return val != INVALID_VAL && dev != INVALID_DEV; }
String ValDevCls::GetName() const { return GetDevClsName(dev) + "." + GetValClsName(val); }
void ValDevCls::operator=(const Nuller& n)
{
	val = INVALID_VAL;
	dev = INVALID_DEV;
}
void ValDevCls::operator=(const ValDevCls& n)
{
	val = n.val;
	dev = n.dev;
}
bool ValDevCls::operator==(const ValDevCls& c) const { return val == c.val && dev == c.dev; }
bool ValDevCls::operator!=(const ValDevCls& c) const { return val != c.val || dev != c.dev; }
hash_t ValDevCls::GetHashValue() const
{
	return (int)dev * (int)ValCls::TYPE_COUNT + (int)val;
}
String ValDevCls::ToString() const { return GetDevClsName(dev) + "-" + GetValClsName(val); }
String ValDevCls::GetActionName() const
{
	return ToLower(GetDevClsName(dev)) + "." + ToLower(GetValClsName(val));
}
void ValDevCls::Visit(Vis& v) { v("val",(int&)val)("dev",(int&)dev); }

void AtomCls::Visit(Vis& v) { v VISN(sink) VISN(src) VISN(side); }
bool AtomCls::IsValid() const { return sink.IsValid() && src.IsValid() && side.IsValid(); }

void ValDevTuple::Channel::Set(ValDevCls vd, bool is_opt)
{
	this->vd = vd;
	this->is_opt = is_opt;
}
String ValDevTuple::Channel::ToString() const
{
	return vd.ToString() + (is_opt ? " optional" : "");
}
bool ValDevTuple::Channel::IsValid() const { return vd.IsValid(); }
bool ValDevTuple::Channel::operator==(const Channel& c) const
{
	return vd == c.vd && is_opt == c.is_opt;
}
bool ValDevTuple::Channel::operator!=(const Channel& c) const { return !(*this == c); }
hash_t ValDevTuple::Channel::GetHashValue() const
{
	CombineHash c;
	c.Put(vd.GetHashValue());
	c.Put((int)is_opt);
	return c;
}

ValDevTuple::ValDevTuple() {}
ValDevTuple::ValDevTuple(const ValDevCls& v) { Add(v, false); }
ValDevTuple::ValDevTuple(const ValDevTuple& v) { *this = v; }
void ValDevTuple::Clear() { channels.Clear(); }
int ValDevTuple::GetCount() const { return channels.GetCount(); }
ValDevTuple::Channel& ValDevTuple::operator[](int i) { return channels[i]; }
const ValDevTuple::Channel& ValDevTuple::operator[](int i) const { return channels[i]; }
ValDevTuple& ValDevTuple::Add(const ValDevCls& o, bool is_opt)
{
	channels.Add().Set(o, is_opt);
	return *this;
}
String ValDevTuple::ToString() const
{
	int count = channels.GetCount();
	String s;
	s << "(" << (int)count;
	for(int i = 0; i < count; i++)
		s << ", " << channels[i].ToString();
	s << ")";
	return s;
}
bool ValDevTuple::IsValid() const
{
	int count = channels.GetCount();
	for(int i = 0; i < count; i++)
		if(!channels[i].IsValid())
			return false;
	return true;
}
void ValDevTuple::operator=(const Nuller& n) { Clear(); }
void ValDevTuple::operator=(const ValDevTuple& o) { channels <<= o.channels; }
bool ValDevTuple::operator==(const ValDevTuple& o) const
{
	if(channels.GetCount() != o.channels.GetCount())
		return false;
	for(int i = 0; i < channels.GetCount(); i++)
		if(channels[i] != o.channels[i])
			return false;
	return true;
}

bool ValDevTuple::operator!=(const ValDevTuple& o) const { return !operator==(o); }

hash_t ValDevTuple::GetHashValue() const
{
	int count = channels.GetCount();
	CombineHash ch;
	ch.Put(count);
	for(int i = 0; i < count; i++)
		ch.Put(channels[i].GetHashValue());
	return ch;
}





AtomIfaceTypeCls::AtomIfaceTypeCls() {}
AtomIfaceTypeCls::AtomIfaceTypeCls(const AtomIfaceTypeCls& c) {*this = c;}
AtomIfaceTypeCls::AtomIfaceTypeCls(const ValDevTuple& sink, const ValDevTuple& src) : sink(sink), src(src) {}
void AtomIfaceTypeCls::AddSink(const ValDevCls& vd, bool is_opt) {sink.Add(vd, is_opt);}
void AtomIfaceTypeCls::AddSource(const ValDevCls& vd, bool is_opt) {src.Add(vd, is_opt);}
bool AtomIfaceTypeCls::IsValid() const {return sink.IsValid() && src.IsValid();}
void AtomIfaceTypeCls::operator=(const Nuller& n) {sink = n; src = n;}
void AtomIfaceTypeCls::operator=(const AtomIfaceTypeCls& o) {
	sink = o.sink;
	src = o.src;
}
hash_t AtomIfaceTypeCls::GetHashValue() const {
	CombineHash c;
	c.Put(sink.GetHashValue());
	c.Put(src.GetHashValue());
	return c;
}
bool AtomIfaceTypeCls::operator==(const AtomIfaceTypeCls& c) const {
	return	sink		== c.sink &&
			src			== c.src
			;
}
bool AtomIfaceTypeCls::operator!=(const AtomIfaceTypeCls& c) const {return !(*this == c);}
String AtomIfaceTypeCls::ToString() const {return "(sink(" + sink.ToString() + "), src(" + src.ToString() + ")";}




AtomTypeCls::AtomTypeCls() {}
AtomTypeCls::AtomTypeCls(const AtomTypeCls& c) {*this = c;}
AtomTypeCls::AtomTypeCls(TypeCls cls, AtomRole role, const ValDevCls& sink, const ValDevCls& src) : iface(sink,src), sub(cls), role(role) {}
AtomTypeCls::AtomTypeCls(TypeCls cls, AtomRole role, const ValDevTuple& sink, const ValDevTuple& src) : iface(sink,src), sub(cls), role(role) {}
void AtomTypeCls::AddIn(ValDevCls vd, bool is_opt) {
	iface.sink.channels.Add().Set(vd, is_opt);
}
void AtomTypeCls::AddOut(ValDevCls vd, bool is_opt) {
	iface.src.channels.Add().Set(vd, is_opt);
}
bool AtomTypeCls::IsRoleDriver() const {return role == AtomRole::DRIVER;}
bool AtomTypeCls::IsRoleCustomer() const {return role == AtomRole::CUSTOMER;}
bool AtomTypeCls::IsRolePipe() const {return role == AtomRole::PIPE;}
bool AtomTypeCls::IsSourceChannelOptional(int ch_i) const {
	return iface.src[ch_i].is_opt;
}
bool AtomTypeCls::IsSinkChannelOptional(int ch_i) const {
	return iface.sink[ch_i].is_opt;
}
bool AtomTypeCls::IsValid() const {return iface.IsValid() && sub && role != AtomRole::INVALID_ATOMROLE;}
hash_t AtomTypeCls::GetHashValue() const {
	CombineHash c;
	c.Put(iface.GetHashValue());
	c.Put(sub);
	c.Put(role);
	return c;
}
void AtomTypeCls::operator=(const Nuller& n) {iface = n; sub = AsVoidTypeCls(); role = AtomRole::INVALID_ATOMROLE;}
void AtomTypeCls::operator=(const AtomTypeCls& o) {
	iface = o.iface;
	sub = o.sub;
	role = o.role;
}
bool AtomTypeCls::operator==(const AtomTypeCls& c) const {
	return	iface == c.iface &&
			sub == c.sub &&
			role == c.role
			;
}
bool AtomTypeCls::operator!=(const AtomTypeCls& c) const {return !(*this == c);}
String AtomTypeCls::ToString() const {return sub.ToString() + "-" + GetAtomRoleString(role) + "-" + iface.ToString();}




void IfaceConnTuple::Realize(const AtomTypeCls& type) {
	if (!this->type.IsValid()) {
		//DUMP(type);
		this->type = type;
		const ValDevTuple& type_sink = type.iface.sink;
		const ValDevTuple& type_src = type.iface.src;
		this->sink.SetCount(0); // clear
		this->sink.SetCount(type_sink.channels.GetCount());
		this->src.SetCount(0);
		this->src.SetCount(type_src.channels.GetCount());
	}
	else {
		ASSERT(this->type == type);
	}
}

bool IfaceConnTuple::HasCommonConnection(const IfaceConnTuple& src) const {
	for (const IfaceConnLink& a : this->sink) {
		for (const IfaceConnLink& b : src.src) {
			if (a.conn == b.conn)
				return true;
		}
	}
	for (const IfaceConnLink& a : this->src) {
		for (const IfaceConnLink& b : src.sink) {
			if (a.conn == b.conn)
				return true;
		}
	}
	return false;
}

void IfaceConnTuple::SetSource(int conn, int src_ch, int sink_ch) {
	ASSERT(src_ch >= 0 && src_ch < type.iface.src.GetCount());
	ASSERT(src_ch < type.iface.src.GetCount());
	src[src_ch].conn = conn;
	src[src_ch].local = src_ch;
	src[src_ch].other = sink_ch;
}

void IfaceConnTuple::SetSink(int conn, int sink_ch, int src_ch) {
	ASSERT(sink_ch >= 0 && sink_ch < type.iface.sink.GetCount());
	ASSERT(sink_ch < type.iface.src.GetCount());
	sink[sink_ch].conn = conn;
	sink[sink_ch].local = sink_ch;
	sink[sink_ch].other = src_ch;
}

bool IfaceConnTuple::IsComplete() const {
	ASSERT(sink.GetCount() == type.iface.sink.GetCount());
	if (sink.GetCount() != type.iface.sink.GetCount()) return false;
	
	ASSERT(src.GetCount() == type.iface.src.GetCount());
	if (src.GetCount() != type.iface.src.GetCount()) return false;
	
	for(int i = 1; i < sink.GetCount(); i++)
		if (sink[i].conn < 0 && !type.iface.sink[i].is_opt)
			return false;
		
	for(int i = 1; i < src.GetCount(); i++)
		if (src[i].conn < 0 && !type.iface.src[i].is_opt)
			return false;
	
	return true;
}

dword IfaceConnTuple::GetSinkMask() const {
	// primary link is not usually written to this class,
	// but it's always required so set it true
	dword m = 0;
	for(int i = 0; i < sink.GetCount(); i++)
		m |= (i == 0 || sink[i].conn >= 0) ? 1 << i : 0;
	return m;
}

void IfaceConnTuple::operator=(const IfaceConnTuple& s) {
	sink <<= s.sink;
	src <<= s.src;
	type = s.type;
}


END_UPP_NAMESPACE
