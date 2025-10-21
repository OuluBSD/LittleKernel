#ifndef _Vfs_Ecs_LinkSystem_h_
#define _Vfs_Ecs_LinkSystem_h_


class LinkSystem :
	public System
{
	struct Once {
		PacketForwarder*		fwd;
		RealtimeSourceConfig*	cfg;
	};
	One<LinkedList<Once>> once_cbs;
	LinkedList<LinkBasePtr> updated, customers, drivers, pollers;
	Mutex lock;
	
public:
	SYS_CTOR(LinkSystem);
	SYS_DEF_VISIT_H
	
	void AddOnce(PacketForwarder& fwd, RealtimeSourceConfig& cfg);
	
	Callback1<PacketForwarder*>				WhenEnterOnceForward;
	Callback1<LinkBase*>					WhenEnterLinkForward;
	Callback1<FwdScope&>					WhenEnterFwdScopeForward;
	
	Callback								WhenLeaveOnceForward;
	Callback								WhenLeaveLinkForward;
	Callback								WhenLeaveFwdScopeForward;
	
	
	static inline Callback& WhenUninit() {static Callback cb; return cb;}
	
protected:
	
    bool Initialize(const WorldState&) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    
    void ForwardLinks(double dt, const char* id, LinkedList<LinkBasePtr>& links);
    
    
public:
    void AddUpdated(LinkBasePtr p);
    void AddCustomer(LinkBasePtr p);
    void AddDriver(LinkBasePtr p);
    void AddPolling(LinkBasePtr p);
    void RemoveUpdated(LinkBasePtr p);
    void RemoveCustomer(LinkBasePtr p);
    void RemoveDriver(LinkBasePtr p);
    void RemovePolling(LinkBasePtr p);
    
    String GetDebugPacketString(LinkBasePtr& c, RealtimeSourceConfig* cfg);
    
};

using LinkSystemPtr = Ptr<LinkSystem>;

#endif
