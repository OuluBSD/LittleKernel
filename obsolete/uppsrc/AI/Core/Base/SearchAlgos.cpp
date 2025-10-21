#include "Base.h"

NAMESPACE_UPP




Searcher::Searcher() {
	
}

bool Searcher::GenerateSubValues(Val& n, NodeRoute& prev) {
	if (generator) {
		return generator->GenerateSubValues(n);
	}
	else {
		ASSERT(n.ext);
		if (!n.ext)
			return false;
		return n.ext->GenerateSubValues(generator_params, prev);
	}
}

bool Searcher::TerminalTest(Val& n) {
	if (termtester) {
		return termtester->TerminalTest(n);
	}
	else {
		ASSERT(n.ext);
		if (!n.ext)
			return true;
		return n.ext->TerminalTest();
	}
}

double Searcher::Utility(Val& n) {
	if (heuristic) {
		return heuristic->Utility(n);
	}
	else {
		ASSERT(n.ext);
		if (!n.ext)
			return -DBL_MAX;
		return n.ext->GetUtility();
	}
}

double Searcher::Estimate(Val& n) {
	if (heuristic) {
		return heuristic->Estimate(n);
	}
	else {
		ASSERT(n.ext);
		if (!n.ext)
			return DBL_MAX;
		return n.ext->GetEstimate();
	}
}

double Searcher::Distance(Val& a, Val& b) {
	if (heuristic) {
		return heuristic->Distance(a, b);
	}
	else {
		ASSERT(a.ext);
		if (!a.ext)
			return DBL_MAX;
		return a.ext->GetDistance(b);
	}
}

Vector<Val*> Searcher::Search(Val& src) {
	if (!SearchBegin(src))
		return Vector<Val*>();
	while (1) {
		if (!SearchIteration())
			break;
	}
	return SearchEnd();
}




MiniMax::MiniMax() {route.from_owner_only = true;}
	
double MiniMax::MaxValue(Val& n, int* decision_pos) {
	Val* p = 0;
	if (!GenerateSubValues(n,route))
		return -DBL_MAX;
	if (TerminalTest(n))
		return this->Utility(n);
	double v = -DBL_MAX;
	int pos = -1;
	for(int i = 0; i < n.GetCount(); i++) {
		// Find minimum...
		double d = MinValue(n[i]);
		// ..but use maximum
		if (d > v) {
			v = d;
			pos = i;
		}
	}
	if (decision_pos) *decision_pos = pos;
	return v;
}

double MiniMax::MinValue(Val& n, int* decision_pos) {
	Val* p = 0;
	if (!GenerateSubValues(n,route))
		return DBL_MAX;
	if (TerminalTest(n))
		return Searcher::Utility(n);
	double v = DBL_MAX;
	int pos = -1;
	for(int i = 0; i < n.GetCount(); i++) {
		double d = MaxValue(n[i]);
		if (d < v) {
			v = d;
			pos = i;
		}
	}
	if (decision_pos) *decision_pos = pos;
	return v;
}

bool MiniMax::SearchBegin(Val& src) {
	out.Clear();
	ptr = &src;
	prev = 0;
	return true;
}

bool MiniMax::SearchIteration() {
	Val& t = *out.Add(ptr);
	if (!GenerateSubValues(*ptr,route)) {
		out.Clear();
		return false;
	}
	if (TerminalTest(*ptr))
		return false;
	int type = out.GetCount() % 2;
	int pos = -1;
	double v;
	if (type == 1)
		v = MinValue(*ptr, &pos);
	else
		v = MaxValue(*ptr, &pos);
	if (pos == -1)
		return false;
	ptr = &(*ptr)[pos];
	//LOG(pos << " " << v);
	return true;
}

Vector<Val*> MiniMax::SearchEnd() {
	return pick(out);
}


















AlphaBeta::AlphaBeta() {route.from_owner_only = true;}

double AlphaBeta::MaxValue(Val& n, double alpha, double beta, int* decision_pos) {
	if (!GenerateSubValues(n,route))
		return -DBL_MAX;
	if (TerminalTest(n))
		return this->Utility(n);
	int pos = -1;
	double v = -DBL_MAX;
	for(int i = 0; i < n.GetCount(); i++) {
		// Find minimum...
		double d = MinValue(n[i], alpha, beta);
		// ..but use maximum
		if (d > v) {
			v = d;
			pos = i;
		}
		if (v > alpha)
			alpha = v;
		if (beta <= alpha)
			break;
	}
	if (decision_pos) *decision_pos = pos;
	return v;
}

