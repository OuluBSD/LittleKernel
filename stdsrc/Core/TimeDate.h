// Minimal Date/Time utilities compatible with common U++ usage

// Forward declarations for helpers used below
inline bool IsLeapYear(int year);
inline int  GetDaysOfMonth(int month, int year);
inline int  to_days(int y, int m, int d);
inline void from_days(int z, int16& y, byte& m, byte& d);

struct Date {
    byte  day = 0;
    byte  month = 0;
    int16 year = -32768; // null when year == -32768

    Date() = default;
    Date(const Nuller&) { year = -32768; day = month = 0; }
    Date(int y, int m, int d) { year = (int16)y; month = (byte)m; day = (byte)d; }

    bool IsValid() const {
        if(year == -32768) return false;
        if(month < 1 || month > 12) return false;
        if(day < 1) return false;
        int dim = GetDaysOfMonth(month, year);
        return day <= dim;
    }

    // Scalar as days since 1970-01-01
    void Set(int scalar) {
        from_days(scalar, year, month, day);
    }
    int Get() const { return to_days(year, month, day); }

    static Date Low() { return Date(-4000, 1, 1); }
    static Date High(){ return Date(4000, 1, 1); }

    int Compare(Date b) const {
        if(year != b.year) return year < b.year ? -1 : 1;
        if(month != b.month) return month < b.month ? -1 : 1;
        if(day != b.day) return day < b.day ? -1 : 1;
        return 0;
    }
};

inline bool operator==(Date a, Date b) { return a.year == b.year && a.month == b.month && a.day == b.day; }
inline bool operator<(Date a, Date b)  { return a.Compare(b) < 0; }
inline int  operator-(Date a, Date b)  { return a.Get() - b.Get(); }
inline Date operator+(Date a, int b)   { Date r; r.Set(a.Get() + b); return r; }
inline Date operator-(Date a, int b)   { Date r; r.Set(a.Get() - b); return r; }
inline Date& operator+=(Date& a, int b){ a.Set(a.Get() + b); return a; }
inline Date& operator-=(Date& a, int b){ a.Set(a.Get() - b); return a; }

inline bool IsLeapYear(int year) {
    if(year % 400 == 0) return true;
    if(year % 100 == 0) return false;
    return year % 4 == 0;
}

inline int GetDaysOfMonth(int month, int year) {
    static const int mdays[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if(month == 2) return mdays[1] + (IsLeapYear(year) ? 1 : 0);
    return mdays[month-1];
}

inline int DayOfWeek(Date d) { // 0=Sunday..6=Saturday
    int z = d.Get();
    // 1970-01-01 was Thursday (4); convert to 0..6 with Sunday=0
    int w = (z + 4) % 7; if(w < 0) w += 7; // 0=Thursday -> shift to Sunday
    // Convert so that 0=Sunday
    return w;
}

inline Date GetSysDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm lt{};
#ifdef _WIN32
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    return Date(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday);
}

inline String Format(Date date) {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(4) << (int)date.year << '-' << std::setw(2) << (int)date.month << '-' << std::setw(2) << (int)date.day;
    return String(os.str().c_str());
}

// Time
struct Time : Date {
    byte hour = 0;
    byte minute = 0;
    byte second = 0;

    Time() = default;
    Time(const Nuller&) {}
    Time(int y, int m, int d, int h = 0, int n = 0, int s = 0) { year = (int16)y; month = (byte)m; day = (byte)d; hour = (byte)h; minute = (byte)n; second = (byte)s; }

    void   Set(int64 scalar) {
        int days = (int)(scalar / 86400);
        int64 rem = scalar % 86400; if(rem < 0) { rem += 86400; days--; }
        from_days(days, year, month, day);
        hour = (byte)(rem / 3600); rem %= 3600;
        minute = (byte)(rem / 60);
        second = (byte)(rem % 60);
    }
    int64  Get() const { return (int64)Date::Get() * 86400 + (int64)hour * 3600 + (int64)minute * 60 + (int64)second; }

    bool   IsValid() const {
        if(!Date::IsValid()) return false;
        if(hour > 23 || minute > 59 || second > 59) return false;
        return true;
    }

    int    Compare(Time b) const {
        int c = Date::Compare(b); if(c) return c;
        if(hour != b.hour) return hour < b.hour ? -1 : 1;
        if(minute != b.minute) return minute < b.minute ? -1 : 1;
        if(second != b.second) return second < b.second ? -1 : 1;
        return 0;
    }
    
