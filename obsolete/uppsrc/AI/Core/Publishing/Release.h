#ifndef _AI_Core_Release_h_
#define _AI_Core_Release_h_


struct Song;


// TODO REMOVE
struct ComponentIdea {
	String title, target_song, reference_song, description;
	
	void Serialize(Stream& s) {
		s % title
		  % target_song
		  % reference_song
		  % description
		  ;
	}
	void Jsonize(JsonIO& json) {
		json
			("title", title)
			("target_song", target_song)
			("reference_song", reference_song)
			("description", description)
			;
	}
};

// TODO rename Release
struct Release : Component
{
	// Public
	String						title;
	Date						date;
	VectorMap<String,String>	data;
	Array<ComponentIdea>		ideas;
	int							year_of_content = 0;
	Vector<String>				lyric_summaries;
	Vector<String>				song_analysis;
	Vector<String>				analysis;
	Vector<String>				cover_suggestions;
	
	Entity*						entity = 0;
	
	
	COMPONENT_CONSTRUCTOR(Release)
	void Store(Entity& e);
	void LoadTitle(Entity& e, String title);
	Component& GetAddComponent(String name);
	//Component& RealizeReversed(Component& s);
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("title", title)
			("date", date)
			("data", data)
			("ideas", ideas)
			("year_of_content", year_of_content)
			("lyric_summaries", lyric_summaries)
			("song_analysis", song_analysis)
			("analysis", analysis)
			("cover_suggestions", cover_suggestions)
			;
		/*if (json.IsStoring()) {
			{
				Vector<String> names;
				for (Component& s : components) {s.Store(*this); names.Add(s.file_title);}
				json(__comps, names);
			}
		}
		if (json.IsLoading()) {
			{
				components.Clear();
				Vector<String> names;
				json(__comps, names);
				for (String n : names) components.Add().LoadTitle(*this, n);
			}
			analysis.SetCount(SNAPANAL_COUNT);
		}*/
	}
	
	Vector<Song*>& GetSongs() const;
	
	bool operator()(const Release& a, const Release& b) const {
		if (a.date != b.date) return a.date < b.date;
		return a.title < b.title;
	}
	
	
	
	
};

INITIALIZE(Release)

// TODO rename to ReleaseSolver
class SnapSolver : public SolverBase {
	
public:
	enum {
		PHASE_LYRICS_SUMMARIES,
		PHASE_LYRICS_SUMMARY,
		PHASE_LYRICS_PSYCHOANALYSIS,
		PHASE_LYRICS_SOCIAL_PSYCHOLOGY_ANALYSIS,
		PHASE_MARKET_VALUE_ANALYSIS,
		PHASE_MARKETING_SUGGESTION,
		PHASE_ART_SUGGESTION,
		PHASE_COVER_SUGGESTION,
		PHASE_COVER_SUGGESTION_DALLE2,
		
		PHASE_COUNT
	};
	
	Release* snap = 0;
	Entity* entity = 0;
	
	void OnProcessAnalyzeRoleScores(String res);
	
public:
	typedef SnapSolver CLASSNAME;
	SnapSolver();
	
	int GetPhaseCount() const override;
	void DoPhase() override;
	
	static SnapSolver& Get(Release& s);
	
};



#endif
