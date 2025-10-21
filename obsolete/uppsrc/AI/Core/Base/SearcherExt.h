#ifndef _AICore2_SearcherExt_h_
#define _AICore2_SearcherExt_h_


class CommitTreeExt : public VfsValueExt {
	
public:
	CLASSTYPE(CommitTreeExt)
	CommitTreeExt(VfsValue& val);
	void Visit(Vis& v) override {}
	
	
};



class CommitDiffListExt : public VfsValueExt {
	
public:
	CLASSTYPE(CommitDiffListExt)
	CommitDiffListExt(VfsValue& val);
	void Visit(Vis& v) override {}
	
	
};



class SearcherExt : public VfsValueExt {
	SearchStrategyType	searchstrategy = SEARCHSTRATEGY_NULL;
	HeuristicType		heuristic_type = HEURISTIC_NULL;
	GeneratorType		generator_type = GENERATOR_NULL;
	TerminalTestType	termtest_type = TERMTEST_NULL;
	One<Searcher>		searcher;
	Ptr<Generator>		generator;
	Ptr<TerminalTester> termtester;
	Ptr<HeuristicEval>	heuristic;
	One<Generator>		own_generator;
	One<TerminalTester> own_termtester;
	One<HeuristicEval>	own_heuristic;
	One<OmniSearcher>	own_omni;
	
	RunningFlagSingle	flag;
	bool				verbose = false;
	dword				random_seed = 0;
	
	Value				searcher_params;
	Value				generator_params;
	Event<Val&>			generator_set_value;
	Vector<Val*>		result;
	
	void				CreateOmni();
	bool				CreateSearcher();
	void				CreateGenerator();
	void				CreateTerminalTester();
	void				CreateHeuristic();
	static String		PtrVecStr(const Vector<Val*>& vec);
public:
	CLASSTYPE(SearcherExt)
	SearcherExt(VfsValue& val);
	
	void Visit(Vis& v) override {}
	void Update(double ts) override;
	
	void				ClearFS();
	VfsValue&			GetFS();
	VfsValue&			GetInitial();
	CommitTreeExt&		GetCommitTree();
	CommitDiffListExt&	GetCommitDiffList();
	void				SetSearchStrategy(SearchStrategyType t);
	void				SetHeuristics(HeuristicType t);
	void				SetGenerator(GeneratorType t);
	void				SetTerminalTest(TerminalTestType t);
	void				SetSearcherParams(Value params);
	void				SetGeneratorParams(Value params, Event<Val&> set_value);
	void				SetVerbose(bool b=true) {verbose = b;}
	void				SetRandomSeed(dword seed);
	void				ClearGeneratorParams();
	bool				RunSearch();
	bool				SearchBegin();
	bool				SearchIteration();
	bool				SearchEnd();
	const Vector<Val*>&	GetResult() const;
	bool				RunGenerator();
	String				GetTreeString() const;
	String				GetTreeString(VfsValue& v, int indent) const;
	String				GetResultString() const;
	
	template <class T>
	void SetGenerator() {
		generator_type = GENERATOR_CUSTOM;
		generator = new T;
	}
	
	Event<>				WhenGenerated;
	Event<String>		WhenError;
};



struct ActionEventValue {
	ValueArray pre, post;
	double cost = 0;
	ActionEventValue& Pre(String atom, bool value);
	ActionEventValue& Post(String atom, bool value);
	ActionEventValue& Cost(double cost);
	Value ToValue() const;
	operator Value() const;
};

#endif
