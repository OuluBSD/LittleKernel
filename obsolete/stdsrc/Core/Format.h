// U++-style Format implementation (subset covering provided examples)

inline constexpr Nuller Null{}; // convenience

namespace detail_fmt {

struct Spec {
	int index = -1; // 0-based index if >=0
	bool has_width = false;
	int width = 0; // width or obtained via arg
	bool width_from_arg = false;
	bool zero = false;       // '0' pad
	bool left = false;       // '-' left align
	char align = '>';        // '<' left, '>' right, '=' center
	bool plus = false;       // '+' sign
	bool space = false;      // ' ' sign space
	bool comma = false;      // ',' decimal comma
	bool apostrophe = false; // '\'' thousands grouping
	bool hash = false;       // '#'
	bool excl = false;       // '!'
	bool caret = false;      // '^' exponent sign hide
	bool amp = false;        // '&' exponent zero-trim
	bool underscore = false; // '_' suppress -0
	bool qmark = false;      // '?' NaN->empty
	bool have_prec = false;
	int prec = 6;      // precision or significant digits for %m
	String null_text;  // from %[text]~
	String map_spec;   // from %[...]
	String keyword;    // e.g. month, Mon, MON, day, dy, tw
	char type = 0;     // conversion type (d,i,o,x,X,e,E,f,g,G,s,c,m,r,R,a,A)
	char m_suffix = 0; // for %m[f/e/E]
};

inline bool isdigit_c(char c) { return c >= '0' && c <= '9'; }

inline const char* parse_int(const char* p, int& out)
{
	out = 0;
	bool any = false;
	while(isdigit_c(*p)) {
		any = true;
		out = out * 10 + (*p - '0');
		++p;
	}
	return any ? p : nullptr;
}

inline const char* parse_spec(const char* p, Spec& s)
{
	// %[mapping]~ optional
	if(*p == '[') {
		++p;
		const char* b = p;
		while(*p && *p != ']')
			++p;
		if(*p != ']')
			return nullptr;
		s.map_spec = String(b, int(p - b));
		++p;
        if(*p == '~') {
            ++p;
            s.null_text = s.map_spec; // %[text]~ => treat as null-text mapping
        }
	}
	// positional index N:
	if(isdigit_c(*p)) {
		const char* q = p;
		int v = 0;
		while(isdigit_c(*q)) {
			v = v * 10 + (*q - '0');
			++q;
		}
		if(*q == ':') {
			s.index = v - 1;
			p = q + 1;
		}
	}
	// flags
	bool flags = true;
	while(flags) {
		switch(*p) {
		case '0':
			s.zero = true;
			++p;
			break;
		case '+':
			s.plus = true;
			++p;
			break;
		case ' ':
			s.space = true;
			++p;
			break;
		case ',':
			s.comma = true;
			++p;
			break;
		case '\'':
			s.apostrophe = true;
			++p;
			break;
		case '#':
			s.hash = true;
			++p;
			break;
		case '!':
			s.excl = true;
			++p;
			break;
		case '^':
			s.caret = true;
			++p;
			break;
		case '&':
			s.amp = true;
			++p;
			break;
		case '_':
			s.underscore = true;
			++p;
			break;
		case '?':
			s.qmark = true;
			++p;
			break;
		default:
			flags = false;
			break;
		}
	}
	// width and align
	if(*p == '*') {
		s.width_from_arg = true;
		s.has_width = true;
		++p;
	}
	else if(isdigit_c(*p)) {
		int w = 0;
		const char* q = parse_int(p, w);
		if(q) {
			s.has_width = true;
			s.width = w;
			p = q;
		}
	}
	if(*p == '<' || *p == '>' || *p == '=') {
		s.align = *p;
		++p;
	}
	// precision
	if(*p == '.') {
		++p;
		int pr = 0;
		const char* q = parse_int(p, pr);
		if(!q)
			return nullptr;
		s.have_prec = true;
		s.prec = pr;
		p = q;
	}
	// m/M special with optional suffix f/e/E
	if((*p == 'm' && strncmp(p, "month", 5)) || (*p == 'M' && strncmp(p, "Mon", 3))) {
		s.type = *p;
		++p;
		if(*p == 'f' || *p == 'e' || *p == 'E') {
			s.m_suffix = *p;
			++p;
		}
		return p;
	}
	// keyword formats (month/mon/day/dy/tw)
	if(std::isalpha((unsigned char)*p)) {
		const char* b = p;
		while(std::isalpha((unsigned char)*p))
			++p;
		s.keyword = String(b, int(p - b));
		if (s.keyword.GetLength() == 1) {
			s.type = s.keyword[0];
			s.keyword.Clear();
		}
		return p; // keyword consumes entire spec
	}
	// type
	if(*p) {
		s.type = *p++;
	}
	return p;
}

inline bool is_null_arg(const std::any& a)
{
	if(!a.has_value())
		return true;
	if(a.type() == typeid(Nuller))
		return true;
	if(a.type() == typeid(std::nullptr_t))
		return true;
	if(a.type() == typeid(Value))
		return std::any_cast<const Value&>(a).IsVoid();
	return false;
}

inline std::string to_std_string(const String& s) { return std::string(s.Begin(), s.End()); }

inline String roman(int n, bool upper)
{
	if(n <= 0) {
		return String();
	}
	struct RN {
		int v;
		const char* s;
	};
	static RN rn[] = {{1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"}, {100, "C"},
	                  {90, "XC"},  {50, "L"},   {40, "XL"}, {10, "X"},   {9, "IX"},
	                  {5, "V"},    {4, "IV"},   {1, "I"}};
	String out;
	for(auto& r : rn) {
		while(n >= r.v) {
			for(const char* p = r.s; *p; ++p)
				out.Cat(upper ? std::toupper(*p) : std::tolower(*p));
			n -= r.v;
		}
	}
	return out;
}

inline String letters(int n, bool upper)
{
	if(n <= 0)
		return String();
	String out;
	while(n > 0) {
		int rem = (n - 1) % 26;
		char c = (upper ? 'A' : 'a') + rem;
		out.Cat(c);
		n = (n - 1) / 26;
	}
	std::reverse((char*)out.Begin(), (char*)out.End());
	return out;
}

inline String int_to_base(uint64_t v, int base, bool upper)
{
	if(base == 10) {
		return String(std::to_string(v).c_str());
	}
	const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
	String out;
	do {
		int d = int(v % base);
		out.Cat(digits[d]);
		v /= base;
	} while(v);
	std::reverse((char*)out.Begin(), (char*)out.End());
	return out;
}

inline String format_double_m(double x, const Spec& s, char mode = 0)
{
	if(std::isnan(x))
		return s.qmark ? String() : String("-nan");
	if(s.underscore && x == 0.0)
		x = 0.0; // drop negative zero sign
	std::ostringstream os;
	if(s.comma)
		os.imbue(std::locale("C")); // we will post-process comma
	if(mode == 'f')
		os.setf(std::ios::fixed);
	else if(mode == 'e')
		os.setf(std::ios::scientific);
	else
		os.setf(std::ios::fmtflags(0), std::ios::floatfield);
	os.setf(std::ios::fmtflags(0), std::ios::floatfield);
	if(mode == 'f')
		os.setf(std::ios::fixed);
	else if(mode == 'e')
		os.setf(std::ios::scientific);
	else
		os.setf(std::ios::fmtflags(0), std::ios::floatfield);
	int prec = s.have_prec ? s.prec : 6;
	if(mode == 0) {
		os.setf(std::ios::fmtflags(0), std::ios::floatfield);
		os.precision(prec);
	}
	else {
		os.precision(prec);
	}
	if(s.plus)
		os.setf(std::ios::showpos);
	os << std::setprecision(prec);
	if(mode == 0)
		os << std::defaultfloat;
	else if(mode == 'f')
		os << std::fixed;
	else
		os << std::scientific;
	os << x;
	std::string t = os.str();
	// For default mode (m), mimic %.pg trimming
	if(mode == 0) {
		// std::defaultfloat already trims; ensure trimming of trailing zeros and dot
		auto pos_e = t.find('e');
		if(pos_e == std::string::npos)
			pos_e = t.find('E');
		if(pos_e == std::string::npos) {
			// fixed form: trim trailing zeros
			if(!s.excl) {
				while(t.size() > 0 && t.back() == '0')
					t.pop_back();
				if(!t.empty() && t.back() == '.')
					t.pop_back();
			}
			else { // '!' keep zeros to fill significant digits
				size_t intd = t.find('.');
				size_t id = intd == std::string::npos ? t.size() : intd;
				int needed = prec - int(id - (t[0] == '+' || t[0] == '-'));
				if(needed > 0) {
					if(intd == std::string::npos) {
						t.push_back('.');
						intd = t.size() - 1;
					}
					while((int)(t.size() - intd - 1) < needed)
						t.push_back('0');
				}
			}
		}
		else {
			// exponent tweaks
			if(s.caret && pos_e + 1 < t.size() && (t[pos_e + 1] == '+'))
				t.erase(pos_e + 1, 1);
			// zero-trim in exponent
			size_t es = pos_e + 1;
			if(es < t.size() && (t[es] == '+' || t[es] == '-'))
				++es;
			while(es + 1 < t.size() && t[es] == '0' && std::isdigit((unsigned char)t[es + 1]))
				t.erase(es, 1);
			if(s.hash) { /* leave as is */
			}
		}
	}
	if(s.hash) {
		if(t.find('.') == std::string::npos) {
			auto pos_e = t.find_first_of("eE");
			if(pos_e == std::string::npos)
				t.push_back('.');
			else
				t.insert(pos_e, ".");
		}
	}
	if(s.comma) {
		for(char& c : t)
			if(c == '.')
				c = ',';
	}
    if(s.type == 'M' || s.m_suffix == 'E') {
        for(char& c : t)
            if(c == 'e')
                c = 'E';
    }
    return String(t.c_str());
}

inline String apply_width_align(const String& in, const Spec& s)
{
	if(!s.has_width)
		return in;
	int w = s.width;
	int len = in.GetLength();
	if(len >= w)
		return in;
	int pad = w - len;
	String out;
	out.Reserve(w);
	char fill = s.zero ? '0' : ' ';
	if(s.align == '<') {
		out = in;
		out.Cat(fill, pad);
	}
	else if(s.align == '>') {
		out.Cat(fill, pad);
		out.Cat(in);
	}
	else {
		int left = pad / 2;
		int right = pad - left;
		out.Cat(' ', left);
		out.Cat(in);
		out.Cat(' ', right);
	}
	return out;
}

inline String keyword_format(const String& kw, const std::any& a, const Spec& s)
{
	auto lower = [&](String x) {
		std::string t = to_std_string(x);
		std::transform(t.begin(), t.end(), t.begin(),
		               [](unsigned char c) { return (char)std::tolower(c); });
		return String(t.c_str());
	};
	auto upper = [&](String x) {
		std::string t = to_std_string(x);
		std::transform(t.begin(), t.end(), t.begin(),
		               [](unsigned char c) { return (char)std::toupper(c); });
		return String(t.c_str());
	};
	if(kw == "month" || kw == "Month" || kw == "MONTH" || kw == "mon" || kw == "Mon" ||
	   kw == "MON") {
		static const char* full[] = {"",        "january",  "february", "march",  "april",
		                             "may",     "june",     "july",     "august", "september",
		                             "october", "november", "december"};
		static const char* shortm[] = {"",    "jan", "feb", "mar", "apr", "may", "jun",
		                               "jul", "aug", "sep", "oct", "nov", "dec"};
		int m = 0;
		if(a.type() == typeid(int))
			m = std::any_cast<int>(a);
		else if(a.type() == typeid(double))
			m = (int)std::any_cast<double>(a);
		else
			return String();
		String out;
		if(kw == "mon" || kw == "Mon" || kw == "MON")
			out = String(shortm[m]);
		else
			out = String(full[m]);
		if(kw == "Month" || kw == "Mon") {
			if(out.GetLength()) {
				std::string t = to_std_string(out);
				t[0] = (char)std::toupper((unsigned char)t[0]);
				out = String(t.c_str());
			}
		}
		else if(kw == "MONTH" || kw == "MON")
			out = upper(out);
		return out;
	}
	if(kw == "day" || kw == "Day" || kw == "DAY" || kw == "dy" || kw == "Dy" || kw == "DY") {
		static const char* full[] = {"sunday",   "monday", "tuesday", "wednesday",
		                             "thursday", "friday", "saturday"};
		static const char* shortd[] = {"su", "mo", "tu", "we", "th", "fr", "sa"};
		int d = 0;
		if(a.type() == typeid(int))
			d = std::any_cast<int>(a);
		else if(a.type() == typeid(double))
			d = (int)std::any_cast<double>(a);
		else
			return String();
		d = (d % 7 + 7) % 7;
		String out;
		if(kw.Find("dy") >= 0) {
			out = String(shortd[d]);
		}
		else
			out = String(full[d]);
		if(kw == "Day" || kw == "Dy") {
			if(out.GetLength()) {
				std::string t = to_std_string(out);
				t[0] = (char)std::toupper((unsigned char)t[0]);
				out = String(t.c_str());
			}
		}
		else if(kw == "DAY" || kw == "DY") {
			std::string t = to_std_string(out);
			std::transform(t.begin(), t.end(), t.begin(), ::toupper);
			out = String(t.c_str());
		}
		return out;
	}
    if(kw.Find("tw") >= 0) {
        int v = 0;
        if(a.type() == typeid(int))
            v = std::any_cast<int>(a);
        else if(a.type() == typeid(double))
            v = (int)std::any_cast<double>(a);
        v = ((v - 1) % 12 + 12) % 12 + 1;
        String out = String(std::to_string(v).c_str());
        int minw = s.zero ? 2 : 0;
        if(s.has_width)
            minw = std::max(minw, s.width);
        while(out.GetLength() < minw) {
            String p("0"); p.Cat(out); out = p;
        }
        return out;
    }
    return String();
}

inline String map_select(const String& mapspec, const std::any& a)
{
	// format: [k:text;...;default]
	int val = 0;
	bool hasint = false;
	if(a.type() == typeid(int)) {
		val = std::any_cast<int>(a);
		hasint = true;
	}
	else if(a.type() == typeid(double)) {
		val = (int)std::any_cast<double>(a);
		hasint = true;
	}
	Vector<String> parts;
	{
		String ms = mapspec;
		int start = 0;
		for(int i = 0; i <= ms.GetLength(); ++i) {
			if(i == ms.GetLength() || ms[i] == ';') {
				parts.Add(ms.Mid(start, i - start));
				start = i + 1;
			}
		}
	}
	String def;
	auto to_int = [](const String& s) { return (int)std::strtol(s.Begin(), nullptr, 10); };
    // Detect shared modulus if any key uses M%r
    int sharedM = 0;
    for(const auto& e : parts) {
        int colon = e.Find(':'); if(colon < 0) continue;
        String key = e.Left(colon);
        int modpos = key.Find('%');
        if(modpos >= 0) { sharedM = to_int(String(key.Begin(), key.Begin() + modpos)); break; }
    }
    for(const auto& e : parts) {
        int colon = e.Find(':');
        if(colon < 0) { def = e; continue; }
        String key = e.Left(colon);
        String text = e.Mid(colon + 1);
        if(!hasint) continue;
        int modpos = key.Find('%');
        if(modpos >= 0) {
            int M = to_int(String(key.Begin(), key.Begin() + modpos));
            int r = to_int(String(key.Begin() + modpos + 1, key.End()));
            if(M != 0 && val % M == r)
                return text;
        }
        else if(sharedM > 0) {
            int r = to_int(key);
            if(val % sharedM == r)
                return text;
        }
        else {
            if(to_int(key) == val)
                return text;
        }
    }
    return def;
}

template <class T>
inline String to_string_default(const T& x)
{
	return AsString(x);
}
inline String to_string_default(const String& s) { return s; }
inline String to_string_default(const char* s) { return String(s ? s : ""); }
inline String to_string_default(const std::string& s) { return String(s.c_str()); }

inline String format_arg(const std::any& a, const Spec& s)
{
	// null handling
	if(!s.null_text.IsEmpty() && is_null_arg(a))
		return s.null_text;
	// keywords
	if(!s.keyword.IsEmpty())
		return keyword_format(s.keyword, a, s);
	// mapping for %s
	if(!s.map_spec.IsEmpty() && (s.type == 's' || s.type == 0))
		return map_select(s.map_spec, a);

	char t = s.type ? s.type : 's';
	// character
	if(t == 'c') {
		int v = 0;
		if(a.type() == typeid(int))
			v = std::any_cast<int>(a);
		else if(a.type() == typeid(double))
			v = (int)std::any_cast<double>(a);
		String r;
		r.Cat((char)v);
		return r;
	}
	// integer formats
	if(t == 'd' || t == 'i' || t == 'o' || t == 'x' || t == 'X') {
		long long v = 0;
		if(a.type() == typeid(int))
			v = std::any_cast<int>(a);
		else if(a.type() == typeid(long long))
			v = std::any_cast<long long>(a);
		else if(a.type() == typeid(double))
			v = (long long)std::any_cast<double>(a);
		else {
			return String();
		}
		bool neg = v < 0;
		uint64_t uv = neg ? (uint64_t)(-v) : (uint64_t)v;
		String r;
		if(t == 'o')
			r = int_to_base(uv, 8, false);
		else if(t == 'x')
			r = int_to_base(uv, 16, false);
		else if(t == 'X')
			r = int_to_base(uv, 16, true);
		else
			r = int_to_base(uv, 10, false);
		// thousands grouping for decimal when requested
		if((t == 'd' || t == 'i') && s.apostrophe) {
			char sep = s.comma ? '.' : ','; // with decimal-comma flag, use '.' as thousands sep
			// insert separators every 3 digits from the right
			std::string tmp = to_std_string(r);
			std::string grouped;
			grouped.reserve(tmp.size() + tmp.size() / 3);
			int count = 0;
			for(int i = int(tmp.size()) - 1; i >= 0; --i) {
				grouped.push_back(tmp[i]);
				if(++count == 3 && i > 0) { grouped.push_back(sep); count = 0; }
			}
			std::reverse(grouped.begin(), grouped.end());
			r = String(grouped.c_str());
		}
		// sign handling
		if(neg) {
			String m("-"); m.Cat(r); r = m;
		}
		else if(s.plus) {
			String m("+"); m.Cat(r); r = m;
		}
		else if(s.space) {
			String m(" "); m.Cat(r); r = m;
		}
		return r;
	}
	// strings
	if(t == 's' || t == 0) {
		if(!s.map_spec.IsEmpty())
			return map_select(s.map_spec, a);
		if(a.type() == typeid(String))
			return std::any_cast<const String&>(a);
		if(a.type() == typeid(const char*))
			return String(std::any_cast<const char*>(a));
		if(a.type() == typeid(std::string))
			return String(std::any_cast<const std::string&>(a).c_str());
		if(a.type() == typeid(int))
			return String(std::to_string(std::any_cast<int>(a)).c_str());
		if(a.type() == typeid(double))
			return String(std::to_string(std::any_cast<double>(a)).c_str());
		return String();
	}
	// roman
	if(t == 'r' || t == 'R') {
		int v = 0;
		if(a.type() == typeid(int))
			v = std::any_cast<int>(a);
		else if(a.type() == typeid(double))
			v = (int)std::any_cast<double>(a);
		return roman(v, t == 'R');
	}
	// letters
	if(t == 'a' || t == 'A') {
		int v = 0;
		if(a.type() == typeid(int))
			v = std::any_cast<int>(a);
		else if(a.type() == typeid(double))
			v = (int)std::any_cast<double>(a);
		return letters(v, t == 'A');
	}
	// floating
	if(t == 'e' || t == 'E' || t == 'f' || t == 'g' || t == 'G' || t == 'm' || t == 'M') {
		double x = 0.0;
		if(a.type() == typeid(double))
			x = std::any_cast<double>(a);
		else if(a.type() == typeid(int))
			x = (double)std::any_cast<int>(a);
		else
			return String();
		if(t == 'm' || t == 'M') { // %m default like %.pg general, optional suffix
			char mode = 0;
			if(s.m_suffix == 'f')
				mode = 'f';
			else if(s.m_suffix == 'e' || s.m_suffix == 'E')
				mode = 'e';
			return format_double_m(x, s, mode);
		}
		std::ostringstream os;
		os.setf(t == 'e' || t == 'E' ? std::ios::scientific
		                             : (t == 'f' ? std::ios::fixed : std::ios::fmtflags(0)));
		int p = s.have_prec ? s.prec : (t == 'g' || t == 'G' ? 6 : 6);
		os << std::setprecision(p);
		if(t == 'G')
			os.setf(std::ios::uppercase);
		os << x;
		String r(os.str().c_str());
		return r;
	}
	// default
	// Dates and times
	if(a.type() == typeid(Date))
		return Format(std::any_cast<const Date&>(a));
	if(a.type() == typeid(Time))
		return Format(std::any_cast<const Time&>(a));
	return String();
}

} // namespace detail_fmt

template <class... Args>
inline String Format(const char* fmt, Args&&... args)
{
	using namespace detail_fmt;
	std::vector<std::any> argv;
	argv.reserve(sizeof...(Args));
	(argv.emplace_back(std::forward<Args>(args)), ...);
	String out;
	const char* p = fmt;
	int next = 0;
	while(*p) {
		if(*p != '%') {
			out.Cat(*p++);
			continue;
		}
		++p;
		if(*p == '%') {
			out.Cat('%');
			++p;
			continue;
		}
		Spec s;
		const char* q = parse_spec(p, s);
		if(!q) {
			out.Cat('%');
			continue;
		}
		// Dynamic width
		if(s.width_from_arg) {
			if(next < (int)argv.size() && argv[next].type() == typeid(int)) {
				s.has_width = true;
				s.width = std::any_cast<int>(argv[next++]);
			}
		}
		// Null text mapping when mapspec present and followed by ~ previously parsed
		if(!s.map_spec.IsEmpty() && s.type) { /* map handled in format_arg */
		}
		// Arg selection
		int idx = s.index >= 0 ? s.index : next++;
		if(idx < 0 || idx >= (int)argv.size()) { /* missing arg */
			continue;
		}
		// For "%" (single percent) with single arg: treat as default formatting
        String frag = format_arg(argv[idx], s);
        frag = apply_width_align(frag, s);
        out.Cat(frag);
        // Optional suffix separated by backtick: %...`suffix
        if(*q == '`'){
            ++q; const char* suf = q; while(*q && *q != '%') ++q; out.Cat(String(suf, int(q - suf)));
        }
        p = q;
    }
    return out;
}
