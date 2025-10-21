// Minimal conversion helpers

inline int Atoi(const char* s) { return s ? (int)std::strtol(s, nullptr, 10) : 0; }
inline int64 Atoi64(const char* s) { return s ? (int64)std::strtoll(s, nullptr, 10) : 0; }
inline double Atof(const char* s) { return s ? std::strtod(s, nullptr) : 0.0; }

// Scanning with endptr and radix
inline int ScanInt(const char* ptr, const char** endptr, int radix) {
    if(!ptr) { if(endptr) *endptr = nullptr; return 0; }
    char* e = nullptr; long v = std::strtol(ptr, &e, radix);
    if(endptr) *endptr = e;
    return (int)v;
}
inline int ScanInt(const char* ptr, const char** endptr) { return ScanInt(ptr, endptr, 10); }
inline int ScanInt(const char* ptr) { const char* e; return ScanInt(ptr, &e, 10); }

inline int64 ScanInt64(const char* ptr, const char** endptr, int radix) {
    if(!ptr) { if(endptr) *endptr = nullptr; return 0; }
    char* e = nullptr; long long v = std::strtoll(ptr, &e, radix);
    if(endptr) *endptr = e;
    return (int64)v;
}
inline int64 ScanInt64(const char* ptr, const char** endptr) { return ScanInt64(ptr, endptr, 10); }
inline int64 ScanInt64(const char* ptr) { const char* e; return ScanInt64(ptr, &e, 10); }

inline double ScanDouble(const char* ptr, const char** endptr, bool accept_comma) {
    if(!ptr) { if(endptr) *endptr = nullptr; return 0.0; }
    if(accept_comma) {
        std::string tmp(ptr);
        for(char& c : tmp) if(c == ',') c = '.';
        char* e = nullptr; double v = std::strtod(tmp.c_str(), &e);
        if(endptr) *endptr = ptr + (e - tmp.c_str());
        return v;
    }
    char* e = nullptr; double v = std::strtod(ptr, &e);
    if(endptr) *endptr = e;
    return v;
}
inline double ScanDouble(const char* ptr, const char** endptr) { return ScanDouble(ptr, endptr, false); }
inline double ScanDouble(const char* ptr) { const char* e; return ScanDouble(ptr, &e, false); }

// String conversions
inline String IntStr(int i) { return String(std::to_string(i).c_str()); }
inline String IntStr64(int64 i) { return String(std::to_string((long long)i).c_str()); }
inline String DblStr(double d) { return String(std::to_string(d).c_str()); }

inline String FormatInt(int i) { return IntStr(i); }
inline String FormatInt64(int64 i) { return IntStr64(i); }
inline String FormatDouble(double d, int digits = 6) {
    std::ostringstream os; os.setf(std::ios::fixed); os << std::setprecision(digits) << d; return String(os.str().c_str());
}

inline int StrInt(const char* s) { return Atoi(s); }
inline int64 StrInt64(const char* s) { return Atoi64(s); }
inline double StrDbl(const char* s) { return Atof(s); }

inline Value StrIntValue(const char* s) { return Value(StrInt(s)); }
inline Value StrDblValue(const char* s) { return Value(StrDbl(s)); }

class Convert {
public:
    virtual ~Convert() = default;
    virtual Value Format(const Value& q) const { return q; }
    virtual Value Scan(const Value& text) const { return text; }
    virtual int   Filter(int chr) const { return chr; }
    Value operator()(const Value& q) const { return Format(q); }
};

// unsigned helpers
inline unsigned stou(const char* ptr, void* endptr = NULL, unsigned radix = 10) {
    char* e = nullptr; unsigned long v = std::strtoul(ptr, &e, radix);
    if(endptr) *reinterpret_cast<const char**>(endptr) = e;
    return (unsigned)v;
}
inline uint64_t stou64(const char* s, void* endptr = NULL, unsigned radix = 10) {
    char* e = nullptr; unsigned long long v = std::strtoull(s, &e, radix);
    if(endptr) *reinterpret_cast<const char**>(endptr) = e;
    return (uint64_t)v;
}

