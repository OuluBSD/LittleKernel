#include "Content.h"


NAMESPACE_UPP


TextComparison::TextComparison() {
	
}

void TextComparison::ReplaceWithWhite(WString& a) {
	for(int i = 0; i < a.GetCount(); i++) {
		int chr = a[i];
		if (chr >= 0x2000 && chr < 0x3000) {
			a = a.Left(i) + WString(" ") + a.Mid(i+1);
		}
	}
}

double TextComparison::Process(const String& a_, const String& b_) {
	double ret = 0;
	
	this->a = a_.ToWString();
	this->b = b_.ToWString();
	
	ReplaceWithWhite(a);
	ReplaceWithWhite(b);
	
	aw = Split(a, String(" ").ToWString());
	bw = Split(b, String(" ").ToWString());
	
	a_in_use.SetCount(0);
	b_in_use.SetCount(0);
	a_in_use.SetCount(aw.GetCount(), false);
	b_in_use.SetCount(bw.GetCount(), false);
	link_weights.Clear();
	
	for(int i = 0; i < aw.GetCount(); i++) {
		const WString& a = aw[i];
		for(int j = 0; j < bw.GetCount(); j++) {
			const WString& b = bw[j];
			double dist = JaroWinklerDistance(a, b);
			int key = i * 10000 + j;
			link_weights.Add(key, dist);
		}
	}
	SortByValue(link_weights, StdGreater<double>());
	
	int a_remaining = aw.GetCount();
	int b_remaining = bw.GetCount();
	map_ab.SetCount(0);
	map_ab.SetCount(aw.GetCount(), -1);
	map_ab_weights.SetCount(0);
	map_ab_weights.SetCount(aw.GetCount(), 0);
	int union_count = 0;
	while (a_remaining && b_remaining) {
		int best_link = -1;
		for(int i = 0; i < link_weights.GetCount(); i++) {
			double weight = link_weights[i];
			if (weight < 0.5)
				break;
			int key = link_weights.GetKey(i);
			int ai = key / 10000;
			int bi = key % 10000;
			if (!a_in_use[ai] && !b_in_use[bi]) {
				best_link = i;
				break;
			}
		}
		if (best_link < 0)
			break;
		double weight = link_weights[best_link];
		int key = link_weights.GetKey(best_link);
		int ai = key / 10000;
		int bi = key % 10000;
		a_in_use[ai] = true;
		b_in_use[bi] = true;
		map_ab[ai] = bi;
		map_ab_weights[ai] = weight;
		a_remaining--;
		b_remaining--;
		union_count++;
	}
	
	unlinked_a.Clear();
	unlinked_b.Clear();
	for(int i = 0; i < a_in_use.GetCount(); i++)
		if (!a_in_use[i])
			unlinked_a << i;
	for(int i = 0; i < b_in_use.GetCount(); i++)
		if (!b_in_use[i])
			unlinked_b << i;
	
	// Use modified Jaccard similarity
	// Use weights instead of just 0/1 values
	double union_sum = 0;
	for (double d: map_ab_weights)
		union_sum += d; // just add meaningless 0-values too
	double div = max(0.0, aw.GetCount() + bw.GetCount() - union_sum);
	ASSERT(div >= union_sum);
	if (div <= 0)
		ret = 0;
	else
		ret = union_sum / div;
	
	if (0) {
		if (0) {
			DUMPC(map_ab);
			DUMPC(unlinked_a);
			DUMPC(unlinked_b);
		}
		for(int i = 0; i < map_ab.GetCount(); i++) {
			int j = map_ab[i];
			if (j < 0) continue;
			const WString& a = aw[i];
			const WString& b = bw[j];
			LOG("Link " << i << ": " << a << " --> " << b << ": " << map_ab_weights[i]);
		}
		for(int i = 0; i < unlinked_a.GetCount(); i++) {
			LOG("unlinked A: " << aw[unlinked_a[i]]);
		}
		for(int i = 0; i < unlinked_b.GetCount(); i++) {
			LOG("unlinked B: " << bw[unlinked_b[i]]);
		}
		LOG("similarity: " << ret);
	}
	
	
	return ret;
}

