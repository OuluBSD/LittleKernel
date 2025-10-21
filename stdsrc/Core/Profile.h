// Minimal profiling/timing shims for source compatibility

class TimingInspector {
protected:
    static bool active;
    const char* name = nullptr;
public:
    TimingInspector(const char* nm = nullptr) : name(nm) {}
    ~TimingInspector() {}
    void   Add(unsigned /*time*/, int /*nesting*/) {}
    static void Activate(bool b) { active = b; }

    class Routine {
        TimingInspector& st;
        int& nesting;
    public:
        Routine(TimingInspector& s, int& n) : st(s), nesting(n) { ++nesting; }
        ~Routine() { --nesting; st.Add(0u, nesting); }
    };
};

inline bool TimingInspector::active = false;

class HitCountInspector {
    const char* name;
    int hitcount;
public:
    HitCountInspector(const char* nm, int hc = 0) : name(nm), hitcount(hc) {}
    ~HitCountInspector() {}
    void Step() { ++hitcount; }
    void Add(int i) { hitcount += i; }
    void operator++() { Step(); }
    void operator+=(int i) { Add(i); }
};

#define RTIMING(x) \
    static TimingInspector UPP_COMBINE(_tim_, __LINE__)(x); \
    static thread_local int UPP_COMBINE(_timnest_, __LINE__); \
    TimingInspector::Routine UPP_COMBINE(_timsc_, __LINE__)(UPP_COMBINE(_tim_, __LINE__), UPP_COMBINE(_timnest_, __LINE__))

#define RACTIVATE_TIMING()    TimingInspector::Activate(true)
#define RDEACTIVATE_TIMING()  TimingInspector::Activate(false)

#define RHITCOUNT(n) \
    do { static HitCountInspector UPP_COMBINE(_hc_, __LINE__)(n); UPP_COMBINE(_hc_, __LINE__).Step(); } while(0)

// Simple stopwatch
class TimeStop {
    using clock = std::chrono::steady_clock;
    clock::time_point start = clock::now();
public:
    void Reset() { start = clock::now(); }
    double Seconds() const { return std::chrono::duration<double>(clock::now() - start).count(); }
    int    Msecs()   const { return (int)std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count(); }
};

