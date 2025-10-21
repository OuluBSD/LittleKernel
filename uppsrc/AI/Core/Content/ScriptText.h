#ifndef _AI_Core_ScriptText_h_
#define _AI_Core_ScriptText_h_



class ScriptTextProcess : public SolverBase {
	
public:
	enum {
		// 0
		PHASE_INPUT,
		PHASE_CONTEXT,
		// 1
		PHASE_TOKENIZE,
		// 2
		PHASE_ANALYZE_PUBLIC_FIGURE,
		PHASE_ANALYZE_ELEMENTS,
		// 3
		PHASE_TOKENS_TO_LANGUAGES,
		PHASE_TOKENS_TO_WORDS,
		PHASE_COUNT_WORDS,
		// 4
		// 5
		PHASE_IMPORT_TOKEN_TEXTS,
		PHASE_CLASSIFY_SENTENCES,
		// 6
		PHASE_VIRTUAL_PHRASE_PARTS,
		// 7
		PHASE_VIRTUAL_PHRASE_PART_STRUCTS,
		// 8
		PHASE_ELEMENT,
		PHASE_COLOR,
		PHASE_ATTR,
		PHASE_ACTIONS,
		PHASE_SCORES,
		PHASE_TYPECLASS,
		PHASE_CONTENT,
		// 9
		PHASE_COLORS,
		PHASE_ATTRS,
		// 10
		PHASE_MAIN_GROUPS,
		PHASE_SIMPLIFY_ATTRS,
		// 11
		PHASE_WORDNET,
		PHASE_PHRASE_TRANSFER,
		PHASE_TRANSITION_PARALLEL,
		PHASE_TRANSITION_SERIAL,
		// 12
		PHASE_TEXT_KEYPOINTS,
		PHASE_TEXT_KEYPOINT_DESCRIPTORS,
		PHASE_TEXT_KEYPOINT_CLUSTERS,
		PHASE_TEXT_OPPOSITES,
		
		PHASE_COUNT,
		
		// TODO check if these are needed. Remove if not... if data is okay without -> remove
		// note: export and reconstruct old database before removing
		PHASE_JOIN_ORPHANED,
		PHASE_FIX_DATA,
	};
	
	hash_t hash = 0;
	Event<> WhenUserInput;
	Value params;
	SrcTextData* data = 0;
	One<NaturalTokenizer> tk;
	ContextType ctxtype;
	
	// Params
	TaskArgs args;
	
	// Configuration
	int tokentexts_per_action_task = 5;
	int words_per_action_task = 25;
	int vpp_per_action_task = 65;
	
	// Temp (per phase)
	int total = 0, actual = 0;
	int iter;
	TimeStop ts;
	Vector<WordPairType*> tmp_wp_ptrs;
	Vector<VirtualPhrasePart*> tmp_vpp_ptrs;
	Vector<VirtualPhraseStruct*> tmp_vps_ptrs;
	Vector<PhrasePart*> tmp_pp_ptrs;
	Vector<int> tmp, tmp_iters;
	VectorMap<int,int> vmap;
	VectorMap<String, VectorMap<String, int>> uniq_acts;
	VectorMap<int,int> tokens_to_languages;
	
	// 1
	void Tokenize();
	void RealizeContext();
	// 2
	void AnalyzePublicFigure();
	void AnalyzeElements();
	// 3.0
	void TokensToLanguages();
	void TokensToWords();
	// 3.1
	void CountWords();
	// 4
	// 5
	void ImportTokenTexts();
	void ClassifySentences();
	// 6
	void VirtualPhraseParts();
	// 7
	void VirtualPhraseStructs();
	// 8
	void PhrasePartAnalysis();
	void OnPhraseElements(String result);
	void OnPhraseColors(String result);
	void OnPhraseAttrs(String result);
	void OnPhraseActions(String result);
	void OnPhraseScores(String result);
	void OnPhraseTypeclasses(String result);
	void OnPhraseContrast(String result);
	void OnPhraseElement(String result);
	// 9
	void Prepare(TaskFn fn);
	void Colors();
	void Attrs();
	// 10
	struct AttrExtremesBatch : Moveable<AttrExtremesBatch> {
		String group;
	};
	struct Batch : Moveable<Batch> {
		AuthorDataset* artist;
		ScriptDataset* scripts;
		String txt;
		int lng_i;
		bool song_begins;
	};
	struct AttrPolarBatch : Moveable<AttrPolarBatch> {
		String group, attr0, attr1;
		Vector<String> attrs;
	};
	struct AttrJoinBatch : Moveable<AttrJoinBatch> {
		Vector<String> groups;
		Vector<AttrHeader> values;
	};
	VectorMap<String,Index<String>> uniq_attrs;
	Vector<AttrExtremesBatch> attr_extremes_batches;
	Vector<AttrPolarBatch> attr_polar_batches;
	Vector<AttrJoinBatch> attr_join_batches;
	Vector<Batch> batches;
	void MainGroups();
	void SimplifyAttrs();
	void JoinOrphaned();
	void FixData();
	void RealizeBatch_AttrExtremesBatch();
	
public:
	typedef ScriptTextProcess CLASSNAME;
	ScriptTextProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static ScriptTextProcess& Get(DatasetPtrs p, VfsPath path, Value params, SrcTextData& data, Event<> WhenStopped);
	
};

INITIALIZE_VALUECOMPONENT(ScriptText);




#endif
