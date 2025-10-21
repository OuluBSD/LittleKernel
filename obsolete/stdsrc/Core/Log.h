// Simple logging utilities and LOG macro

namespace LogDetail {
    enum class Level { Debug = 0, Info = 1, Warn = 2, Error = 3 };

    struct State {
        inline static std::mutex mtx;
        inline static std::ofstream file;
        inline static bool to_stderr = true;
        inline static Level min_level = Level::Info;
        inline static std::map<std::string, Level> module_levels; // base filename (lower) -> level
        inline static std::string filepath;
        inline static size_t bytes_written = 0;
        inline static size_t max_bytes = 0; // 0 = unlimited
        inline static bool rotate_keep = true;
    };

    inline const char* LevelText(Level l) {
        switch(l) {
            case Level::Debug: return "DEBUG";
            case Level::Info:  return "INFO";
            case Level::Warn:  return "WARN";
            case Level::Error: return "ERROR";
        }
        return "INFO";
    }

    inline std::string NowIsoLocal() {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t tt = system_clock::to_time_t(now);
        std::tm lt{};
    #ifdef _WIN32
        localtime_s(&lt, &tt);
    #else
        localtime_r(&tt, &lt);
    #endif
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                      lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
        return std::string(buf);
    }
}

// Public API (ends up in Upp namespace via Core.h wrapper)

inline void LogSetLevel(int level) {
    using LogDetail::Level; using LogDetail::State;
    std::lock_guard<std::mutex> lk(State::mtx);
    if(level <= 0) State::min_level = Level::Debug;
    else if(level == 1) State::min_level = Level::Info;
    else if(level == 2) State::min_level = Level::Warn;
    else State::min_level = Level::Error;
}

inline void LogToFile(const char* path, bool append = true) {
    using LogDetail::State;
    std::lock_guard<std::mutex> lk(State::mtx);
    if(State::file.is_open()) State::file.close();
    std::ios::openmode m = std::ios::out | std::ios::binary;
    if(append) m |= std::ios::app; else m |= std::ios::trunc;
    State::filepath = path ? path : "log.txt";
    State::file.open(State::filepath, m);
    // initialize bytes_written
    if(State::file.is_open()) {
        if(append) {
            State::file.seekp(0, std::ios::end);
            auto pos = State::file.tellp();
            State::bytes_written = pos >= 0 ? (size_t)pos : 0;
        } else {
            State::bytes_written = 0;
        }
    }
}

inline void LogToStdErr(bool enable = true) {
    using LogDetail::State;
    std::lock_guard<std::mutex> lk(State::mtx);
    State::to_stderr = enable;
}

inline void LogWriteImpl(int level, const char* file, int line, const std::string& msg) {
    using LogDetail::Level; using LogDetail::State; using LogDetail::LevelText; using LogDetail::NowIsoLocal;
    Level lev = Level::Info;
    if(level <= 0) lev = Level::Debug; else if(level == 1) lev = Level::Info; else if(level == 2) lev = Level::Warn; else lev = Level::Error;
    std::lock_guard<std::mutex> lk(State::mtx);
    // Determine module-specific level from file base name (lowercased)
    Level minlev = State::min_level;
    if(file) {
        const char* base = std::max(strrchr(file, '/'), strrchr(file, '\\'));
        base = base ? base + 1 : file;
        const char* dot = strrchr(base, '.');
        std::string mod(base, dot ? (size_t)(dot - base) : strlen(base));
        std::transform(mod.begin(), mod.end(), mod.begin(), [](unsigned char c){ return (char)std::tolower(c); });
        auto it = State::module_levels.find(mod);
        if(it != State::module_levels.end())
            minlev = it->second;
    }
    if((int)lev < (int)minlev) return;
    std::string ts = NowIsoLocal();
    std::string linebuf;
    linebuf.reserve(64 + msg.size());
    linebuf.append("[").append(LevelText(lev)).append("] ").append(ts).append(" ");
    if(file) {
        linebuf.append("(").append(file);
        if(line > 0) { char num[16]; std::snprintf(num, sizeof(num), ":%d", line); linebuf.append(num); }
        linebuf.append(") ");
    }
    linebuf.append(msg);
    linebuf.push_back('\n');
    if(State::file.is_open()) {
        // rotation
        if(State::max_bytes > 0 && State::bytes_written + linebuf.size() > State::max_bytes) {
            State::file.close();
            if(State::rotate_keep) {
                std::error_code ec;
                std::filesystem::path p(State::filepath);
                std::filesystem::path b = p;
                b += ".1";
                std::filesystem::remove(b, ec);
                std::filesystem::rename(p, b, ec);
            }
            // reopen truncated
            State::file.open(State::filepath, std::ios::out | std::ios::binary | std::ios::trunc);
            State::bytes_written = 0;
        }
        State::file.write(linebuf.data(), (std::streamsize)linebuf.size());
        State::file.flush();
        State::bytes_written += linebuf.size();
    }
    if(State::to_stderr) {
        std::fwrite(linebuf.data(), 1, linebuf.size(), stderr);
        std::fflush(stderr);
    }
}