double AlphaBeta::MinValue(Val& n, double alpha, double beta, int* decision_pos) {
	if (!GenerateSubValues(n,route))
		return DBL_MAX;
	if (TerminalTest(n))
		return this->Utility(n);
	int pos = -1;
	double v = DBL_MAX;
	for(int i = 0; i < n.GetCount(); i++) {
		double d = MaxValue(n[i], alpha, beta);
		if (d < v) {
			v = d;
			pos = i;
		}
		if (v < beta)
			beta = v;
		if (beta <= alpha)
			break;
	}
	if (decision_pos) *decision_pos = pos;
	return v;
}

bool AlphaBeta::SearchBegin(Val& src) {
	out.Clear();
	ptr = &src;
	root = &src;
	return true;
}

bool AlphaBeta::SearchIteration() {
	Val& t = *out.Add((Val*)ptr);
	int type = out.GetCount() % 2;
	if (!GenerateSubValues(*ptr,route)) {
		out.Clear();
		return false;
	}
	if (TerminalTest(*ptr))
		return false;
	int pos = -1;
	double v;
	double alpha = -DBL_MAX;
	double beta = DBL_MAX;
	if (type == 1)
		v = MinValue(*ptr, alpha, beta, &pos);
	else
		v = MaxValue(*ptr, alpha, beta, &pos);
	if (pos == -1)
		return false;
	ptr = &(*ptr)[pos];
	return true;
}

Vector<Val*> AlphaBeta::SearchEnd() {
	return pick(out);
}
















BreadthFirst::BreadthFirst() {route.from_owner_only = true;}

bool BreadthFirst::SearchBegin(Val& src) {
	root = &src;
	queue.Clear();
	next_queue.Clear();
	next_queue.Add(&src);
	v = DBL_MAX;
	ptr = 0;
	return true;
}

bool BreadthFirst::SearchIteration() {
	queue <<= next_queue;
	next_queue.Clear();
	
	bool all_terminals = true;
	for(int i = 0; i < queue.GetCount(); i++) {
		Val& t = *queue[i];
		
		if (!GenerateSubValues(t,route))
			return false;
		if (TerminalTest(t)) {
			ptr = &t;
			all_terminals = true;
			break;
		}
		else all_terminals = false;
		
		for(int j = 0; j < t.GetCount(); j++) {
			next_queue.Add(&t[j]);
		}
	}
	if (all_terminals)
		return false;
	return true;
}

Vector<Val*> BreadthFirst::SearchEnd() {
	Vector<Val*> out;
	while (ptr) {
		out.Insert(0, ptr);
		if (ptr == root)
			break;
		ptr = ptr->owner;
	}
	return out;
}









UniformCost::UniformCost() {route.from_owner_only = true;}

bool UniformCost::SearchBegin(Val& src) {
	root = &src;
	frontier.Clear();
	frontier.Add(&src);
	v = DBL_MAX;
	ptr = 0;
	return true;
}

bool UniformCost::SearchIteration() {
	if (!frontier.GetCount())
		return false;
	
	bool all_terminals = true;
	Val& t = *frontier[0];
	if (!GenerateSubValues(t,route))
		return false;
	if (TerminalTest(t)) {
		ptr = &t;
		all_terminals = true;
		return false;
	}
	else all_terminals = false;
	
	frontier.Remove(0);
	
	//TODO: change this to search like in AStar, because it is faster than insert, which moves huge block of memory always. Or do something completely different and better.
	for(int j = 0; j < t.GetCount(); j++) {
		Val& sub = t[j];
		double utility = this->Utility(sub);
		bool inserted = false;
		for(int k = 0; k < frontier.GetCount(); k++) {
			double f_utility = this->Utility(*frontier[k]);
			if (utility <= f_utility) {
				frontier.Insert(k, &sub);
				inserted = true;
				break;
			}
		}
		// Worst utility
		if (!inserted) {
			frontier.Add(&sub);
		}
	}
	if (all_terminals)
		return false;
	
	return true;
}

Vector<Val*> UniformCost::SearchEnd() {
	Vector<Val*> out;
	while (ptr) {
		out.Insert(0, ptr);
		if (ptr == root)
			break;
		ptr = ptr->owner;
	}
	return out;
}