double TextComparison::JaroWinklerDistance(const WString& s, const WString& a) {
    int i, j, l;
    int m = 0, t = 0;
    int sl = s.GetCount();
    int al = a.GetCount();
    sflags.SetCount(0);
    aflags.SetCount(0);
    sflags.SetCount(sl, 0);
    aflags.SetCount(al, 0);
    int range = max(0, max(sl, al) / 2 - 1);
    double dw;

    if (!sl || !al)
        return 0.0;

    for (i = 0; i < al; i++)
        aflags[i] = 0;

    for (i = 0; i < sl; i++)
        sflags[i] = 0;

    /* calculate matching characters */
    for (i = 0; i < al; i++) {
        for (j = max(i - range, 0), l = min(i + range + 1, sl); j < l; j++) {
            if (a[i] == s[j] && !sflags[j]) {
                sflags[j] = 1;
                aflags[i] = 1;
                m++;
                break;
            }
        }
    }

    if (!m)
        return 0.0;

    /* calculate character transpositions */
    l = 0;
    for (i = 0; i < al; i++) {
        if (aflags[i] == 1) {
            for (j = l; j < sl; j++) {
                if (sflags[j] == 1) {
                    l = j + 1;
                    break;
                }
            }
            if (a[i] != s[j])
                t++;
        }
    }
    t /= 2;

    /* Jaro distance */
    dw = (((double)m / sl) + ((double)m / al) + ((double)(m - t) / m)) / 3.0;

    /* calculate common string prefix up to 4 chars */
    l = 0;
    for (i = 0; i < min(min(sl, al), 4); i++)
        if (s[i] == a[i])
            l++;

    /* Jaro-Winkler distance */
    dw = dw + (l * SCALING_FACTOR * (1 - dw));

    return dw;
}


bool SoftMatchString::operator==(const SoftMatchString& s) const {
	thread_local static TextComparison cmp;
	if (lines.GetCount() != s.lines.GetCount())
		return false;
	double weight_sum = 0;
	for(int i = 0; i < s.lines.GetCount(); i++) {
		const String& a = lines[i];
		const String& b = s.lines[i];
		double weight = cmp.Process(a, b);
		weight_sum += weight;
	}
	double av = weight_sum / lines.GetCount();
	return av;
}

hash_t SoftMatchString::GetHashValue() const {
	return lines.GetHashValue();
}

