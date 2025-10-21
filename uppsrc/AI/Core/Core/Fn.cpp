#include "Core.h"

NAMESPACE_UPP


#define COLOR_GROUP_COUNT (3+6*3)
int GetColorGroupCount() {
	return COLOR_GROUP_COUNT;
}

const Color& GetGroupColor(int i) {
	static Color clrs[COLOR_GROUP_COUNT] = {
		Color(255, 255, 255),
		Color(128, 128, 128),
		
		Color(255, 0, 0),
		Color(255, 0, 255),
		Color(0, 0, 255),
		Color(0, 255, 255),
		Color(0, 255, 0),
		Color(255, 255, 0),
		
		Color(255, 128, 128),
		Color(255, 128, 255),
		Color(128, 128, 255),
		Color(128, 255, 255),
		Color(128, 255, 128),
		Color(255, 255, 128),
		
		Color(128, 0, 0),
		Color(128, 0, 128),
		Color(0, 0, 128),
		Color(0, 128, 128),
		Color(0, 128, 0),
		Color(128, 128, 0),
		
		Color(0, 0, 0)
	};
	i = max(min(COLOR_GROUP_COUNT-1, i), 0);
	return clrs[i];
}

String GetColorString(int i) {
	static const char* clrs[COLOR_GROUP_COUNT] = {
		"White",
		"Gray",
		
		"Red",
		"Violet",
		"Blue",
		"Cyan",
		"Aqua",
		"Lime",

		"Light Red",
		"Light Violet",
		"Light Pink",
		"Light Cyan",
		"Light Blue",
		"Light Yellow",

		"Dark Red",
		"Dark Violet",
		"Dark Blue",
		"Dark Cyan",
		"Dark Green",
		"Dark Yellow",
		
		"Black"
	};
	if (i < 0 || i >= COLOR_GROUP_COUNT) return "<error>";
	return clrs[i];
}

int GetColorGroup(const Color& clr) {
	const Color* begin = &GetGroupColor(0);
	const Color* it = begin;
	int count = GetColorGroupCount();
	const Color* end = begin + count;
	
	int closest_dist = GetColorDistance(*it, clr);
	int closest_group = 0;
	it++;
	while (it != end) {
		int dist = GetColorDistance(*it, clr);
		if (dist < closest_dist) {
			closest_group = (int)(it - begin);
			closest_dist = dist;
		}
		it++;
	}
	
	return closest_group;
}

void RgbHsv(float r, float g, float b, float& h, float& s, float& v) {
	// R, G, B values are divided by 255
	// to change the range from 0..255 to 0..1:
	r /= 255.0;
	g /= 255.0;
	b /= 255.0;
	float cmax = max(r, g, b); // maximum of r, g, b
	float cmin = min(r, g, b); // minimum of r, g, b
	float diff = cmax-cmin; // diff of cmax and cmin.
	if (cmax == cmin)
	  h = 0;
	else if (cmax == r)
	  h = fmod((60 * ((g - b) / diff) + 360), 360.0f);
	else if (cmax == g)
	  h = fmod((60 * ((b - r) / diff) + 120), 360.0f);
	else if (cmax == b)
	  h = fmod((60 * ((r - g) / diff) + 240), 360.0f);
	// if cmax equal zero
	  if (cmax == 0)
	     s = 0;
	  else
	     s = (diff / cmax);// * 100;
	// compute v
	v = cmax;// * 100;
}

int GetColorDistance(const Color& a, const Color& b) {
	float h0, s0, v0;
	float h1, s1, v1;
	RgbHsv((float)a.GetR(), (float)a.GetG(), (float)a.GetB(), h0, s0, v0);
	RgbHsv((float)b.GetR(), (float)b.GetG(), (float)b.GetB(), h1, s1, v1);
	
	float dh = min(fabs(h1-h0), 360.f-fabs(h1-h0)) / 180.0f;
	float ds = fabs(s1-s0);
	float dv = fabs(v1-v0) / 255.0f;
	float distance = sqrtf(dh*dh+ds*ds+dv*dv);
	return (int)(distance * 10000);
}


bool TextColorDistanceSorter::operator()(const int& ai, const int& bi) const {
	Color a, b;
	a = (*clr)[ai];
	b = (*clr)[bi];
	int dist0 = GetColorDistance(a, cmp);
	int dist1 = GetColorDistance(b, cmp);
	return dist0 < dist1;
}

void TextColorDistanceSorter::Sort() {
	ASSERT(str && clr && str->GetCount() == clr->GetCount());
	Vector<int> idx;
	for(int i = 0; i < str->GetCount(); i++)
		idx << i;
	UPP::Sort(idx, *this);
	
	Vector<String> new_str;
	Vector<Color> new_clr;
	new_str.Reserve(idx.GetCount());
	new_clr.Reserve(idx.GetCount());
	for (int i : idx) {
		new_str << (*str)[i];
		new_clr << (*clr)[i];
	}
	Swap(*str, new_str);
	Swap(*clr, new_clr);
}

String Capitalize(String s) {
	return ToUpper(s.Left(1)) + s.Mid(1);
}



const char* AttrKeys[ATTR_COUNT+1][4] = {
#define ATTR_ITEM(e, g, i0, i1) {#e, g, i0, i1},
ATTR_LIST
#undef ATTR_ITEM
	0
};

int FindAttrGroup(const char* group) {
	// HOTFIX
	if (strncmp(group, "authenticity", 12) == 0)
		group = "authencity";
	for(int i = 0; i < ATTR_COUNT; i++) {
		if (strncmp(group, AttrKeys[i][1], 100) == 0)
			return i;
	}
	return -1;
}

