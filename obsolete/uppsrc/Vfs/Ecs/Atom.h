#ifndef _Vfs_Ecs_Atom_h_
#define _Vfs_Ecs_Atom_h_

namespace Eon {
class ScriptLoopLoader;
class ScriptDriverLoader;
struct ExtScriptEcsLoader;
struct LoopContext;
}

#define ATOM_CTOR_(x, base) \
	CLASSTYPE(x) \
	x(VfsValue& n) : base(n) {}

class LinkBase;
class WorldState;


class PacketForwarderData {};


struct AtomBase :
	VfsValueExt,
	PacketForwarderData,
	Destroyable
{
	using AtomBasePtr = Ptr<AtomBase>;
	
	struct CustomerData {
		RealtimeSourceConfig	cfg;
		off32_gen				gen;
		
		CustomerData();
		~CustomerData();
	};
	
	
protected:
	friend class Eon::ScriptLoopLoader;
	friend class Eon::ScriptDriverLoader;
	friend class Loop;
	
	int64					idx = -1;
	bool					is_running = false;
	
	void					SetIdx(int64 i) {idx = i;}
	void					SetRunning(bool b=true) {is_running = b;}
	
protected:
	friend class LinkBase;
	friend class Eon::LoopContext;
	
	Mutex					fwd_lock;
	IfaceConnTuple			iface;
	LinkBase*				link = 0;
	AtomBasePtr				atom_dependency;
	int						dep_count = 0;
	//Value					user_data; // use val.value instead
	
	
public:
	virtual AtomTypeCls		GetType() const = 0;
	virtual void			CopyTo(AtomBase* atom) const = 0;
	virtual bool			InitializeAtom(const WorldState& ws) = 0;
	virtual void			UninitializeAtom() = 0;
	virtual void			VisitSource(Vis& vis) = 0;
	virtual void			VisitSink(Vis& vis) = 0;
	virtual void			ClearSinkSource() = 0;
	virtual ISourcePtr		GetSource() = 0;
	virtual ISinkPtr		GetSink() = 0;
	virtual bool			Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) = 0;
	virtual bool			IsReady(PacketIO& io) {return true;}
	virtual void			UpdateConfig(double dt) {Panic("Unimplemented"); NEVER();}
	virtual bool			Recv(int sink_ch, const Packet& in);
	virtual void			Finalize(RealtimeSourceConfig& cfg) {}
	// internal format should be sink:0
	virtual bool			Consume(const void* data, int len) {Panic("Unimplemented"); return false;}
	virtual bool			IsForwardReady() {Panic("Unimplemented"); NEVER(); return false;}
	virtual void			ForwardPacket(PacketValue& in, PacketValue& out) {Panic("Unimplemented"); NEVER();}
	virtual bool			AttachContext(AtomBase& a) {Panic("Unimplemented"); NEVER(); return false;}
	virtual void			DetachContext(AtomBase& a) {Panic("Unimplemented"); NEVER();}
	virtual RealtimeSourceConfig* GetConfig() {return 0;}
	virtual bool			NegotiateSinkFormat(LinkBase& link, int sink_ch, const ValueFormat& new_fmt) {return false;}
	
	String					ToString() const override;
	void					UninitializeDeep() override;
	void					Visit(Vis& vis) override {}
	
	Value&					UserData() {return val.value;}
	bool					IsRunning() const {return is_running;}
	void					AddAtomToUpdateList();
	void					RemoveAtomFromUpdateList();
	void					UpdateSinkFormat(ValCls val, ValueFormat fmt);
	void					PostContinueForward();
	void					SetQueueSize(int queue_size);
	
	Engine&					GetEngine();
	void					SetInterface(const IfaceConnTuple& iface);
	const IfaceConnTuple&	GetInterface() const;
	int						FindSourceWithValDev(ValDevCls vd);
	int						FindSinkWithValDev(ValDevCls vd);
	void					SetPrimarySinkQueueSize(int i);
	
public:
	AtomBase(VfsValue& n);
	virtual ~AtomBase();
	
	template <class T> T* GetSourceT() {return dynamic_cast<T*>(&*this->GetSource());}
	template <class T> T* GetSinkT()   {return dynamic_cast<T*>(&*this->GetSink());}
	
	VfsValue*		GetSpace();
	//VfsValue&		GetParent();
	LinkBase*		GetLink();
	int64			GetIdx() const {return idx;}
	
	template <class T> Ptr<T> As() {return ::UPP::CastPtr<T>(this);}
	
	template <class S> void AddToSystem();
	template <class S> void RemoveFromSystem();
	
	template <class ValDevSpec, class T> bool LinkManually(T& o, String* err_msg=0);
	
	
};

using AtomBasePtr = Ptr<AtomBase>;

#endif
