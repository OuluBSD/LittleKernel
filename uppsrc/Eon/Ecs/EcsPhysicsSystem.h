#ifndef _Eon_Ecs_EcsPhysicsSystem_h_
#define _Eon_Ecs_EcsPhysicsSystem_h_


class PhysicsBody;


class PhysicsSystem : public System
{
	Vector<PhysicsBody*> bodies;
	double time = 0;
	double last_dt = 0;
	bool debug_log = false;
	
	
	float area_length = 100.0f;
	bool remove_outside_area = false;
	
	
	void TestPlayerLookFn(PhysicsBody& b, Point mouse_diff);
	void TestPlayerMoveFn(PhysicsBody& b, vec3 rel_dir, float step);
	
protected:
    void Update(double dt) override;
	bool Arg(String key, Value value) override;
    
    void RunTestFn(PhysicsBody& b);
    
public:
	CLASSTYPE(PhysicsSystem);
    vec3	gravity;
    
    PhysicsSystem(VfsValue& n);

    
    void Attach(PhysicsBody& b);
    void Detach(PhysicsBody& b);
    void Visit(Vis&) override;
    
};


class PhysicsBody : public Component {
	
public:
	friend class PhysicsSystem;
	
	enum {
		TESTFN_NULL,
		TESTFN_FIXED,
		TESTFN_CIRCLE,
	};
	
	int test_fn;
	bool is_bound = false;
	
	Ptr<Transform> trans;
	Ptr<PlayerBodyComponent> player;
	
public:
	ECS_COMPONENT_CTOR(PhysicsBody)
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	
	bool BindDefault();
	void UnbindDefault();
	
};

using PhysicsBodyPtr = Ptr<PhysicsBody>;


#endif
