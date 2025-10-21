#include "AgentDemo.h"

NAMESPACE_UPP



struct SimpleValueNode : VfsValueExt {
	DEFAULT_EXT(SimpleValueNode)
	void Visit(Vis& v) override {}
	String ToString() const override {
		return AsString(val.value);
	}
	double GetUtility() override {
		Value& o = val.value;
		ValueArray arr = o;
		ASSERT(arr.GetCount());
		Value ov = arr[1];
		double value = ov;
		return value;
	}
	
	bool TerminalTest() override {
		return val.sub.GetCount() == 0;
	}
};


// Function which sets Node<Value> value in GenerateTree function
void SetValue1(Val& i) {
	static int counter;
	if (!i.ext)
		i.CreateExt<SimpleValueNode>();
	Value& v = i.value;
	ValueArray arr;
	
	String str;
	str.Cat('A' + counter++);
	arr.Add(str);
	arr.Add(-2 + (int)Random(10));
	v = arr;
}

// Class which only only tells the utility value
struct SimpleGeneratorNode : VfsValueExt {
	int value = 0;
	
	DEFAULT_EXT(SimpleGeneratorNode)
	void Visit(Vis& v) override {}
	String ToString() const override {return IntStr(value);}
	double GetUtility() override {return value;}
	
	// Use TerminalTest to generate sub nodes
	bool GenerateSubValues(const Value& params, NodeRoute& prev) override {
		int depth = val.GetDepth();
		if (depth >= 3 || val.GetCount())
			return true;
		int sub_node_count = 1 + Random(2);
		for(int i = 0; i < sub_node_count; i++) {
			SimpleGeneratorNode& sub = val.Add<SimpleGeneratorNode>();
			sub.value = -2 + Random(10);
		}
		return true;
	}
	
	bool TerminalTest() override {
		return !val.GetCount();
	}
};




// Pretty print vector of pointers
String PtrVecStr(const Vector<Val*>& vec) {
	String out;
	for(int i = 0; i < vec.GetCount(); i++) {
		if (i) out << "\n";
		out << i << ": ";
		auto& s = *vec[i];
		if (s.ext)
			out << s.ext->ToString();
		else if (!s.value.IsVoid())
			out << s.value.ToString();
	}
	return out;
}

void PrintTotal(Vector<VfsValue*>& vec) {
	double total = 0;
	for(int i = 0; i < vec.GetCount(); i++) {
		RouteGeneratorNode* rgn = CastPtr<RouteGeneratorNode>(&*vec[i]->ext);
		total += rgn->length;
	}
	double average = total / vec.GetCount();
	LOG("total=" << total << " average=" << average);
}



