#include "Content.h"
#include <AI/Core/Prompting/Prompting.h>


NAMESPACE_UPP


void NavigatorState::RemoveDuplicate(const NavigatorState& s) {
	if (sorter == s.sorter) sorter = 0;
	if (element == s.element) element.Clear();
	if (attr.group == s.attr.group) attr.group.Clear();
	if (attr.value == s.attr.value) attr.value.Clear();
	if (clr_i == s.clr_i) clr_i = -1;
	if (act.action == s.act.action) act.action.Clear();
	if (act.arg == s.act.arg) act.arg.Clear();
	if (typeclass_i == s.typeclass_i) typeclass_i = -1;
	if (con_i == s.con_i) con_i = -1;
}

void NavigatorState::Clear() {
	line = 0;
	depth = -1;
	el = 0;
	sorter = 0;
	element.Clear();
	attr.group.Clear();
	attr.value.Clear();
	clr_i = -1;
	act.action.Clear();
	act.arg.Clear();
	ActionHeader act;
	typeclass_i = -1;
	con_i = -1;
}

ArrayMap<String, ScriptSolver>& __ScriptSolvers() {
	static ArrayMap<String, ScriptSolver> map;
	return map;
}


ScriptSolver::ScriptSolver() {
	
}

ScriptSolver& ScriptSolver::Get(const DatasetPtrs& p, const String& ecs_path) {
	ASSERT(p.script);
	ArrayMap<String, ScriptSolver>& map = __ScriptSolvers();
	int i = map.Find(ecs_path);
	if (i >= 0)
		return map[i];
	
	ScriptSolver& ls = map.Add(ecs_path);
	ls.p = p;
	return ls;
}

int ScriptSolver::GetPhaseCount() const {
	return LS_COUNT;
}

void ScriptSolver::DoPhase() {
	TODO
}

void ScriptSolver::GetSuggestions(const DynPart& part, const DynSub& sub, const Vector<const DynLine*>& lines, Event<> WhenPartiallyReady) {
	tmp_part = const_cast<DynPart*>(&part);
	tmp_sub = const_cast<DynSub*>(&sub);
	tmp_lines <<= lines;
	this->WhenPartiallyReady = WhenPartiallyReady;
	
	Lyrics& l = *this->p.lyrics;
	ScriptSolverArgs args; // 18
	args.fn = 18;
	
	args.lng = l.lang;
	
	for(int i = 0; i < lines.GetCount(); i++) {
		const DynLine& l = *lines[i];
		if (l.text.IsEmpty())
			break;
		args.phrases << l.text;
		if (!l.user_text.IsEmpty())
			args.phrases2 << l.user_text;
	}
	
	Index<String> elements;
	if (part.el.element.GetCount()) elements.FindAdd(part.el.element);
	if (sub.el.element.GetCount()) elements.FindAdd(sub.el.element);
	args.elements <<= elements.GetKeys();
	
	TaskMgr& m = AiTaskManager();
	m.GetScriptSolver(args, [this](String res) {
		Vector<String> lines = Split(res, "\n");
		
		for(int i = 0; i < lines.GetCount(); i++) {
			String& l = lines[i];
			l = TrimBoth(l);
			if (l.IsEmpty()) {
				lines.Remove(i--);
				continue;
			}
			if (!IsDigit(l[0]) && i > 0) {
				lines[i-1] << " / " << l;
				lines.Remove(i--);
				continue;
			}
		}
		
		for (String& s : lines) {
			if (IsDigit(s[0])) {
				int a = s.Find(".");
				if (a >= 0)
					s = TrimLeft(s.Mid(a+1));
			}
		}
		DUMPC(lines);
		
		for (const DynLine* l : tmp_lines) {
			DynLine& dl = const_cast<DynLine&>(*l);
			dl.suggs.Clear();
		}
		
		for (String& s : lines) {
			Vector<String> parts = Split(s, "/");
			for(int i = 0; i < parts.GetCount() && i < tmp_lines.GetCount(); i++) {
				DynLine& dl = const_cast<DynLine&>(*tmp_lines[i]);
				dl.suggs << TrimBoth(parts[i]);
			}
			for(int i = parts.GetCount(); i < tmp_lines.GetCount(); i++) {
				DynLine& dl = const_cast<DynLine&>(*tmp_lines[i]);
				dl.suggs.Clear();
			}
		}
		
		this->WhenPartiallyReady();
	});
}

