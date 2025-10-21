#include <AICore/AICore.h>

void ForwardOnly() {
	using namespace UPP;
	VfsValue& app_root = MetaEnv().root;
	SearcherExt& searcher = app_root.GetAdd<SearcherExt>("searcher");
	searcher.SetVerbose();
	
	ValueMap params;
	params.Set("total", 25);
	params.Set("low", 2);
	params.Set("high", 3);
	
	for(int i = 7; i < 8; i++) {
		ValueMap atoms, goal, actions, initial;
		if (i == 0) {
			atoms	.Add("A", false);
			goal	.Add("A", true);
			actions	.Add("write A", ActionEventValue().Pre("A",false).Post("A",true));
		}
		else if (i == 1) {
			atoms	.Add("A", false);
			atoms	.Add("B", false);
			goal	.Add("A", true);
			actions	.Add("write A", ActionEventValue().Pre("A",false).Pre("B",true).Post("A",true));
			actions	.Add("write B", ActionEventValue().Pre("B",false).Post("B",true));
		}
		else if (i == 2) {
			atoms	.Add("A", false);
			atoms	.Add("B", false);
			atoms	.Add("C", false);
			goal	.Add("A", true);
			// avoid 'write A (via B)' by setting cost multiplier to 5
			actions	.Add("write A via B", ActionEventValue().Cost(5).Pre("A",false).Pre("B",true).Post("A",true));
			actions	.Add("write A via C", ActionEventValue().Pre("A",false).Pre("C",true).Post("A",true));
			actions	.Add("write B", ActionEventValue().Pre("B",false).Post("B",true));
			// ... even though writing C is slightly more costly
			actions	.Add("write C", ActionEventValue().Cost(2).Pre("C",false).Post("C",true));
		}
		else if (i == 3) {
			// Same as previous, but B=B(0) and C=(B1)
			// This test is for testing parameter parsing, comparison, etc.
			params("use_params") = true;
			atoms	.Add("A", false);
			atoms	.Add("B(id)", false);
			initial	.Add("B(0)", false);
			initial	.Add("B(1)", false);
			goal	.Add("A", true);
			actions	.Add("write A via B0", ActionEventValue().Cost(5).Pre("A",false).Pre("B(0)",true).Post("A",true));
			actions	.Add("write A via B1", ActionEventValue().Pre("A",false).Pre("B(1)",true).Post("A",true));
			actions	.Add("write B0", ActionEventValue().Pre("B(0)",false).Post("B(0)",true));
			actions	.Add("write B1", ActionEventValue().Cost(2).Pre("B(1)",false).Post("B(1)",true));
		}
		else if (i == 4) {
			// Very basic resolver test
			params("use_params") = true;
			params("use_resolver") = true;
			initial	.Add("A(\"abc\")", true);
			atoms	.Add("A(id)", false);
			atoms	.Add("B", false);
			goal	.Add("B", true);
			actions	.Add("write B(id)", ActionEventValue().Pre("A(id)",true).Post("B",true));
		}
		else if (i == 5) {
			// Inducts from initial
			// initial.Add("A(\"abc\")");
			params("use_params") = true;
			params("use_resolver") = true;
			initial	.Add("A(\"abc\")", true);
			atoms	.Add("A(id)", false);
			atoms	.Add("B(id)", false);
			atoms	.Add("C", false);
			goal	.Add("C", true);
			actions	.Add("write B(id)", ActionEventValue().Pre("A(id)",true).Pre("B(id)",false).Post("B(id)",true).Post("C",true));
		}
		else if (i == 6) {
			// Inducts from goal (single-step, no reverse)
			params("use_params") = true;
			params("use_resolver") = true;
			atoms	.Add("A", true);
			atoms	.Add("B(id)", false);
			goal	.Add("B(\"abc\")", true);
			actions	.Add("write B(id)", ActionEventValue()
				.Pre	("A",		true)
				.Pre	("B(id)",	false)
				.Post	("B(id)",	true));
		}
		else if (i == 7) {
			// multi-param combination tests
			params("use_params") = true;
			params("use_resolver") = true;
			atoms	.Add("A(a,b)", false);
			atoms	.Add("B(c,b)", false);
			atoms	.Add("C(b)", false);
			initial	.Add("A(0,0)", true);
			initial	.Add("B(0,0)", true);
			initial	.Add("C(0)", true);
			goal	.Add("B(1,1)", true);
			actions	.Add("F1(b)", ActionEventValue()
				.Pre	("A(0,b)",	true)
				.Pre	("C(b)",	true)
				.Post	("B(1,b)",	true));
			actions	.Add("F2(b)", ActionEventValue()
				.Pre	("B(1,b)",	true)
				.Pre	("C(b)",	true)
				.Post	("B(1,1)",	true));
		}
		else if (i == 8) {
			// 2-planner problem
			// where the other is reversed and a common goal is searched for
			TODO
		}
		params("atoms") = atoms;
		params("actions") = actions;
		params("goal") = goal;
		params("initial") = initial;
		params("dump_intermediate_trees") = true;
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
			LOG(searcher.GetTreeString());
		}
		ASSERT(ret);
		//LOG(PtrVecStr(searcher.GetResult()));
		LOG(searcher.GetResultString());
	}

}

