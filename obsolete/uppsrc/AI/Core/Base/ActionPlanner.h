#ifndef _AICore2_ActionPlanner_h_
#define _AICore2_ActionPlanner_h_


class ActionPlanner;
class ActionNode;


class PlannerEvent : public Moveable<PlannerEvent> {
	
protected:
	friend class ActionPlanner;
	friend class ActionNode;
	friend class ActionPlannerWrapper;
	friend class OmniActionPlanner;
	friend class ActionParamResolver;
	
	WorldStateKey key;
	BinaryWorldState precond, postcond;
	double cost;
	
public:

	PlannerEvent();
	String GetName() const;
};


class ActionPlannerWrapper;

class ActionPlanner {
	
protected:
	friend class ActionNode;
	friend class ActionPlannerWrapper;
	
	int atom_count = 0;
	Vector<PlannerEvent> actions;
	ActionPlannerWrapper* wrapper = 0;
	
	Array<BinaryWorldState> search_cache;
	
public:
	ActionPlanner();
	
	
	void Clear();
	
	int GetEventCount() const {return actions.GetCount();}
	int GetAtomCount() const {return atom_count;}
	
	void AddSize(int action_count, int atom_count);
	void SetSize(int action_count, int atom_count);
	bool SetPreCondition(int action_id, int atom_id, bool value);
	bool SetPostCondition(int action_id, int atom_id, bool value);
	bool SetCost(int action_id, int cost );
	
	
	void DoAction( int action_id, const BinaryWorldState& src, BinaryWorldState& dest);
	void GetPossibleStateTransition(const BinaryWorldState& src, Array<BinaryWorldState*>& dest, Vector<int>& act_ids, Vector<double>& action_costs);
	
};

class ActionPlannerWrapper {
	ActionPlanner& ap;
	Vector<String> acts;
	BinaryWorldStateSession ws_session;
	
protected:
	friend class ActionPlanner;
	
	void OnResize();
	
public:
	ActionPlannerWrapper(ActionPlanner& planner);
	
	int GetAtomIndex(String atom_name);
	int GetEventIndex(String event_name);
	String GetAtomName(int i);
	String GetActionName(int i) {return acts[i];}
	String GetWorldstateDescription( const BinaryWorldState& ws );
	String GetDescription();
	
	void SetAction(int act_i, String s) {acts[act_i] = s;}
	void SetAtom(int atom_i, String s);
	bool SetPreCondition(String event_name, String atom_name, bool value);
	bool SetPostCondition(String event_name, String atom_name, bool value);
	bool SetCost(String event_name, int cost );
	
};

class ActionNode : public VfsValueExt {
	BinaryWorldState* ws;
	double cost;
	int act_id;
	hash_t hash = 0;
	
	ActionPlanner* ap = 0;
	ActionNode* goal = 0;
	ArrayMap<hash_t, ActionNode*> tmp_sub;
	
public:
	CLASSTYPE(ActionNode)
	void Visit(Vis& v) override {}
	
	
	ActionNode(VfsValue& n);
	
	ActionPlanner& GetActionPlanner() {return *ap;}
	BinaryWorldState& GetWorldState() {return *ws;}
	
	void SetActionPlanner(ActionPlanner& ap_) {ap = &ap_;}
	void SetGoal(ActionNode& ws) {goal = &ws;}
	
	void SetWorldState(BinaryWorldState& ws) {this->ws = &ws; hash = ws.GetHashValue();}
	inline void SetCost(double d) {cost = d;}
	inline void SetActionId(int i) {act_id = i;}
	
	double GetDistance(VfsValue& to) override;
	double GetEstimate() override;
	bool GenerateSubValues(const Value& params, NodeRoute& route) override;
	bool TerminalTest() override;
	inline double GetCost() const {return cost;}
	inline int GetActionId() const {return act_id;}
};

typedef Node<ActionNode> APlanNode;

