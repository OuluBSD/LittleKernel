#ifndef _CtrlLib_TabMgrCtrl_h_
#define _CtrlLib_TabMgrCtrl_h_

NAMESPACE_UPP


class TabMgrCtrl : public ParentCtrl {
	TabCtrl tabs;
	Array<Ctrl> ctrls;
	
public:
	typedef TabMgrCtrl CLASSNAME;
	TabMgrCtrl();
	
	void Updated() override;
	
	template <class T>
	T& Add(String title) {
		T* o = new T();
		ctrls.Add(o);
		tabs.Add(o->SizePos(), title);
		return *o;
	}
};


END_UPP_NAMESPACE

#endif
