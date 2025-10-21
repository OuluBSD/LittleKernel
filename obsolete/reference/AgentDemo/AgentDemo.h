#ifndef _AgentDemo_AgentDemo_h_
#define _AgentDemo_AgentDemo_h_

#include <AICore/AICore.h>

NAMESPACE_UPP



// Class which tells length of route from the root to the node
struct RouteGeneratorNode : VfsValueExt {
	double length = 0;
	double length_to_node = 0;
	double estimate_to_goal = 0;
	static const int goal = 0;
	
	DEFAULT_EXT(RouteGeneratorNode)
	void Visit(Vis& v) override {}
	String ToString() const override {return DblStr(length) + ", " + DblStr(length_to_node) + ", " + DblStr(estimate_to_goal);}
	double GetUtility() override {return length_to_node;}
	double GetEstimate() override {return estimate_to_goal;}
	double GetDistance(VfsValue& n) override {
		RouteGeneratorNode* rgn = CastPtr<RouteGeneratorNode>(&*n.ext);
		ASSERT(rgn);
		return rgn->length; // no links, so this is always the parent
	}
	// Use TerminalTest to generate sub nodes
	bool GenerateSubValues(const Value& params, NodeRoute& prev) override {
		if (!length && !length_to_node && !estimate_to_goal) {
			ValueMap map = params;
			estimate_to_goal = map.Get("estimate_to_goal", 30);
		}
		if (val.GetCount())
			return true;
		int sub_node_count = 2 + Random(1);
		for(int i = 0; i < sub_node_count; i++) {
			RouteGeneratorNode& sub = val.Add<RouteGeneratorNode>();
			double length = 5 + Random(10);
			// Accumulate total route length
			if (sub.val.owner) {
				sub.length				 = length;
				sub.length_to_node		 = length_to_node + length;
				sub.estimate_to_goal	 = estimate_to_goal - length;
				if (sub.estimate_to_goal < goal) sub.estimate_to_goal = goal;
			}
		}
		return true;
		}
	bool TerminalTest() override {
		if (estimate_to_goal <= goal)
			return true;
		return !val.sub.GetCount();
	}
};

void SetValue1(Val& i);
void SetValue2(Val& i);
String PtrVecStr(const Vector<Val*>& vec);
void FileSystemExample();
void ConstraintSolverTests();
void ActionPlannerExample();
void BasicTests();
void IntegratedTests();

END_UPP_NAMESPACE

#endif