void ScriptSolver::GetExpanded(int part_i, int sub_i, int line_i, Event<> WhenPartiallyReady) {
	auto& src = p.src->Data();
	if (!p.lyric_struct || !p.lyrics) return;
	auto& l = *p.lyric_struct;
	auto& ly = *p.lyrics;
	if (part_i >= l.parts.GetCount()) {
		String err = "ScriptSolver::GetExpanded: error: part_i >= l.parts.GetCount(): " + IntStr(part_i) + " >= " + IntStr(l.parts.GetCount());
		LOG(err);
		WhenError(err);
		return;
	}
	DynPart& part = l.parts[part_i];
	DynSub& sub = part.sub[sub_i];
	DynLine& line = sub.lines[line_i];
	tmp_part = &part;
	tmp_sub = &sub;
	tmp_line = &line;
	this->WhenPartiallyReady = WhenPartiallyReady;
	
	if (line.text.IsEmpty())
		return;
	
	ScriptSolverArgs args; // 21
	args.fn = 21;
	args.lng = ly.lang;
	
	if (!part.story.IsEmpty()) args.phrases2 << part.story;
	if (!sub.story.IsEmpty())  args.phrases2 << sub.story;
	
	int gender = p.entity->GetGender();
	
	TODO
	#if 0
	for(int i = 0; i < part.sub.GetCount(); i++) {
		const auto& s = part.sub[i];
		for(int j = 0; j < s.lines.GetCount(); j++) {
			const auto& dl = s.lines[j];
			args.phrases << dl.text;
			
			auto& state = args.line_states.Add();
			state.content = dl.text;
			
			int tcent_i = GetTypeclassEntity(dl.safety, gender);
			const auto& ents = src.typeclass_entities[tcent_i];
			state.style_type = ents.GetKey(dl.style_type);
			const auto& vec = dl.style_type < ents.GetCount() ? ents[dl.style_type] : ents.Top();
			state.style_entity = dl.style_entity < vec.GetCount() ? vec [dl.style_entity] : vec.Top();
			state.safety = dl.safety;
			state.connector = dl.connector;
		}
	}
	args.ref = line.text;
	
	TaskMgr& m = AiTaskManager();
	m.GetScriptSolver(args, [this](String res) {
		res = TrimBoth(res);
		res.Replace("\r", "");
		res.Replace("\n\n", "\n");
		RemoveQuotes(res);
		if (res.Left(2) == "- ") res = TrimBoth(res.Mid(2));
		
		tmp_line->expanded = res;
		
		this->WhenPartiallyReady();
	});
	#endif
}

