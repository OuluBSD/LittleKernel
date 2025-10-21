#include "AgentDemo.h"

NAMESPACE_UPP


void SetValue2(Val& i) {
	i.value = -2 + (int)Random(100);
}

void FirstAlgo(SearcherExt& searcher) {
	searcher.SetRandomSeed(0x12345);
	for(int i = 0; i < 2; i++) {
		searcher.SetSearchStrategy(
			i == 0 ?
				SEARCHSTRATEGY_MINIMAX :
				SEARCHSTRATEGY_ALPHA_BETA);
		searcher.SetGenerator(GENERATOR_RANDOM);
		searcher.ClearFS();
		if (!i)	searcher.WhenGenerated = [&]{LOG(searcher.GetFS().GetTreeString());};
		else	searcher.WhenGenerated.Clear();
		bool ret = searcher.RunSearch();
		ASSERT(ret);
		const auto& ans = searcher.GetResult();
		LOG(PtrVecStr(ans));
	}
}

void IntegratedTests() {
	using namespace UPP;
	VfsValue& app_root = MetaEnv().root;
	SearcherExt& searcher = app_root.GetAdd<SearcherExt>("searcher");
	searcher.SetVerbose();
	
	ValueMap params;
	params.Set("total", 25);
	params.Set("low", 2);
	params.Set("high", 3);
	
	bool all = 1;
	
	// Simple game algorithms
	if (all) {
		searcher.SetGeneratorParams(params, callback(SetValue1));
		FirstAlgo(searcher);
		
		const auto& ans = searcher.GetResult();
		ASSERT(ans.GetCount() == 4);
		ASSERT(ValueArray(ans[0]->value)[1] == -2);
		ASSERT(ValueArray(ans[1]->value)[1] ==  5);
		ASSERT(ValueArray(ans[2]->value)[1] == -2);
		ASSERT(ValueArray(ans[3]->value)[1] ==  1);
	}
	
	// Simple game algorithms, with runtime node generation.
	if (all) {
		params.Set("initial",0);
		params.Set("runtime",1);
		params.Set("depth_limit",3);
		searcher.SetGeneratorParams(params, callback(SetValue2));
		searcher.SetTerminalTest(TERMTEST_NO_SUB);
		searcher.SetHeuristics(HEURISTIC_SIMPLE);
		FirstAlgo(searcher);
		LOG(searcher.GetFS().GetTreeString());
	}
	
	// Uninformed search strategies, with runtime node generation
	if (all) {
		params.Set("initial",0);
		params.Set("runtime",1);
		params.Set("depth_limit",3);
		params.Set("estimate_to_goal", 20);
		searcher.SetRandomSeed(0x12345);
		searcher.SetSearcherParams(params);
		searcher.SetGeneratorParams(params, Null);
		for(int i = 0; i < 5; i++) {
			SearchStrategyType t;
			switch (i) {
				case 0: t = SEARCHSTRATEGY_BREADTH_FIRST; break;
				case 1: t = SEARCHSTRATEGY_UNIFORM_COST; break;
				case 2: t = SEARCHSTRATEGY_DEPTH_FIRST; break;
				case 3: t = SEARCHSTRATEGY_DEPTH_LIMITED; break;
				case 4: t = SEARCHSTRATEGY_ASTAR; break;
			}
			searcher.SetSearchStrategy(t);
			searcher.SetTerminalTest(TERMTEST_NULL);
			searcher.SetGenerator(GENERATOR_NULL);
			searcher.SetHeuristics(HEURISTIC_NULL);
			searcher.ClearFS();
			searcher.GetFS().CreateExt<RouteGeneratorNode>();
			searcher.WhenGenerated.Clear();
			bool ret = searcher.RunSearch();
			ASSERT(ret);
			const auto& ans = searcher.GetResult();
			LOG(PtrVecStr(ans));
		}
	}
	
	
	// Action planner
	if (all) {
		ValueMap atoms;
		atoms.Add("armed with claws", true);
		atoms.Add("mouse visible",    false);
		atoms.Add("near mouse",       false);
		atoms.Add("at high place",    true);
		atoms.Add("claws extended",   false);
		atoms.Add("ready to attack",  false);
		atoms.Add("mouse alive",      true);
		atoms.Add("alive",            true);
		
		ValueMap goal;
		goal.Add("mouse alive",		  false);
		goal.Add("alive",			  true); // add this to avoid hurting by 'fall' action in the plan.
		goal.Add("near mouse",		  false);
		
		ValueMap actions;
		actions.Add("cat", ActionEventValue()
			.Pre("armed with claws", true)
			.Post("mouse visible", true));
		actions.Add("approach", ActionEventValue()
			.Pre("mouse visible", true)
			.Post("near mouse", true));
		actions.Add("come down", ActionEventValue()
			.Pre("at high place", true)
			.Post("at high place", false));
		actions.Add("aim", ActionEventValue()
			.Pre("mouse visible", true)
			.Pre("claws extended", true)
			.Post("ready to attack", true));
		actions.Add("attack", ActionEventValue()
			.Pre("ready to attack", true)
			.Pre("at high place", false)
			.Pre("near mouse", true)
			.Post("mouse alive", false));
		actions.Add("prepare claws", ActionEventValue()
			.Pre("armed with claws", true)
			.Post("claws extended", true));
		actions.Add("very high jump attack", ActionEventValue()
			.Pre("at high place", true)
			.Pre("near mouse", true)
			.Post("alive", false)
			.Post("mouse alive", false)
			.Cost(5));
		actions.Add("flee", ActionEventValue()
			.Pre("mouse visible", true)
			.Post("near mouse", false));
		
		params("atoms") = atoms;
		params("actions") = actions;
		params("goal") = goal;
		params("dump_intermediate_trees") = false;
		searcher.SetSearcherParams(params);
		searcher.SetGeneratorParams(params, Null);
		searcher.SetSearchStrategy(SEARCHSTRATEGY_ASTAR);
		searcher.SetTerminalTest(TERMTEST_ACTION_PLANNER);
		searcher.SetGenerator(GENERATOR_ACTION_PLANNER);
		searcher.SetHeuristics(HEURISTIC_ACTION_PLANNER);
		searcher.ClearFS();
		searcher.WhenGenerated.Clear();
		searcher.WhenError = [](String s) {LOG("ActionPlanner error: " << s);};
		bool ret = searcher.RunSearch();
		if (!ret) {
			LOG(searcher.GetFS().GetTreeString());
		}
		ASSERT(ret);
		LOG(searcher.GetResultString());
	}
	
	// Single-agent decider: (Classic) Decision Tree
	if (0) {
		
	}
	
	// Multi-agent co-operative decider
	// - has fixed predictor hash as in decider tree
	if (0) {
		
	}
	
	// Multi-agent zero-sum game: (Classic) CFR: Counter-factual regret minimization
	// - will have dynamically changing predictor hash
	if (0) {
		// Kuhn Poker
	}
	
	if (0) {
		// Texas Hold 'em
		// - generate data (super high error rate)
		
	}
}


END_UPP_NAMESPACE
