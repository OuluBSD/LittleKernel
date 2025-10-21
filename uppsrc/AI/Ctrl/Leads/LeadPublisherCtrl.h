#ifndef _AI_Ctrl_LeadPublisherCtrl_h_
#define _AI_Ctrl_LeadPublisherCtrl_h_

NAMESPACE_UPP


class LeadPublisherCtrl : public AiComponentCtrl {
	Splitter hsplit, vsplit;
	ArrayCtrl artists;
	WithPublisherInfo<Ctrl> info;
	
public:
	typedef LeadPublisherCtrl CLASSNAME;
	LeadPublisherCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	void ValueChange();
	void PasteArtists();
	void ImportJson();
	
	
};

INITIALIZE(LeadPublisherCtrl)


END_UPP_NAMESPACE

#endif
 