class OmniActionPlanner :
	public OmniSearcher
{
protected:
	Vector<PlannerEvent> actions;
	Index<String> atoms;
	BinaryWorldState ws_initial, ws_goal;
	BinaryWorldStateSession ws_session;
	mutable Ptr<BinaryWorldStateMask> ws_mask;
	ValPtr initial, goal;
	ValueMap params;
	double cost_multiplier = 1.5;
	bool use_params = false;
	bool use_resolver = false;
	bool run_initial = false;
	
	using Key = WorldStateKey;
	
	// Runtime temp vars
	BinaryWorldState tmp0, tmp1;
	Vector<BinaryWorldState*> possibilities;
	VectorMap<hash_t, ValPtr> tmp_sub;
	Array<BinaryWorldState> search_cache;
	
	bool GetPossibleStateTransition(const BinaryWorldState& src, Vector<BinaryWorldState*>& dest, Vector<int>& act_ids, Vector<double>& action_costs);
	bool Set(VfsValue& v, const BinaryWorldState& ws);
	bool Get(const VfsValue& v, BinaryWorldState& ws) const;
	void SetAction(VfsValue& v, int action);
	bool GetAction(VfsValue& v, int& action);
	bool GetCost(const VfsValue& v, double& cost);
	void DoAction( int action_id, const BinaryWorldState& src, BinaryWorldState& dest) const;
	bool ParseDecl(const String& str, Key& key);
	bool ParseCall(const String& str, Key& key);
	bool ParseCondParam(const Key& action, const String& str, Key& key);
	BinaryWorldStateMask& GetMask() const;
	bool ResolveCall(const Key& call_key, int& atom_idx);
	bool ParseWorldState(BinaryWorldState& ws, ValueMap in, bool initial);
public:
	OmniActionPlanner();
	VfsValue& GetInitial(Val& fs) override;
	bool SetParams(Value val) override;
	bool Run(Val& fs) override;
	bool GenerateSubValues(Val& val) override;
	bool TerminalTest(Val& v) override;
	double Utility(Val& val) override;
	double Estimate(Val& n) override;
	double Distance(Val& a, Val& b) override;
	String GetTreeString() const override;
	String GetTreeString(Val& v, BinaryWorldState& parent, int indent) const;
	String GetResultString(const Vector<Val*>& result) const override;
	int FindAction(const WorldStateKey& key) const;
	
	static bool HasDuplicateParams(const Key& key);
};

class ActionParamResolver {
	using Key = WorldStateKey;
	
	typedef enum : int {
		PRE,
		POST,
		ACTION,
		GOAL,
	} Source;
	
	struct SharedParam : Moveable<SharedParam> {
		struct Atom : Moveable<Atom> {
			int mask_idx = -1;
			int param_idx = -1;
			Source src;
		};
		int val = -1, def_val = -1;
		int cls = -1, def_cls = -1;
		Vector<Atom> atoms;
	};
	
	BinaryWorldStateSession* ses = 0;
	ArrayMap<int,SharedParam> shared;
	const PlannerEvent* ev = 0;
	const BinaryWorldState* src = 0;
	const BinaryWorldState* goal = 0;
	int pre_count = 0;
	int post_count = 0;
	int goal_count = 0;
	String err;
	BinaryWorldState solved_pre;
	
	bool IsPreTailMismatch();
	bool FindSharedVariables(const BinaryWorldState& ws, Source src);
	bool FindSharedVariables(int mask_idx, const Key& key, Source src);
	bool TestBasic();
	bool SolveShared();
	bool MakeDestination();
	bool MakeSharedPreCondition();
	bool TestSharedPreCondition();
	bool SolveSharedParam(WorldStateKey::Param& p, bool& changed);
public:
	typedef ActionParamResolver CLASSNAME;
	ActionParamResolver(BinaryWorldStateSession& ses);
	
	bool Resolve(const PlannerEvent& ev, const BinaryWorldState& current, const BinaryWorldState& goal);
	String GetError() const {return err;}
	bool IsError() const {return !err.IsEmpty();}
	
	Vector<One<BinaryWorldState>> results;
	
};


#endif
