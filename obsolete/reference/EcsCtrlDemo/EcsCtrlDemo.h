#ifndef _EcsCtrlDemo_EcsCtrlDemo_h
#define _EcsCtrlDemo_EcsCtrlDemo_h

#include <Complete/Complete.h>
using namespace Upp;



NAMESPACE_UPP



class EcsCtrlDemo :
	public TopWindow
{
	TabMgrCtrl tabs;
	
	TimeCallback tc, data_tc;
	TimeStop ts;
	
	void MachineUpdater();
	
	
public:
	typedef EcsCtrlDemo CLASSNAME;
	EcsCtrlDemo(Engine& mach);
	
	bool InitializeDefault();
	
	void Updated() override;
	void OnError();
	
	
	Engine& mach;
	static constexpr const char* POOL_NAME = "shaders";
	
	PoolRef GetPool() {return mach.Get<EntityStore>()->GetRoot()->GetAddPool(POOL_NAME);}
	
};



void Initializer();


END_UPP_NAMESPACE


#endif
