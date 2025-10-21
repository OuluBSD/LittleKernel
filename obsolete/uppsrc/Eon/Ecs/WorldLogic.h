#ifndef _Eon_Ecs_WorldLogic_h_
#define _Eon_Ecs_WorldLogic_h_

	
class WorldLogicSystem : public System
{
	
	
public:
    ECS_SYS_CTOR(WorldLogicSystem)
	ECS_SYS_DEF_VISIT
	
	void Attach(Transform* t);
	void Detach(Transform* t);
	
protected:
	void Update(double dt) override;
	void UpdateByVisit(double dt);
	void UpdateByList(double dt);
	void UpdateTransform(Transform& t, double dt);
	
	Vector<Transform*> list;
	
};


#endif
