#include "Content.h"


NAMESPACE_UPP



void RemoveSinger(String& s) {
	int a = s.Find(" by the singer");
	if (a >= 0)
		s = s.Left(a);
}


// TODO rename to GetLyricTypeString
String GetTextTypeString(int i) {
	switch (i) {
		case TXT_NULL:			return "Null";
		case TXT_NORMAL:		return "Verse";
		case TXT_PRE_REPEAT:	return "Pre-Chorus";
		case TXT_REPEAT:		return "Chorus";
		case TXT_TWIST:			return "Bridge";
		default:				return "<error>";
	}
}

void ParseTextPartType(String part_name, TextPartType& text_type, int& text_num) {
	part_name = TrimBoth(ToLower(part_name));
	for(int i = 0; i < TXT_COUNT; i++) {
		String cmp = ToLower(GetTextTypeString(i));
		if (part_name.Left(cmp.GetCount()) == cmp) {
			text_type = (TextPartType)i;
			if (part_name.GetCount() == cmp.GetCount()) {
				text_num = 0;
			}
			else {
				String tail = TrimLeft(part_name.Mid(cmp.GetCount()));
				if (tail.GetCount() && IsDigit(tail[0]))
					text_num = ScanInt(tail)-1;
			}
			return;
		}
	}
	text_type = TXT_NULL;
}


void LineElement::Overlay(const LineElement& le) {
	if (!le.element.IsEmpty()) element = le.element;
	if (!le.attr.group.IsEmpty()) attr.group = le.attr.group;
	if (!le.attr.value.IsEmpty()) attr.value = le.attr.value;
	if (!le.act.action.IsEmpty()) act.action = le.act.action;
	if (!le.act.arg.IsEmpty()) act.arg = le.act.arg;
	if (le.clr_i >= 0) clr_i = le.clr_i;
	if (le.typeclass_i >= 0) typeclass_i = le.typeclass_i;
	if (le.con_i >= 0) con_i = le.con_i;
}


String DynPart::GetName() const {
	String s = GetTextTypeString(text_type);
	if (text_num >= 0)
		s += " " + IntStr(text_num+1);
	return s;
}

int DynPart::GetExpectedLineCount() const {
	int line_count = 0;
	for (const auto& p : sub) {
		for (const auto& l : p.lines)
			if (l.text.GetCount())
				line_count++;
	}
	return line_count;
}

int DynPart::GetContrastIndex() const {
	int idx = PART_COUNT-1;
	
	if (text_type == TXT_NORMAL)
		idx = PART_BEGIN;
	
	if (text_type == TXT_PRE_REPEAT)
		idx = PART_MID;
	
	if (text_type == TXT_REPEAT)
		idx = PART_MID;
	
	if (text_type == TXT_TWIST)
		idx = PART_END;
	
	return idx;
}

String LyricalStructure::GetStructText(bool user_text) const {
	String s;
	int line_i = 0;
	for (const DynPart& p: parts) {
		s << "[" << p.GetName() << "]\n";
		for(int i = 0; i < p.sub.GetCount(); i++) {
			const DynSub& ds = p.sub[i];
			for(int j = 0; j < ds.lines.GetCount(); j++) {
				const DynLine& dl = ds.lines[j];
				if (!user_text)
					s << dl.text << "\n";
				else
					s << dl.user_text << "\n";
			}
		}
		s << "\n";
	}
	return s;
}

String DynPart::GetLineElementString(int line) const {
	String s;
	int line_i = 0;
	for(int i = 0; i < sub.GetCount(); i++) {
		const DynSub& ds = sub[i];
		for(int j = 0; j < ds.lines.GetCount(); j++) {
			const DynLine& dl = ds.lines[j];
			if (line == line_i) {
				return ds.el.element;
			}
			line_i++;
		}
	}
	return s;
}





String LineScore::Get(int i, int j) const {
	ASSERT(line_n > 0);
	ASSERT(j >= 0 && j < line_n);
	int pos = i * line_n + j;
	return lines[pos];
}

int LineScore::GetScore(int i, int j) const {
	ASSERT(i >= 0 && i < GetCount());
	ASSERT(j >= 0 && j < SCORE_COUNT);
	int pos = i * SCORE_COUNT + j;
	return scores[pos];
}

void LineScore::SetCount(int i, int line_n) {
	this->line_n = line_n;
	int total = i * line_n;
	int score_total = i * SCORE_COUNT;
	lines.SetCount(total);
	scores.SetCount(score_total, 0);
}

void LineScore::Set(int i, int j, const String& s) {
	ASSERT(line_n > 0);
	ASSERT(j >= 0 && j < line_n);
	int pos = i * line_n + j;
	lines[pos] = s;
}

