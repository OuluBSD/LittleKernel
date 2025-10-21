#ifndef _AI_Core_Social_BiographyPlatform_h_
#define _AI_Core_Social_BiographyPlatform_h_





const VectorMap<String, Vector<String>>& GetMarketplaceSections();


struct BiographyProfileAnalysis {
	struct Response : Moveable<Response> {
		int year, category;
		String text, keywords;
		double score[BIOSCORE_COUNT] = {0,0,0,0};
		void Visit(Vis& v) {
			v.Ver(1)
			(1)	("year", year)
				("cat", category)
				("txt", text)
				("keyw", keywords)
				;
			for(int i = 0; i < BIOSCORE_COUNT; i++)
				v("sc" + IntStr(i), score[i]);
		}
	};
	
	Vector<Response> responses;
	VectorMap<int, String> categories;
	String biography_reaction;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	.VisitVector("responses", responses)
			("categories", categories)
			("biography_reaction", biography_reaction)
			;
	}
};

struct BiographyRoleAnalysis {
	Vector<String> merged_biography_reactions;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("merged_biography_reactions", merged_biography_reactions)
			;
	}
	
};

struct PlatformBiographyPlatform {
	Vector<String> packed_reactions;
	String profile_description_from_biography;
	String descriptions[PLATDESC_LEN_COUNT][PLATDESC_MODE_COUNT];
	bool platform_enabled = false;
	ArrayMap<String,PlatformAnalysisPhoto> epk_photos;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("packed_reactions", packed_reactions)
			("profile_description_from_biography", profile_description_from_biography)
			("platform_enabled", platform_enabled)
			.VisitMap("epk_photos", epk_photos)
			;
		for(int i = 0; i < PLATDESC_LEN_COUNT; i++) {
			for(int j = 0; j < PLATDESC_MODE_COUNT; j++) {
				String key = GetPlatformDescriptionModeKey(j) + "_" + GetPlatformDescriptionLengthKey(i);
				v(key, descriptions[i][j]);
			}
		}
	}
};

struct PhotoPromptGroupAnalysis {
	Vector<PlatformAnalysisPhoto> cluster_centers;
	int image_count = 0;
	String prompt;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	.VisitVector("cluster_centers", cluster_centers)
			("image_count", image_count)
			("prompt", prompt)
			;
	}
};

struct PhotoPromptLink : Moveable<PhotoPromptLink> {
	PlatformBiographyPlatform* pba;
	PlatformAnalysisPhoto* pap;
	PhotoPrompt* pp;
};

struct MarketplaceItem : Moveable<MarketplaceItem> {
	int priority = 0;
	Time added;
	String generic, brand, model;
	double price = 0., cx = 0., cy = 0., cz = 0., weight = 0.;
	String faults, works;
	bool broken = false, good = false;
	Vector<int64> images;
	int64 input_hash = 0;
	String other;
	String title, description;
	int category = 0, subcategory = 0;
	int year_of_manufacturing = 0;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("priority",priority)
			("added",added)
			("generic",generic)
			("brand",brand)
			("model",model)
			("price",price)
			("cx",cx)
			("cy",cy)
			("cz",cz)
			("weight",weight)
			("faults",faults)
			("works",works)
			("broken",broken)
			("good",good)
			("images",images)
			("title",title)
			("category",category)
			("subcategory",subcategory)
			("description",description)
			("input_hash",input_hash)
			("year_of_manufacturing",year_of_manufacturing)
			("other",other)
			;
	}
	
	String GetTitle() const {
		String s;
		s << generic;
		if (brand.GetCount()) {if (!s.IsEmpty()) s << " "; s << brand;}
		if (model.GetCount()) {if (!s.IsEmpty()) s << " "; s << model;}
		return s;
	}
};

struct BiographyPlatform : Component {
	Array<Array<BiographyProfileAnalysis>> profiles;
	Array<BiographyRoleAnalysis> roles;
	Array<PlatformBiographyPlatform> platforms;
	ArrayMap<String, PhotoPromptGroupAnalysis> image_types;
	Array<MarketplaceItem> market_items;
	