void TryStrDistSectionSolverBase::MakeLines() {
	String s = input;
	s.Replace("\r", "");
	Vector<String> txt_lines = Split(s, "\n");
	lines.SetCount(0);
	sections.SetCount(0);
	LOG("TryStrDistSectionSolverBase::MakeLines: warning: empty line!");
	if (input.IsEmpty())
		return;
	
	VectorMap<hash_t, TextDescriptor::Item> items;
	
	for(int i = 0; i < txt_lines.GetCount(); i++) {
		String& l = txt_lines[i];
		l = TrimBoth(l);
		if (l.IsEmpty())
			continue;
		Line& line = lines.Add();
		line.txt = l;
		TextDescriptor& td = line.descriptor;
		td.Zero();
		
		items.Clear();
		
		WString ws = l.ToWString();
		ws = ToLower(ws);
		
		// Clear common special characters for better hash matching
		ws.Replace(","," ");
		ws.Replace("."," ");
		ws.Replace("!"," ");
		ws.Replace("?"," ");
		ws.Replace("-"," ");
		ws.Replace(""," ");
		ws.Replace(":"," ");
		ws.Replace(";"," ");
		ws.Replace("\""," ");
		ws.Replace("'"," ");
		ws.Replace("("," ");
		ws.Replace(")"," ");
		ws.Replace("["," ");
		ws.Replace("]"," ");
		ws.Replace("/"," ");
		ws.Replace("&"," ");
		ws.Replace("&"," ");
		ws.Replace("+"," ");
		ws.Replace("    "," ");
		ws.Replace("   "," ");
		ws.Replace("  "," ");
		ws = TrimBoth(ws.ToString()).ToWString();
		//LOG(ws);
		
		for(int j = 1; j < ws.GetCount(); j++) {
			int chr0 = ws[j-1];
			int chr1 = ws[j];
			CombineHash ch;
			ch.Do(chr0).Do(chr1);
			hash_t hash = ch;
			int k = items.Find(hash);
			if (k < 0) {
				k = items.GetCount();
				TextDescriptor::Item& it = items.Add(hash);
				it.from = chr0;
				it.to = chr1;
				it.count = 0;
			}
			TextDescriptor::Item& it = items[k];
			it.count++;
		}
		
		SortByValue(items, TextDescriptor::Item());
		int count = min(TextDescriptor::ITEM_COUNT, items.GetCount());
		
		bool print_dbg = false;
		
		if (print_dbg) debug_out << "Line #" << i << ": " << l;
		for(int j = 0; j < count; j++) {
			const TextDescriptor::Item& from = items[j];
			TextDescriptor::Item& to = td.items[j];
			to.from = from.from;
			to.to = from.to;
			to.count = from.count;
			if (print_dbg) debug_out << j << ": " << (int)to.from << " -> " << (int)to.to << ": " << (int)to.count << "\n";
		}
		if (print_dbg) debug_out << "\n";
		
		//line.hash = td.GetHash(section_cmp_header_len);
	}
}

void TryStrDistSectionSolverBase::MakeUniqueLines() {
	uniq_lines.Clear();
	
	for(int i = 0; i < lines.GetCount(); i++) {
		Line& line = lines[i];
		UniqueLine& u = GetAddUniqueLine(line.txt);
		if (u.count == 0)
			u.txt = line.txt;
		u.count++;
		u.lines << i;
	}
	Sort(uniq_lines, UniqueLine());
	
	debug_out << "uniq_lines:\n";
	for(int i = 0; i < uniq_lines.GetCount(); i++) {
		debug_out << Format("%d: [%X]: %s\n", i, (int64)uniq_lines[i].txt.GetHashValue(), uniq_lines[i].ToString());
	}
}

void TryStrDistSectionSolverBase::DefaultProcess() {
	double best_sect_weight = 0;
	double best_sect_limit = 0;
	double last_d = 0;
	if (!force_limit) {
		for (double d = 0.8; d >= 0.099; d -= 0.1) {
			iter_r_limit = d;
			last_d = d;
			SingleIteration();
			
			double weight = 0;
			if (1) {
				for (auto& sect : sections)
					weight += max(0, sect.count - 1) * sect.lines.GetCount();
			}
			else if (1) {
				for (auto& sect : sections)
					weight += max(0, sect.count - 1);
			}
			else if (1) {
				for (auto& sect : sections)
					weight += max(0, sect.count - 1);
				double percentage = weight / (double)lines.GetCount();
				
				// when weight is over 75% of total lyrics, assume failure
				if (percentage >= 0.75)
					weight = 0;
			}
			else {
				for (auto& sect : sections) {
					if (sect.lines.GetCount())
						weight += pow(sect.count * sect.lines.GetCount(), 2);
					else
						weight += pow(sect.count * sect.orig_weight, 2);
				}
			}
			if (weight > best_sect_weight) {
				best_sect_weight = weight;
				best_sect_limit = d;
			}
		}
	}
	else {
		last_d = DBL_MAX;
		best_sect_limit = forced_limit_value;
	}
	
	if (best_sect_limit == 0) best_sect_limit = 0.8;
	
	if (last_d != best_sect_limit) {
		iter_r_limit = best_sect_limit;
		last_d = best_sect_limit;
		SingleIteration();
	}
}