DepthFirst::DepthFirst() {route.from_owner_only = true;}

bool DepthFirst::SearchBegin(Val& src) {
	root = &src;
	it.Create(src.BeginDeep());
	ptr = 0;
	prev = 0;
	v = DBL_MAX;
	return true;
}

bool DepthFirst::SearchIteration() {
	if (it->IsEnd())
		return false;
	
	if (!GenerateSubValues(**it,route))
		return false;
	if (TerminalTest(**it)) {
		ptr = *it;
		return false;
	}
	(*it)++;
	return true;
}

Vector<Val*> DepthFirst::SearchEnd() {
	Vector<Val*> out;
	while (ptr) {
		out.Insert(0, ptr);
		if (ptr == root)
			break;
		ptr = ptr->owner;
	}
	return out;
}















DepthLimited::DepthLimited() : limit(-1) {route.from_owner_only = true;}

void DepthLimited::SetLimit(int lim) {limit = lim;}

bool DepthLimited::SetParams(Value val) {
	ValueMap map = val;
	limit = map.Get("depth_limit", 10);
	return true;
}

bool DepthLimited::SearchBegin(Val& src) {
	root = &src;
	it.Create(src.BeginDeep());
	ptr = 0;
	prev = 0;
	v = DBL_MAX;
	return true;
}

bool DepthLimited::SearchIteration() {
	if (it->IsEnd())
		return false;
	
	if (!GenerateSubValues(**it,route))
		return false;
	if (TerminalTest(**it)) {
		ptr = *it;
		return false;
	}
	int depth = it->GetDepth();
	if (depth >= limit)
		it->IncNotDeep();
	else
		(*it)++;
	
	return true;
}

Vector<Val*> DepthLimited::SearchEnd() {
	Vector<Val*> out;
	while (ptr) {
		out.Insert(0, ptr);
		if (ptr == root)
			break;
		ptr = ptr->owner;
	}
	return out;
}
















BestFirst::BestFirst() {route.from_owner_only = true;}

bool BestFirst::SearchBegin(Val& src) {
	out.Clear();
	ptr = &src;
	prev = &src;
	return true;
}

bool BestFirst::SearchIteration() {
	out.Add((Val*)ptr);
	Val& t = *ptr;
	if (!GenerateSubValues(*ptr,route)) {
		out.Clear();
		return false;
	}
	if (TerminalTest(*ptr))
		return false;
	int pos = -1;
	double v = DBL_MAX;
	for(int i = 0; i < t.GetCount(); i++) {
		Val& sub = t[i];
		double estimate = this->Estimate(sub);
		if (estimate < v) {
			v = estimate;
			pos = i;
			if (v <= 0)
				break;
		}
	}
	if (pos == -1)
		return false;
	ptr = &(*ptr)[pos];
	
	return true;
}

Vector<Val*> BestFirst::SearchEnd() {
	return pick(out);
}















int AStar::FindNode(const Vector<NodePtr*>& vec, const Val* ptr) {
	int i = 0;
	for (const NodePtr* p : vec) {
		if (p->ptr == ptr)
			return i;
		i++;
	}
	return -1;
}

AStar::AStar() : max_worst(0), limit(0) {}

void AStar::operator=(const AStar& as) {
	max_worst = as.max_worst;
	do_search = as.do_search;
	limit = as.limit;
	closed_set <<= as.closed_set;
	open_set <<= as.open_set;
}
void AStar::SetLimit(int i) {limit = i;}
void AStar::Stop() {do_search = false;}

void AStar::TrimWorst(int limit, int count) {rm_limit = limit; max_worst = count; ASSERT(count >= 0);}

Vector<Val*> AStar::GetBestKnownPath() {
	if (smallest_id < 0)
		return Vector<Val*>();
	
	NodePtr* t_ptr = open_set[smallest_id];
	Val& t = *t_ptr->ptr;
	Val& s = *t_ptr->src;
	return ReconstructPath(t, s, closed_set, open_set);
}