	COMPONENT_CONSTRUCTOR(BiographyPlatform)
	void Realize();
	void RealizePromptImageTypes();
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("profiles", profiles, VISIT_VECTOR_VECTOR)
			("roles", roles, VISIT_VECTOR)
			("platforms", platforms, VISIT_VECTOR)
			("image_types", image_types, VISIT_MAP)
			("items", market_items, VISIT_VECTOR)
			;
	}
	void Serialize(Stream& s) {TODO}
	hash_t GetHashValue() const {TODO; return 0;}
	Index<int> GetRequiredRoles() const;
	Index<int> GetRequiredCategories() const;
	Vector<PhotoPromptLink> GetImageTypePrompts(String image_type);
	
};

INITIALIZE(BiographyPlatform)




class SocialHeaderProcess : public SolverBase {
	
public:
	enum {
		PHASE_AUDIENCE_REACTS_SUMMARY,
		
		PHASE_COUNT,
	};
	
	Vector<BiographyProfileAnalysis*> ptrs;
	Vector<const RoleProfile*> prof_ptrs;
	Vector<String> role_descs;
	
public:
	typedef SocialHeaderProcess CLASSNAME;
	SocialHeaderProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static SocialHeaderProcess& Get(DatasetPtrs p);
	
private:
	
	void ProcessAudienceReactsSummary();
	void OnProcessAudienceReactsSummary(String res);
	
};

struct Platform;
struct PlatformData;
struct PlatformEntry;
struct PlatformThread;
struct PlatformComment;

class SocialContentProcess : public SolverBase {
	
public:
	enum {
		PHASE_MERGE_MESSAGES,
		
		PHASE_COUNT
	};
	
	
	struct MessageTask : Moveable<MessageTask> {
		const Platform* plat = 0;
		PlatformData* plat_data = 0;
		PlatformEntry* entry = 0;
		PlatformThread* thrd = 0;
		Vector<PlatformComment*> comments;
	};
	Vector<MessageTask> msg_tasks;
	MessageTask tmp_task;
	
	
	void TraverseMessageTasks(int plat_i);
	void TraverseMessageTasks(Vector<PlatformComment*>& before, PlatformComment& plc);
	
	
	void ProcessMergeMessages();
	void OnProcessMergeMessages(String res);
	
public:
	typedef SocialContentProcess CLASSNAME;
	SocialContentProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static SocialContentProcess& Get(DatasetPtrs mp);
	
};



class SocialNeedsProcess : public SolverBase {
	enum {
		PHASE_PACK_ROLE_REACTIONS,
		PHASE_PACK_PLATFORM_REACTIONS,
		PHASE_PLATFORM_DESCRIPTIONS,
		PHASE_PLATFORM_DESCRIPTION_REFINEMENTS,
		PHASE_PLATFORM_DESCRIPTION_TRANSLATED,
		
		PHASE_COUNT
	};
	Owner* owner = 0;
	Profile* profile = 0;
	BiographyPlatform* analysis = 0;
	Biography* biography = 0;
	BiographyPerspectives* snap = 0;
	
	
	struct Range : Moveable<Range> {
		int off = 0, len = 0;
		int input[2] = {0,0};
		String ToString() const {return Format("%d, %d: %d, %d", off, len, input[0], input[1]);}
	};
	Vector<Range> ranges;
	
	
	int CreateRange(int off, int len);
	void ProcessRoleReactions();
	void ProcessPlatformReactions();
	void ProcessPlatformDescriptions();
	void ProcessPlatformDescriptionRefinements();
	void ProcessPlatformDescriptionTranslated();
	void OnProcessRoleReactions(String res);
	void OnProcessPlatformReactions(String res);
	void OnProcessPlatformDescriptions(String res);
	void OnProcessPlatformDescriptionRefinements(String res);
	void OnProcessPlatformDescriptionTranslated(String res);
	
public:
	typedef SocialNeedsProcess CLASSNAME;
	SocialNeedsProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	void OnBatchError() override;
	
	static SocialNeedsProcess& Get(Profile& p, BiographyPerspectives& snap);
	
	Callback2<int,int> WhenProgress;
	
};

class MarketplaceProcess : public SolverBase {
	
public:
	enum {
		PHASE_DESCRIPTION,
		
		PHASE_COUNT
	};
	
	Owner* owner = 0;
	
public:
	typedef MarketplaceProcess CLASSNAME;
	MarketplaceProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static MarketplaceProcess& Get(DatasetPtrs p);
	
	void ProcessDescription();
	void MakeArgs(MarketplaceArgs& args);
	
};





#endif