void TryStrDistSectionSolverBase::MakeSingleLineSections() {
	int sect_i = -1;
	for(int i = 0; i < lines.GetCount(); i++) {
		Line& line = lines[i];
		if (line.section < 0) {
			double repeat_sum = line.GetRepeatabilitySum();
			if (sect_i >= 0 &&
				((repeat_sum > sections[sect_i].repeat + repeatability_range) ||
				(repeat_sum < sections[sect_i].repeat - repeatability_range)))
				sect_i = -1;
			if (sect_i < 0) {
				sect_i = sections.GetCount();
				Section& sect = sections.Add();
				sect.orig_count = 1;
				sect.count = 1;
				sect.repeat = repeat_sum;
				sect.first_line = i;
			}
			line.section = sect_i;
			sections[sect_i].lines << line.txt;
		}
		else sect_i = -1;
	}
}

void TryStrDistSectionSolverBase::MakeMetaSections() {
	TextComparison tc;
	
	meta_sections.Clear();
	
	TextPartType prev_type = TXT_INVALID;
	int prev_section = -1;
	int meta_sect = -1;
	int type_counts[TXT_COUNT];
	memset(type_counts, 0, sizeof(type_counts));
	
	for(int i = 0; i < lines.GetCount(); i++) {
		const Line& line = lines[i];
		const Line* next_line = i+1 < lines.GetCount() ? &lines[i+1] : 0;
		
		if (line.section != prev_section) {
			Section& sect = sections[line.section];
			Section* next_sect = next_line ? &sections[next_line->section] : 0;
			
			TextPartType type = TXT_INVALID;
			if (sect.flag_repeating || sect.repeat > 0.666) {
				type = TXT_REPEAT;
			}
			else if (prev_type == TXT_REPEAT && next_sect && (next_sect->flag_repeating || next_sect->repeat > 0.666)) {
				type = TXT_REPEAT;
			}
			else if (sect.repeat > 0.2 && prev_type == TXT_NORMAL) {
				type = TXT_PRE_REPEAT;
			}
			else {
				type = TXT_NORMAL;
			}
			
			if (sect.meta_section < 0) {
				bool skip_msect = false;
				if (sect.lines.GetCount() <= 1 && prev_type >= 0 && meta_sect >= 0) {
					if (next_sect && next_sect->lines.GetCount() > 1 && next_sect->repeat < 0.2)
						skip_msect = true;
					else if (!next_sect)
						skip_msect = true;
				}
				
				if (skip_msect) {
					type = prev_type; // reset
				}
				else if (prev_type != type) {
					meta_sect = meta_sections.GetCount();
					MetaSection& ms = meta_sections.Add();
					ms.type = type;
					ms.num = type_counts[type]++;
					ms.count = 1;
				}
			}
			else {
				meta_sections[sect.meta_section].count++;
				meta_sect = sect.meta_section;
			}
			
			VectorFindAdd(meta_sections[meta_sect].sections, line.section);
			
			sect.meta_section = meta_sect;
			
			prev_type = type;
		}
		ASSERT(meta_sect >= 0);
		
		prev_section = line.section;
	}
	
	
	// Simple check for twist (must have repeat after it)
	bool matching_len = true;
	int normal_len = 0;
	int normal_count = 0;
	int last_repeat = -1;
	Vector<int> potential_twists;
	const double same_diff_limit = 0.25;
	const double twist_diff_limit = 0.33;
	for(int i = 0; i < meta_sections.GetCount(); i++) {
		MetaSection& ms = meta_sections[i];
		if (ms.type == TXT_NORMAL) {
			normal_count++;
			if (ms.num == 0)
				normal_len = GetMetaSectionLen(i);
			else if (ms.num == 1) {
				double diff = fabs((double)normal_len / GetMetaSectionLen(i) - 1.0);
				if (diff > same_diff_limit)
					matching_len = false;
			}
			else if (ms.num > 1)
				potential_twists << i;
		}
		else if (ms.type == TXT_REPEAT)
			last_repeat = i;
	}
	
	// Assume that different lengths of "normal" parts are twists (=bridges)
	if (matching_len) {
		for (int potential_twist_i : potential_twists) {
			MetaSection& ms = meta_sections[potential_twist_i];
			double diff = fabs((double)normal_len / GetMetaSectionLen(potential_twist_i) - 1.0);
			if (diff > twist_diff_limit) {
				ms.type = TXT_TWIST;
				ms.num = type_counts[ms.type]++;
			}
		}
	}
	// Just assume that last normal is twist
	else {
		if (normal_count > 2) {
			int normal_i = 0;
			for(int i = 0; i < meta_sections.GetCount(); i++) {
				MetaSection& ms = meta_sections[i];
				if (ms.type == TXT_NORMAL) {
					if (normal_i == normal_count - 1)
						ms.type = TXT_TWIST;
					normal_i++;
				}
			}
		}
	}
}