void LineScore::SetScore(int i, int j, int value) {
	ASSERT(i >= 0 && i < GetCount());
	ASSERT(j >= 0 && j < SCORE_COUNT);
	int pos = i * SCORE_COUNT + j;
	scores[pos] = value;
}

int LineScore::GetCount() const {
	if (line_n == 0 || lines.GetCount() == 0)
		return 0;
	return lines.GetCount() / line_n;
}










Script::~Script() {
	
}

String Lyrics::GetText() const {
	if (__text.GetCount())
		return __text;
	DatasetPtrs p;
	GetDataset(p);
	if (!p.lyric_struct)
		return String();
	return p.lyric_struct->GetStructText(0);
}

/*String Script::GetTextStructure(bool coarse) const {
	String out;
	for(const DynPart& dp : parts) {
		if (dp.text_type == TextPartType::TXT_NULL)
			continue;
		out << "[" << dp.GetName();
		if (!dp.person.IsEmpty())
			out << ": " << dp.person;
		out << "]\n";
		for (const DynSub& ds : dp.sub) {
			out.Cat('\t',1);
			out << "[" << ds.el.element << "]\n";
			for (const DynLine& dl : ds.lines) {
				out.Cat('\t',2);
				out << dl.text << "\n";
			}
		}
		out << "\n\n";
	}
	return out;
}*/

/*int Script::GetFirstPartPosition() const {
	#if 0
	for(int i = 0; i < active_struct.parts.GetCount(); i++) {
		String type = active_struct.parts[i];
		for(int j = 0; j < parts.GetCount(); j++) {
			if (parts[j].part_type != StaticPart::SKIP &&
				parts[j].type == type)
				return j;
		}
	}
	#endif
	return -1;
}*/

DynPart* LyricalStructure::FindPartByName(const String& name) {
	String lname = ToLower(name);
	RemoveSinger(lname);
	for (DynPart& sp : parts)
		if (ToLower(sp.GetName()) == lname)
			return &sp;
	return 0;
}

void LyricalStructure::LoadStructuredText(const String& s) {
	parts.Clear();
	Vector<String> lines = Split(s, "\n");
	int indent = 0;
	DynPart* part = 0;
	DynSub* sub = 0;
	String el1;
	for (String& l : lines) {
		l = TrimBoth(l);
		if (l.IsEmpty()) continue;
		int diff = 0;;
		if (l[0] == '[') {
			int a = 1;
			int b0 = l.Find("]");
			int b1 = l.Find(":");
			int b = -1;
			String part_name;
			if (b0 >= 0 && b1 >= 0) {
				b = min(b0,b1);
				b1++;
				part_name = l.Mid(b1,b0-b1);
			}
			else if (b0 >= 0) b = b0;
			else b = b1;
			
			if (b >= 0) {
				indent = Split(l.Mid(a,b-a),".").GetCount();
				
				String el;
				b = l.Find("]",b);
				a = l.Find("(",b);
				if (a >= 0) {
					a++;
					b = l.Find(")",a);
					el = l.Mid(a,b-a);
				}
				
				diff = -1;
				int level = indent + diff;
				if (level == 0) {
					part = &parts.Add();
					ParseTextPartType(part_name, part->text_type, part->text_num);
					part->el.element = el;
				}
				else if (level == 1) {
					el1 = el;
					sub = &part->sub.Add();
					sub->el.element = el;
				}
				else if (level == 2) {
					if (sub->lines.GetCount()) {
						sub = &part->sub.Add();
						sub->el.element = el1;
					}
					sub->el.element = el;
				}
			}
		}
		else {
			DynLine& dl = sub->lines.Add();
			dl.text = l;
		}
	}
}

void LyricalStructure::SetText(const String& s, bool user_text) {
	Vector<String> sparts = Split(s, "[");
	int part_i = 0;
	for(int i = 0; i < sparts.GetCount(); i++) {
		String& p = sparts[i];
		Vector<String> lines = Split(p, "\n");
		lines.Remove(0);
		int line_i = 0;
		if (part_i >= parts.GetCount())
			break;
		auto& part = parts[part_i++];
		if (part.sub.IsEmpty() || (part.sub.GetCount() == 1 && part.sub[0].lines.IsEmpty())) {
			i--;
			continue;
		}
		for (auto& sub : part.sub) {
			for (auto& line : sub.lines) {
				String s;
				if (line_i < lines.GetCount())
					s = lines[line_i];
				if (user_text)
					line.user_text = s;
				else
					line.text = s;
				line_i++;
			}
		}
	}
	while (part_i < parts.GetCount()) {
		auto& part = parts[part_i];
		for (auto& sub : part.sub) {
			for (auto& line : sub.lines) {
				if (user_text)
					line.user_text = "";
				else
					line.text = "";
			}
		}
		part_i++;
	}
}

