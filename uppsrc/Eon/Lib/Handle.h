#ifndef _EonLib_Handle_h_
#define _EonLib_Handle_h_


class HandleEventsBase :
	public Atom
{
	String						target;
	EnvStatePtr					state;
	int							prev_iter = -1;
	
public:
	CLASSTYPE(HandleEventsBase)
	HandleEventsBase(VfsValue& n);
	
	bool			Initialize(const WorldState& ws) override;
	bool			PostInitialize() override;
	void			Uninitialize() override;
	bool			IsReady(PacketIO& io) override;
	bool			Recv(int sink_ch, const Packet& in) override;
	bool			Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void			Finalize(RealtimeSourceConfig& cfg) override;
	void			Visit(Vis& v) override {VIS_THIS(Atom); v & state;}
	
	
	Callback1<HandleEventsBase*>	WhenInitialize;
	
	EnvStatePtr& State() {return state;}
	
};


#if defined flagSCREEN
class HandleVideoBase :
	public Atom
{
	struct Binder;
	
	Array<Binder> binders;
	HandleVideoBase* active = 0;
	
	String					target;
	EnvStatePtr				state;
	int						prev_iter = -1;
	ValDevCls				src_type;
	//ProgDraw				pd;
	
	bool					draw_mem = false;
	bool					add_ecs = false;
	bool					dbg_info = false;
	int						dbg_win_id = 0;
	#if defined flagGUI
	WindowSystemRef			wins;
	Gu::SurfaceSystemRef	surfs;
	#endif
	int						screen_id = -1;
	int						add_count = 0;
	
	DrawCommand* ProcessWindow(Binder& b, DrawCommand* begin);
	void ProcessWindowCommands(Binder& b, DrawCommand* begin, DrawCommand* end);
	
	//void			RedrawScreen();
	
public:
	CLASSTYPE(HandleVideoBase)
	HandleVideoBase(VfsValue& n);
	
	bool			IsScreenMode() const {return screen_id >= 0;}
	
	bool			Initialize(const WorldState& ws) override;
	bool			PostInitialize() override;
	void			Stop() override;
	void			Uninitialize() override;
	bool			IsReady(PacketIO& io) override;
	bool			Recv(int sink_ch, const Packet& in) override;
	void			Finalize(RealtimeSourceConfig& cfg) override;
	bool			Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void			Visit(Vis& v) override;
	
	//void			AddWindow3D(Binder&, Geom2DComponent&);
	//void			RemoveWindow3D(Binder&, Handle::Geom2DComponent&);
	bool			IsActive() const;
	
	void AddBinders();
	void AddBinderActive(Binder& b);
	
	void AddBinder(BinderIfaceVideo* iface);
	void RemoveBinder(BinderIfaceVideo* iface);
	
	static Callback1<HandleVideoBase*>	WhenInitialize;
	
	
};
#endif


#if 0
class HandleOglBase :
	public OglBufferBase
{
	Vector<BinderIfaceOgl*> binders;
	
public:
	CLASSTYPE(HandleVideoBase)
	HandleOglBase(VfsValue& n);
	
	bool			Initialize(const WorldState& ws) override;
	bool			PostInitialize() override;
	void			Uninitialize() override;
	bool			IsReady(PacketIO& io) override;
	bool			Recv(int sink_ch, const Packet& in) override;
	bool			Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	void			Visit(Vis& v) override {}
	
	void AddBinder(BinderIfaceOgl* iface);
	void RemoveBinder(BinderIfaceOgl* iface);
	
	static Callback1<HandleOglBase*>	WhenInitialize;
	
};
#endif


#endif