int TryStrDistSectionSolverBase::GetMetaSectionLen(int ms_i) const {
	const MetaSection& ms = meta_sections[ms_i];
	int len = 0;
	for(int i = 0; i < ms.sections.GetCount(); i++)
		len += sections[ms.sections[i]].lines.GetCount();
	return len;
}

String TryStrDistSectionSolverBase::GetDebugLines() const {
	String s;
	for(int i = 0; i < uniq_lines.GetCount(); i++) {
		const UniqueLine& ul = uniq_lines[i];
		s << ul.count << ":\t" << ul.txt  << "\n";
	}
	s << "\n\n";
	for(int i = 0; i < lines.GetCount(); i++) {
		const Line& l = lines[i];
		s << l.txt.Left(12) + "... r=" << l.repeatability << ", cr=" << l.circumstacial_repeatability << "\n";
	}
	return s;
}

String TryStrDistSectionSolverBase::GetDebugSections() const {
	String s;
	for(int i = 0; i < sections.GetCount(); i++) {
		const Section& sect = sections[i];
		s << "[section " << i << ": " << sect.orig_count << "]\n";
		for(int j = 0; j < sect.lines.GetCount(); j++) {
			const String& line = sect.lines[j];
			s << line << "\n";
		}
		s << "\n";
	}
	return s;
}

String TryStrDistSectionSolverBase::GetResult() const {
	String s;
	int prev_msect = -1;
	int prev_sect = -1;
	int sect_line_i = -1;
	bool have_sub_sect = false;
	for(int i = 0; i < lines.GetCount(); i++) {
		const Line& line = lines[i];
		if (line.section != prev_sect) {
			const Section& sect = sections[line.section];
			if (sect.meta_section != prev_msect) {
				const MetaSection& ms = meta_sections[sect.meta_section];
				if (s.GetCount()) s << "\n";
				s << Format("[meta-section %d: %s, count %d]\n", sect.meta_section, GetTextTypeString(ms.type) + " " + IntStr(ms.num+1), ms.count);
				prev_msect = sect.meta_section;
			}
			else if (s.GetCount()) s << "\n";
			s << Format("\t[section %d.%d: count %d, repeat %.2!m %s]\n", sect.meta_section, line.section, sect.count, sect.repeat, sect.flag_repeating ? ", flag-repeating" : "");
			
			// Sub-section
			sect_line_i = 0;
			int peek_len = 1;
			for(int j = i+1; j < lines.GetCount(); j++) {
				if (lines[j].section != line.section) break;
				peek_len++;
			}
			have_sub_sect = peek_len >= 6;
		}
		// Sub-section
		if (have_sub_sect && sect_line_i % 4 == 0)
			s << Format("\t\t[sub-section %d.%d.%d]\n", prev_msect, line.section, sect_line_i / 4);
		if (have_sub_sect)
			s.Cat('\t');
		
		s << "\t\t" << line.txt << "\n";
		prev_sect = line.section;
		
		sect_line_i++;
	}
	return s;
}

String TryStrDistSectionSolverBase::GetDebugHashes() const {
	String s;
	for(int i = 0; i < lines.GetCount(); i++) {
		if (i) s << "\n";
		s << lines[i].txt.Left(6) + ": ";
		hash_t h = lines[i].descriptor.GetHash(5);
		s << IntStr64(h);
	}
	return s;
}

