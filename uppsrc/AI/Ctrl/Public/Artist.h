#ifndef _AI_Ctrl_Artist_h_
#define _AI_Ctrl_Artist_h_

NAMESPACE_UPP

class ArtistInfoCtrl : public WithArtistInfo<ComponentCtrl> {
	Index<String> visual_genders;
	
public:
	typedef ArtistInfoCtrl CLASSNAME;
	ArtistInfoCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override {}
	void Clear();
	void OnValueChange();
	
};

INITIALIZE(ArtistInfoCtrl)

END_UPP_NAMESPACE

#endif
 