inline unsigned stou(const wchar* ptr, void* endptr = NULL, unsigned radix = 10) {
    wchar_t* e = nullptr; unsigned long v = std::wcstoul((const wchar_t*)ptr, &e, radix);
    if(endptr) *reinterpret_cast<const wchar_t**>(endptr) = e;
    return (unsigned)v;
}
inline uint64_t stou64(const wchar* s, void* endptr = NULL, unsigned radix = 10) {
    wchar_t* e = nullptr; unsigned long long v = std::wcstoull((const wchar_t*)s, &e, radix);
    if(endptr) *reinterpret_cast<const wchar_t**>(endptr) = e;
    return (uint64_t)v;
}

// --- Richer Convert classes ---

inline Value NotNullError() { return Value(); }

class ConvertInt : public Convert {
public:
    ConvertInt(int minval = std::numeric_limits<int>::min(), int maxval = std::numeric_limits<int>::max(), bool notnull = false)
    : minval(minval), maxval(maxval), notnull(notnull) {}

    Value Format(const Value& q) const override {
        if(q.Is<int>()) return Value(IntStr(q.Get<int>()));
        if(q.Is<String>()) return q;
        return Value(String());
    }
    Value Scan(const Value& text) const override {
        String s = text.ToString();
        const char* e = nullptr; int v = ScanInt(s.Begin(), &e, 10);
        if(notnull && s.IsEmpty()) return NotNullError();
        if(v < minval) v = minval; if(v > maxval) v = maxval; return Value(v);
    }
    int Filter(int chr) const override {
        if(std::isdigit(chr) || chr=='-' || chr=='+') return chr; return 0;
    }

    ConvertInt& MinMax(int _min, int _max) { minval = _min; maxval = _max; return *this; }
    ConvertInt& Min(int _min) { minval = _min; return *this; }
    ConvertInt& Max(int _max) { maxval = _max; return *this; }
    ConvertInt& NotNull(bool b = true) { notnull = b; return *this; }
    int GetMin() const { return minval; }
    int GetMax() const { return maxval; }
    bool IsNotNull() const { return notnull; }

private:
    int  minval, maxval;
    bool notnull;
};

inline const ConvertInt& StdConvertInt() { static ConvertInt c; return c; }
inline const ConvertInt& StdConvertIntNotNull() { static ConvertInt c(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true); return c; }

class ConvertInt64 : public ConvertInt {
public:
    ConvertInt64(int64 minval = std::numeric_limits<long long>::min(), int64 maxval = std::numeric_limits<long long>::max(), bool notnull = false)
    : ConvertInt() { MinMax((int)minval, (int)maxval); NotNull(notnull); }
};

class ConvertDouble : public Convert {
public:
    ConvertDouble(double minval = -std::numeric_limits<double>::max(), double maxval = std::numeric_limits<double>::max(), bool notnull = false)
    : minval(minval), maxval(maxval), notnull(notnull), comma(false) {}

    ConvertDouble& Pattern(const char* p) { pattern = p ? String(p) : String(); return *this; }
    ConvertDouble& MinMax(double _min, double _max) { minval = _min; maxval = _max; return *this; }
    ConvertDouble& Min(double _min) { minval = _min; return *this; }
    ConvertDouble& Max(double _max) { maxval = _max; return *this; }
    ConvertDouble& NotNull(bool b = true) { notnull = b; return *this; }

    Value Format(const Value& q) const override {
        if(q.Is<double>()) {
            double d = q.Get<double>();
            if(!pattern.IsEmpty()) return Value(::Upp::Format(pattern.Begin(), d));
            return Value(FormatDouble(d, 6));
        }
        if(q.Is<String>()) return q;
        return Value(String());
    }
    Value Scan(const Value& text) const override {
        String s = text.ToString();
        const char* e = nullptr; double v = ScanDouble(s.Begin(), &e, comma);
        if(notnull && s.IsEmpty()) return NotNullError();
        if(v < minval) v = minval; if(v > maxval) v = maxval; return Value(v);
    }
    int Filter(int chr) const override {
        if(std::isdigit(chr) || chr=='-' || chr=='+' || chr=='.' || (comma && chr==',')) return chr; return 0;
    }

private:
    double minval, maxval;
    bool   notnull;
    bool   comma;
    String pattern;
};

inline const ConvertDouble& StdConvertDouble() { static ConvertDouble c; return c; }
inline const ConvertDouble& StdConvertDoubleNotNull() { static ConvertDouble c(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), true); return c; }