String TryStrDistSectionSolverBase::FindLine(hash_t h) const {
	for(int i = 0; i < lines.GetCount(); i++) {
		if (lines[i].descriptor.GetHash(section_cmp_header_len) == h)
			return lines[i].txt;
	}
	return "<line not found, hash " + IntStr64(h) + ">";
}

TryStrDistSectionSolverBase::UniqueLine& TryStrDistSectionSolverBase::GetAddUniqueLine(const String& s) {
	ASSERT(s.GetCount());
	TextComparison cmp;
	int closest_i = -1;
	double closest_dist = 0;
	for(int i = 0; i < uniq_lines.GetCount(); i++) {
		UniqueLine& ul = uniq_lines[i];
		double dist = cmp.Process(s, ul.txt);
		if (dist >= unique_dist_limit && dist > closest_dist) {
			closest_i = i;
			closest_dist = dist;
		}
	}
	if (closest_i >= 0) {
		UniqueLine& ul = uniq_lines[closest_i];
		return ul;
	}
	UniqueLine& ul = uniq_lines.Add();
	ul.txt = TrimBoth(s);
	return ul;
}

Vector<int>& TryStrDistSectionSolverBase::GetAddHashRange(const SoftMatchString& s) {
	for(int i = 0; i < hash_ranges.GetCount(); i++) {
		if (hash_ranges[i].k == s)
			return hash_ranges[i].v;
	}
	auto& v = hash_ranges.Add();
	v.k = s;
	return v.v;
}