void ScriptSolver::GetSuggestions2(int part_i, int sub_i, const Vector<const DynLine*>& lines, Event<> WhenPartiallyReady) {
	auto& src = p.src->Data();
	if (!p.lyric_struct || !p.lyrics) return;
	auto& l = *p.lyric_struct;
	auto& ly = *p.lyrics;
	DynPart& part = l.parts[part_i];
	DynSub& sub = part.sub[sub_i];
	tmp_part = &part;
	tmp_sub = &sub;
	tmp_lines <<= lines;
	this->WhenPartiallyReady = WhenPartiallyReady;
	
	ScriptSolverArgs args; // 22
	args.fn = 22;
	args.lng = ly.lang;
	
	int gender = p.entity->GetGender();
	
	TODO
	#if 0
	NavigatorState line_state;
	for(int i = 0; i < lines.GetCount(); i++) {
		const DynLine& dl = *lines[i];
		if (dl.text.IsEmpty())
			break;
		args.phrases << dl.text;
		args.phrases2 << dl.expanded;
		
		auto& state = args.line_states.Add();
		ReadNavigatorState(l, part_i, sub_i, i, line_state,  2);
		CopyState(state, line_state);
		
		int tcent_i = GetTypeclassEntity(dl.safety, gender);
		const auto& ents = src.typeclass_entities[tcent_i];
		state.style_type = ents.GetKey(dl.style_type);
		state.style_entity = ents[dl.style_type][dl.style_entity];
		state.safety = dl.safety;
		state.line_len = dl.line_len;
		state.connector = dl.connector;
		state.line_begin = dl.line_begin;
	}
	
	TaskMgr& m = AiTaskManager();
	m.GetScriptSolver(args, [this](String res) {
		Vector<String> lines = Split(res, "\n");
		
		for(int i = 0; i < lines.GetCount(); i++) {
			String& l = lines[i];
			l = TrimBoth(l);
			if (l.IsEmpty()) {
				lines.Remove(i--);
				continue;
			}
			if (!IsDigit(l[0]) && i > 0) {
				lines[i-1] << " / " << l;
				lines.Remove(i--);
				continue;
			}
		}
		
		for (String& s : lines) {
			if (IsDigit(s[0])) {
				int a = s.Find(".");
				if (a >= 0)
					s = TrimLeft(s.Mid(a+1));
			}
		}
		DUMPC(lines);
		
		for (const DynLine* l : tmp_lines) {
			DynLine& dl = const_cast<DynLine&>(*l);
			dl.suggs.Clear();
		}
		
		for (String& s : lines) {
			RemoveQuotes(s);
			Vector<String> parts = Split(s, "/");
			for(int i = 0; i < parts.GetCount() && i < tmp_lines.GetCount(); i++) {
				DynLine& dl = const_cast<DynLine&>(*tmp_lines[i]);
				dl.suggs << TrimBoth(parts[i]);
			}
			for(int i = parts.GetCount(); i < tmp_lines.GetCount(); i++) {
				DynLine& dl = const_cast<DynLine&>(*tmp_lines[i]);
				dl.suggs.Clear();
			}
		}
		
		this->WhenPartiallyReady();
	});
	#endif
}

void ScriptSolver::GetStyleSuggestion(int part_i, int sub_i, const Vector<const DynLine*>& lines, Event<> WhenPartiallyReady) {
	auto& src = p.src->Data();
	if (!p.lyric_struct || !p.lyrics) return;
	auto& l = *p.lyric_struct;
	auto& ly = *p.lyrics;
	DynPart& part = l.parts[part_i];
	DynSub& sub = part.sub[sub_i];
	tmp_part = &part;
	tmp_sub = &sub;
	tmp_lines <<= lines;
	this->WhenPartiallyReady = WhenPartiallyReady;
	
	ScriptSolverArgs args; // 23
	args.fn = 23;
	args.lng = ly.lang;
	
	int gender = p.entity->GetGender();
	
	TODO
	#if 0
	NavigatorState line_state;
	for(int i = 0; i < lines.GetCount(); i++) {
		const DynLine& dl = *lines[i];
		if (dl.text.IsEmpty())
			break;
		args.phrases << dl.text;
		args.phrases2 << dl.expanded;
		
		auto& state = args.line_states.Add();
		ReadNavigatorState(l, part_i, sub_i, i, line_state,  2);
		CopyState(state, line_state);
		
		int tcent_i = GetTypeclassEntity(dl.safety, gender);
		const auto& ents = src.typeclass_entities[tcent_i];
		state.style_type = ents.GetKey(dl.style_type);
		state.style_entity = ents[dl.style_type][dl.style_entity];
		state.safety = dl.safety;
		state.line_len = dl.line_len;
		state.connector = dl.connector;
		state.line_begin = dl.line_begin;
		
		if (i == 0) {
			for(int i = 0; i < ents.GetCount(); i++) {
				args.styles.Add(ents.GetKey(i));
			}
		}
	}
	
	TaskMgr& m = AiTaskManager();
	m.GetScriptSolver(args, [this](String res) {
		Vector<String> lines = Split(res, "\n");
		
		res = TrimBoth(res);
		if (res.IsEmpty() || !IsDigit(res[0]))
			return;
		
		int type = ScanInt(res);
		
		for (const DynLine* l : tmp_lines) {
			DynLine& dl = const_cast<DynLine&>(*l);
			dl.style_type = type;
		}
		
		this->WhenPartiallyReady();
	});
	#endif
}

