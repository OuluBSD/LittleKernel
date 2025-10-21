#ifndef _EonCtrl_DesktopSystem_h_
#define _EonCtrl_DesktopSystem_h_


class DesktopSuiteSystem : public System {
	Array<TopWindow> apps;
	
	
protected:
    bool Initialize(const WorldState&) override;
    bool Arg(String key, Value value) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    
    
public:
	ECS_SYS_CTOR(DesktopSuiteSystem)
	void Visit(Vis& vis) override;
	~DesktopSuiteSystem() {ASSERT(apps.IsEmpty());}
	
	
};


#endif
