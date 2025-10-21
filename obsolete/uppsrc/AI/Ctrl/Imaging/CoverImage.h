#ifndef _AI_Imaging_CoverImage_h_
#define _AI_Imaging_CoverImage_h_

NAMESPACE_UPP


class ReleaseCoverImageCtrl : public AiComponentCtrl {
	Splitter vsplit, hsplit[3];
	ArrayCtrl attr_list, sugg_list;
	DocEdit attr_text, sugg_text;
	ImageViewerCtrl suggestion[4];
	
	
public:
	typedef ReleaseCoverImageCtrl CLASSNAME;
	ReleaseCoverImageCtrl();
	
	void ToolMenu(Bar& bar) override;
	void Data() override;
	void CreateSuggestionsForPrompts();
	void DataAttribute();
	void DataSuggestion();
	void DataSuggestionImage();
	void OnAttributeChange();
	void OnSuggestionChange();
	void Do(int fn);
	void SuggestionMenu(Bar& bar);
	
};

INITIALIZE(ReleaseCoverImageCtrl)


END_UPP_NAMESPACE

#endif
 
