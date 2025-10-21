#include "Base.h"

NAMESPACE_UPP


CommitTreeExt::CommitTreeExt(VfsValue& val) : VfsValueExt(val) {
	
}
	
CommitDiffListExt::CommitDiffListExt(VfsValue& val) : VfsValueExt(val) {
	
}

SearcherExt::SearcherExt(VfsValue& val) : VfsValueExt(val) {
	
}

void SearcherExt::Update(double ts) {
	if (flag.IsRunning()) {
		if (!SearchIteration()) {
			SearchEnd();
			flag.SetStopped();
		}
	}
}

void SearcherExt::ClearGeneratorParams() {
	generator_params = Value();
	generator_set_value.Clear();
}

void SearcherExt::ClearFS() {
	auto& fs = val.GetAdd("fs");
	fs.sub.Clear();
	fs.type_hash = 0;
	fs.ext.Clear();
	fs.value = Value();
}

VfsValue& SearcherExt::GetFS() {
	return val.GetAdd("fs");
}

VfsValue& SearcherExt::GetInitial() {
	if (generator)
		return generator->GetInitial(GetFS());
	return GetFS();
}

CommitTreeExt& SearcherExt::GetCommitTree() {
	return val.GetAdd<CommitTreeExt>("commits");
}

CommitDiffListExt& SearcherExt::GetCommitDiffList() {
	return val.GetAdd<CommitDiffListExt>("diffs");
}

void SearcherExt::SetSearchStrategy(SearchStrategyType t) {
	searchstrategy = t;
}

void SearcherExt::SetHeuristics(HeuristicType t) {
	heuristic_type = t;
}

void SearcherExt::SetGenerator(GeneratorType t) {
	generator_type = t;
}

void SearcherExt::SetTerminalTest(TerminalTestType t) {
	termtest_type = t;
}

void SearcherExt::SetRandomSeed(dword seed) {
	random_seed = seed;
}

void SearcherExt::SetSearcherParams(Value params) {
	this->searcher_params = params;
}

void SearcherExt::SetGeneratorParams(Value params, Event<Val&> set_value) {
	this->generator_params = params;
	this->generator_set_value = set_value;
}

bool SearcherExt::CreateSearcher() {
	switch (searchstrategy) {
		case SEARCHSTRATEGY_NULL:			searcher.Clear(); break;
		case SEARCHSTRATEGY_MINIMAX:		searcher = new MiniMax; break;
		case SEARCHSTRATEGY_ALPHA_BETA:		searcher = new AlphaBeta; break;
		case SEARCHSTRATEGY_BREADTH_FIRST:	searcher = new BreadthFirst; break;
		case SEARCHSTRATEGY_UNIFORM_COST:	searcher = new UniformCost; break;
		case SEARCHSTRATEGY_DEPTH_FIRST:	searcher = new DepthFirst; break;
		case SEARCHSTRATEGY_DEPTH_LIMITED:	searcher = new DepthLimited; break;
		case SEARCHSTRATEGY_BEST_FIRST:		searcher = new BestFirst; break;
		case SEARCHSTRATEGY_ASTAR:			searcher = new AStar; break;
		case SEARCHSTRATEGY_CUSTOM:			ASSERT(searcher); break;
		default: TODO searcher.Clear(); break;
	}
	
	if (searcher.IsEmpty())
		return false;
	
	searcher->WhenError = [this](String s){this->WhenError(s);};
	
	if (searcher) {
		if (!searcher->SetParams(searcher_params))
			return false;
	}
	return true;
}

void SearcherExt::CreateOmni() {
	own_omni.Clear();
	
	if (termtest_type == TERMTEST_ACTION_PLANNER &&
		generator_type == GENERATOR_ACTION_PLANNER &&
		heuristic_type == HEURISTIC_ACTION_PLANNER) {
		own_omni = new OmniActionPlanner;
		own_omni->WhenError = [this](String s) {this->WhenError(s);};
	}
		
}

void SearcherExt::CreateGenerator() {
	generator = 0;
	switch (generator_type) {
	case GENERATOR_NULL:
		own_generator.Clear();
		break;
	case GENERATOR_RANDOM:
		own_generator = new GeneratorRandom();
		generator = &*own_generator;
		break;
	case GENERATOR_CUSTOM:
		ASSERT(own_generator);
		generator = &*own_generator;
		break;
	case GENERATOR_ACTION_PLANNER:
		ASSERT(own_omni);
		generator = &*own_omni;
		break;
		
		default: TODO break;
	}
	if (searcher)
		searcher->SetGenerator(generator ? &*generator : 0);
}