Vector<Val*> AStar::ReconstructPath(Val& current, Val& current_src, Vector<NodePtr*>& closed_set, Vector<NodePtr*>& open_set) {
	Vector<Val*> path;
	Val* ptr = &current;
	Val* src = &current_src;
	while (1) {
		path.Add(!src->value.IsVoid() ? src : ptr);
		
		int i = FindNode(open_set, ptr);
		if (i == -1) {
			i = FindNode(closed_set, ptr);
			if (i == -1) break;
			else {
				const NodePtr* cf = closed_set[i]->came_from;
				ptr = cf ? cf->ptr : 0;
				src = cf ? cf->src : 0;
			}
		}
		else {
			const NodePtr* cf = open_set[i]->came_from;
			ptr = cf ? cf->ptr : 0;
			src = cf ? cf->src : 0;
		}
		if (!ptr) break;
	}
	Vector<Val*> out;
	out.SetCount(path.GetCount());
	for(int i = 0; i < path.GetCount(); i++) {
		out[path.GetCount()-1-i] = path[i];
	}
	return out;
}

Vector<Val*> AStar::ReconstructPath(Val& current, Val& current_src) {
	return ReconstructPath(current, current_src, closed_set, open_set);
}

Vector<Val*> AStar::GetBest() {
	NodePtr* n = 0;
	double best_score = DBL_MAX;
	for (const NodePtr* p : closed_set) {
		double score = p->g_score + p->f_score;
		if (score < best_score) {
			best_score = score;
			n = const_cast<NodePtr*>(p);
		}
	}
	if (n)
		return ReconstructPath(*n->ptr, *n->src, closed_set, open_set);
	
	return Vector<Val*>();
}

bool AStar::SearchBegin(Val& src) {
	do_search = true;
	
	out.Clear();
	closed_set.Clear();
	open_set.Clear();
	
	// For the first node, that value is completely heuristic.
	NodePtr& np = nodes.Add();
	np.ptr = &src;
	np.src = &src;
	np.g_score = 0;
	np.f_score = this->Estimate(src);
	open_set.Add(&np);
	
	worst_f_score.Clear();
	worst_id.Clear();
	worst_f_score.SetCount(max_worst,0);
	worst_id.SetCount(max_worst,0);
	return true;
}

bool AStar::SetParams(Value val) {
	ValueMap params = val;
	dump_intermediate_trees = params.Get("dump_intermediate_trees", false);
	return true;
}

Vector<Val*> AStar::SearchEnd() {
	return pick(out);
}

Vector<Val*> AStar::ContinueSearch(Val& src) {
	do_search = true;
	
	NodePtr* copy = 0;
	int i = 0;
	for (NodePtr* np : open_set) {
		if (np->ptr == &src) {
			copy = np;
			open_set.Remove(i);
			break;
		}
		++i;
	}
	if (!copy || !copy->ptr)
		return Vector<Val*>();
	
	closed_set.Append(open_set);
	open_set.Clear();
	
	open_set.Add(copy);
	
	worst_f_score.Clear();
	worst_id.Clear();
	worst_f_score.SetCount(max_worst,0);
	worst_id.SetCount(max_worst,0);
	while(SearchIteration())
		;
	
	return SearchEnd();
}