bool FindAttrValue(int group_i, const char* value) {
	if (group_i < 0 || group_i >= ATTR_COUNT) return false;
	const char* cmp_value = AttrKeys[group_i][3];
	return strncmp(cmp_value, value, 100) == 0;
}

int FindAttrGroupByValue(const char* value) {
	for(int i = 0; i < ATTR_COUNT; i++) {
		if (strncmp(value, AttrKeys[i][2], 100) == 0 ||
			strncmp(value, AttrKeys[i][3], 100) == 0)
			return i;
	}
	return -1;
}

void RemoveLineNumber( String& s) {
	if (s.IsEmpty()) return;
	for(int i = 0; i < s.GetCount(); i++) {
		if (!IsDigit(s[i])) {
			if (s[i] == '.' || s[i] == ')') {
				s = TrimBoth(s.Mid(i+1));
				break;
			}
			else if (s[i] == '/') {
				continue;
			}
			else {
				s = TrimBoth(s.Mid(i));
				break;
			}
		}
	}
}

void RemoveLineChar(String& s) {
	if (s.IsEmpty()) return;
	if (s[0] == '-')
		s = TrimBoth(s.Mid(1));
	else
		s = TrimBoth(s);
}

void RemoveEmptyLines(String& s) {
	s.Replace("\r","");
	Vector<String> lines = Split(s, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String& l = lines[i];
		l = TrimBoth(l);
		if (l.IsEmpty())
			lines.Remove(i--);
	}
	s = Join(lines, "\n");
}

void RemoveEmptyLines2(String& s) {
	s.Replace("\r","");
	Vector<String> lines = Split(s, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String& l = lines[i];
		RemoveLineNumber(l);
		l = TrimBoth(l);
		if (l.IsEmpty() || l[0] == '-')
			lines.Remove(i--);
	}
	s = Join(lines, "\n");
}

void RemoveEmptyLines3(String& s) {
	s.Replace("\r","");
	Vector<String> lines = Split(s, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String& l = lines[i];
		RemoveLineChar(l);
		l = TrimBoth(l);
		if (l.IsEmpty())
			lines.Remove(i--);
	}
	s = Join(lines, "\n");
}

void RemoveQuotes(String& s) {
	if (s.GetCount() > 0 && s[0] == '\"')
		s = s.Mid(1);
	int c = s.GetCount();
	if (c > 0 && s[c-1] == '\"')
		s = s.Left(c-1);
}

void RemoveQuotes2(String& s_) {
	WString ws = s_.ToWString();
	if (ws.GetCount() > 0 && (ws[0] == '\"' || ws[0] == L"“"[0]))
		ws = ws.Mid(1);
	int c = ws.GetCount();
	if (c > 0 && (ws[c-1] == '\"' || ws[c-1] == L"”"[0]))
		ws = ws.Left(c-1);
	s_ = ws.ToString();
}

void GetWords(const String& line, Vector<String>& words) {
	words.SetCount(0);
	
	WString w = line.ToWString();
	
	WString tmp;
	for(int i = 0; i < w.GetCount(); i++) {
		wchar chr = w[i];
		
		if (!IsLetter(chr) && !IsDigit(chr) && chr != '\'' && chr != '`' && chr != '-') {
		//if (IsSpace(chr) || chr == '.' || chr == ',' || chr == '?' || chr == '!' || chr == ':' || chr == ';') {
			if (tmp.IsEmpty()) continue;
			words << tmp.ToString();
			tmp.Clear();
		}
		else {
			tmp.Cat(chr);
		}
	}
	
	if (tmp.GetCount()) {
		words << tmp.ToString();
	}
}

String DeHtml(String html, Vector<String>& links) {
	String out;
	int depth = 0;
	String deep;
	for(int i = 0; i < html.GetCount(); i++) {
		int chr = html[i];
		if (chr == '<') {
			if (depth == 0)
				deep.Clear();
			depth++;
		}
		else if (chr == '>') {
			depth--;
			if (depth == 0) {
				int href = deep.Find(" href=\"");
				if (href >= 0) {
					href += 7;
					int end = deep.Find("\"", href);
					if (end >= 0) {
						String addr = deep.Mid(href, end-href);
						links << addr;
					}
				}
			}
		}
		else if (depth == 0) {
			out.Cat(chr);
		}
		else {
			deep.Cat(chr);
		}
	}
	out.Replace("&amp;", "&");
	out.Replace("&quot;", "\"");
	out.Replace("&#39;", "'");
	
	out = TrimBoth(out);
	
	/*for(int i = 0; i < out.GetCount()-1; i++) {
		int chr0 = out[i];
		int chr1 = out[i+1];
		if (chr0 >= 'a' && chr0 <= 'z' &&
			chr1 >= 'A' && chr1 <= 'Z') {
			out.Insert(i+1, '\n');
		}
	}*/
	
	return out;
}

bool IsAllUpper(const String& s) {
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		if (chr >= 'A' && chr <= 'Z')
			continue;
		if (chr == '\'' || chr == '-' || chr == '/' || chr == ',' || chr == '&')
			continue;
		return false;
	}
	return !s.IsEmpty();
}

String GetGlobalProxy() {
	return GlobalProxy();
}

Value FindValueRecursively(Value val, String key) {
	ValueMap map = val;
	int i = map.Find(key);
	if (i >= 0)
		return map.GetValue(i);
	for(int i = 0; i < map.GetCount(); i++) {
		Value val = FindValueRecursively(map.GetValue(i), key);
		if (!val.IsNull())
			return val;
	}
	return Value();
}

END_UPP_NAMESPACE
