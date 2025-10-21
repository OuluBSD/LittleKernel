#include "Platform.h"

NAMESPACE_UPP

DatabaseBrowser::DatabaseBrowser() { Load(); }

#define ITEM(x)                                                                                \
	case x:                                                                                    \
		return KeyToName(#x);
String DatabaseBrowser::GetTypeString(ColumnType t)
{
	switch(t) {
		ITEM(ATTR_GROUP)
		ITEM(ATTR_VALUE)
		ITEM(COLOR)
		ITEM(ACTION)
		ITEM(ACTION_ARG)
		ITEM(ELEMENT)
		ITEM(TYPECLASS)
		ITEM(CONTENT)
	default:
		return "ERROR";
	}
}
#undef ITEM

String DatabaseBrowser::GetModeKey(int i)
{
	switch(i) {
#define MODE(x)                                                                                \
	case x:                                                                                    \
		return #x;
		DBROWSER_MODE_LIST
#undef MODE
	default:
		return "";
	}
}

String DatabaseBrowser::GetModeString(int i)
{
	switch(i) {
#define MODE(x)                                                                                \
	case x:                                                                                    \
		return KeyToName(#x);
		DBROWSER_MODE_LIST
#undef MODE
	default:
		return "ERROR";
	}
}

int DatabaseBrowser::FindMode(hash_t h)
{
	for(int i = 0; i < MODE_COUNT; i++) {
		if(GetModeHash(i) == h)
			return i;
	}
	return -1;
}

hash_t DatabaseBrowser::GetModeHash(int mode)
{
	if(mode < 0 || mode >= MODE_COUNT)
		return 0;
	return GetModeString(mode).GetHashValue();
}

void DatabaseBrowser::SetMode(const DatasetPtrs& p, int i)
{
	ASSERT(p.src);
	if(i == mode)
		return;
	this->p = p;
	mode = i;
	for(int i = 0; i < TYPE_COUNT; i++)
		items[i].Clear();
	phrase_parts.Clear();
	for(int i = 0; i < TYPE_COUNT; i++)
		order[i] = INVALID;
	int o = 0;
	Vector<String> parts = Split(GetModeKey(mode), "_");
	for(String& part : parts) {
		if(part == "ELEMENT") {
			order[o++] = ELEMENT;
		}
		else if(part == "ATTR") {
			order[o++] = ATTR_GROUP;
			order[o++] = ATTR_VALUE;
		}
		else if(part == "COLOR") {
			order[o++] = COLOR;
		}
		else if(part == "ACTION") {
			order[o++] = ACTION;
			order[o++] = ACTION_ARG;
		}
		else if(part == "CONTENT") {
			order[o++] = CONTENT;
		}
		else if(part == "TYPECLASS") {
			order[o++] = TYPECLASS;
		}
		else
			TODO;
	}

	Init();
}

DatabaseBrowser::ColumnType DatabaseBrowser::GetCur(int cursor_i) const
{
	ASSERT(cursor_i >= 0 && cursor_i < TYPE_COUNT);
	return order[cursor_i];
}

bool DatabaseBrowser::IsSub(int cur, int cursor_i) const { return GetCur(cursor_i) > cur; }

void DatabaseBrowser::Init()
{
	uniq_acts.Clear();
	color_counts.Clear();
	for(int i = 0; i < TYPE_COUNT; i++)
		cursor[i] = 0;

	ResetCursor();
}

void DatabaseBrowser::ResetCursor() { ResetCursor(-1, INVALID); }

void DatabaseBrowser::SetAll(const DatasetPtrs& p, hash_t sorter, const String& element, const AttrHeader& attr,
                             int clr, const ActionHeader& act, int tc_i, int con_i)
{
	this->p = p;
	if (!p.src)
		return;
	SetInitialData();

	if(sorter) {
		int m = FindMode(sorter);
		if(m >= 0)
			SetMode(p,m);
	}

	for(int i = 0; i < TYPE_COUNT; i++) {
		auto t = order[i];
		if(t == INVALID)
			break;

		FillItems(t);

		int& tgt = history.GetAdd(GetHash(i), 0);
		tgt = 0;

		switch(t) {
		case ATTR_GROUP: {
			const auto& attr_groups = Get(ATTR_GROUP);
			String attr_group = attr.IsEmpty() ? "All" : attr.group;
			for(int i = 0; i < attr_groups.GetCount(); i++) {
				const auto& at = attr_groups[i];
				if(at.str == attr_group) {
					tgt = i;
					break;
				}
			}
			break;
		}
		case ATTR_VALUE: {
			const auto& attr_values = Get(ATTR_VALUE);
			String attr_value = attr.IsEmpty() ? "All" : attr.value;
			for(int i = 0; i < attr_values.GetCount(); i++) {
				const auto& at = attr_values[i];
				if(at.str == attr_value) {
					tgt = i;
					break;
				}
			}
			break;
		}
		case COLOR: {
			const auto& colors = Get(COLOR);
			for(int i = 0; i < colors.GetCount(); i++) {
				if(colors[i].idx == clr) {
					tgt = i;
					break;
				}
			}
			break;
		}
		case ACTION: {
			const auto& actions = Get(ACTION);
			String action = act.IsEmpty() ? "All" : act.action;
			for(int i = 0; i < actions.GetCount(); i++) {
				if(actions[i].str == action) {
					tgt = i;
					break;
				}
			}
			break;
		}
		case ACTION_ARG: {
			const auto& args = Get(ACTION_ARG);
			String arg = act.IsEmpty() ? "All" : act.arg;
			for(int i = 0; i < args.GetCount(); i++) {
				if(args[i].str == arg) {
					tgt = i;
					break;
				}
			}
			break;
		}
		case ELEMENT: {
			const auto& args = Get(ELEMENT);
			String str = element.IsEmpty() ? "All" : element;
			for(int i = 0; i < args.GetCount(); i++) {
				if(args[i].str == str) {
					tgt = i;
					break;
				}
			}
			break;
		}
		case TYPECLASS: {
			const auto& args = Get(TYPECLASS);
			for(int i = 0; i < args.GetCount(); i++) {
				if(args[i].idx == tc_i) {
					tgt = i;
					break;
				}
			}
			break;
		}
		case CONTENT: {
			const auto& args = Get(CONTENT);
			for(int i = 0; i < args.GetCount(); i++) {
				if(args[i].idx == con_i) {
					tgt = i;
					break;
				}
			}
			break;
		}
		default:
			break;
		}

		cursor[i] = tgt;

		FilterData(t);
	}
}

void DatabaseBrowser::Update() {}

hash_t DatabaseBrowser::GetHash(int columns) const
{
	CombineHash ch;
	ch.Do(mode);
	for(int i = 0; i < TYPE_COUNT; i++)
		ch.Do(i < columns ? cursor[i] : -1);
	return ch;
}

void DatabaseBrowser::Store()
{
	String file = ConfigFile("DatabaseBrowser.bin");
	StoreToFile(*this, file);
}

void DatabaseBrowser::Load()
{
	String file = ConfigFile("DatabaseBrowser.bin");
	LoadFromFile(*this, file);
}

void DatabaseBrowser::SortBy(int i)
{
	auto& src = p.src->Data();

	if(i == 0) {
		struct Sorter {
			VectorMap<hash_t, PhrasePart>* phrase_parts;
			bool operator()(int a, int b) const
			{
				int at = 0, bt = 0;
				PhrasePart& pa = (*phrase_parts)[a];
				PhrasePart& pb = (*phrase_parts)[b];
				for(int i = 0; i < SCORE_COUNT; i++) {
					at += pa.scores[i];
					bt += pb.scores[i];
				}
				return at > bt;
			}
		} s;
		s.phrase_parts = &src.phrase_parts;
		Sort(phrase_parts, s);
	}
	else if(i >= 1 && i < SCORE_COUNT + 1) {
		i--;
		struct Sorter {
			VectorMap<hash_t, PhrasePart>* phrase_parts;
			int i;
			bool operator()(int a, int b) const
			{
				return (*phrase_parts)[a].scores[i] > (*phrase_parts)[b].scores[i];
			}
		} s;
		s.i = i;
		s.phrase_parts = &src.phrase_parts;
		Sort(phrase_parts, s);
	}

	sorting = i;
}

void DatabaseBrowser::SetMidRhymingLimit(double d, bool up)
{
	mid_rhyme_distance_limit = d;
	if(up)
		Update();
}

void DatabaseBrowser::SetEndRhymingLimit(double d, bool up)
{
	end_rhyme_distance_limit = d;
	if(up)
		Update();
}

void DatabaseBrowser::SetMidRhymeFilter(WString wrd, bool up)
{
	mid_rhyme = wrd;
	filter_mid_rhyme = !wrd.IsEmpty();
	if(up)
		Update();
}

void DatabaseBrowser::SetEndRhymeFilter(WString wrd, bool up)
{
	end_rhyme = wrd;
	filter_end_rhyme = !wrd.IsEmpty();
	if(up)
		Update();
}

bool DatabaseBrowser::FilterPronounciation(SrcTextData& da, const PhrasePart& pp)
{
	if(filter_mid_rhyme) {
		bool found = false;
		int count = pp.words.GetCount() - 1;
		for(int i = 0; i < count; i++) {
			WString phonetic = da.words_[pp.words[i]].phonetic;
			double limit = mid_rhyme_distance_limit;

			// Limit phonetic word using only ending (easy solution)
			int count = min(phonetic.GetCount(), mid_rhyme.GetCount());
			if(!count)
				return true;
			if(limit >= 0)
				phonetic = phonetic.Mid(phonetic.GetCount() - count, count);
			else {
				limit = -limit;
				phonetic = phonetic.Left(count);
			}

			double dist = GetSpellingRelativeDistance(mid_rhyme, phonetic);
			if(dist <= limit) {
				found = true;
				break;
			}
		}
		if(!found && count > 0)
			return true;
	}
	if(filter_end_rhyme) {
		int last_wrd = pp.words.Top();
		WString phonetic = da.words_[last_wrd].phonetic;
		double limit = end_rhyme_distance_limit;

		// Limit phonetic word using only ending (easy solution)
		int count = min(phonetic.GetCount(), end_rhyme.GetCount());
		if(!count)
			return true;
		if(limit >= 0)
			phonetic = phonetic.Mid(phonetic.GetCount() - count, count);
		else {
			limit = -limit;
			phonetic = phonetic.Left(phonetic.GetCount() - count);
		}

		double dist = GetSpellingRelativeDistance(end_rhyme, phonetic);
		if(dist > limit)
			return true;
	}
	return false;
}

//TextDatabase& DatabaseBrowser::GetDatabase() { return TextDatabase::Single(); }

DatabaseBrowser& DatabaseBrowser::Single()
{
	static DatabaseBrowser db;
	return db;
}

int DatabaseBrowser::FindAction(const String& s)
{
	const auto& actions = Get(ACTION);
	for(int i = 0; i < actions.GetCount(); i++) {
		if(actions[i].str == s)
			return i;
	}
	return 0; // return "All", which is first
}

int DatabaseBrowser::FindArg(const String& s)
{
	const auto& args = Get(ACTION_ARG);
	for(int i = 0; i < args.GetCount(); i++) {
		if(args[i].str == s)
			return i;
	}
	return 0; // return "All", which is first
}

Vector<DatabaseBrowser::Item>& DatabaseBrowser::Get(ColumnType t)
{
	for(int i = 0; i < TYPE_COUNT; i++) {
		if(order[i] == t)
			return items[i];
	}
	static Vector<DatabaseBrowser::Item> empty;
	return empty;
}

const Vector<DatabaseBrowser::Item>& DatabaseBrowser::Get(ColumnType t) const
{
	for(int i = 0; i < TYPE_COUNT; i++) {
		if(order[i] == t)
			return items[i];
	}
	ASSERT_(0, "Invalid ColumnType");
	return items[0];
}

bool DatabaseBrowser::IsFirstInOrder(ColumnType t) const { return order[0] == t; }

int DatabaseBrowser::GetColumnCursor(ColumnType t) const
{
	for(int i = 0; i < TYPE_COUNT; i++) {
		if(order[i] == t)
			return cursor[i];
	}
	return -1;
}

int DatabaseBrowser::GetColumnOrder(ColumnType t) const
{
	for(int i = 0; i < TYPE_COUNT; i++)
		if(order[i] == t)
			return i;
	return -1;
}

void DatabaseBrowser::RealizeUniqueAttrs()
{
	if(!uniq_attr.IsEmpty())
		return;

	auto& src = p.src->Data();
	uniq_attr_values.Clear();
	for(int i = 0; i < src.phrase_parts.GetCount(); i++) {
		const PhrasePart& pp = src.phrase_parts[i];
		if(pp.attr < 0)
			continue;
		const auto& ah = src.attrs.GetKey(pp.attr);
		uniq_attr.GetAdd(ah.group).GetAdd(ah.value, 0)++;
		uniq_attr_values.GetAdd(ah.value, 0)++;
	}
	SortByValue(uniq_attr, VMapSumSorter());
	for(auto& v : uniq_attr.GetValues())
		SortByValue(v, StdGreater<int>());
}

void DatabaseBrowser::RealizeUniqueActions()
{
	if(!uniq_acts.IsEmpty())
		return;

	auto& src = p.src->Data();
	uniq_act_args.Clear();
	for(int i = 0; i < src.phrase_parts.GetCount(); i++) {
		const PhrasePart& pp = src.phrase_parts[i];
		for(int act_i : pp.actions) {
			const auto& ah = src.actions.GetKey(act_i);
			uniq_acts.GetAdd(ah.action).GetAdd(ah.arg, 0)++;
			uniq_act_args.GetAdd(ah.arg, 0)++;
		}
	}
	SortByValue(uniq_acts, VMapSumSorter());
	for(auto& v : uniq_acts.GetValues())
		SortByValue(v, StdGreater<int>());
}

void DatabaseBrowser::SetInitialData()
{
	ASSERT(p.src);
	auto& src = p.src->Data();
	
	phrase_parts.SetCount(src.phrase_parts.GetCount());
	int i = 0;
	for(int& pp_i : phrase_parts)
		pp_i = i++;
}

void DatabaseBrowser::FilterNextFrom(ColumnType t)
{
	int cursor = GetColumnOrder(t);
	for(int i = cursor; i < (int)TYPE_COUNT; i++) {
		FilterData(order[i]);
	}
}

void DatabaseBrowser::FilterAll()
{
	for(int i = 0; i < (int)TYPE_COUNT; i++)
		FilterData(order[i]);
}

void DatabaseBrowser::SetColumnCursor(ColumnType t, int i)
{
	int c = GetColumnOrder(t);
	if(i >= 0 && i < items[c].GetCount()) {
		cursor[c] = i;
		history.GetAdd(GetHash(c)) = i;
	}
}

void DatabaseBrowser::FillItems(ColumnType t)
{
	ASSERT(p.src);
	auto& src = p.src->Data();
	
	int c = GetColumnOrder(t);
	if(c > 0)
		RemoveExcessData(c);

	auto& type_items = items[c];
	type_items.SetCount(1);

	Item& it = type_items[0];
	it.str = "All";
	it.count = phrase_parts.GetCount();
	it.idx = -1;
	
	
	// TODO get context actually
	LOG("DatabaseBrowser::FillItems: warning: TODO: get context actually");
	const ContextData& cd = src.ctxs[0];

	switch(t) {
	case ELEMENT: {
		VectorMap<int, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			if(pp.el_i >= 0)
				vmap.GetAdd(pp.el_i, 0)++;
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		for(int i = 0; i < vmap.GetCount(); i++) {
			int el_id = vmap.GetKey(i);
			Item& it = type_items[1 + i];
			it.str = src.element_keys[el_id];
			it.count = vmap[i];
			it.idx = el_id;
		}
		break;
	}
	case ATTR_GROUP: {
		VectorMap<String, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			if(pp.attr < 0)
				continue;
			const AttrHeader& ah = src.attrs.GetKey(pp.attr);
			vmap.GetAdd(ah.group, 0)++;
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		for(int i = 0; i < vmap.GetCount(); i++) {
			Item& it = type_items[1 + i];
			it.str = vmap.GetKey(i);
			it.count = vmap[i];
			it.idx = -1;
		}
		break;
	}
	case ATTR_VALUE: {
		VectorMap<String, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			if(pp.attr < 0)
				continue;
			const AttrHeader& ah = src.attrs.GetKey(pp.attr);
			vmap.GetAdd(ah.value, 0)++;
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		for(int i = 0; i < vmap.GetCount(); i++) {
			Item& it = type_items[1 + i];
			it.str = vmap.GetKey(i);
			it.count = vmap[i];
			it.idx = -1;
		}
		break;
	}
	case COLOR: {
		VectorMap<int, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			int clr_i = GetColorGroup(pp.clr);
			vmap.GetAdd(clr_i, 0)++;
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		for(int i = 0; i < vmap.GetCount(); i++) {
			Item& it = type_items[1 + i];
			int clr_i = vmap.GetKey(i);
			it.str = "#" + IntStr(clr_i);
			it.count = vmap[i];
			it.idx = clr_i;
		}
		break;
	}
	case ACTION: {
		VectorMap<String, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			for(int act_i : pp.actions) {
				const ActionHeader& ah = src.actions.GetKey(act_i);
				vmap.GetAdd(ah.action, 0)++;
			}
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		for(int i = 0; i < vmap.GetCount(); i++) {
			Item& it = type_items[1 + i];
			it.str = vmap.GetKey(i);
			it.count = vmap[i];
			it.idx = -1;
		}
		break;
	}
	case ACTION_ARG: {
		VectorMap<String, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			for(int act_i : pp.actions) {
				const ActionHeader& ah = src.actions.GetKey(act_i);
				vmap.GetAdd(ah.arg, 0)++;
			}
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		for(int i = 0; i < vmap.GetCount(); i++) {
			Item& it = type_items[1 + i];
			it.str = vmap.GetKey(i);
			it.count = vmap[i];
			it.idx = -1;
		}
		break;
	}
	case TYPECLASS: {
		VectorMap<int, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			for(int tc : pp.typecasts)
				vmap.GetAdd(tc, 0)++;
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		for(int i = 0; i < vmap.GetCount(); i++) {
			Item& it = type_items[1 + i];
			int tc = vmap.GetKey(i);
			it.str = cd.typeclasses[tc].name;
			it.count = vmap[i];
			it.idx = tc;
		}
		break;
	}
	case CONTENT: {
		VectorMap<int, int> vmap;
		for(int pp_i : phrase_parts) {
			const PhrasePart& pp = src.phrase_parts[pp_i];
			for(int con : pp.contrasts)
				vmap.GetAdd(con, 0)++;
		}
		SortByValue(vmap, StdGreater<int>());
		type_items.SetCount(1 + vmap.GetCount());
		const auto& cons = cd.contents;
		for(int i = 0; i < vmap.GetCount(); i++) {
			Item& it = type_items[1 + i];
			int tc = vmap.GetKey(i);
			int tc_i = tc / 3;
			int mod_i = tc % 3;
			if(tc_i < cons.GetCount()) {
				const auto& con = cons[tc_i];
				it.str = con.name + ": " + con.parts[mod_i];
			}
			else
				it.str = "<Error>";
			it.count = vmap[i];
			it.idx = tc;
		}
		break;
	}
	default:
		TODO break;
	}
}

void DatabaseBrowser::SetAttrGroup(int i)
{
	RealizeUniqueAttrs();

	auto& attr_groups = Get(ATTR_GROUP);
	if(IsFirstInOrder(ATTR_GROUP)) {
		SetInitialData();
	}
	else {
		FillItems(ATTR_GROUP);
	}
	SetColumnCursor(ATTR_GROUP, i);
}

void DatabaseBrowser::SetAttrValue(int i)
{
	RealizeUniqueAttrs();

	auto& attr_values = Get(ATTR_VALUE);
	if(IsFirstInOrder(ATTR_VALUE)) {
		SetInitialData();
	}
	else {
		FillItems(ATTR_VALUE);
	}
	SetColumnCursor(ATTR_VALUE, i);
}

void DatabaseBrowser::SetColor(int i)
{
	auto& colors = Get(COLOR);

	if(IsFirstInOrder(COLOR)) {
		/*colors.SetCount(1 + GetColorGroupCount());
		Item& a = colors[0];
		a.str = "All";
		a.idx = -1;
		a.count = da.phrase_parts.GetCount();
		for(int i = 0; i < GetColorGroupCount(); i++) {
		    Item& a = colors[1+i];
		    a.str = "#" + IntStr(i);
		    a.idx = i;
		    a.count = color_counts[i];
		}*/
		SetInitialData();
	}
	else {
		FillItems(COLOR);
	}
	SetColumnCursor(COLOR, i);
}

void DatabaseBrowser::SetAction(int i)
{
	auto& actions = Get(ACTION);
	int cur = GetColumnCursor(ACTION);

	RealizeUniqueActions();

	if(IsFirstInOrder(ACTION)) {
		SetInitialData();
	}
	else {
		FillItems(ACTION);
	}
	SetColumnCursor(ACTION, i);
}

void DatabaseBrowser::SetActionArg(int i)
{
	auto& args = Get(ACTION_ARG);
	int cur = GetColumnCursor(ACTION_ARG);

	RealizeUniqueActions();

	if(IsFirstInOrder(ACTION_ARG)) {
		SetInitialData();
	}
	else {
		FillItems(ACTION_ARG);
	}
	SetColumnCursor(ACTION_ARG, i);
}

void DatabaseBrowser::SetElement(int i)
{
	auto& elements = Get(ELEMENT);
	int cur = GetColumnCursor(ELEMENT);

	RealizeUniqueActions();

	if(IsFirstInOrder(ELEMENT)) {
		SetInitialData();
	}
	else {
		FillItems(ELEMENT);
	}
	SetColumnCursor(ELEMENT, i);
}

void DatabaseBrowser::SetTypeclass(int i)
{
	auto& tcs = Get(TYPECLASS);
	int cur = GetColumnCursor(TYPECLASS);

	RealizeUniqueActions();

	if(IsFirstInOrder(TYPECLASS)) {
		SetInitialData();
	}
	else {
		FillItems(TYPECLASS);
	}
	SetColumnCursor(TYPECLASS, i);
}

void DatabaseBrowser::SetContrast(int i)
{
	auto& cons = Get(CONTENT);
	int cur = GetColumnCursor(CONTENT);

	RealizeUniqueActions();

	if(IsFirstInOrder(CONTENT)) {
		SetInitialData();
	}
	else {
		FillItems(CONTENT);
	}
	SetColumnCursor(CONTENT, i);
}

void DatabaseBrowser::DataCursorTail(int cursor)
{
	for(int i = cursor; i < TYPE_COUNT; i++) {
		if(order[i] == INVALID)
			break;
		DataCursor(i);
	}
}

void DatabaseBrowser::SetCursor(int c, ColumnType type)
{
	switch(type) {
	case INVALID:
		break;
	case ELEMENT:
		SetElement(c);
		break;
	case ATTR_GROUP:
		SetAttrGroup(c);
		break;
	case ATTR_VALUE:
		SetAttrValue(c);
		break;
	case COLOR:
		SetColor(c);
		break;
	case ACTION:
		SetAction(c);
		break;
	case ACTION_ARG:
		SetActionArg(c);
		break;
	case TYPECLASS:
		SetTypeclass(c);
		break;
	case CONTENT:
		SetContrast(c);
		break;
	default:
		TODO break;
	}
}

void DatabaseBrowser::ResetCursor(int c, ColumnType type)
{
	SetInitialData();
	for(int i = 0; i < TYPE_COUNT; i++) {
		ColumnType t = order[i];
		if(t == INVALID)
			break;

		int& tgt = history.GetAdd(GetHash(i), 0);

		if(type != INVALID && t == type) {
			const auto& items = this->items[i];
			if(c >= 0 && c < items.GetCount()) {
				tgt = c;
				SetCursor(c, type);
			}
			else
				break;
		}
		else {
			FillItems(t);
			int& c = cursor[i];
			if(c < 0 || c >= items[i].GetCount()) {
				c = 0;
				tgt = 0;
			}
		}

		FilterData(t);
	}
}

void DatabaseBrowser::DataCursor(int i)
{
	int c = history.GetAdd(GetHash(i), 0);
	ColumnType type = order[i];
	SetCursor(c, type);
}

void DatabaseBrowser::FilterData(ColumnType t)
{
	auto& src = p.src->Data();
	int order_i = GetColumnOrder(t);
	auto& items = Get(t);

	int cur = GetColumnCursor(t);
	if(!cur) {
		if(order_i > 0)
			RemoveExcessData(order_i);
		return;
	}

	Item& filter = items[cur];
	int filter_idx = filter.idx;
	String filter_str = filter.str;

	rm_list.SetCount(0);

	int* iter = phrase_parts.Begin();
	for(int i = 0; i < phrase_parts.GetCount(); i++) {
		int pp_i = phrase_parts[i];
		const PhrasePart& pp = src.phrase_parts[pp_i];
		bool rem = false;

		switch(t) {
		case ELEMENT:
			rem = pp.el_i < 0 || pp.el_i != filter_idx;
			break;
		case ATTR_GROUP:
			rem = pp.attr < 0 || src.attrs.GetKey(pp.attr).group != filter_str;
			break;
		case ATTR_VALUE:
			rem = pp.attr < 0 || src.attrs.GetKey(pp.attr).value != filter_str;
			break;
		case COLOR: {
			int clr_i = GetColorGroup(pp.clr);
			rem = clr_i != filter_idx;
			break;
		}
		case ACTION: {
			rem = true;
			for(int act_i : pp.actions) {
				if(src.actions.GetKey(act_i).action == filter_str) {
					rem = false;
					break;
				}
			}
			break;
		}
		case ACTION_ARG: {
			rem = true;
			for(int act_i : pp.actions) {
				if(src.actions.GetKey(act_i).arg == filter_str) {
					rem = false;
					break;
				}
			}
			break;
		}
		case TYPECLASS: {
			rem = true;
			for(int tt : pp.typecasts) {
				if(tt == filter_idx) {
					rem = false;
					break;
				}
			}
			break;
		}
		case CONTENT: {
			rem = true;
			for(int tt : pp.contrasts) {
				if(tt == filter_idx) {
					rem = false;
					break;
				}
			}
			break;
		}
		default:
			TODO break;
		}

		if(rem)
			rm_list << i;
	}
	phrase_parts.Remove(rm_list);

	if(order_i > 0)
		RemoveExcessData(order_i);
}

void DatabaseBrowser::RemoveExcessData(int order_i)
{
	// Don't remove with the first category
	if(order_i <= 0)
		return;

	// Don't remove, if already category-limited
	int prev_cur = cursor[order_i - 1];
	if(prev_cur != 0)
		return;

	// Remove excess data
	if(secondary_category_limit > 0 && phrase_parts.GetCount() > secondary_category_limit)
		phrase_parts.SetCount(secondary_category_limit);
}

END_UPP_NAMESPACE