bool AStar::SearchIteration() {
	if (!(open_set.GetCount() && do_search))
		return Fail();
	
	// while open_set is not empty
	if (limit) {
		limit--;
		if (limit <= 0)
			do_search = 0;
	}
	
	bool rm = open_set.GetCount() > rm_limit;
	if (rm) {
		for(int i = 0; i < max_worst; i++) {
			worst_f_score[i] = -DBL_MAX;
			worst_id[i] = -1;
		}

		for(int i = 0; i < open_set.GetCount(); i++) {
			const NodePtr& nptr = *open_set[i];
			double f_score = nptr.f_score;
			
			for(int j = 0; j < max_worst; j++) {
				if (f_score > worst_f_score[j]) {
					for(int k = max_worst-1; k > j; k--) {
						worst_f_score[k] = worst_f_score[k-1];
						worst_id[k] = worst_id[k-1];
					}
					worst_f_score[j] = f_score;
					worst_id[j] = i;
					break;
				}
			}
		}
		
		int count = 0;
		Vector<int> rm_list;
		for(int i = 0; i < max_worst; i++) {
			int id = worst_id[i];
			if (id == -1) break;
			closed_set.Add(open_set[id]);
			rm_list.Add(id);
		}
		
		if (rm_list.GetCount()) {
			if (rm_list.GetCount() > 1) {
				Sort(rm_list, StdLess<int>());
			}
			open_set.Remove(rm_list);
		}
	}
	
	
	double smallest_f_score = DBL_MAX;
	smallest_id = -1;
	
	for(int i = 0; i < open_set.GetCount(); i++) {
		const NodePtr& nptr = *open_set[i];
		double f_score = nptr.f_score;
		if (f_score < smallest_f_score) {
			smallest_f_score = f_score;
			smallest_id = i;
		}
	}
	if (smallest_id < 0) {
		WhenError("internal error: smallest id not found");
		return false;
	}
	
	
	route.route.SetCount(0);
	NodePtr* t_ptr = open_set[smallest_id];
	Val& t = *t_ptr->ptr;
	Val& s = *t_ptr->src;
	const NodePtr* prev = t_ptr->came_from;
	for(int i = 0; i < 50; i++) {
		if (prev) {
			route.route.Add(prev->ptr);
			prev = prev->came_from;
		}
		else break;
	}
	double current_g_score = t_ptr->g_score;
	
	if (!GenerateSubValues(t, route)) {
		out.Clear();
		return false;
	}
	if (TerminalTest(t)) {
		out = ReconstructPath(t, s, closed_set, open_set);
		return false; // "don't continue + out-vector" == ready
	}
	
	if (!do_search)
		return Fail();
	
	open_set.Remove(smallest_id);
	smallest_id = -1;
	closed_set.Add(t_ptr);
	
	if (dump_intermediate_trees && generator) {
		LOG(generator->GetTreeString());
	}
	
	for(int j = 0; j < t.sub.GetCount(); j++) {
		Val& sub = t.sub[j];
		Val* link = sub.Resolve();
		ASSERT(!link->symbolic_link);
		if (FindNode(closed_set, link) != -1)
			continue; // Ignore the neighbor which is already evaluated.
		// The distance from start to a neighbor
		double sub_g_score = current_g_score + this->Distance(t, sub);
		double sub_f_score = sub_g_score + this->Estimate(*link);
		int k = FindNode(open_set, link);
		if (k == -1) {
			// Discover a new node
			k = open_set.GetCount();
			NodePtr& subptr = nodes.Add();
			subptr.ptr = link;
			subptr.src = &sub;
			subptr.came_from = t_ptr;
			subptr.f_score = sub_f_score;
			subptr.g_score = sub_g_score;
			ASSERT(subptr.ptr);
			open_set.Add(&subptr);
		}
		else if (sub_g_score >= current_g_score)
			continue; // This is not a better path.
		else {
			NodePtr& subptr = *open_set[k];
			ASSERT(subptr.ptr == &sub);
			ASSERT(subptr.came_from == t_ptr);
			ASSERT(subptr.ptr);
			subptr.f_score = sub_f_score;
			subptr.g_score = sub_g_score;
			open_set.Remove(k);
		}
	}
	
	return true;
}

bool AStar::Fail() {
	out.Clear();
	return false;
}






void Generator::SetValueFunction(Event<Val&> e) {
	set_value = e;
}



GeneratorRandom::GeneratorRandom() {
	
}

bool GeneratorRandom::SetParams(Value val) {
	ValueMap map = val;
	total = map.Get("total",0);
	low = map.Get("low",0);
	high = map.Get("high",0);
	depth_limit = map.Get("depth_limit",0);
	initial = map.Get("initial", 1);
	runtime = map.Get("runtime", 0);
	return true;
}

bool GeneratorRandom::Run(Val& fs) {
	root = &fs;
	if (!initial) {
		set_value(fs);
		return true;
	}
	ASSERT(set_value);
	if (!set_value)
		return false;
	GenerateTree(fs, total, low, high, set_value);
	count = total;
	return true;
}

bool GeneratorRandom::GenerateSubValues(Val& val) {
	int d = val.GetDepth(root);
	if ((depth_limit && d >= depth_limit) || val.GetCount())
		return true;
	int range = high - low;
	int max_count = max(0, total - count);
	if (!max_count)
		return true;
	int sub_node_count = min<int>(max_count, low + Random(range));
	val.SetCount(sub_node_count);
	count += sub_node_count;
	for (auto& s : val.sub)
		set_value(s);
	return true;
}




bool NoSubTerminal::TerminalTest(Val& v) {
	return v.sub.IsEmpty();
}





double SimpleHeuristic::Utility(Val& n) {
	return n.value;
}

double SimpleHeuristic::Estimate(Val& n) {
	double a = n.value;
	double est = goal - a;
	return est;
}

double SimpleHeuristic::Distance(Val& a, Val& b) {
	double av = a.value;
	double bv = b.value;
	return bv - av;
}








END_UPP_NAMESPACE