    static Time Low() { return Time(-4000, 1, 1); }
    static Time High(){ return Time(4000, 1, 1); }
    
};

inline bool operator==(Time a, Time b) { return a.Compare(b) == 0; }
inline bool operator<(Time a, Time b)  { return a.Compare(b) < 0; }
inline int64 operator-(Time a, Time b) { return a.Get() - b.Get(); }
inline Time  operator+(Time a, int64 s){ Time r; r.Set(a.Get() + s); return r; }
inline Time  operator-(Time a, int64 s){ Time r; r.Set(a.Get() - s); return r; }
inline Time& operator+=(Time& a, int64 s){ a.Set(a.Get() + s); return a; }
inline Time& operator-=(Time& a, int64 s){ a.Set(a.Get() - s); return a; }

inline Time GetSysTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm lt{};
#ifdef _WIN32
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    return Time(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
}

inline Time GetUtcTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm gt{};
#ifdef _WIN32
    gmtime_s(&gt, &t);
#else
    gmtime_r(&t, &gt);
#endif
    return Time(gt.tm_year + 1900, gt.tm_mon + 1, gt.tm_mday, gt.tm_hour, gt.tm_min, gt.tm_sec);
}

inline String Format(Time time, bool seconds = true) {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(4) << (int)time.year << '-' << std::setw(2) << (int)time.month << '-' << std::setw(2) << (int)time.day
       << ' ' << std::setw(2) << (int)time.hour << ':' << std::setw(2) << (int)time.minute;
    if(seconds) os << ':' << std::setw(2) << (int)time.second;
    return String(os.str().c_str());
}

// Helpers: civil date conversions (Howard Hinnant's algorithms)
inline int to_days(int y, int m, int d) {
    // days since 1970-01-01
    y -= m <= 2;
    const int era = (y >= 0 ? y : y-399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);
    const unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;
    const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + yoe/400 + doy;
    return (int)(era * 146097 + (int)doe - 719468);
}

inline void from_days(int z, int16& y, byte& m, byte& d) {
    z += 719468;
    const int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = (unsigned)(z - era * 146097);
    const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;
    int y_ = (int)yoe + era * 400;
    const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100 + yoe/400);
    const unsigned mp = (5*doy + 2)/153;
    const unsigned d_ = doy - (153*mp+2)/5 + 1;
    const unsigned m_ = mp < 10 ? mp + 3 : mp - 9;
    y_ += (m_ <= 2);
    y = (int16)y_;
    m = (byte)m_;
    d = (byte)d_;
}

// Parsing helpers (simple ISO formats)
inline const char* StrToDate(Date& d, const char* s) {
    if(!s) return nullptr;
    const char* p = s;
    auto rd = [&](int n)->int{ int v=0; for(int i=0;i<n;i++){ if(!std::isdigit((unsigned char)*p)) return -1; v = v*10 + (*p++ - '0'); } return v; };
    int y = rd(4); if(y < 0) return nullptr;
    if(*p=='-'||*p=='/') ++p; int mo = rd(2); if(mo < 1 || mo > 12) return nullptr;
    if(*p=='-'||*p=='/') ++p; int da = rd(2); if(da < 1 || da > 31) return nullptr;
    d = Date(y, mo, da); if(!d.IsValid()) return nullptr; return p;
}

inline const char* StrToTime(Time& t, const char* s) {
    if(!s) return nullptr;
    const char* p = s; Date d; const char* pe = StrToDate(d, p);
    int h=0, m=0, sec=0; if(pe){ p = pe; if(*p=='T'||*p==' '||*p=='_') ++p; }
    auto rd = [&](int n)->int{ int v=0; for(int i=0;i<n;i++){ if(!std::isdigit((unsigned char)*p)) return -1; v = v*10 + (*p++ - '0'); } return v; };
    h = rd(2); if(h < 0) return nullptr; if(*p==':') ++p; m = rd(2); if(m<0) return nullptr; if(*p==':'){ ++p; sec = rd(2); if(sec<0) sec=0; }
    if(pe) t = Time(d.year, d.month, d.day, h, m, sec); else { Time tt(GetSysTime()); t = Time(tt.year, tt.month, tt.day, h, m, sec); }
    return p;
}

inline Date ScanDate(const char* s, Date def = Date()) { Date d; return StrToDate(d, s) ? d : def; }
inline Time ScanTime(const char* s, Time def = Time()) { Time t; return StrToTime(t, s) ? t : def; }