inline bool LogEnabled(int level) {
    using LogDetail::Level; using LogDetail::State;
    Level lev = Level::Info;
    if(level <= 0) lev = Level::Debug; else if(level == 1) lev = Level::Info; else if(level == 2) lev = Level::Warn; else lev = Level::Error;
    std::lock_guard<std::mutex> lk(State::mtx);
    return (int)lev >= (int)State::min_level;
}

#define LOG_FILE 0x01
#define LOG_COUT 0x02

inline void StdLogSetup(int flags, const char* path = nullptr, bool append = true) {
    LogToStdErr((flags & LOG_COUT) != 0);
    if(flags & LOG_FILE) {
        String p;
        if(path)
            p = String(path);
        else
            p = AppendFileName(GetTempPath(), "std.log");
        LogToFile(p.Begin(), append);
    }
}

inline void LogSetModuleLevel(const char* module, int level) {
    using LogDetail::Level; using LogDetail::State;
    if(!module) return;
    Level lev = Level::Info;
    if(level <= 0) lev = Level::Debug; else if(level == 1) lev = Level::Info; else if(level == 2) lev = Level::Warn; else lev = Level::Error;
    std::lock_guard<std::mutex> lk(State::mtx);
    std::string mod(module);
    std::transform(mod.begin(), mod.end(), mod.begin(), [](unsigned char c){ return (char)std::tolower(c); });
    State::module_levels[mod] = lev;
}

inline void LogSetRotation(size_t max_bytes, bool keep_backup = true) {
    using LogDetail::State;
    std::lock_guard<std::mutex> lk(State::mtx);
    State::max_bytes = max_bytes;
    State::rotate_keep = keep_backup;
}

#ifndef LOG
#define LOG(expr) do { std::ostringstream _upp_log_os_; _upp_log_os_ << expr; Upp::LogWriteImpl(1, __FILE__, __LINE__, _upp_log_os_.str()); } while(0)
#endif

#ifndef ILOG
#define ILOG(expr) LOG(expr)
#endif

#ifndef WLOG
#define WLOG(expr) do { std::ostringstream _upp_log_wos_; _upp_log_wos_ << expr; Upp::LogWriteImpl(2, __FILE__, __LINE__, _upp_log_wos_.str()); } while(0)
#endif

#ifndef ELOG
#define ELOG(expr) do { std::ostringstream _upp_log_eos_; _upp_log_eos_ << expr; Upp::LogWriteImpl(3, __FILE__, __LINE__, _upp_log_eos_.str()); } while(0)
#endif

#ifndef DLOG
  #ifdef _DEBUG
    #define DLOG(expr) do { if(Upp::LogEnabled(0)) { std::ostringstream _upp_log_dos_; _upp_log_dos_ << expr; Upp::LogWriteImpl(0, __FILE__, __LINE__, _upp_log_dos_.str()); } } while(0)
  #else
    #define DLOG(expr) do { } while(0)
  #endif
#endif

// printf-style logging helpers
inline void LogVWritef(int level, const char* file, int line, const char* fmt, va_list ap)
{
    char small[512];
    va_list ap2; va_copy(ap2, ap);
    int n = std::vsnprintf(small, (int)sizeof(small), fmt, ap2);
    va_end(ap2);
    if(n >= 0 && n < (int)sizeof(small)) {
        Upp::LogWriteImpl(level, file, line, std::string(small, small + n));
        return;
    }
    int need = n > 0 ? n + 1 : (int)sizeof(small) * 2;
    std::vector<char> buf((size_t)need);
    std::vsnprintf(buf.data(), buf.size(), fmt, ap);
    Upp::LogWriteImpl(level, file, line, std::string(buf.data()));
}

