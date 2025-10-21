#ifndef _AI_Ctrl_LeadSourceCtrl_h_
#define _AI_Ctrl_LeadSourceCtrl_h_

NAMESPACE_UPP


class LeadSourceCtrl : public AiComponentCtrl {
	Splitter vsplit, hsplit, mainsplit, bsplit, bvsplit, bssplit;
	ArrayCtrl list, payouts, prices, attrs;
	ArrayCtrl bools, strings, list_names, list_values;
	ArrayCtrl song_typecasts, lyrics_ideas, music_styles;
	
	// params
	bool have_last_seen_limit = false;
	int last_seen_limit_mins = 24*60*60;
	
public:
	typedef LeadSourceCtrl CLASSNAME;
	LeadSourceCtrl();
	
	void Data() override;
	void DataWebsite();
	void DataPayout();
	void DataPrice();
	void DataOpportunity();
	void DataAnalyzedList();
	void CreateScript();
	void CopyHeaderClipboard();
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	void ImportJson();
	
};

INITIALIZE(LeadSourceCtrl)

struct LeadCache {
	VectorMap<String, Time> last_update;
	
	
	LeadCache() {LoadFromFile(*this, ConfigFile("lead-cache.bin"));}
	~LeadCache() {StoreToFile(*this, ConfigFile("lead-cache.bin"));}
	void Serialize(Stream& s) {s % last_update;}
	
};

class LeadSolver : public SolverBase {
	enum {
		LS_DOWNLOAD_WEBSITES,
		LS_PARSE_WEBSITES,
		LS_ANALYZE_BOOLEANS,
		LS_ANALYZE_STRINGS,
		LS_ANALYZE_LISTS,
		
		LS_COARSE_RANKING,
		LS_AVERAGE_PAYOUT_ESTIMATION,
		
		LS_ANALYZE_POTENTIAL_SONG_TYPECAST,
		LS_ANALYZE_POTENTIAL_SONG_IDEAS,
		LS_ANALYZE_POTENTIAL_MUSIC_STYLE_TEXT,
		
		LS_TEMPLATE_TITLE_AND_TEXT,
		LS_TEMPLATE_ANALYZE,
		
		LS_COUNT
	};
	
	// Params
	double score_limit_factor = 0.8;
	int max_rank = 100;
	
	void ProcessDownloadWebsites(bool parse);
	void ParseWebsite(int batch, String content);
	String ProcessDownloadWebsiteUrl(String url);
	void ProcessAnalyzeFn(int fn, Event<String> cb);
	void ProcessAnalyzeBooleans();
	void ProcessAnalyzeStrings();
	void ProcessAnalyzeLists();
	void ProcessAnalyzeSongTypecast();
	void ProcessAnalyzeLyricsIdeas();
	void ProcessAnalyzeMusicStyle();
	void ProcessCoarseRanking();
	void ProcessAveragePayoutEstimation();
	void ProcessTemplateTitleAndText();
	void ProcessTemplateAnalyze();
	void OnProcessAnalyzeBooleans(String res);
	void OnProcessAnalyzeStrings(String res);
	void OnProcessAnalyzeLists(String res);
	void OnProcessAnalyzeSongTypecast(String res);
	void OnProcessAnalyzeLyricsIdeas(String res);
	void OnProcessAnalyzeMusicStyle(String res);
	void OnProcessAveragePayoutEstimation(String res);
	void OnProcessTemplateTitleAndText(String res);
	void OnProcessTemplateAnalyze(String res);
	
	double GetAverageOpportunityScore();
	bool SkipLowScoreOpportunity();
	
	static String GetLeadCacheDir();
	
public:
	typedef LeadSolver CLASSNAME;
	LeadSolver();
	
	static LeadSolver& Get(DatasetPtrs p);
	int GetPhaseCount() const override;
	void DoPhase() override;
	
	Callback2<int,int> WhenProgress;
	
};


END_UPP_NAMESPACE

#endif
 
