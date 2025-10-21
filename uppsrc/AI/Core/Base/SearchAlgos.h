#ifndef _AICore2_SearchAlgos_h_
#define _AICore2_SearchAlgos_h_




class Generator : public Pte<Generator> {
protected:
	Event<Val&> set_value;
	ValPtr fs;
public:
	Generator() {}
	virtual ~Generator() {}
	
	void SetValueFunction(Event<Val&> e);
	virtual bool SetParams(Value val) {return true;}
	virtual bool Run(Val& fs) {set_value(fs); this->fs = &fs; return true;}
	virtual bool GenerateSubValues(Val& val) {return true;}
	virtual VfsValue& GetInitial(Val& fs) {return fs;}
	virtual String GetTreeString() const {return fs ? fs->GetTreeString() : String();}
};

class TerminalTester : public Pte<TerminalTester> {
public:
	TerminalTester() {}
	virtual ~TerminalTester() {}
	virtual bool TerminalTest(Val& val) = 0;
};

class HeuristicEval : public Pte<HeuristicEval> {
public:
	HeuristicEval() {}
	virtual ~HeuristicEval() {}
	virtual double Utility(Val& val) = 0;
	virtual double Estimate(Val& n) = 0;
	virtual double Distance(Val& a, Val& b) = 0;
	virtual String GetResultString(const Vector<Val*>& result) const {return String();}
};

struct OmniSearcher :
	Generator,
	TerminalTester,
	HeuristicEval
{
	OmniSearcher() {}
	Event<String>		WhenError;
};

class Searcher {
	
protected:
	friend class SearcherExt;
	Ptr<Generator>			generator;
	Ptr<TerminalTester>		termtester;
	Ptr<HeuristicEval>		heuristic;
	Value					generator_params;
	Event<Val&>				generator_set_value;
	
public:
	Searcher();
	virtual ~Searcher() {}
	
	void SetGenerator(Generator* gen) {generator = gen;}
	void SetTerminalTester(TerminalTester* t) {termtester = t;}
	void SetHeuristic(HeuristicEval* h) {heuristic = h;}
	bool GenerateSubValues(Val& n, NodeRoute& prev);
	bool TerminalTest(Val& n);
	double Utility(Val& n);
	double Estimate(Val& n);
	double Distance(Val& a, Val& b);
	
	Vector<Val*> Search(Val& src);
	
	virtual bool SetParams(Value val) {return true;}
	virtual bool SearchBegin(Val& src) = 0;
	virtual bool SearchIteration() = 0;
	virtual Vector<Val*> SearchEnd() = 0;
	
	Event<String>		WhenError;
};


class MiniMax : public Searcher {
	NodeRoute route;
	Vector<Val*> out;
	Val* ptr = 0;
	Val* prev = 0;
public:
	MiniMax();
	double MaxValue(Val& n, int* decision_pos=0);
	double MinValue(Val& n, int* decision_pos=0);
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
};


class AlphaBeta : public Searcher {
	NodeRoute route;
	Vector<Val*> out;
	ValPtr ptr, root;
public:
	AlphaBeta();
	double MaxValue(Val& n, double alpha, double beta, int* decision_pos=0);
	double MinValue(Val& n, double alpha, double beta, int* decision_pos=0);
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
};


// UNINFORMED SEARCH STRATEGIES

class BreadthFirst : public Searcher {
	NodeRoute route;
	Vector<Val*> queue, next_queue;
	double v = 0;
	ValPtr ptr, root;
public:
	BreadthFirst();
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
};


class UniformCost : public Searcher {
	NodeRoute route;
	Vector<Val*> frontier;
	double v = DBL_MAX;
	ValPtr ptr, root;
public:
	UniformCost();
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
};

class DepthFirst : public Searcher {
	NodeRoute route;
	One<typename Val::IteratorDeep> it;
	ValPtr ptr, root;
	Val* prev = 0;
	double v = 0;
public:
	DepthFirst();
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
};

class DepthLimited : public Searcher {
	int limit = 10;
	NodeRoute route;
	One<typename Val::IteratorDeep> it;
	ValPtr ptr, root;
	Val* prev = 0;
	double v = DBL_MAX;
public:
	DepthLimited();
	void SetLimit(int lim);
	bool SetParams(Value val) override;
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
};




// INFORMED (HEURISTIC) SEARCH STRATEGIES


class BestFirst : public Searcher {
	NodeRoute route;
	Vector<Val*> out;
	Val* ptr = 0;
	Val* prev = 0;
public:
	BestFirst();
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
};


    
class AStar : public Searcher {
	
	struct NodePtr {
		ValPtr ptr;
		ValPtr src;
		NodePtr() : g_score(0), f_score(0), came_from(0) {g_score = DBL_MAX; f_score = DBL_MAX;}
		
		hash_t GetHashValue() const {return UPP::GetHashValue((size_t)&*ptr);}
		bool operator == (const NodePtr& np) const {return np.ptr == ptr;}
		// For each node, the cost of getting from the start node to that node.
		double g_score;
		// For each node, the total cost of getting from the start node to the goal
		// by passing by that node. That value is partly known, partly heuristic.
		double f_score;
		// For each node, which node it can most efficiently be reached from.
		// If a node can be reached from many nodes, came_from will eventually contain the
		// most efficient previous step.
		const NodePtr* came_from = 0;
	};
	
	int max_worst;
	bool do_search;
	int limit;
	int rm_limit = 1000;
	int smallest_id = -1;
	Vector<Val*> out;
	Vector<double> worst_f_score;
	Vector<int> worst_id;
	NodeRoute route;
	bool dump_intermediate_trees = false;
	
	// The set of nodes already evaluated.
	Array<NodePtr> nodes;
	Vector<NodePtr*> closed_set;
	
	// The set of currently discovered nodes still to be evaluated.
	// Initially, only the start node is known.
	Vector<NodePtr*> open_set;
	
	
	static int FindNode(const Vector<NodePtr*>& vec, const Val* ptr);
	
	bool Fail();
public:
	AStar();
	void operator=(const AStar& as);
	void SetLimit(int i);
	void Stop();
	void TrimWorst(int limit, int count);
	Vector<Val*> GetBestKnownPath();
	Vector<Val*> ReconstructPath(Val& current, Val& current_src, Vector<NodePtr*>& closed_set, Vector<NodePtr*>& open_set);
	Vector<Val*> ReconstructPath(Val& current, Val& current_src);
	Vector<Val*> GetBest();
	Vector<Val*> ContinueSearch(Val& src);
	bool SearchBegin(Val& src) override;
	bool SearchIteration() override;
	Vector<Val*> SearchEnd() override;
	bool SetParams(Value val) override;
};



class GeneratorRandom : public Generator {
	int total = 0, low = 0, high = 0, depth_limit = 0, count = 0;
	bool initial = 1, runtime = 0;
	Ptr<VfsValue> root;
public:
	GeneratorRandom();
	bool SetParams(Value val) override;
	bool Run(Val& fs) override;
	bool GenerateSubValues(Val& val) override;
};

struct NoSubTerminal : TerminalTester {
	bool TerminalTest(Val& v) override;
};

class SimpleHeuristic : public HeuristicEval {
	double goal = 0;
public:
	SimpleHeuristic() {}
	double Utility(Val& val) override;
	double Estimate(Val& n) override;
	double Distance(Val& a, Val& b) override;
};


#endif
