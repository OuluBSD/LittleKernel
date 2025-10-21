#ifndef _Eon_Player_h_
#define _Eon_Player_h_


class PlayerBodyComponent;
using PlayerBodyComponentPtr = Ptr<PlayerBodyComponent>;


typedef enum : byte {
	Unspecified,
	LeftHand,
	RightHand
} PlayerHandedness;

typedef enum : byte {
	HighHandPosition,
	ApproximateHandPosition,
} HandPositionAccuracy;

struct HandActionSourcePose {
	vec3 forward_direction;
	quat orientation;
	vec3 position;
	HandPositionAccuracy position_accuracy;
	vec3 up_direction;
	
	vec3 GetPosition() const {return position;}
	vec3 GetForwardDirection() const {return forward_direction;}
	quat GetOrientation() const {return orientation;}
};

// "SpatialSource"
struct HandLocationSource : ControllerSource {
	
};

struct HandSourceLocation {
	vec3 position;
	quat orientation;
	HandActionSourcePose* pose = 0;
	
	const vec3& GetPosition() const {return position;}
	const quat& GetOrientation() const {return orientation;}
	virtual HandActionSourcePose* GetHandPose() const {return pose;}
	
};

struct HandActionSourceLocation : HandSourceLocation {
	
	
};


class PlayerHandComponent : public Component {
	
public:
	ECS_COMPONENT_CTOR(PlayerHandComponent)
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	
    bool IsSource(const ControllerSource& rhs) const;
	
	
	PlayerBodyComponentPtr				body;
    const HandLocationSource*			source = 0;
    HandActionSourceLocation*			location = 0;
	
	bool								is_simulated = false;
    bool								attach_ctrl_model = false;
    byte								req_hand = Unspecified;
    
};

using PlayerHandComponentPtr = Ptr<PlayerHandComponent>;



class PlayerHeadComponent : public Component {
	
public:
	ECS_COMPONENT_CTOR(PlayerHeadComponent)
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	
	
	PlayerBodyComponentPtr				body;
	
};

using PlayerHeadComponentPtr = Ptr<PlayerHeadComponent>;



class PlayerBodyComponent : public Component {
	
protected:
	friend class PlayerBodySystem;
	
	PlayerHandComponentPtr hands[2];
	PlayerHeadComponentPtr head;
	
	float height = 1.8f;
	
public:
	ECS_COMPONENT_CTOR(PlayerBodyComponent)
	COPY_PANIC(PlayerBodyComponent)
	
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	
	bool SetHand(PlayerHandedness hand, PlayerHandComponentPtr comp);
	bool SetHead(PlayerHeadComponentPtr head);
	
	double GetHeight() const {return height;}
	const PlayerHeadComponentPtr& GetHead() const {return head;}
	
};




class PlayerBodySystem :
    public System,
    public InteractionListener
{
	
public:
	ECS_SYS_CTOR(PlayerBodySystem)
	ECS_SYS_DEF_VISIT_((vis && bodies) & iasys)
	
	const Vector<PlayerBodyComponentPtr>& GetComponents() const {return bodies;}
	
	void Attach(PlayerBodyComponentPtr h);
	void Detach(PlayerBodyComponentPtr h);
protected:
    // System
    bool Initialize(const WorldState&) override;
    void Update(double dt) override;
    void Uninitialize() override;
    System* GetSystem() override {return this;}
	
	void OnControllerDetected(const GeomEvent& e) override;
	void OnControllerLost(const GeomEvent& e) override;
	void OnControllerPressed(const GeomEvent& e) override;
	void OnControllerUpdated(const GeomEvent& e) override;
	void OnControllerReleased(const GeomEvent& e) override;
    
private:
    void RefreshComponentsForSource(const HandLocationSource& source);
    
    Ptr<InteractionSystem> iasys;
    Vector<PlayerBodyComponentPtr> bodies;
    
};


#endif
