#ifndef _AI_Ctrl_BiographyPlatform_h_
#define _AI_Ctrl_BiographyPlatform_h_

NAMESPACE_UPP


class BiographyPlatformCtrl : public AiComponentCtrl {
	TabCtrl tabs;
	
	
	struct WithOwner1 {BiographyPlatformCtrl& o; WithOwner1(BiographyPlatformCtrl& o) : o(o){}};
	
	// Platforms tab
	struct Platforms : WithOwner1 {
		typedef Platforms CLASSNAME;
		Splitter hsplit;
		TabCtrl tabs;
		ArrayCtrl platforms;
		
		Platforms(BiographyPlatformCtrl& o) : WithOwner1(o), header(o,*this), messaging(o,*this), epk_photo(o,*this), needs(o,*this), marketplace(o,*this) {}
		void Ctor();
        void Do(int fn);
		void EpkPhoto_Ctor();
		void Data();
		void DataPlatform();
		void ToolMenu(Bar& bar);
		void Menu(Bar& bar);
		
		struct WithOwner {BiographyPlatformCtrl& o; Platforms& p; WithOwner(BiographyPlatformCtrl& o, Platforms& p) : o(o),p(p){}};
		
		struct Header : WithOwner {
			typedef Header CLASSNAME;
			using WithOwner::WithOwner;
			Splitter vsplit;
			ArrayCtrl entries;
			Splitter entry_split;
			ArrayCtrl attr_keys;
			DocEdit attr_value;
            void Ctor();
            void DataPlatform();
            void ToolMenu(Bar& bar);
            void EntryListMenu(Bar& bar);
            void OnValueChange();
            void Do(int fn);
		} header;
		
		struct Messaging : WithOwner {
			typedef Messaging CLASSNAME;
			using WithOwner::WithOwner;
			Splitter vsplit, threadsplit;
			ArrayCtrl threads, entries, comments;
			WithSocialEntry<Ctrl> entry;
			void Ctor();
			void DataPlatform();
			void DataEntry();
			void DataThread();
			void DataComment();
			void Clear();
			void ClearEntry();
			void OnValueChange();
			void AddEntry();
			void RemoveEntry();
			void AddThread();
			void RemoveThread();
			void AddComment();
			void RemoveComment();
			void PasteResponse(int fn);
			void ToolMenu(Bar& bar);
			void EntryListMenu(Bar& bar);
			void ThreadListMenu(Bar& bar);
			void CommentListMenu(Bar& bar);
			void Do(int fn);
		} messaging;
		
		struct EpkPhoto : WithOwner {
			typedef EpkPhoto CLASSNAME;
			using WithOwner::WithOwner;
			Splitter epk_photo_prompt_split, epk_photo_multi_image_split;
			ArrayCtrl  epk_photo_prompts;
			ImageViewerCtrl epk_photo[4];
			void Ctor();
			void DataPlatform();
			void ToolMenu(Bar& bar);
			void PhotoPromptMenu(Bar& bar);
			void OnPhotoPrompt();
            void Do(int fn);
		} epk_photo;
		
		struct Needs : WithOwner {
			typedef Needs CLASSNAME;
			using WithOwner::WithOwner;
			Splitter vsplit, rolesplit, platsplit, eventsplit;
			ArrayCtrl roles, needs, causes, messages;
			ArrayCtrl actions, action_causes;
			ArrayCtrl events, entries;
			DocEdit event, entry;
            void Ctor();
            void DataPlatform();
            void DataRole();
            void DataNeed();
            void DataAction();
            void DataEvent();
            void DataEntry();
            void ToolMenu(Bar& bar);
            void Do(int fn);
		} needs;
		
		struct Marketplace : WithOwner {
			typedef Marketplace CLASSNAME;
			using WithOwner::WithOwner;
			Splitter hsplit, imgsplit;
			ArrayCtrl items, images;
			TabCtrl tabs;
			WithMarketplace<Ctrl> form;
			WithMarketplaceViewer<Ctrl> viewer;
			ImageViewerCtrl img;
			int filter_priority = MARKETPRIORITY_SELL_UPCOMING;
			void Ctor();
			void DataPlatform();
			void ToolMenu(Bar& bar);
			void Do(int fn);
			void DataItem();
			void DataImage();
			void DataCategory();
			void DataSubCategory();
			void OnValueChange();
			void OnDimensionChange();
			void ClearForm();
			void PasteImagePath();
			void MakeTempImages();
			void OnCategory();
			void OnSubCategory();
			void SetCategoryShorcut(int i);
			void SetCurrentImage(Image img);
			void LoadImagePath(String path);
			void ShowItems(int priority);
			String GetPackageString(int w, int h, int d, double weight);
		} marketplace;
		
	} p;
	// Clustered prompts tab
	struct Clusters : WithOwner1 {
		typedef Clusters CLASSNAME;
		using WithOwner1::WithOwner1;
		Splitter hsplit, vsplit, bsplit;
		ArrayCtrl image_types, prompts;
		DocEdit final_prompt;
		ImageViewerCtrl epk_photo[4];
        void Ctor();
        void Do(int fn);
        void Data();
        void DataImageType();
        void ToolMenu(Bar& bar);
	} c;
	
	// Audience tab
	struct Audience : WithOwner1 {
		typedef Audience CLASSNAME;
		using WithOwner1::WithOwner1;
		Splitter menusplit, hsplit, vsplit, bsplit;
		ArrayCtrl roles, profiles, responses, entries;
		WithAudience<Ctrl> entry;
		ImageViewerCtrl img;
		void Ctor();
        void Do(int fn);
		void Data();
		void DataRole();
		void DataProfile();
		void DataResponse();
		void ToolMenu(Bar& bar);
		void EntryListMenu(Bar& bar);
	} a;
public:
	typedef BiographyPlatformCtrl CLASSNAME;
	BiographyPlatformCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	void SetSorting(int col);
	void ImportJson();
};

INITIALIZE(BiographyPlatformCtrl)


END_UPP_NAMESPACE

#endif
 
