#ifndef _AI_Ctrl_PlatformManager_h_
#define _AI_Ctrl_PlatformManager_h_

NAMESPACE_UPP


class PlatformManagerCtrl : public AiComponentCtrl {
	Splitter hsplit, vsplit, bottom, epk_photo_prompt_split;
	TabCtrl plat_tabs, epk_tabs;
	ArrayCtrl roles, platforms, epk_text_fields, epk_photo_types, epk_photo_prompts;
	WithSocialPlatform<Ctrl> plat;
	ImageViewerCtrl epk_photo_prompt_example;
	
public:
	typedef PlatformManagerCtrl CLASSNAME;
	PlatformManagerCtrl();
	
	void Data() override;
	void DataPlatform();
	void ToolMenu(Bar& bar) override;
	void PlatformMenu(Bar& bar);
	void Do(int fn);
	void SetSorting(int col);
	void OnPhotoPrompt();
	void ImportJson();
};

INITIALIZE(PlatformManagerCtrl)


END_UPP_NAMESPACE

#endif
 
