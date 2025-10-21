#ifndef _AI_Ctrl_Song_h_
#define _AI_Ctrl_Song_h_

NAMESPACE_UPP


class SongInfoCtrl : public WithComponentInfo<ComponentCtrl> {
	int focus_lyr = -1;
	
public:
	typedef SongInfoCtrl CLASSNAME;
	SongInfoCtrl();
	
	void Data() override;
	void Clear();
	void DataScript();
	void OnValueChange();
	void SetScript();
	void ToolMenu(Bar& bar) override;
	
	
};


INITIALIZE(SongInfoCtrl)

END_UPP_NAMESPACE

#endif
 