void TryNo5tStructureSolver::MakeSections() {
	
	int max_lines = uniq_lines[0].count;
	
	for(int i = 0; i < lines.GetCount(); i++) {
		Line& line = lines[i];
		UniqueLine& ul = GetAddUniqueLine(line.txt);
		line.repeatability = max_lines > 0 ? (double)(ul.count - 1) / (double)(max_lines - 1) : 0;
	}
	for(int i = 1; i < lines.GetCount()-1; i++) {
		Line& line0 = lines[i-1];
		Line& line1 = lines[i];
		Line& line2 = lines[i+1];
		if (line1.repeatability < cr_limit && line0.repeatability > iter_r_limit && line2.repeatability > iter_r_limit) {
			line1.circumstacial_repeatability = (line0.repeatability + line2.repeatability) * 0.5;
		}
	}
	
	sections.Clear();
	
	struct RangeHash : Moveable<RangeHash> {
		SoftMatchString txt;
		int begin, end, len;
		Vector<int> sections;
		bool operator()(const RangeHash& a, const RangeHash& b) const {return (a.sections.GetCount()*a.len) > (b.sections.GetCount()*b.len);}
		String ToString() const {
			return Format("%d-%d (%d) %X: count %d", begin, end, len, (int)txt.lines.GetHashValue(), sections.GetCount());
		}
	};
	
	struct TmpSection : Moveable<TmpSection> {
		//Vector<hash_t> hashes;
		int begin, end, len;
		double r = 0, cr = 0;
		Vector<int> ranges;
		String ToString() const {
			return Format("%d-%d (%d) r=%f,cr=%f: count %d", begin, end, len, r, cr, ranges.GetCount());
		}
	};
	Vector<TmpSection> tmp;
	
	TmpSection* ts = 0;
	for(int i = 0; i < lines.GetCount(); i++) {
		Line& line = lines[i];
		if (line.repeatability > iter_r_limit || line.circumstacial_repeatability > iter_r_limit) {
			if (!ts) {
				ts = &tmp.Add();
				ts->begin = i;
				ts->end = -1;
				ts->len = 1;
			}
			ts->r += line.repeatability;
			ts->cr += line.circumstacial_repeatability;
		}
		else if (ts) {
			ts->end = i;
			ts->len = ts->end - ts->begin;
			ts->r /= ts->len;
			ts->cr /= ts->len;
			ts = 0;
		}
	}
	if (ts) {
		ts->end = lines.GetCount();
		ts->len = ts->end - ts->begin;
		ts->r /= ts->len;
		ts->cr /= ts->len;
	}
	
	// Make potential ranges
	VectorMap<int,RangeHash> ranges;
	int ts_i = 0;
	for (TmpSection& ts : tmp) {
		for(int i = ts.begin; i < ts.end; i++) {
			Vector<String> range_lines;
			for(int j = i+1; j < ts.end; j++) {
				Line& line = lines[j-1];
				range_lines << line.txt;
				
				int key = i * 1000 + j;
				int range_i = ranges.Find(key);
				if (range_i < 0) {
					range_i = ranges.GetCount();
					RangeHash& rh = ranges.Add(key);
					rh.begin = i;
					rh.end = j;
					rh.len = rh.end - rh.begin;
					rh.txt = range_lines;
				}
				
				RangeHash& rh = ranges[range_i];
				ASSERT(rh.begin == i && rh.end == j);
				ASSERT(rh.txt.lines.GetCount() == range_lines.GetCount());
				rh.sections << ts_i;
				ts.ranges << range_i;
			}
		}
		ts_i++;
	}
	SortByValue(ranges, RangeHash());
	
	debug_out << "tmp:\n";
	for(int i = 0; i < tmp.GetCount(); i++) {
		auto& t = tmp[i];
		debug_out << "[" << i << "]: " << t.ToString() << "\n";
	}
	debug_out << "\n";
	debug_out << "ranges:\n";
	for(int i = 0; i < ranges.GetCount(); i++) {
		auto& r = ranges[i];
		debug_out << "[" << i << "]: (" << ranges.GetKey(i) << "): " << r.ToString() << "\n";
	}
	debug_out << "\n";
	
	
	// Find most important ranges
	hash_ranges.Clear();
	for(int i = 0; i < ranges.GetCount(); i++) {
		RangeHash& rh = ranges[i];
		GetAddHashRange(rh.txt).Add(i);
	}
	struct VecSort {
		VectorMap<int,RangeHash>& ranges;
		bool operator()(const HashRangeLink& a, const HashRangeLink& b) const {
			RangeHash& ra = ranges[a.v[0]];
			RangeHash& rb = ranges[b.v[0]];
			int weight_a = ra.len * a.v.GetCount();
			int weight_b = rb.len * b.v.GetCount();
			return weight_a > weight_b;
		}
	};
	Sort(hash_ranges, VecSort{ranges});
	debug_out << "hash_ranges:\n";
	for(int i = 0; i < hash_ranges.GetCount(); i++) {
		debug_out << Format("%d: (%X) [", i, (int64)hash_ranges[i].k.lines.GetHashValue());
		const auto& v = hash_ranges[i].v;
		for(int j = 0; j < v.GetCount(); j++) {
			if (j) debug_out << ", ";
			debug_out << v[j];
		}
		debug_out << "]\n";
	}
	
	// Make sections based on non-overlapping important ranges
	for(int i = 0; i < hash_ranges.GetCount(); i++) {
		auto& rv = hash_ranges[i].v;
		
		bool fail = false;
		for(int j = 0; j < rv.GetCount(); j++) {
			RangeHash& rh = ranges[rv[j]];
			for (int ts_i : rh.sections) {
				TmpSection& ts = tmp[ts_i];
				for(int k = ts.begin; k < ts.end; k++) {
					Line& line = lines[k];
					if (line.section >= 0) {
						fail = true;
						break;
					}
				}
				if (fail) break;
			}
			if (fail) break;
		}
		if (fail)
			continue;
		
		if (rv.GetCount() <= 1)
			continue;
		
		#if 0
		int max_len = 0;
		for (const auto& r : rv)
			max_len = max(max_len, ranges[r].len);
		
		bool sub_ranges = max_len >= 6;
		
		// Without sub-ranges
		if (!sub_ranges)
		#endif
		{
			int sect_i = sections.GetCount();
			Section& sect = sections.Add();
			sect.repeat = 1.0;
			sect.flag_repeating = true;
			sect.count = 0;
			int max_range = 0;
			for(int j = 0; j < rv.GetCount(); j++) {
				RangeHash& rh = ranges[rv[j]];
				for (int ts_i : rh.sections) {
					TmpSection& ts = tmp[ts_i];
					bool add_lines = sect.first_line < 0;
					if (add_lines)
						sect.first_line = ts.begin;
					for(int k = ts.begin; k < ts.end; k++) {
						Line& line = lines[k];
						line.section = sect_i;
						if (add_lines)
							sect.lines << line.txt;
					}
					max_range = max(max_range, ts.end - ts.begin);
					if (sect.first_line < 0)
						sect.first_line = ts.begin;
				}
			}
			
			// Count parts
			int prev_sect = -1;
			for (const Line& l : lines) {
				if (l.section == sect_i && prev_sect != sect_i)
					sect.count++;
				prev_sect = l.section;
			}
			
			sect.orig_weight = max_range;
		}
		#if 0
		// With sub-ranges
		else {
			int range_i = 0;
			while (true) {
				int begin = range_i * 4;
				int end = begin + 4;
				bool any_overlap = false;
				for(int j = 0; j < rv.GetCount(); j++) {
					RangeHash& rh = ranges[rv[j]];
					int b = 0, e = rh.len;
					if ((b <  begin && e > end) ||
						(b >= begin && b < end) ||
						(e >= begin && e < end)) {
						any_overlap = true;
						break;
					}
				}
				if (!any_overlap)
					break;
				
				int sect_i = sections.GetCount();
				Section& sect = sections.Add();
				sect.repeat = 1.0;
				sect.flag_repeating = true;
				sect.count = 0;
				int max_range = 0;
				for(int j = 0; j < rv.GetCount(); j++) {
					RangeHash& rh = ranges[rv[j]];
					int b = 0, e = rh.len;
					bool overlap = false;
					if ((b <  begin && e > end) ||
						(b >= begin && b < end) ||
						(e >= begin && e < end)) {
						overlap = true;
					}
					if (!overlap)
						continue;
					
					for (int ts_i : rh.sections) {
						TmpSection& ts = tmp[ts_i];
						bool add_lines = sect.first_line < 0;
						if (add_lines)
							sect.first_line = ts.begin + begin;
						for(int k = ts.begin + begin; k < ts.begin + end; k++) {
							Line& line = lines[k];
							line.section = sect_i;
							if (add_lines)
								sect.lines << line.txt;
						}
						max_range = max(max_range, min(ts.begin + end, ts.end) - (ts.begin + begin));
						if (sect.first_line < 0)
							sect.first_line = ts.begin + begin;
					}
				}
				
				// Count parts
				int prev_sect = -1;
				for (const Line& l : lines) {
					if (l.section == sect_i && prev_sect != sect_i)
						sect.count++;
					prev_sect = l.section;
				}
				
				sect.orig_weight = max_range;
				
				range_i++;
			}
		}
		#endif
	}
	
	
}

void TryNo5tStructureSolver::MakeRepeatingSectionLines() {
	
}




ScriptStructureSolverBase::ScriptStructureSolverBase() {
	
}

void ScriptStructureSolverBase::Process(String s) {
	input = s;
		
	DefaultProcess();
}

void ScriptStructureSolverBase::SingleIteration() {
	debug_out.Clear();
	MakeLines();
	MakeUniqueLines();
	MakeSections();
	MakeRepeatingSectionLines();
	MakeSingleLineSections();
	MakeMetaSections();
}




hash_t TextDescriptor::GetHash(int c) const {
	CombineHash ch;
	c = max(0,min(c, ITEM_COUNT));
	for(int i = 0; i < c; i++) {
		const TextDescriptor::Item& it = items[i];
		//if (i >= 3 && it.count <= 1) break;
		if (it.count < 1) break;
		ch.Do(it.from);
		ch.Do(it.to);
	}
	return ch;
}


END_UPP_NAMESPACE