void SearcherExt::CreateTerminalTester() {
	termtester = 0;
	switch (termtest_type) {
		case TERMTEST_NULL:				own_termtester.Clear(); break;
		case TERMTEST_NO_SUB:			own_termtester = new NoSubTerminal(); termtester = &*own_termtester; break;
		case TERMTEST_CUSTOM:			ASSERT(own_termtester); termtester = &*own_termtester; break;
		case TERMTEST_ACTION_PLANNER:	ASSERT(own_omni); termtester = &*own_omni; break;
		default: TODO break;
	}
	if (searcher)
		searcher->SetTerminalTester(termtester ? &*termtester : 0);
}

void SearcherExt::CreateHeuristic() {
	heuristic = 0;
	switch (heuristic_type) {
		case HEURISTIC_NULL:			own_heuristic.Clear(); break;
		case HEURISTIC_SIMPLE:			own_heuristic = new SimpleHeuristic; heuristic = &*own_heuristic; break;
		case HEURISTIC_HAMMING_DISTANCE_OF_PREDICATES: TODO; break;
		case HEURISTIC_CUSTOM:			ASSERT(own_heuristic); heuristic = &*own_heuristic; break;
		case HEURISTIC_ACTION_PLANNER:	ASSERT(own_omni); heuristic = &*own_omni; break;
		default: TODO break;
	}
	if (searcher)
		searcher->SetHeuristic(heuristic ? &*heuristic : 0);
}

bool SearcherExt::RunGenerator() {
	ASSERT(generator);
	
	if (!generator->SetParams(generator_params))
		return false;
	
	generator->SetValueFunction(generator_set_value);
	
	bool succ = generator->Run(GetFS());
	if (succ)
		WhenGenerated();
	
	return succ;
}

String SearcherExt::GetResultString() const {
	String s;
	if (heuristic)
		 s = heuristic->GetResultString(result);
	if (s.IsEmpty())
		s = PtrVecStr(result);
	return s;
}

String SearcherExt::GetTreeString() const {
	if (generator)
		return generator->GetTreeString();
	Val& fs = const_cast<SearcherExt*>(this)->GetFS();
	return fs.GetTreeString();
}

bool SearcherExt::SearchBegin() {
	flag.Stop();
	result.Clear();
	
	if (random_seed) {
		SeedRandom(random_seed);
		// Get few random values to skip first non changing constants
		for(int i = 0; i < 10; i++) Random();
	}
	
	// Create sub paths
	VfsValue& fs			= GetFS();
	CommitTreeExt& tree		= GetCommitTree();
	CommitDiffListExt& list	= GetCommitDiffList();
	
	if (!CreateSearcher())
		return false;
	
	CreateOmni();
	CreateTerminalTester();
	CreateHeuristic();
	CreateGenerator();
	
	if (generator && !RunGenerator())
		return false;
	
	if (!generator && searcher) {
		searcher->generator_params     = generator_params;
		searcher->generator_set_value  = generator_set_value;
	}
	
	if (!searcher->SearchBegin(GetInitial()))
		return false;
	
	flag.Start();
	
	return true;
}

bool SearcherExt::SearchIteration() {
	if (!flag.IsRunning())
		return false;
	
	return searcher->SearchIteration();
}

bool SearcherExt::SearchEnd() {
	result = searcher->SearchEnd();
	return !result.IsEmpty();
}

bool SearcherExt::RunSearch()
{
	if (!SearchBegin())
		return false;
	
	while (1) {
		if (!SearchIteration())
			break;
	}
	
	bool ret = SearchEnd();
	
	flag.SetStopped();
	return ret;
}

const Vector<Val*>& SearcherExt::GetResult() const {
	return result;
}

String SearcherExt::PtrVecStr(const Vector<Val*>& vec) {
	String out;
	for(int i = 0; i < vec.GetCount(); i++) {
		if (i) out << "\n";
		out << i << ": ";
		if (vec[i]->ext)
			out << vec[i]->ext->ToString();
	}
	return out;
}


	


ActionEventValue& ActionEventValue::Pre(String atom, bool value) {
	ValueArray arr;
	arr.Add(atom);
	arr.Add(value);
	pre.Add(arr);
	return *this;
}

ActionEventValue& ActionEventValue::Post(String atom, bool value) {
	ValueArray arr;
	arr.Add(atom);
	arr.Add(value);
	post.Add(arr);
	return *this;
}

ActionEventValue& ActionEventValue::Cost(double cost) {
	this->cost = cost;
	return *this;
}

Value ActionEventValue::ToValue() const {
	ValueMap map;
	map.Add("pre", pre);
	map.Add("post", post);
	if (cost != 0.0)
		map.Add("cost", cost);
	return map;
}

ActionEventValue::operator Value() const {return ToValue();}

END_UPP_NAMESPACE
