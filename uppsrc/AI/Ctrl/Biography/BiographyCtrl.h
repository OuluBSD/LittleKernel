#ifndef _AI_Ctrl_BiographyCtrl_h_
#define _AI_Ctrl_BiographyCtrl_h_

NAMESPACE_UPP



class BiographyCtrl : public AiComponentCtrl {
	Splitter hsplit;
	TabCtrl tabs;
	ArrayCtrl categories;
	
	struct {
		Splitter vsplit;
		ArrayCtrl years;
		WithBiography<Ctrl> year;
		VectorMap<String,String> element_hints;
	} main;
	
	struct {
		Splitter vsplit;
		ArrayCtrl elements;
		WithBiography<Ctrl> block;
		int sort_column = 0;
	} el; // elements
	
	struct {
		Splitter vsplit;
		ArrayCtrl blocks;
		WithBiography<Ctrl> block;
	} summary;
	
	struct {
		Splitter vsplit, bsplit;
		ArrayCtrl years, entries;
		WithImageBiography<Ctrl> year;
		ImageViewerCtrl img;
	} image;
	
	struct {
		Splitter vsplit;
		ArrayCtrl blocks;
		WithBiography<Ctrl> block;
	} image_summary;
	
	struct {
		Splitter vsplit;
		ArrayCtrl blocks;
		WithBiography<Ctrl> block;
	} audience;
	
	
public:
	typedef BiographyCtrl CLASSNAME;
	BiographyCtrl();
	
	void Data() override;
	void DataCategory();
	
	// Main
	void Main_Ctor();
	void Main_ToolMenu(Bar& bar);
	void Main_DataCategory();
	void Main_DataYear();
	void Main_OnValueChange();
	void Main_Do(int fn);
	void Main_ImportJson();
	void Main_OnTranslate(String s);
	void Main_OnKeywords(String s);
	void Main_UpdateElementHints();
	void Main_UpdateElements();
	
	// Elements
	void El_Ctor();
	void El_ToolMenu(Bar& bar);
	void El_Do(int fn);
	void El_DataCategory();
	void El_DataElement();
	void El_DataYear();
	void El_OnValueChange();
	
	// Summary
	void Summary_Ctor();
	void Summary_DataCategory();
	void Summary_DataYear();
	void Summary_OnValueChange();
	void Summary_Translate();
	void Summary_MakeKeywords();
	void Summary_UpdateElements();
	void Summary_Do(int fn);
	void Summary_OnTranslate(String s);
	void Summary_OnKeywords(String s);
	void Summary_ToolMenu(Bar& bar);
	void Summary_EntryListMenu(Bar& bar);
	
	// Image
	void Image_Ctor();
	void Image_Data();
	void Image_DataCategory();
	void Image_DataYear();
	void Image_DataEntry();
	void Image_OnCategoryCursor();
	void Image_OnValueChange();
	void Image_Translate();
	void Image_MakeKeywords(int fn);
	void Image_OnTranslate(String s);
	void Image_OnKeywords(int fn, String s);
	void Image_ToolMenu(Bar& bar);
	void Image_EntryListMenu(Bar& bar);
	void Image_AddEntry();
	void Image_RemoveEntry();
	void Image_PasteImagePath();
	void Image_AnalyseImage();
	void Image_SetCurrentImage(Image img);
	void Image_Do(int fn);
	
	// Image summary
	void ImageSummary_Ctor();
	void ImageSummary_DataCategory();
	void ImageSummary_DataYear();
	void ImageSummary_OnValueChange();
	void ImageSummary_Translate();
	void ImageSummary_MakeKeywords();
	void ImageSummary_Do(int fn);
	void ImageSummary_OnTranslate(String s);
	void ImageSummary_OnKeywords(String s);
	void ImageSummary_ToolMenu(Bar& bar);
	void ImageSummary_EntryListMenu(Bar& bar);
	
	
	
	void Translate();
	void MakeKeywords();
	void GetElements();
	void GetElementHints();
	void GetElementScores();
	void ToolMenu(Bar& bar) override;
	void EntryListMenu(Bar& bar);
	void SnapshotMenu(Bar& bar);
	void EditPos(JsonIO& json) override;
};

INITIALIZE(BiographyCtrl)


END_UPP_NAMESPACE

#endif
 
