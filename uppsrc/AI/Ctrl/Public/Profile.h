#ifndef _AI_Ctrl_Profile_h_
#define _AI_Ctrl_Profile_h_

NAMESPACE_UPP


class LeadsCtrl;

class ProfileInfoCtrl : public WithProfileInfo<ComponentCtrl> {
	
public:
	typedef ProfileInfoCtrl CLASSNAME;
	ProfileInfoCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override {}
	void Clear();
	void OnValueChange();
	
	LeadsCtrl* editor = 0;
	
};

INITIALIZE(ProfileInfoCtrl)


END_UPP_NAMESPACE

#endif
 
