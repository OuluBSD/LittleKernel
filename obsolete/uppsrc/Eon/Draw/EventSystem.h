#ifndef _Eon_Draw_EventSystem_h_
#define _Eon_Draw_EventSystem_h_


class EventSystem :
	public System,
	public BinderIfaceEvents
{
	//Serial::EcsEventsBase* serial = 0;
	
protected:
    bool Initialize(const WorldState&) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    void Dispatch(const GeomEvent& state) override {TODO}
    
public:
	using Base = System;
    ECS_SYS_CTOR(EventSystem)
	ECS_SYS_DEF_VISIT
	
	//void Attach(Serial::EcsEventsBase* b);
	
};

using EventSystemPtr = Ptr<EventSystem>;


#endif