void BasicTests() {
	using namespace UPP;
	VfsValue& app_root = MetaEnv().root;
	
	// Simple game algorithms
	if (true) {
		VfsValue& n = app_root.Add("simplegame");
		SeedRandom(0x12345);
		GenerateTree(n, 25, 2, 3, callback(SetValue1));
		LOG(n.GetTreeString());
		
		MiniMax mm;
		AlphaBeta ab;
		
		Vector<Val*> ans = mm.Search(n);
		LOG(PtrVecStr(ans));
		
		ans = ab.Search(n);
		LOG(PtrVecStr(ans));
		
		ASSERT(ans.GetCount() == 4);
		ASSERT(ValueArray(ans[0]->value)[1] == -2);
		ASSERT(ValueArray(ans[1]->value)[1] == -2);
		ASSERT(ValueArray(ans[2]->value)[1] ==  3);
		ASSERT(ValueArray(ans[3]->value)[1] == -1);
	}
	
	// Simple game algorithms, with runtime node generation.
	if (true) {
		VfsValue& n = app_root.Add("simplegame_rt");
		ASSERT(n.GetCount() == 0);
		
		n.CreateExt<SimpleGeneratorNode>();
		MiniMax mm;
		AlphaBeta ab;
		
		Vector<Val*> ans = mm.Search(n);
		LOG(n.GetTreeString());
		LOG(PtrVecStr(ans));
		
		ans = ab.Search(n);
		LOG(PtrVecStr(ans));
	}
	
	// Uninformed search strategies, with runtime node generation
	if (true) {
		VfsValue& n = app_root.Add("uninformed_rt");
		RouteGeneratorNode& rgn = n.CreateExt<RouteGeneratorNode>();
		rgn.estimate_to_goal = 20;
		rgn.length_to_node = 0;
		rgn.length = 0;
		
		BreadthFirst bf;
		UniformCost uc;
		DepthFirst df;
		DepthLimited dl;
		
		Vector<Val*> ans = bf.Search(n);
		LOG(n.GetTreeString());
		LOG(PtrVecStr(ans));
		
		ans = uc.Search(n);
		LOG(PtrVecStr(ans));
		
		ans = df.Search(n);
		LOG(PtrVecStr(ans));
		
		dl.SetLimit(3);
		ans = dl.Search(n);
		LOG(PtrVecStr(ans));
	}
	
	// Informed (heuristic) search strategies, with runtime node generation
	if (true) {
		VfsValue& n = app_root.Add("informed_rt");
		auto& rgn = n.CreateExt<RouteGeneratorNode>();
		rgn.estimate_to_goal = 100;
		rgn.length_to_node = 0;
		rgn.length = 0;
		
		BestFirst bf;
		AStar as;
		as.TrimWorst(0,0);
		
		Vector<VfsValue*> ans;
		
		ans = bf.Search(n);
		//LOG(n.AsString());
		LOG(PtrVecStr(ans));
		PrintTotal(ans);
		
		TimeStop ts;
		
		ts.Reset();
		ans = as.Search(n);
		LOG(PtrVecStr(ans));
		PrintTotal(ans);
		LOG(ts.ToString());
		
		
	}
	
	// Decision tree
	if (true) {
		if (1) {
			QueryTable<String> qt;
			
			Vector<String>& ot = qt.AddPredictor("Outlook");
			Vector<String>& temp = qt.AddPredictor("Temp");
			Vector<String>& hum = qt.AddPredictor("Humidity");
			Vector<String>& w = qt.AddPredictor("Windy");
			
			ot.Add("Rainy");				temp.Add("Hot");				hum.Add("High");				w.Add("False");		qt.AddTargetValue("No");
			ot.Add("Rainy");				temp.Add("Hot");				hum.Add("High");				w.Add("True");		qt.AddTargetValue("No");
			ot.Add("Overcast");				temp.Add("Hot");				hum.Add("High");				w.Add("False");		qt.AddTargetValue("Yes");
			ot.Add("Sunny");				temp.Add("Mild");				hum.Add("High");				w.Add("False");		qt.AddTargetValue("Yes");
			ot.Add("Sunny");				temp.Add("Cool");				hum.Add("Normal");				w.Add("False");		qt.AddTargetValue("Yes");
			ot.Add("Sunny");				temp.Add("Cool");				hum.Add("Normal");				w.Add("True");		qt.AddTargetValue("No");
			ot.Add("Overcast");				temp.Add("Cool");				hum.Add("Normal");				w.Add("True");		qt.AddTargetValue("Yes");
			ot.Add("Rainy");				temp.Add("Mild");				hum.Add("High");				w.Add("False");		qt.AddTargetValue("No");
			ot.Add("Rainy");				temp.Add("Cool");				hum.Add("Normal");				w.Add("False");		qt.AddTargetValue("Yes");
			ot.Add("Sunny");				temp.Add("Mild");				hum.Add("Normal");				w.Add("False");		qt.AddTargetValue("Yes");
			ot.Add("Rainy");				temp.Add("Mild");				hum.Add("Normal");				w.Add("True");		qt.AddTargetValue("Yes");
			ot.Add("Overcast");				temp.Add("Mild");				hum.Add("High");				w.Add("True");		qt.AddTargetValue("Yes");
			ot.Add("Overcast");				temp.Add("Hot");				hum.Add("Normal");				w.Add("False");		qt.AddTargetValue("Yes");
			ot.Add("Sunny");				temp.Add("Mild");				hum.Add("High");				w.Add("True");		qt.AddTargetValue("No");
			
			int i = qt.GetLargestInfoGainPredictor();
			DUMP(i);
			DUMPC(qt.GetInfoGains());
		}
		
		{
			QueryTable<String> qt;
			
			Vector<String>& x = qt.AddPredictor("X");
			Vector<String>& y = qt.AddPredictor("Y");
			
			x.Add("0");		y.Add("10");		qt.AddTargetValue("a");
			x.Add("1");		y.Add("10");		qt.AddTargetValue("a");
			x.Add("1");		y.Add("100");		qt.AddTargetValue("b");
			x.Add("0");		y.Add("100");		qt.AddTargetValue("b");
			x.Add("1");		y.Add("100");		qt.AddTargetValue("a");
			x.Add("1");		y.Add("100");		qt.AddTargetValue("a");
			x.Add("0");		y.Add("100");		qt.AddTargetValue("b");
			x.Add("0");		y.Add("100");		qt.AddTargetValue("b");
			
			int i = qt.GetLargestInfoGainPredictor();
			DUMP(i);
			DUMPC(qt.GetInfoGains());
		}
	}
	
	// Action planner
	if (true) {
		ActionPlannerExample();
	}
}

