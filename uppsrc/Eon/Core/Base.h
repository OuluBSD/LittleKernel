#ifndef _Eon_Core_Base_h_
#define _Eon_Core_Base_h_




class CustomerBase :
	public Atom
{
	int			packet_count = 0;
	int			packet_thrds = 0;
	
protected:
	friend class Space;
	using CustomerData = Atom::CustomerData;
	
	One<CustomerData>		customer;
	
	
public:
	using Atom::Atom;
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this);}
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	bool IsForwardReady() override;
	void ForwardPacket(PacketValue& in, PacketValue& out) override;
	
	
	RTSrcConfig* GetConfig() override {ASSERT(customer); return customer ? &customer->cfg : 0;}
	bool PostInitialize() override;
	void UpdateConfig(double dt) override;
	
};


class RollingValueBase :
	public Atom
{
	byte				rolling_value = 0;
	uint64				seq = 0;
	double				time = 0;
	ValueFormat			internal_fmt;
	
public:
	using Atom::Atom;
	bool Initialize(const WorldState& ws) override;
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this);}
	void Uninitialize() override {}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	
};


class VoidSinkBase :
	public Atom
{
	byte				rolling_value = 0;
	ValueFormat			internal_fmt;
	int					dbg_total_samples = 0;
	int					dbg_total_bytes = 0;
	int					dbg_iter = 0;
	int					dbg_limit = 0;
	ValueFormat			fmt;
	
public:
	using Atom::Atom;
	typedef VoidSinkBase CLASSNAME;
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this);}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	bool Consume(const void* data, int len) override;
	bool NegotiateSinkFormat(LinkBase& link, int sink_ch, const ValueFormat& new_fmt) override;
	
	
};

class VoidPollerSinkBase :
	public Atom
{
	struct Thread {
		byte				rolling_value = 0;
		double				time = 0;
	};
	ArrayMap<uint64, Thread> thrds;
	ValueFormat			internal_fmt;
	double				dt = 0;
	double				ts = 0;
	bool				fail = false;
	int					dbg_total_samples = 0;
	int					dbg_total_bytes = 0;
	int					dbg_limit = 0;
	
public:
	using Atom::Atom;
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	void Update(double dt) override;
	bool IsReady(PacketIO& io) override;
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this);}
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	
	
};

class VoidBase :
	public Atom
{
	
public:
	using Atom::Atom;
	bool Initialize(const WorldState& ws) override {return true;}
	void Uninitialize() override {}
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this);}
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override {return true;}
	
};

#ifdef flagSCREEN
class EventStateBase :
	public Atom
{
	static Vector<BinderIfaceEvents*>	binders;
	String			target;
	EnvStatePtr		state;
	bool			dbg_print = false;
	int				dbg_iter = 0;
	int				dbg_limit = 0;
	
	static EventStateBase* latest;
	
public:
	EventStateBase(VfsValue& n);
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	void Update(double dt) override;
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this); vis & state;}
	bool IsReady(PacketIO& io) override;
	bool Recv(int sink_ch, const Packet& in) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	
	void Event(const GeomEvent& e);
	void LeftDown(Point pt, dword keyflags);
	void LeftUp(Point pt, dword keyflags);
	void MouseMove(Point pt, dword keyflags);
	bool Key(dword key, int count);
	void MouseWheel(Point p, int zdelta, dword keyflags);
	
	void			SetBool(dword key, bool b) {state->SetBool(key, b);}
	void			SetInt(dword key, int i) {state->SetInt(key, i);}
	
	bool			GetBool(dword key) {return state->GetBool(key);}
	int				GetInt(dword key) {return state->GetInt(key);}
	EnvState&		GetState() const;
	
	static EventStateBase* Latest() {return latest;}
	
	static void AddBinder(BinderIfaceEvents* iface);
	static void RemoveBinder(BinderIfaceEvents* iface);
	
};
#endif

class TestEventSrcBase :
	public Atom
{
	int sent_count = 0;
	TransformMatrix trans;
	ControllerMatrix ctrl;
	
public:
	using Atom::Atom;
	TestEventSrcBase(VfsValue& n);
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	void Visit(Vis& vis) override {vis.VisitT<Atom>("Atom", *this);}
	bool IsReady(PacketIO& io) override;
	bool Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	
	
};






#endif