void ScriptSolver::GetSubStory(int part_i, int sub_i, Event<> WhenPartiallyReady) {
	if (!p.lyric_struct || !p.lyrics) return;
	auto& l = *p.lyric_struct;
	auto& ly = *p.lyrics;
	if (part_i >= l.parts.GetCount()) {
		String err = "ScriptSolver::GetSubStory: error: part_i >= l.parts.GetCount(): " + IntStr(part_i) + " >= " + IntStr(l.parts.GetCount());
		LOG(err);
		WhenError(err);
		return;
	}
	DynPart& part = l.parts[part_i];
	DynSub& sub = part.sub[sub_i];
	tmp_part = const_cast<DynPart*>(&part);
	tmp_sub = const_cast<DynSub*>(&sub);
	this->WhenPartiallyReady = WhenPartiallyReady;
	
	ScriptSolverArgs args; // 19
	args.fn = 19;
	args.lng = ly.lang;
	
	// Get line phrases and properties
	NavigatorState sub_state, line_state;
	ReadNavigatorState(l, part_i, sub_i, -1, sub_state,  1);
	CopyState(args.state, sub_state);
	
	for(int i = 0; i < sub.lines.GetCount(); i++) {
		const DynLine& dl = sub.lines[i];
		if (dl.text.IsEmpty())
			break;
		
		ReadNavigatorState(l, part_i, sub_i, i, line_state,  2);
		line_state.RemoveDuplicate(sub_state);
		CopyState(args.line_states.Add(), line_state);
		
		args.phrases << dl.text;
	}
	
	// Know the previous story
	for(int i = 0; i <= part_i; i++) {
		DynPart& p0 = l.parts[i];
		if (p0.story.GetCount()) {
			args.previously << p0.story << "\n";
			continue;
		}
		for(int j = 0; j < p0.sub.GetCount(); j++) {
			DynSub& s0 = p0.sub[j];
			if (&s0 == &sub)
				break;
			args.previously << s0.story << "\n";
			continue;
		}
	}
	
	// Peek upcoming lyrics
	bool write = false;
	for(int i = part_i; i < l.parts.GetCount(); i++) {
		DynPart& p0 = l.parts[i];
		for(int j = 0; j < p0.sub.GetCount(); j++) {
			DynSub& s0 = p0.sub[j];
			if (&s0 == &sub) {
				write = true;
				continue;
			}
			if (write) {
				for(int k = 0; k < s0.lines.GetCount(); k++) {
					DynLine& l0 = s0.lines[k];
					if (l0.text.GetCount())
						args.peek << l0.text << "\n";
				}
			}
		}
	}
	
	TaskMgr& m = AiTaskManager();
	m.GetScriptSolver(args, [this](String res) {
		res = TrimBoth(res);
		res.Replace("\r", "");
		res.Replace("\n\n", "\n");
		tmp_sub->story = res;
		this->WhenPartiallyReady();
	});
}

void ScriptSolver::GetPartStory(int part_i, Event<> WhenPartiallyReady) {
	if (!p.lyric_struct || !p.lyrics) return;
	auto& l = *p.lyric_struct;
	auto& ly = *p.lyrics;
	DynPart& part = l.parts[part_i];
	tmp_part = const_cast<DynPart*>(&part);
	tmp_sub = 0;
	this->WhenPartiallyReady = WhenPartiallyReady;
	
	ScriptSolverArgs args; // 20
	args.fn = 20;
	args.lng = ly.lang;
	
	// Get line phrases and properties
	NavigatorState part_state, sub_state;
	ReadNavigatorState(l, part_i, -1, -1, part_state,  0);
	CopyState(args.state, part_state);
	
	for(int i = 0; i < part.sub.GetCount(); i++) {
		const DynSub& ds = part.sub[i];
		if (ds.story.IsEmpty())
			continue;
		
		ReadNavigatorState(l, part_i, i, -1, sub_state,  1);
		sub_state.RemoveDuplicate(part_state);
		CopyState(args.line_states.Add(), sub_state);
		
		args.phrases << ds.story;
	}
	
	// Know the previous story
	for(int i = 0; i < part_i; i++) {
		DynPart& p0 = l.parts[i];
		if (p0.story.GetCount()) {
			args.previously << p0.story << "\n";
			continue;
		}
		for(int j = 0; j < p0.sub.GetCount(); j++) {
			DynSub& s0 = p0.sub[j];
			args.previously << s0.story << "\n";
			continue;
		}
	}
	
	// Peek upcoming lyrics
	for(int i = part_i+1; i < l.parts.GetCount(); i++) {
		DynPart& p0 = l.parts[i];
		for(int j = 0; j < p0.sub.GetCount(); j++) {
			DynSub& s0 = p0.sub[j];
			for(int k = 0; k < s0.lines.GetCount(); k++) {
				DynLine& l0 = s0.lines[k];
				if (l0.text.GetCount())
					args.peek << l0.text << "\n";
			}
		}
	}
	
	TaskMgr& m = AiTaskManager();
	m.GetScriptSolver(args, [this](String res) {
		res = TrimBoth(res);
		res.Replace("\r", "");
		res.Replace("\n\n", "\n");
		tmp_part->story = res;
		this->WhenPartiallyReady();
	});
}