void ActionPlannerExample() {
	using namespace Upp;
	
	// Macros are poor man's meta-programming. Don't underestimate it! (I did it too earlier...)
	
	// I mean... actual meta-programming would be better (not that constexpr crap though)...
	// ...but meta-programming is better solution often, and this is the closest to it.
	
	#define ACT_LIST \
		ACT(CAT) \
		ACT(APPROACH) \
		ACT(COME_DOWN) \
		ACT(AIM) \
		ACT(ATTACK) \
		ACT(WAIT) \
		ACT(VERY_HIGH_JUMP_ATTACK) \
		ACT(FLEE)
	
	#define ATOM_LIST \
		ATOM(ARMED_WITH_CLAWS,	true) \
		ATOM(MOUSE_VISIBLE,		false) \
		ATOM(NEAR_MOUSE,		false) \
		ATOM(AT_HIGH_PLACE,		true) \
		ATOM(CLAWS_EXTENDED,	false) \
		ATOM(READY_TO_ATTACK,	false) \
		ATOM(MOUSE_ALIVE,		true) \
		ATOM(ALIVE,				true)
	
	enum {
		#define ACT(x) x,
		ACT_LIST
		#undef ACT
		ACT_COUNT
	};
	
	String act_names[ACT_COUNT+1] = {
		#define ACT(x) ToLower(#x),
		ACT_LIST
		#undef ACT
		""
	};
	
	enum {
		#define ATOM(x,y) x,
		ATOM_LIST
		#undef ATOM
		ATOM_COUNT
	};
	
	String atom_names[ATOM_COUNT+1] = {
		#define ATOM(x,y) ToLower(#x),
		ATOM_LIST
		#undef ATOM
		""
	};
	
	ActionPlanner planner;
	planner.SetSize(ACT_COUNT, ATOM_COUNT);
	
	ActionPlannerWrapper ap(planner);
	for(int i = 0; i < ACT_COUNT; i++)	ap.SetAction(i, act_names[i]);
	for(int i = 0; i < ATOM_COUNT; i++)	ap.SetAtom(i, atom_names[i]);
	
	planner.SetPreCondition (CAT, ARMED_WITH_CLAWS, true );
	planner.SetPostCondition(CAT, MOUSE_VISIBLE, true );

	planner.SetPreCondition (APPROACH, MOUSE_VISIBLE, true );
	planner.SetPostCondition(APPROACH, NEAR_MOUSE, true );
	
	planner.SetPreCondition (COME_DOWN, AT_HIGH_PLACE, true );
	planner.SetPostCondition(COME_DOWN, AT_HIGH_PLACE, false );
	
	planner.SetPreCondition (AIM, MOUSE_VISIBLE, true );
	planner.SetPreCondition (AIM, CLAWS_EXTENDED, true );
	planner.SetPostCondition(AIM, READY_TO_ATTACK, true );

	planner.SetPreCondition (ATTACK, READY_TO_ATTACK, true );
	planner.SetPreCondition (ATTACK, AT_HIGH_PLACE, false);
	planner.SetPreCondition (ATTACK, NEAR_MOUSE, true);
	planner.SetPostCondition(ATTACK, MOUSE_ALIVE, false );

	planner.SetPreCondition (WAIT, ARMED_WITH_CLAWS, true );
	planner.SetPostCondition(WAIT, CLAWS_EXTENDED, true );

	planner.SetPreCondition (VERY_HIGH_JUMP_ATTACK, AT_HIGH_PLACE, true );
	planner.SetPreCondition (VERY_HIGH_JUMP_ATTACK, NEAR_MOUSE, true );
	planner.SetPostCondition(VERY_HIGH_JUMP_ATTACK, ALIVE, false );
	planner.SetPostCondition(VERY_HIGH_JUMP_ATTACK, MOUSE_ALIVE, false );

	planner.SetPreCondition (FLEE, MOUSE_VISIBLE, true );
	planner.SetPostCondition(FLEE, NEAR_MOUSE, false );

	LOG(ap.GetDescription());
	
	BinaryWorldState src;
	#define ATOM(x,y) src.SetMasked(x, y);
	ATOM_LIST
	#undef ATOM
	
	planner.SetCost(VERY_HIGH_JUMP_ATTACK, 5 );	// make hurting by fall more expensive than attacking at the same level.

	BinaryWorldState goal;
	goal.SetMasked(MOUSE_ALIVE, false);
	goal.SetMasked(ALIVE, true); // add this to avoid hurting by fall actions in plan.
	goal.SetMasked(NEAR_MOUSE, false);
	
	VfsValue& example_root = MetaEnv().root.Add("example");
	APlanNode goal_node(example_root.Add<ActionNode>("goal"));
	goal_node->SetWorldState(goal);
	
	APlanNode root = example_root.Add<ActionNode>("root");
	root->SetActionPlanner(planner);
	root->SetGoal(goal_node);
	root->SetWorldState(src);
	AStar as;
	Vector<VfsValue*> plan = as.Search(root);
	
	if (plan.IsEmpty()) {
		LOG("error: did not found path");
		LOG("warning: using best path found");
		plan = as.GetBest();
	}
	
	LOG("Beginning:");
	double total_cost = 0;
	for(int i = 0; i < plan.GetCount(); i++) {
		auto& n = plan[i]->GetExt<ActionNode>();
		total_cost += n.GetCost();
		int id = n.GetActionId();
		if (id >= 0) {LOG(id << ": " << ap.GetActionName(id));}
		LOG("    " << i << ": " << ap.GetWorldstateDescription(n.GetWorldState()));
	}
	
	LOG("plan_cost=" << total_cost);
	
	// TODO:
	//  - tidy tmp_sub using
	//  - clear temp memory from ActionNode and ActionPlanner
	
}

END_UPP_NAMESPACE


CONSOLE_APP_MAIN {
	using namespace Upp;
	
	TypedStringHasher<SimpleValueNode>("SimpleValueNode");
	TypedStringHasher<SimpleGeneratorNode>("SimpleGeneratorNode");
	TypedStringHasher<RouteGeneratorNode>("RouteGeneratorNode");
	
	try {
		//BasicTests();
		IntegratedTests();
		//FileSystemExample();
		//ConstraintSolverTests();
	}
	catch (Exc e) {
		LOG(e);
	}
	catch (...) {
		LOG("error");
	}
}
