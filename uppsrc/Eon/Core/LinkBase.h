#ifndef _Eon_Core_LinkBase_h_
#define _Eon_Core_LinkBase_h_






class CustomerLink : public LinkBase {
	off32_gen	off_gen;
	
	
public:
	LINK_CTOR(CustomerLink)
	
	bool			Initialize(const WorldState& ws) override;
	void			Uninitialize() override;
	bool			ProcessPackets(PacketIO& io) override;
	RTSrcConfig*	GetConfig() override;
	void			Forward(FwdScope& fwd) override;
	bool			IsLoopComplete(FwdScope& fwd) override;
	void			Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};

class PipeLink : public LinkBase {
	
	
public:
	LINK_CTOR(PipeLink)
	
	bool			Initialize(const WorldState& ws) override;
	void			Uninitialize() override;
	bool			ProcessPackets(PacketIO& io) override;
	void			Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};

class PipeOptSideLink : public LinkBase {
	bool finalize_on_side = false;
	
	
public:
	LINK_CTOR(PipeOptSideLink)
	
	bool			Initialize(const WorldState& ws) override;
	void			Uninitialize() override;
	bool			ProcessPackets(PacketIO& io) override;
	void			Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	void			SetFinalizeOnSide(bool b=true) {finalize_on_side = b;}
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};

class IntervalPipeLink : public AsyncMemForwarderBase {
	RunningFlag			flag;
	
	
public:
	LINK_CTOR_(IntervalPipeLink, AsyncMemForwarderBase)
	~IntervalPipeLink();
	
	bool			Initialize(const WorldState& ws) override;
	void			Uninitialize() override;
	void			Visit(Vis& vis) override {vis.VisitT<AsyncMemForwarderBase>("AsyncMemForwarderBase", *this);}
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
	void IntervalSinkProcess();
	
};

class PollerLink : public FramePollerBase {
	bool finalize_on_side = false;
	
public:
	LINK_CTOR_(PollerLink, FramePollerBase)
	~PollerLink();
	
	bool			Initialize(const WorldState& ws) override;
	void			Uninitialize() override;
	void			Visit(Vis& vis) override {vis.VisitT<FramePollerBase>("FramePollerBase", *this);}
	bool			IsReady(PacketIO& io) override;
	bool			ProcessPackets(PacketIO& io) final;
	void			SetFinalizeOnSide(bool b=true) {finalize_on_side = b;}
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
	
};

class ExternalPipeLink : public AsyncMemForwarderBase {
	
public:
	LINK_CTOR_(ExternalPipeLink, AsyncMemForwarderBase)
	~ExternalPipeLink();
	
	bool			Initialize(const WorldState& ws) override;
	void			Uninitialize() override;
	void			Visit(Vis& vis) override {vis.VisitT<AsyncMemForwarderBase>("AsyncMemForwarderBase", *this);}
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};

class DriverLink : public LinkBase {
	
public:
	LINK_CTOR(DriverLink)
	~DriverLink();
	
	bool			Initialize(const WorldState& ws) override;
	void			Uninitialize() override;
	void			Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	bool			ProcessPackets(PacketIO& io) override;
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};

class MergerLink : public LinkBase
{
	byte scheduler_iter = 1;
	
public:
	LINK_CTOR(MergerLink)
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	void Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	bool IsReady(PacketIO& io) override;
	bool ProcessPackets(PacketIO& io) override;
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};

class JoinerLink : public LinkBase
{
	byte scheduler_iter = 1;
	
public:
	LINK_CTOR(JoinerLink)
	bool Initialize(const WorldState& ws) override;
	bool PostInitialize() override;
	void Uninitialize() override;
	void Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	bool IsReady(PacketIO& io) override;
	bool ProcessPackets(PacketIO& io) override;
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};

class SplitterLink : public LinkBase
{
	
public:
	LINK_CTOR(SplitterLink)
	bool Initialize(const WorldState& ws) final;
	void Uninitialize() final;
	void Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	bool IsReady(PacketIO& io) final;
	bool ProcessPackets(PacketIO& io) override;
	
	static LinkTypeCls GetLinkTypeStatic();
	LinkTypeCls GetLinkType() const override {return GetLinkTypeStatic();}
	
};




#endif