void ReadNavigatorState(LyricalStructure& s, int part_i, int sub_i, int line_i, NavigatorState& state, int depth_limit) {
	state.Clear();
	if (part_i < 0 || part_i >= s.parts.GetCount())
		return;
	
	DynPart& dp = s.parts[part_i];
	
	#define COPY(v)   if (state.v.IsEmpty()) state.v = el . v;
	#define COPY_I(v) if (state.v < 0) state.v = el . v;
	#define COPY_S(v) if (state.v == 0) state.v = el . v;
	LineElement* elp = 0;
	if (sub_i >= 0 && line_i >= 0 && depth_limit >= 2) {
		DynSub& ds = dp.sub[sub_i];
		DynLine& dl = ds.lines[line_i];
		auto& el = dl.el;
		COPY(element)
		COPY(attr.group)
		COPY(attr.value)
		COPY_I(clr_i)
		COPY(act.action)
		COPY(act.arg)
		COPY_I(typeclass_i)
		COPY_I(con_i)
		COPY_S(sorter)
		if (state.el == 0) {state.depth = 2; state.el = &el;}
	}
	if (sub_i >= 0 && depth_limit >= 1) {
		DynSub& ds = dp.sub[sub_i];
		auto& el = ds.el;
		COPY(element)
		COPY(attr.group)
		COPY(attr.value)
		COPY_I(clr_i)
		COPY(act.action)
		COPY(act.arg)
		COPY_I(typeclass_i)
		COPY_I(con_i)
		COPY_S(sorter)
		if (state.el == 0) {state.depth = 1; state.el = &el;}
	}
	if (depth_limit >= 0) {
		auto& el = dp.el;
		COPY(element)
		COPY(attr.group)
		COPY(attr.value)
		COPY_I(clr_i)
		COPY(act.action)
		COPY(act.arg)
		COPY_I(typeclass_i)
		COPY_I(con_i)
		COPY_S(sorter)
		if (state.el == 0) {state.depth = 0; state.el = &el;}
	}
	#undef COPY
	#undef COPY_I
	#undef COPY_S
}

void ScriptSolver::CopyState(ScriptSolverArgs::State& to, const NavigatorState& from) {
	
	TODO
	#if 0
	auto& src = p.src->Data();
	#define COPY(x) to.x = from.x;
	COPY(element)
	to.attr_group = from.attr.group;
	to.attr_value = from.attr.value;
	COPY(clr_i)
	to.act_action = from.act.action;
	to.act_arg = from.act.arg;
	to.typeclass = from.typeclass_i >= 0 ? src.ctx.typeclass.labels[from.typeclass_i] : String();
	const auto& cons = src.ctx.content.labels;
	int c0 = from.con_i / PART_COUNT;
	int c1 = from.con_i % PART_COUNT;
	to.content.Clear();
	to.content_mod.Clear();
	if (from.con_i >= 0 && c0 < cons.GetCount()) {
		to.content = cons[c0].key;
		to.content_mod = cons[c0].parts[c1];
	}
	#undef COPY
	#endif
}


END_UPP_NAMESPACE
