#ifndef _AI_Ctrl_Release_h_
#define _AI_Ctrl_Release_h_

NAMESPACE_UPP


class ReleaseInfoCtrl : public WithSnapshotInfo<ComponentCtrl> {
	
	
public:
	typedef ReleaseInfoCtrl CLASSNAME;
	ReleaseInfoCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override {}
	void Clear();
	void OnValueChange();
	
};

INITIALIZE(ReleaseInfoCtrl)


END_UPP_NAMESPACE

#endif
 
