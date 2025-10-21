#ifndef _AI_Core_Prompting_TaskManager_h_
#define _AI_Core_Prompting_TaskManager_h_



struct FarStage;

struct TaskMgr {
	Array<AiTask> tasks;

	RWMutex lock;
	Mutex task_lock;
	AiTask* active_task = 0;
	int actual = 0, total = 0;
	String status;

	// Local
	Vector<hash_t> task_order;
	int task_i = 0;
	int keep_going_counter = 0;

	// Temp
	Vector<String> task_order_dbg;
	bool task_order_cache_missed = false;
	mutable hash_t hash = 0;
	int iters = 0;
	int spawn_id = 0;

	typedef TaskMgr CLASSNAME;
	virtual ~TaskMgr() {}

	AiTask& AddTask();
	void Process();
	void ProcessSingle(int task_i);
	void StartSingle(int task_i) { Thread::Start(THISBACK1(ProcessSingle, task_i)); }

	void Translate(String orig_lang, String orig_txt, String trans_lang,
	               Event<String> WhenResult, bool slightly_dialect = false);
	void CreateImage(String prompt, int count, Event<Array<Image>&> WhenResult,
	                 int reduce_size_mode = 0, Event<> WhenError = Event<>());
	void GetEditImage(Image orig, Image mask, String prompt, int count,
	                  Event<Array<Image>&> WhenResult, Event<> WhenError = Event<>());
	void VariateImage(Image orig, int count, Event<Array<Image>&> WhenResult,
	                  Event<> WhenError = Event<>());
	void GetModels(const ModelArgs& args, Event<String> WhenResult);
	void RawCompletion(String prompt, Event<String> WhenResult);
	void GetCompletion(CompletionArgs& args, Event<String> WhenResult);
	void GetChat(ChatArgs& args, Event<String> WhenResult);
	void GetGenericPrompt(const GenericPromptArgs& args, Event<String> WhenResult, String title=String());
	void GetVision(const String& jpeg, const VisionArgs& args, Event<String> WhenResult);
	void GetTranscription(const TranscriptionArgs& args, Event<String> WhenResult);
	void GetBasic(const TaskArgs& args, Event<String> WhenResult, String title=String(), bool keep_going=false);
	void GetJson(const TaskArgs& args, Event<String> WhenResult, String title=String(), bool keep_going=false);
	void Get(bool json, const TaskArgs& args, Event<String> WhenResult, String title=String(), bool keep_going=false);
	void GetFarStage(Ptr<FarStage> stage, int fn_i, Value args, Event<String> WhenResult, Event<> WhenDone);



	// DEPRECATED INTERFACE:
	// Update to use Get(TokenArgs... function

	
	void GetCode(const CodeArgs& args, Event<String> WhenResult);
	
	// Source text analysis
	void GetTokenData(const TokenArgs& args, Event<String> WhenResult);
	void GetSourceDataAnalysis(const SourceDataAnalysisArgs& args, Event<String> WhenResult, bool keep_going=false);
	void GetPhraseData(const PhraseArgs& args, Event<String> WhenResult);
	void GetActionAnalysis(const ActionAnalysisArgs& args, Event<String> WhenResult);
	void GetAttributes(const AttrArgs& args, Event<String> WhenResult);
	void GetScriptSolver(const ScriptSolverArgs& args, Event<String> WhenResult);
	
	// ECS Components
	void GetSocial(const SocialArgs& args, Event<String> WhenResult);
	void GetBiographySummary(const BiographySummaryProcessArgs& args, Event<String> WhenResult);
	void GetLeadSolver(const LeadSolverArgs& args, Event<String> WhenResult);
	void GetPerspectiveProcess(const BeliefArgs& args, Event<String> WhenResult);
	void GetMarketplace(const MarketplaceArgs& args, Event<String> WhenResult);
	
	template <class T>
	String MakeName(T& o, const char* name);

	static void Setup(::Ide* ide);
};

TaskMgr& AiTaskManager();



#endif