class ConvertFloat : public ConvertDouble {
public:
    ConvertFloat(double minval = -(double)std::numeric_limits<float>::max(), double maxval = (double)std::numeric_limits<float>::max(), bool notnull = false)
    : ConvertDouble(minval, maxval, notnull) { Pattern("%.7g"); }
};

inline const ConvertFloat& StdConvertFloat() { static ConvertFloat c; return c; }
inline const ConvertFloat& StdConvertFloatNotNull() { static ConvertFloat c(-(double)std::numeric_limits<float>::max(), (double)std::numeric_limits<float>::max(), true); return c; }

class ConvertDate : public Convert {
public:
    ConvertDate(Date minval = Date::Low(), Date maxval = Date::High(), bool notnull = false)
    : minval(minval), maxval(maxval), defaultval(Date()), notnull(notnull) {}

    Value Format(const Value& q) const override { return Value(Format(q.Is<Date>() ? q.Get<Date>() : ScanDate(q.ToString().Begin()))); }
    Value Scan(const Value& text) const override {
        Date d = ScanDate(text.ToString().Begin());
        if(!d.IsValid()) d = defaultval;
        if(notnull && !d.IsValid()) return NotNullError();
        if(d.Compare(minval) < 0) d = minval; if(d.Compare(maxval) > 0) d = maxval; return Value(d);
    }
    int Filter(int chr) const override { if(std::isdigit(chr) || chr=='-' || chr=='/' || chr=='.' || chr==' ') return chr; return 0; }

    ConvertDate& MinMax(Date _min, Date _max) { minval = _min; maxval = _max; return *this; }
    ConvertDate& Min(Date _min) { minval = _min; return *this; }
    ConvertDate& Max(Date _max) { maxval = _max; return *this; }
    ConvertDate& NotNull(bool b = true) { notnull = b; return *this; }
    ConvertDate& Default(Date d) { defaultval = d; return *this; }
    static void SetDefaultMinMax(Date, Date) {}
    static Date GetDefaultMin() { return Date::Low(); }
    static Date GetDefaultMax() { return Date::High(); }

private:
    Date minval, maxval, defaultval;
    bool notnull;
};

inline const ConvertDate& StdConvertDate() { static ConvertDate c; return c; }
inline const ConvertDate& StdConvertDateNotNull() { static ConvertDate c(Date::Low(), Date::High(), true); return c; }

class ConvertTime : public Convert {
public:
    ConvertTime(Time minval = Time::Low(), Time maxval = Time::High(), bool notnull = false)
    : minval(minval), maxval(maxval), defaultval(Time()), notnull(notnull), seconds(true), timealways(false), dayend(false) {}

    Value Format(const Value& q) const override { return Value(::Upp::Format(q.Is<Time>() ? q.Get<Time>() : ScanTime(q.ToString().Begin()), seconds)); }
    Value Scan(const Value& text) const override {
        Time t = ScanTime(text.ToString().Begin());
        if(!t.IsValid()) t = defaultval;
        if(notnull && !t.IsValid()) return NotNullError();
        if(t.Compare(minval) < 0) t = minval; if(t.Compare(maxval) > 0) t = maxval; return Value(t);
    }
    int Filter(int chr) const override { if(std::isdigit(chr) || chr=='-' || chr==':' || chr==' ' || chr=='T') return chr; return 0; }

    ConvertTime& MinMax(Time _min, Time _max) { minval = _min; maxval = _max; return *this; }
    ConvertTime& Min(Time _min) { minval = _min; return *this; }
    ConvertTime& Max(Time _max) { maxval = _max; return *this; }
    ConvertTime& NotNull(bool b = true) { notnull = b; return *this; }
    ConvertTime& Seconds(bool b = true) { seconds = b; return *this; }
    ConvertTime& TimeAlways(bool b = true) { timealways = b; return *this; }
    ConvertTime& DayEnd(bool b = true) { dayend = b; return *this; }

    static Time GetDefaultMin() { return Time::Low(); }
    static Time GetDefaultMax() { return Time::High(); }

private:
    Time minval, maxval, defaultval;
    bool notnull;
    bool seconds;
    bool timealways;
    bool dayend;
};
