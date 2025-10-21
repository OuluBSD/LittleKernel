#ifndef _AI_Core_PlatformProcess_h_
#define _AI_Core_PlatformProcess_h_



struct Platform : Moveable<Platform> {
	const char* group = 0;
	const char* name = 0;
	const char* description = 0;
	int profile_type = PLATFORM_PROFILE_ANY;
	bool attrs[PLATFORM_ATTR_COUNT];
	Vector<String> functions;
	
	void SetAttr(String name, bool value);
	
	Platform& operator << (const char* fn) {functions << fn; return *this;}
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1);
	}
};

const Vector<Platform>& GetPlatforms();


struct PlatformComment {
	String user;
	String orig_message, message, keywords, location;
	String text_merged_status;
	Time published;
	bool generate = false;
	Array<PlatformComment> responses;
	
	
	int GetTotalComments() const;
	void ClearMerged();
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("user", user)
			("orig_message", orig_message)
			("message", message)
			("keywords", keywords)
			("location", location)
			("tms", text_merged_status)
			("published", published)
			("generate", generate)
			.VisitVector("responses", responses)
			;
	}
};

struct PlatformThread {
	String user, title;
	Array<PlatformComment> comments;
	
	int GetTotalComments() const;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("user", user)
			("title", title)
			("comments", comments, VISIT_VECTOR)
			;
	}
};

struct PlatformEntry {
	Array<PlatformThread> threads;
	String title, subforum;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("threads", threads, VISIT_VECTOR)
			("title", title)
			("subforum", subforum)
			;
	}
};

struct PlatformData {
	Array<PlatformEntry> entries;
	
	
	int GetTotalEntryCount() const;
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("entries", entries, VISIT_VECTOR)
			;
	}
};

struct ProfileData {
	hash_t hash = 0;
	Array<PlatformData> platforms;
	
	Profile* profile = 0;
	
	
	void Visit(Vis& v);
	void Load();
	void Store();
	
	static void StoreAll();
	static Array<ProfileData>& GetAll();
	static ProfileData& Get(Profile& p);
	
	
};


struct PlatformAnalysis {
	Index<int> roles;
	VectorMap<String,String> epk_text_fields;
	VectorMap<String,PlatformAnalysisPhoto> epk_photos;
	
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("roles", roles)
			("epk_text_fields", epk_text_fields)
			("epk_photos", epk_photos, VISIT_MAP)
			;
	}
	
	int GetRoleScoreSum(const PlatformManager& plat, int score_i) const;
	double GetRoleScoreSumWeighted(const PlatformManager& plat, int score_i) const;
	
};

struct SocietyRoleAnalysis {
	Vector<int> scores;
	
	SocietyRoleAnalysis() {scores.SetCount(SOCIETYROLE_SCORE_COUNT,0);}
	void Zero() {for (int& i : scores) i = 0;}
	void Visit(Vis& v) {v.Ver(1)(1)("scores",scores);}
	int GetScoreSum() const {return Sum(scores);}
};

struct PlatformManager : Component {
	ArrayMap<String,PlatformAnalysis> platforms;
	ArrayMap<String, SocietyRoleAnalysis> roles;
	
	COMPONENT_CONSTRUCTOR(PlatformManager)
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("roles", roles, VISIT_MAP)
			("platforms", platforms, VISIT_MAP)
			;
	}
	PlatformAnalysis& GetPlatform(int plat_i);
	SocietyRoleAnalysis& GetAddRole(int role_i);
	const SocietyRoleAnalysis* FindRole(int role_i) const;
	
};

INITIALIZE(PlatformManager)


class PlatformProcess : public SolverBase {
	
public:
	enum {
		PHASE_ANALYZE_ROLE_SCORES,
		PHASE_ANALYZE_PLATFORM_ROLES,
		PHASE_ANALYZE_PLATFORM_EPK_TEXT_FIELDS,
		PHASE_ANALYZE_PLATFORM_EPK_PHOTO_TYPES,
		PHASE_ANALYZE_PLATFORM_EPK_PHOTO_AI_PROMPTS,
		
		PHASE_COUNT,
	};
	
	struct VisionTask : Moveable<VisionTask> {
		BioImage* bimg = 0;
		String jpeg;
	};
	Vector<VisionTask> vision_tasks;
	void TraverseVisionTasks();
	
	
	struct ImageSummaryTask : Moveable<ImageSummaryTask> {
		BiographyCategory* bcat = 0;
		BioYear* by = 0;
		BioImage* summary = 0;
		BioRange range;
		int bcat_i = -1;
	};
	Vector<ImageSummaryTask> imgsum_tasks;
	void TraverseImageSummaryTasks();
	
	
public:
	typedef PlatformProcess CLASSNAME;
	PlatformProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static PlatformProcess& Get(DatasetPtrs p);
	
private:
	
	void ProcessAnalyzeRoleScores();
	void ProcessAnalyzePlatformRoles();
	void ProcessAnalyzePlatformEpkTextFields();
	void ProcessAnalyzePlatformEpkPhotoTypes();
	void ProcessAnalyzePlatformEpkPhotoAiPrompts();
	void OnProcessAnalyzeRoleScores(String res);
	void OnProcessAnalyzePlatformRoles(String res);
	void OnProcessAnalyzePlatformEpkTextFields(String res);
	void OnProcessAnalyzePlatformEpkPhotoTypes(String res);
	void OnProcessAnalyzePlatformEpkPhotoAiPrompts(String res);
	
};




#endif
