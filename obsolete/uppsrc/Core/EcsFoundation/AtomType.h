#ifndef _Core_EcsFoundation_AtomType_h_
#define _Core_EcsFoundation_AtomType_h_


#define IFACE_LIST \
	IFACE(AUDIO) \
	IFACE(VIDEO) \
	IFACE(VOLUME) \
	IFACE(MIDI) \
	IFACE(EVENT) \
	IFACE(DATA) \
	IFACE(ORDER) \
	IFACE(RECEIPT) \
	IFACE(FBO) \
	IFACE(PROG) \

#define ATOM_ROLE_LIST \
	ATOM_ROLE(DRIVER) \
	ATOM_ROLE(CUSTOMER) \
	ATOM_ROLE(PIPE) \
	ATOM_ROLE(DRIVER_PIPE) \

#define SUB_ATOM_CLS AsTypeCls()

typedef enum : int {
	INVALID_VAL,
	#define IFACE(x) x,
	IFACE_LIST
	#undef IFACE
	
	TYPE_COUNT,
		
	ValAudio = AUDIO,
	ValVideo = VIDEO,
	ValVolume = VOLUME,
	ValMidi = MIDI,
	ValEvent = EVENT,
	ValData = DATA,
	ValOrder = ORDER,
	ValReceipt = RECEIPT,
	ValFbo = FBO,
	ValProg = PROG,
} ValCls;

typedef enum : int {
	INVALID_ATOMROLE=-1,
	
	#define ATOM_ROLE(x) x,
	ATOM_ROLE_LIST
	#undef ATOM_ROLE
	
	ATOMROLE_COUNT,
} AtomRole;

#define DEV_IFACE(val) \
	IFACE_CTX_CLS(CENTER, val, ) \
	IFACE_CTX_CLS(NET, val, Net) \
	IFACE_CTX_CLS(OGL, val, Ogl) \
	IFACE_CTX_CLS(DX, val, Dx) \

typedef enum : int {
	INVALID_DEV,
	CENTER,
	NET,
	OGL,
	DX,
	
	DEVCLS_COUNT,
		
	DevCenter = CENTER,
	DevNet = NET,
	DevOgl = OGL,
	DevDx = DX,
} DevCls;

String GetAtomRoleString(AtomRole t);
String GetValClsName(ValCls t);
String GetDevClsName(DevCls t);

struct ValDevCls : Moveable<ValDevCls> {
	ValCls	val;
	DevCls	dev;
	
	ValDevCls();
	ValDevCls(DevCls d, ValCls v);
	ValDevCls(ValCls v, DevCls d);
	ValDevCls(const ValDevCls& v);
	void Visit(Vis& v);
	void Clear();
	bool IsValid() const;
	String GetName() const;
	void operator=(const Nuller& n);
	void operator=(const ValDevCls& n);
	bool operator==(const ValDevCls& c) const;
	bool operator!=(const ValDevCls& c) const;
	hash_t GetHashValue() const;
	String ToString() const;
	String GetActionName() const;
};

#define VD(dev, val) ValDevCls(DevCls::dev, ValCls::val)

struct AtomCls : Moveable<AtomCls> {
	ValDevCls sink, side, src;
	
	void Visit(Vis& v);
	bool IsValid() const;
};

struct ValDevTuple : Moveable<ValDevTuple> {
	
	struct Channel : Moveable<Channel> {
		ValDevCls vd;
		bool is_opt = false;
		
		void Set(ValDevCls vd, bool is_opt);
		String ToString() const;
		bool IsValid() const;
		bool operator==(const Channel& c) const;
		bool operator!=(const Channel& c) const;
		hash_t GetHashValue() const;
	};
	Vector<Channel> channels;
	
	ValDevTuple();
	ValDevTuple(const ValDevCls& v);
	ValDevTuple(const ValDevTuple& v);
	void Clear();
	int GetCount() const;
	Channel&       operator[](int i);
	const Channel& operator[](int i) const;
	ValDevTuple& Add(const ValDevCls& o, bool is_opt);
	String ToString() const;
	bool IsValid() const;
	void operator=(const Nuller& n);
	void operator=(const ValDevTuple& o);
	bool operator==(const ValDevTuple& o) const;
	bool operator!=(const ValDevTuple& o) const;
	hash_t GetHashValue() const;
};

struct IfaceConnLink : Moveable<IfaceConnLink> {
	int conn = -1;
	int local = -1;
	int other = -1;
	
	void Set(int conn, int local, int other) {this->conn = conn; this->local = local; this->other = other;}
	String ToString() const {String s; s << "conn:" << conn << ", local:" << local << ", other:" << other; return s;}
};

struct AtomIfaceTypeCls : Moveable<AtomIfaceTypeCls> {
	ValDevTuple		sink;
	ValDevTuple		src;
	
	AtomIfaceTypeCls();
	AtomIfaceTypeCls(const AtomIfaceTypeCls& c);
	AtomIfaceTypeCls(const ValDevTuple& sink, const ValDevTuple& src);
	void AddSink(const ValDevCls& vd, bool is_opt);
	void AddSource(const ValDevCls& vd, bool is_opt);
	bool IsValid() const;
	void operator=(const Nuller& n);
	void operator=(const AtomIfaceTypeCls& o);
	bool operator==(const AtomIfaceTypeCls& c) const;
	bool operator!=(const AtomIfaceTypeCls& c) const;
	String ToString() const;
	hash_t GetHashValue() const;
};

struct AtomTypeCls : Moveable<AtomTypeCls> {
	AtomIfaceTypeCls iface;
	TypeCls sub;
	AtomRole role = INVALID_ATOMROLE;
	
	AtomTypeCls();
	AtomTypeCls(const AtomTypeCls& c);
	AtomTypeCls(TypeCls cls, AtomRole role, const ValDevCls& sink, const ValDevCls& src);
	AtomTypeCls(TypeCls cls, AtomRole role, const ValDevTuple& sink, const ValDevTuple& src);
	void AddIn(ValDevCls vd, bool is_opt);
	void AddOut(ValDevCls vd, bool is_opt);
	bool IsRoleDriver()		const;
	bool IsRoleCustomer()	const;
	bool IsRolePipe()		const;
	bool IsSourceChannelOptional(int ch_i) const;
	bool IsSinkChannelOptional(int ch_i) const;
	bool IsValid() const;
	hash_t GetHashValue() const;
	void operator=(const Nuller& n);
	void operator=(const AtomTypeCls& o);
	bool operator==(const AtomTypeCls& c) const;
	bool operator!=(const AtomTypeCls& c) const;
	String ToString() const;
};

struct IfaceConnTuple {
	Vector<IfaceConnLink>		sink;
	Vector<IfaceConnLink>		src;
	AtomTypeCls					type;
	
	void Realize(const AtomTypeCls& type);
	void SetSource(int conn, int src_ch, int sink_ch);
	void SetSink(int conn, int sink_ch, int src_ch);
	bool IsComplete() const;
	dword GetSinkMask() const;
	void operator=(const IfaceConnTuple& s);
	bool HasCommonConnection(const IfaceConnTuple& src) const;
};

#endif
