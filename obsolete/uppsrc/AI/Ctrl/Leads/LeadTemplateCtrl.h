#ifndef _AI_Ctrl_LeadTemplateCtrl_h_
#define _AI_Ctrl_LeadTemplateCtrl_h_

NAMESPACE_UPP


class LeadTemplateCtrl : public AiComponentCtrl {
	ArrayCtrl templates;
	
public:
	typedef LeadTemplateCtrl CLASSNAME;
	LeadTemplateCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	void ImportJson();
	
};

INITIALIZE(LeadTemplateCtrl)


END_UPP_NAMESPACE

#endif
 
