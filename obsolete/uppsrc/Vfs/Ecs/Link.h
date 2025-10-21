#ifndef _Vfs_Ecs_Link_h_
#define _Vfs_Ecs_Link_h_

namespace Eon {
class ScriptLoopLoader;
class ScriptDriverLoader;
class ScriptLoader;
class LoopContext;
}

#define LINK_CTORH(x) \
	CLASSTYPE(x) \
	x(VfsValue& n);

#define LINK_CTOR(x) \
	CLASSTYPE(x) \
	x(VfsValue& n) : LinkBase(n) {}

#define LINK_CTOR_(x, base) \
	CLASSTYPE(x) \
	x(VfsValue& n) : base(n) {}

class LinkBase :
	public PacketForwarder
{
	
protected:
	friend class LoopSystem;
	friend class LinkSystem;
	friend class Eon::LoopContext;
	
	AtomBasePtr atom;
	
	
public:
	#ifdef flagDEBUG
	bool dbg_async_race = false;
	#endif
	
	using CustomerData = AtomBase::CustomerData;
	using LinkBasePtr = Ptr<LinkBase>;
	
protected:
	friend class Eon::ScriptLoopLoader;
	friend class Eon::ScriptDriverLoader;
	friend class Loop;
	friend class Eon::LoopContext;
	
	int						id = -1;
	
	
	void					SetId(int i) {id = i;}
	
public:
	void					SetPrimarySink(LinkBasePtr b) {prim_link_sink = b;}
	void					SetPrimarySource(LinkBasePtr b) {prim_link_src = b;}
	
protected:
	struct Exchange {
		LinkBasePtr				other;
		int						local_ch_i = -1;
		int						other_ch_i = -1;
		
		void Clear() {other = 0; local_ch_i = -1; other_ch_i = -1;}
		void Visit(Vis& vis) {vis & other;}
		String ToString() const;
	};
	
	
	RealtimeSourceConfig*	last_cfg = 0;
	int						packets_forwarded = 0;
	int						skipped_fwd_count = 0;
	LinkedList<Exchange>	side_sink_conn, side_src_conn;
	LinkBasePtr				prim_link_sink, prim_link_src;
	
	bool					IsAllSideSourcesFull(const Vector<int>& src_chs);
	bool					IsAnySideSourceFull(const Vector<int>& src_chs);
	bool					IsPrimarySourceFull();
	void					UpdateLinkedExchangeFormats(int src_ch, const ValueFormat& fmt);
	
public:
	virtual bool			ProcessPackets(PacketIO& io) = 0;
	virtual bool			ForwardAsyncMem(byte* mem, int size) {Panic("ForwardAsyncMem unimplemented"); return false;}
	virtual bool			IsConsumedPartialPacket() {return 0;}
	virtual void			Forward(FwdScope& fwd);
	virtual bool			IsReady(PacketIO& io);
	virtual bool			PassLinkSideSink(LinkBasePtr sink) {return true;}
	virtual bool			PassLinkSideSource(LinkBasePtr src) {return true;}
	virtual LinkTypeCls		GetLinkType() const = 0;
	
	virtual void			Visit(Vis& vis) override {
		vis || side_sink_conn || side_src_conn;
		vis & prim_link_sink & prim_link_src;
	}
	virtual RTSrcConfig*	GetConfig() {return last_cfg;}
	
	AtomBase*				GetAtom();
	//Engine&				GetEngine();
	int						GetId() const;
	void					ForwardAsync();
	Packet					InitialPacket(int src_ch, off32 off);
	Packet					ReplyPacket(int src_ch, const Packet& in);
	Packet					ReplyPacket(int src_ch, const Packet& in, Packet content);
	
	LinkBasePtr				GetLinkedSideSink()   {ASSERT(side_sink_conn.GetCount() == 1); return side_sink_conn.First().other;}
	LinkBasePtr				GetLinkedSideSource() {ASSERT(side_src_conn.GetCount()  == 1); return side_src_conn.First().other;}
	bool					LinkSideSink(LinkBasePtr sink, int local_ch_i, int other_ch_i);
	
	bool					NegotiateSourceFormat(int src_ch, const ValueFormat& fmt);
	virtual bool			NegotiateSinkFormat(int sink_ch, const ValueFormat& new_fmt, bool chk_other=true);
	
	int						GetSinkPacketCount();
	int						GetSourcePacketCount();
	void					PostContinueForward();
	void					SetPrimarySinkQueueSize(int i);
	String					GetInlineConnectionsString() const;
	String					ToString() const override;
	AtomTypeCls				GetAtomType() const;
	ISourcePtr				GetSource();
	ISinkPtr				GetSink();
	void					AddLinkToUpdateList();
	void					RemoveLinkFromUpdateList();
	
	
	void					Clear();
	
	bool					IsPacketStuck() override;
	bool					IsLoopComplete(FwdScope& fwd) override {return false;}
	void					ForwardAtom(FwdScope& fwd) override;
	void					ForwardExchange(FwdScope& fwd) override;
	String					GetSecondaryName() override;
	void*					GetSecondaryPtr() override;
	
	void					ForwardPipe(FwdScope& fwd);
	void					ForwardSideConnections();
	LinkedList<Exchange>&	SideSinks() {return side_sink_conn;}
	LinkedList<Exchange>&	SideSources() {return side_src_conn;}
	
public:
	LinkBase(VfsValue& n);
	virtual ~LinkBase();
	
	Callback2<LinkBase&, PacketIO&>			WhenEnterProcessPackets;
	Callback2<LinkBase&, PacketIO&>			WhenLeaveProcessPackets;
	
};

using LinkBasePtr = Ptr<LinkBase>;
using LinkMap = ArrayMap<LinkTypeCls, LinkBase>;

bool Serial_Link_ForwardAsyncMem(LinkBase* l, byte* data, int size);


#endif
