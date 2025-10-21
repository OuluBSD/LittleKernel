#ifndef _EonCtrl_InterfaceConnectionCtrl_h_
#define _EonCtrl_InterfaceConnectionCtrl_h_


class InterfaceConnectionGraph : public Ctrl {
	
	
public:
	//RTTI_DECL1(InterfaceConnectionGraph, Ctrl)
	typedef InterfaceConnectionGraph CLASSNAME;
	InterfaceConnectionGraph();
	
	void Updated() override;
	
};


#endif