void ReverseOnly() {
	using namespace UPP;
	VfsValue& app_root = MetaEnv().root;
	SearcherExt& searcher = app_root.GetAdd<SearcherExt>("searcher");
	searcher.SetVerbose();
	
	ValueMap params;
	params.Set("total", 25);
	params.Set("low", 2);
	params.Set("high", 3);
	
	for(int i = 0; i < 1; i++) {
		ValueMap atoms, goal, actions, initial;
		if (i == 0) {
			atoms	.Add("A", false);
			goal	.Add("A", true);
			actions	.Add("write A", ActionEventValue().Pre("A",false).Post("A",true));
		}
		
		params("atoms") = atoms;
		params("actions") = actions;
		params("goal") = goal;
		params("initial") = initial;
		params("dump_intermediate_trees") = true;
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
			LOG(searcher.GetTreeString());
		}
		ASSERT(ret);
		//LOG(PtrVecStr(searcher.GetResult()));
		LOG(searcher.GetResultString());
	}
}

void BiDirectional() {
	using namespace UPP;
	VfsValue& app_root = MetaEnv().root;
	SearcherExt& searcher = app_root.GetAdd<SearcherExt>("searcher");
	searcher.SetVerbose();
	
	ValueMap params;
	params.Set("total", 25);
	params.Set("low", 2);
	params.Set("high", 3);
	
	for(int i = 0; i < 1; i++) {
		ValueMap atoms, goal, actions, initial;
		if (i == 0) {
			// Bi-directional: forward init and reverse goal
			atoms	.Add("A(n)", false);
			atoms	.Add("AA(n)", false);
			atoms	.Add("C", false);
			atoms	.Add("BB(n)", false);
			atoms	.Add("B(n)", false);
			initial	.Add("A(1)", true);
			goal	.Add("B(2)", true);
			actions	.Add("ForwardA(n)", ActionEventValue().Pre("A(n)",true).Post("AA(n)",true));
			actions	.Add("ReverseB(n)", ActionEventValue().Pre("BB(n)",true).Post("B(n)",true));
			actions	.Add("ReverseBB(n)", ActionEventValue().Pre("BB(n)",true).Post("BBB",true));
			actions	.Add("Exchange(n)", ActionEventValue().Pre("AA(n)",true).Post("BB(n)",true));
		}
		else if (i == 1 || i == 2) {
			// Bi-directional mid-point
			atoms	.Add("A", true);
			atoms	.Add("AA(n)", false);
			if (i == 1)
				atoms	.Add("TEMP(n)", false);
			else
				atoms	.Add("TEMP(n:script=\"init(param) {param.val = 1; return true;}\")", false);
			atoms	.Add("BB(n)", false);
			atoms	.Add("B", false);
			initial	.Add("TEMP(1)", true);
			goal	.Add("B", true);
			actions	.Add("ReverseA(n)", ActionEventValue().Pre("A",true).Post("AA(n)",true));
			actions	.Add("ReverseTemp(n)", ActionEventValue().Pre("AA(n)",true).Post("TEMP(n)",true));
			actions	.Add("ForwardTemp(n)", ActionEventValue().Pre("TEMP(n)",true).Post("BB(n)",true));
			actions	.Add("ForwardB(n)", ActionEventValue().Pre("BB(n)",true).Post("B",true));
		}
		else if (i == 3) {
			
		}
		
		params("atoms") = atoms;
		params("actions") = actions;
		params("goal") = goal;
		params("initial") = initial;
		params("dump_intermediate_trees") = true;
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
			LOG(searcher.GetTreeString());
		}
		ASSERT(ret);
		//LOG(PtrVecStr(searcher.GetResult()));
		LOG(searcher.GetResultString());
	}
}

CONSOLE_APP_MAIN {
	//ForwardOnly();
	ReverseOnly();
	//BiDirectional();
}