void LyricalStructure::LoadStructuredTextExt(const String& s, bool user_text) {
	Vector<String> lines = Split(s, "\n");
	int indent = 0;
	DynPart* part = 0;
	DynSub* sub = 0;
	int part_i = -1, sub_i = -1, line_i = -1;
	int sub_lines = 0;
	String el1;
	for (String& l : lines) {
		l = TrimBoth(l);
		if (l.IsEmpty()) continue;
		int diff = 0;;
		if (l[0] == '[') {
			int a = 1;
			int b0 = l.Find("]");
			int b1 = l.Find(":");
			int b = -1;
			//String part_name;
			if (b0 >= 0 && b1 >= 0) {
				b = min(b0,b1);
				b1++;
				//part_name = l.Mid(b1,b0-b1);
			}
			else if (b0 >= 0) b = b0;
			else b = b1;
			
			if (b >= 0) {
				indent = Split(l.Mid(a,b-a),".").GetCount();
				
				String el;
				b = l.Find("]",b);
				a = l.Find("(",b);
				if (a >= 0) {
					a++;
					b = l.Find(")",a);
					el = l.Mid(a,b-a);
				}
				
				diff = -1;
				int level = indent + diff;
				if (level == 0) {
					part_i++;
					sub_i = -1;
					line_i = -1;
					sub_lines = 0;
					if (part_i < parts.GetCount())
						part = &parts[part_i];
					else
						part = &parts.Add();
					//ParseTextPartType(part_name, part->text_type, part->text_num);
					//part->element = el;
				}
				else if (level == 1) {
					sub_i++;
					line_i = -1;
					sub_lines = 0;
					el1 = el;
					if (sub_i < part->sub.GetCount())
						sub = &part->sub[sub_i];
					else
						sub = &part->sub.Add();
					//sub->element0 = el;
				}
				else if (level == 2) {
					if (sub_lines > 0) {
						sub_i++;
						line_i = -1;
						sub_lines = 0;
						if (sub_i < part->sub.GetCount())
							sub = &part->sub[sub_i];
						else
							sub = &part->sub.Add();
						//sub->element0 = el1;
					}
					//sub->element1 = el;
				}
			}
		}
		else {
			line_i++;
			sub_lines++;
			DynLine* dl;
			if (line_i < sub->lines.GetCount())
				dl = &sub->lines[line_i];
			else
				dl = &sub->lines.Add();
			if (user_text)
				dl->user_text = l;
			else
				dl->text = l;
		}
	}
}

double ScriptStruct::GetNormalScore() const {
	bool verse1 = false;
	bool verse2 = false;
	bool prerepeat1 = false;
	bool repeat1 = false;
	bool repeat2 = false;
	bool twist1 = false;
	
	for (const auto& part : parts) {
		if (!verse1 && part.type == TXT_NORMAL && part.num == 0)
			verse1 = true;
		if (!verse2 && part.type == TXT_NORMAL && part.num == 1)
			verse2 = true;
		if (!prerepeat1 && part.type == TXT_PRE_REPEAT && part.num == 0)
			prerepeat1 = true;
		if (!repeat1 && part.type == TXT_REPEAT && part.num == 0)
			repeat1 = true;
		if (!repeat2 && part.type == TXT_REPEAT && part.num == 1)
			repeat2 = true;
		if (!twist1 && part.type == TXT_TWIST && part.num == 0)
			twist1 = true;
	}
	double score =
		verse1 * 1.0 +
		verse2 * 0.9 +
		prerepeat1 * 0.5 +
		repeat1 * 1.0 +
		repeat2 * 0.6 +
		twist1 * 0.8
		;
	
	return score;
}


void ReplaceWord(String& s, const String& orig_word, const String& replace_word) {
	String low_text = ToLower(s);
	
	int prev = -1;
	while (prev < low_text.GetCount()) {
		int a = low_text.Find(orig_word, prev+1);
		if (a < 0) break;
		
		bool left_separated = false, right_separated = false;
		
		if (a == 0)
			left_separated = true;
		else {
			int chr = low_text[a-1];
			if (IsAlpha(chr) ||IsLetter(chr) || IsDigit(chr) || chr == '\'' || chr == '-')
				;
			else
				left_separated = true;
		}
		
		if (left_separated) {
			int b = a + orig_word.GetCount();
			if (b >= low_text.GetCount())
				right_separated = true;
			else {
				int chr = low_text[b];
				if (IsAlpha(chr) ||IsLetter(chr) || IsDigit(chr) || chr == '\'' || chr == '-')
					;
				else
					right_separated = true;
			}
		}
		
		if (left_separated && right_separated) {
			s = s.Left(a) + replace_word + s.Mid(a + orig_word.GetCount());
			low_text = low_text.Left(a) + replace_word + low_text.Mid(a + orig_word.GetCount());
			a += replace_word.GetCount()-1;
		}
		
		prev = a;
	}
}

