#ifndef _AI_Core_Biography_h_
#define _AI_Core_Biography_h_



struct BioImage {
	String keywords, text, native_text;
	String image_keywords, image_text;
	String location;
	String people;
	Time time;
	int time_accuracy = TIME_ACCURACY_NONE;
	int64 image_hash = 0;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("keywords", keywords)
			("text", text)
			("native_text", native_text)
			("image_keywords", image_keywords)
			("image_text", image_text)
			("location", location)
			("people", people)
			("time", time)
			("time_accuracy", time_accuracy)
			("image_hash", image_hash)
			;
	}
};

struct BioRange : Moveable<BioRange> {
	int off = 0, len = 0;
	BioRange() {}
	BioRange(BioRange&& r) {*this = r;}
	BioRange(const BioRange& r) {*this = r;}
	void operator=(const BioRange& r) {off = r.off; len = r.len;}
	hash_t GetHashValue() const {CombineHash c; c.Do(off).Do(len); return c;}
	bool operator==(const BioRange& r) const {return r.off == off && r.len == len;}
	void Visit(Vis& v) {v("off", off)("len", len);}
	bool operator()(const BioRange& a, const BioRange& b) const {
		int a0 = a.off + a.len - 1; // last item in range
		int b0 = b.off + b.len - 1;
		if (a0 != b0) return a0 < b0; // sort primarily by lesser last item
		return a.len < b.len; // otherwise sort by lesser range length
	}
};


struct BioYear {
	struct Element : Moveable<Element> {
		String key, value;
		byte scores[SCORE_COUNT] = {0,0,0,0,0, 0,0,0,0,0};
		void Visit(Vis& v) {v("k",key)("v",value); for(int i = 0; i < SCORE_COUNT; i++) v("s" + IntStr(i),scores[i]);}
		void ResetScore() {memset(scores, 0, sizeof(scores));}
		double GetAverageScore() const;
	};
	int year = 0;
	String keywords, text, native_text;
	Array<BioImage> images;
	ArrayMap<BioRange,BioImage> image_summaries;
	String image_text;
	Vector<Element> elements;
	hash_t source_hash = 0;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("year", year)
			("keywords", keywords)
			("native_text", native_text)
			("text", text)
		.VisitVector("images", images)
		.VisitMapKV("image_summaries", image_summaries)
			("image_text", image_text)
		.VisitVector("elements", elements)
			("source_hash", (int64&)source_hash)
			;
	}
	bool operator()(const BioYear& a, const BioYear& b) const {return a.year < b.year;}
	void RealizeImageSummaries();
	BioImage& GetAddImageSummary(int begin_year, int years);
	int FindElement(const String& key) const;
	String JoinElementMap(String delim0, String delim1);
	double GetAverageScore() const {double d = 0; for (const auto& el : elements) d += el.GetAverageScore(); return d;}
	
};


struct BiographyCategory {
	Array<BioYear> years;
	ArrayMap<BioRange,BioYear> summaries;
	
	
	BiographyCategory() {}
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	.VisitVector("years", years)
			.VisitMapKV("summaries", summaries)
			;
	}
	BioYear& GetAdd(int year);
	int GetFilledCount() const;
	int GetFilledImagesCount() const;
	void RealizeSummaries();
	BioYear& GetAddSummary(int begin_year, int years);
	
};

struct Biography : Component {
	
protected:
	ArrayMap<String, BiographyCategory> categories;
	
public:
	COMPONENT_CONSTRUCTOR(Biography);
	