inline void LogWritef(int level, const char* file, int line, const char* fmt, ...)
{
    va_list ap; va_start(ap, fmt);
    LogVWritef(level, file, line, fmt, ap);
    va_end(ap);
}

#ifndef LOGF
#define LOGF(fmt, ...)  do { Upp::LogWritef(1, __FILE__, __LINE__, fmt, ##__VA_ARGS__); } while(0)
#endif
#ifndef ILOGF
#define ILOGF(fmt, ...) LOGF(fmt, ##__VA_ARGS__)
#endif
#ifndef WLOGF
#define WLOGF(fmt, ...) do { Upp::LogWritef(2, __FILE__, __LINE__, fmt, ##__VA_ARGS__); } while(0)
#endif
#ifndef ELOGF
#define ELOGF(fmt, ...) do { Upp::LogWritef(3, __FILE__, __LINE__, fmt, ##__VA_ARGS__); } while(0)
#endif
#ifndef DLOGF
  #ifdef _DEBUG
    #define DLOGF(fmt, ...) do { if(Upp::LogEnabled(0)) Upp::LogWritef(0, __FILE__, __LINE__, fmt, ##__VA_ARGS__); } while(0)
  #else
    #define DLOGF(fmt, ...) do { } while(0)
  #endif
#endif

// Environment-driven setup: LOG_LEVEL, LOG_FILE, LOG_COUT, LOG_TRUNCATE
inline void LogInitFromEnv() {
    const char* level = std::getenv("LOG_LEVEL");
    if(level) {
        String s(level);
        {
            std::string t = (std::string)s;
            std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c){ return (char)std::tolower(c); });
            s = String(t.c_str());
        }
        int lv = 1;
        if(s == "0" || s == "debug") lv = 0;
        else if(s == "1" || s == "info") lv = 1;
        else if(s == "2" || s == "warn" || s == "warning") lv = 2;
        else if(s == "3" || s == "error" || s == "err") lv = 3;
        Upp::LogSetLevel(lv);
    }
    const char* coutv = std::getenv("LOG_COUT");
    bool en_cout = coutv && (std::strcmp(coutv, "0") != 0);
    const char* filev = std::getenv("LOG_FILE");
    bool en_file = filev && (std::strcmp(filev, "0") != 0);
    bool trunc = false;
    if(const char* t = std::getenv("LOG_TRUNCATE")) trunc = (std::strcmp(t, "0") != 0);
    if(en_cout || en_file) {
        int flags = 0; if(en_cout) flags |= LOG_COUT; if(en_file) flags |= LOG_FILE;
        const char* path = nullptr;
        if(filev && std::strcmp(filev, "1") != 0) path = filev; // treat as path
        Upp::StdLogSetup(flags, path, !trunc);
    }
    if(const char* lvmods = std::getenv("LOG_LEVELS")) {
        // format: name=level[,name=level...], names match base filename without extension
        std::string str(lvmods);
        size_t i = 0;
        auto parse_level = [](const std::string& s){
            std::string t = s; std::transform(t.begin(), t.end(), t.begin(), ::tolower);
            if(t=="0"||t=="debug") return 0; if(t=="1"||t=="info") return 1; if(t=="2"||t=="warn"||t=="warning") return 2; return 3; };
        while(i < str.size()) {
            size_t j = str.find('=', i); if(j == std::string::npos) break; std::string name = str.substr(i, j-i);
            size_t k = str.find(',', j+1); std::string lev = str.substr(j+1, k == std::string::npos ? std::string::npos : k-(j+1));
            auto trim = [](std::string& x){ size_t a=0; while(a<x.size() && isspace((unsigned char)x[a])) ++a; size_t b=x.size(); while(b>a && isspace((unsigned char)x[b-1])) --b; x = x.substr(a,b-a);};
            trim(name); trim(lev);
            if(!name.empty()) Upp::LogSetModuleLevel(name.c_str(), parse_level(lev));
            if(k == std::string::npos) break; else i = k+1;
        }
    }
    if(const char* mx = std::getenv("LOG_MAX_BYTES")) {
        long long v = std::strtoll(mx, nullptr, 10); if(v > 0) Upp::LogSetRotation((size_t)v, true);
    }
    if(const char* keep = std::getenv("LOG_ROTATE_KEEP")) {
        bool kb = std::strcmp(keep, "0") != 0; Upp::LogSetRotation(LogDetail::State::max_bytes, kb);
    }
}