void HotfixReplaceWord(WString& ws) {
	String s = ws.ToString();
	HotfixReplaceWord(s);
	ws = s.ToWString();
}

void HotfixReplaceWord(String& s) {
	ReplaceWord(s, "im", "I'm");
	ReplaceWord(s, "ive", "I've");
	ReplaceWord(s, "ill", "I'll");
	ReplaceWord(s, "id", "I'd");
	ReplaceWord(s, "youre", "you're");
	ReplaceWord(s, "youd", "you'd");
	ReplaceWord(s, "youve", "you've");
	ReplaceWord(s, "youll", "you'll");
	ReplaceWord(s, "hes", "he's");
	ReplaceWord(s, "heve", "he've");
	ReplaceWord(s, "hed", "he'd");
	ReplaceWord(s, "shes", "she's");
	ReplaceWord(s, "sheve", "she've");
	ReplaceWord(s, "shed", "she'd");
	ReplaceWord(s, "theyll", "they'll");
	ReplaceWord(s, "theyve", "they've");
	ReplaceWord(s, "theyre", "they're");
	
	ReplaceWord(s, "arent", "aren't");
	ReplaceWord(s, "aint", "ain't");
	ReplaceWord(s, "didnt", "didn't");
	ReplaceWord(s, "dont", "don't");
	
	ReplaceWord(s, "its", "it's");
	ReplaceWord(s, "itll", "it'll");
	ReplaceWord(s, "itve", "it've");
	ReplaceWord(s, "isnt", "isn't");
	
	ReplaceWord(s, "whats", "what's");
	ReplaceWord(s, "couldnt", "couldn't");
	ReplaceWord(s, "shouldnt", "shouldn't");
	ReplaceWord(s, "theres", "there's");
	ReplaceWord(s, "wasnt", "wasn't");
	ReplaceWord(s, "thats", "that's");
	
	if (0) {
		ReplaceWord(s, "alright", "allright");
		// These change too much
		if (0) {
			ReplaceWord(s, "tryna", "tring to");
			ReplaceWord(s, "aint", "aren't");
			ReplaceWord(s, "gotta", "have to");
			ReplaceWord(s, "wanna", "want to");
			ReplaceWord(s, "em", "them");
			ReplaceWord(s, "ol", "old");
			ReplaceWord(s, "bout", "about");
			ReplaceWord(s, "nunya", "none of your");
			ReplaceWord(s, "thang", "thing");
			ReplaceWord(s, "I'ma", "I'll");
		}
		
		ReplaceWord(s, "tryin", "trying");
		ReplaceWord(s, "fuckin", "fucking");
		ReplaceWord(s, "livin", "living");
		ReplaceWord(s, "lookin", "looking");
		ReplaceWord(s, "prayin", "praying");
		ReplaceWord(s, "rollin", "rolling");
		ReplaceWord(s, "workin", "working");
		ReplaceWord(s, "chargin", "charging");
		ReplaceWord(s, "runnin", "running");
		ReplaceWord(s, "doin", "doing");
		ReplaceWord(s, "judgin", "judging");
		ReplaceWord(s, "blendin", "blending");
		ReplaceWord(s, "gettin", "getting");
		ReplaceWord(s, "talkin", "talking");
		ReplaceWord(s, "changin", "changing");
		ReplaceWord(s, "makin", "making");
		ReplaceWord(s, "retracin", "retracing");
		ReplaceWord(s, "motherfuckin", "motherfucking");
		ReplaceWord(s, "rockin", "rocking");
		ReplaceWord(s, "goin", "going");
		ReplaceWord(s, "frontin", "fronting");
		ReplaceWord(s, "somethin", "something");
		ReplaceWord(s, "playin", "playing");
		ReplaceWord(s, "hittin", "hitting");
		ReplaceWord(s, "movin", "moving");
	}
}

INITIALIZER_COMPONENT(LyricalStructure, "text.lyrical.structure", "Text|Lyrical");
INITIALIZER_COMPONENT(Script, "text.lyrical.script", "Text|Lyrical");
INITIALIZER_COMPONENT(Lyrics, "text.lyrics", "Text|Lyrical");

END_UPP_NAMESPACE
