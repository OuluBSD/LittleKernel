#ifndef _Eon_PacketTracker_h_
#define _Eon_PacketTracker_h_


class PacketTracker :
	public System
{
	PacketId id_counter = 1;
	
	
protected:
    bool Initialize(const WorldState&) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    
    #if HAVE_PACKETTRACKER
	void Track0(TrackerInfo info, PacketValue& p);
	void Checkpoint0(TrackerInfo info, PacketValue& p);
	void StopTracking0(TrackerInfo info, PacketValue& p);
	#endif
	
	static PacketTracker*& active_tracker() {static PacketTracker* p; return p;}
public:
    SYS_CTOR(PacketTracker)
	SYS_DEF_VISIT
	
	#if HAVE_PACKETTRACKER
	static void Track(TrackerInfo info, Packet& p) {Track(info, *p);}
	static void Checkpoint(TrackerInfo info, Packet& p) {Checkpoint(info, *p);}
	static void StopTracking(TrackerInfo info, Packet& p) {StopTracking(info, *p);}
	
	static void Track(TrackerInfo info, PacketValue& p) {if (active_tracker()) active_tracker()->Track0(info,p);}
	static void Checkpoint(TrackerInfo info, PacketValue& p) {if (active_tracker()) active_tracker()->Checkpoint0(info,p);}
	static void StopTracking(TrackerInfo info, PacketValue& p) {if (active_tracker()) active_tracker()->StopTracking0(info,p);}
	#endif
};

#undef RTTI_CTX_SYS


#endif
