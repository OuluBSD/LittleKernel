#ifndef _Eon_Core_LinkUtil_h_
#define _Eon_Core_LinkUtil_h_




class AsyncMemForwarderBase :
	public LinkBase
{
	Packet			partial_packet;
	byte*			write_mem = 0;
	int				write_size = 0;
	int				write_pos = 0;
	int				partial_pos = 0;
	PacketBuffer	buffer;
	
	off32_gen	dbg_off_gen;
	off32		dbg_offset;
	int			dbg_data_offset = 0;
	
	
	void	Consume(int data_begin, Packet p); // "const Packet&"" is invalid here
	
public:
	LINK_CTORH(AsyncMemForwarderBase);
	void	Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	
	bool	IsReady(PacketIO& io) final;
	bool	ForwardAsyncMem(byte* mem, int size) override;
	bool	ProcessPackets(PacketIO& io) final;
	bool	IsConsumedPartialPacket() final {return partial_packet;}
	
	virtual bool PassConsumePacket(int sink_ch, const Packet& in) {return true;}
	
};


class FramePollerBase :
	public LinkBase
{
	double		dt = 0;
	double		frame_age = 0;
	
	
public:
	LINK_CTOR(FramePollerBase)
	void	Update(double dt) override;
	bool	IsReady(PacketIO& io) override;
	
	void	Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	
	void	SetFPS(int fps) {dt = 1.0 / (double)fps;}
	
};


class CenterDriver :
	public LinkBase
{
	
protected:
	using CustomerData = Atom::CustomerData;
	
	One<CustomerData>		customer;
	
	
public:
	LINK_CTOR(CenterDriver)
	bool Initialize(const WorldState& ws) override;
	void Uninitialize() override;
	void	Visit(Vis& vis) override {vis.VisitT<LinkBase>("LinkBase", *this);}
	
	RealtimeSourceConfig* GetConfig() final {ASSERT(customer); return customer ? &customer->cfg : 0;}
	
};



#endif