	struct CatSorter {
		bool operator()(const String& a, const String& b) const {
			int ai = FindBiographyCategoryEnum(a);
			int bi = FindBiographyCategoryEnum(b);
			if (bi == -1) return true;
			if (ai == -1) return false;
			return ai < bi;
		}
	};
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("categories", categories, VISIT_MAP)
			;
		if (v.IsLoading()) {
			Sort();
		}
	}
	
	BiographyCategory& GetAdd(Owner& o, int enum_);
	BiographyCategory* Find(Owner& o, int enum_);
	const BiographyCategory* Find(Owner& o, int enum_) const;
	void Sort() {
		ArrayMap<String, BiographyCategory> tmp;
		Swap(tmp, categories);
		for(int i = 0; i < BIOCATEGORY_COUNT; i++) {
			String key = GetBiographyCategoryEnumCstr(i);
			int j = tmp.Find(key);
			if (j >= 0) {
				BiographyCategory& cat = tmp[j];
				Swap(categories.Add(key), cat);
			}
			else categories.Add(key);
		}
	}
	Array<BiographyCategory>& AllCategories() {return categories.GetValues();}
	const Array<BiographyCategory>& AllCategories() const {return categories.GetValues();}
	String GetCategoryName(int i) {return KeyToName(categories.GetKey(i));}
	
	void ClearSummary();
	void ClearAll() {categories.Clear();}
};

INITIALIZE(Biography)

struct PhotoPrompt : Moveable<PhotoPrompt> {
	String prompt;
	Vector<String> instructions;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("prompt", prompt)
			("instructions", instructions)
			;
	}
	
	String GetFilePath(String dir, int i) const;
	
};

struct PlatformAnalysisPhoto : Moveable<PlatformAnalysisPhoto> {
	String description;
	Array<PhotoPrompt> prompts;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("description", description)
			.VisitVector("prompts", prompts)
			;
	}
};


// TODO easy remove?
#define USE_IMPROVED_ELEMENTS 0

#if USE_IMPROVED_ELEMENTS
	#define ELEMENTS_VAR	improved_elements
#else
	#define ELEMENTS_VAR	elements
#endif

struct ConceptStory : Moveable<ConceptStory> {
	struct Element : Moveable<Element> {
		String key, value;
		Color clr;
		byte scores[SCORE_COUNT] = {0,0,0,0,0, 0,0,0,0,0};
		void Visit(Vis& v) {
			v.Ver(1)
			(1)	("k",key)("v",value)("clr",clr);
			for(int i = 0; i < SCORE_COUNT; i++)
				v("s" + IntStr(i),scores[i]);
		}
		void ResetScore() {memset(scores, 0, sizeof(scores));}
		double GetAverageScore() const;
	};
	
	hash_t hash = 0;
	String desc;
	Vector<Element> elements;
	#if USE_IMPROVED_ELEMENTS
	Vector<Element> improved_elements;
	#endif
	int src = 0;
	int typeclass = -1;
	int content = -1;
	
	int FindElement(const String& key) const;
	int FindImprovedElement(const String& key) const;
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("hash", (int64&)hash)
			("desc", desc)
			.VisitVector("elements", elements)
			("src", src)
			("typeclass", typeclass)
			("content", content)
			;
		#if USE_IMPROVED_ELEMENTS
		v.VisitVector("improved_elements", improved_elements);
		#endif
	}
	
	double AvSingleScore(int i) const;
	Color GetAverageColor() const;
	double GetAverageScore() const;
	bool operator()(const ConceptStory& a, const ConceptStory& b) const;
	
};

struct Concept {
	int64 belief_uniq = 0;
	String name;
	int snap_rev = -1;
	Time created;
	Vector<ConceptStory> stories;
	
	int FindStory(hash_t h) const;
	ConceptStory& GetAddStory(hash_t h);
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("belief_uniq", belief_uniq)
			("name", name)
			("snap_rev", snap_rev)
			("created", created)
			.VisitVector("stories", stories)
			;
	}
};

struct BiographyPerspectives : Component {
	int revision = 0;
	Time last_modified;
	Array<Concept> concepts;
	
	CLASSTYPE(BiographyPerspectives)
	BiographyPerspectives(VfsValue& o) : Component(o) {}
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("revision", revision)
			("last_modified", last_modified)
			.VisitVector("concepts", concepts)
		;
	}
	
};

INITIALIZE(BiographyPerspectives)



#endif
