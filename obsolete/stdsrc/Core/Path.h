// Minimal path and filesystem helpers built on std::filesystem

namespace fs = std::filesystem;

// --- File time wrapper and conversions ---
struct FileTime {
    std::chrono::system_clock::time_point tp{};
    operator bool() const { return tp.time_since_epoch().count() != 0; }
};

inline std::chrono::system_clock::time_point filetime_to_sys(const fs::file_time_type& ftp) {
    using namespace std::chrono;
    return time_point_cast<system_clock::duration>(ftp - fs::file_time_type::clock::now() + system_clock::now());
}
inline fs::file_time_type sys_to_filetime(const std::chrono::system_clock::time_point& sctp) {
    using namespace std::chrono;
    auto now_file = fs::file_time_type::clock::now();
    auto now_sys = system_clock::now();
    return time_point_cast<fs::file_time_type::duration>(now_file + (sctp - now_sys));
}

inline bool PatternMatch(const char* p, const char* s) {
    // Simple glob: * and ? only, no character classes
    if(!p || !s) return false;
    while(*p) {
        if(*p == '*') {
            while(*p == '*') ++p;
            if(!*p) return true; // trailing *
            for(const char* t = s; *t; ++t)
                if(PatternMatch(p, t)) return true;
            return false;
        }
        else if(*p == '?') { if(!*s) return false; ++p; ++s; }
        else {
            if(*p != *s) return false; ++p; ++s;
        }
    }
    return *s == 0;
}

inline bool PatternMatchMulti(const char* patterns, const char* s) {
    if(!patterns) return false;
    String p(patterns);
    int start = 0;
    for(int i = 0; i <= p.GetLength(); ++i) {
        if(i == p.GetLength() || p[i] == ';' || p[i] == ',') {
            String one = p.Mid(start, i - start);
            if(PatternMatch(one.Begin(), s)) return true;
            start = i + 1;
        }
    }
    return false;
}

inline const char* GetFileNamePos(const char* path) {
    if(!path) return nullptr;
    const char* last_sep = strrchr(path, '/');
#ifdef _WIN32
    const char* last_bsl = strrchr(path, '\\');
    if(last_bsl && (!last_sep || last_bsl > last_sep)) last_sep = last_bsl;
#endif
    return last_sep ? last_sep + 1 : path;
}

inline const char* GetFileExtPos(const char* path) {
    if(!path) return nullptr;
    const char* base = GetFileNamePos(path);
    const char* dot = strrchr(base, '.');
    return dot ? dot + 1 : base + strlen(base);
}

inline bool HasFileExt(const char* path) { const char* e = GetFileExtPos(path); return e && *e; }
inline bool HasWildcards(const char* path) { return path && (strchr(path, '*') || strchr(path, '?')); }

inline bool IsFullPath(const char* path) {
    if(!path || !*path) return false;
#ifdef _WIN32
    if((isalpha((unsigned char)path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\'))) return true;
    if(path[0] == '\\' && path[1] == '\\') return true; // UNC
    return false;
#else
    return path[0] == '/';
#endif
}

inline String GetFileName(const char* path) { return String(GetFileNamePos(path)); }
inline String GetFileExt(const char* path)  { const char* e = GetFileExtPos(path); return String(e); }
inline String GetFileTitle(const char* path){ String n = GetFileName(path); int p = n.ReverseFind('.'); return p >= 0 ? n.Left(p) : n; }

inline String AppendExt(const char* path, const char* ext) {
    if(!path) return String();
    String p(path);
    if(!ext || !*ext) return p;
    if(ext[0] != '.') p.Cat('.');
    p.Cat(ext);
    return p;
}
inline String ForceExt(const char* path, const char* ext) {
    if(!path) return String();
    String p(path);
    int dot = p.ReverseFind('.');
    if(dot >= 0) p.Trim(dot);
    return AppendExt(p.Begin(), ext);
}

inline String GetFileFolder(const char* path) {
    if(!path) return String();
    fs::path p(path);
    return String(p.parent_path().string().c_str());
}

inline String GetFileDirectory(const char* path) {
    String f = GetFileFolder(path);
    if(f.IsEmpty()) return f;
    if(!f.EndsWith("/") && !f.EndsWith("\\")) f.Cat('/');
    return f;
}

inline String AppendFileName(const String& path, const char* filename) {
    fs::path p((std::string)path);
    p /= filename ? filename : "";
    return String(p.string().c_str());
}

inline String UnixPath(const char* path) {
    if(!path) return String();
    std::string t(path);
    for(char& c : t) if(c == '\\') c = '/';
    return String(t.c_str());
}
inline String WinPath(const char* path) {
    if(!path) return String();
    std::string t(path);
    for(char& c : t) if(c == '/') c = '\\';
    return String(t.c_str());
}
#ifdef _WIN32
inline String NativePath(const char* path) { return WinPath(path); }
#else
inline String NativePath(const char* path) { return UnixPath(path); }
#endif

inline bool FileExists(const char* path) { return path && fs::exists(fs::u8path(path)) && fs::is_regular_file(fs::u8path(path)); }
inline bool DirectoryExists(const char* path) { return path && fs::exists(fs::u8path(path)) && fs::is_directory(fs::u8path(path)); }
inline int64 GetFileLength(const char* path) { if(!FileExists(path)) return 0; return (int64)fs::file_size(fs::u8path(path)); }

inline FileTime GetFileTime(const char* path) {
    std::error_code ec;
    auto ftp = fs::last_write_time(fs::u8path(path), ec);
    if(ec) return FileTime{};
    return FileTime{ filetime_to_sys(ftp) };
}
inline Time FileGetTime(const char* path) {
    FileTime ft = GetFileTime(path);
    if(!ft) return Time();
    std::time_t t = std::chrono::system_clock::to_time_t(ft.tp);
    std::tm lt{};
#ifdef _WIN32
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    return Time(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
}
inline bool SetFileTime(const char* path, FileTime ft) {
    std::error_code ec;
    fs::last_write_time(fs::u8path(path), sys_to_filetime(ft.tp), ec);
    return !ec;
}
inline bool FileSetTime(const char* path, Time time) {
    // Interpret Time as local time; convert to system_clock
    std::tm lt{};
    lt.tm_year = time.year - 1900; lt.tm_mon = time.month - 1; lt.tm_mday = time.day;
    lt.tm_hour = time.hour; lt.tm_min = time.minute; lt.tm_sec = time.second; lt.tm_isdst = -1;
    std::time_t tt = std::mktime(&lt);
    FileTime ft{ std::chrono::system_clock::from_time_t(tt) };
    return SetFileTime(path, ft);
}
inline FileTime TimeToFileTime(Time time) {
    std::tm lt{};
    lt.tm_year = time.year - 1900; lt.tm_mon = time.month - 1; lt.tm_mday = time.day;
    lt.tm_hour = time.hour; lt.tm_min = time.minute; lt.tm_sec = time.second; lt.tm_isdst = -1;
    std::time_t tt = std::mktime(&lt);
    return FileTime{ std::chrono::system_clock::from_time_t(tt) };
}

inline bool DirectoryCreate(const char* path) { return fs::create_directory(fs::u8path(path)); }
inline bool RealizeDirectory(const String& path) { return fs::create_directories(fs::u8path((std::string)path)); }
inline bool RealizePath(const String& path) { return RealizeDirectory(GetFileFolder(path.Begin())); }

inline bool FileCopy(const char* oldpath, const char* newpath) { return fs::copy_file(fs::u8path(oldpath), fs::u8path(newpath), fs::copy_options::overwrite_existing); }
inline bool FileMove(const char* oldpath, const char* newpath) { std::error_code ec; fs::rename(fs::u8path(oldpath), fs::u8path(newpath), ec); return !ec; }
inline bool FileDelete(const char* path) { std::error_code ec; return fs::remove(fs::u8path(path), ec); }
inline bool DirectoryDelete(const char* path) { std::error_code ec; return fs::remove(fs::u8path(path), ec); }
inline bool DeleteFolderDeep(const char* dir, bool /*rdonly*/ = false) { std::error_code ec; fs::remove_all(fs::u8path(dir), ec); return !ec; }

inline String GetCurrentDirectory() { return String(fs::current_path().string().c_str()); }
inline bool ChangeCurrentDirectory(const char* path) { std::error_code ec; fs::current_path(fs::u8path(path), ec); return !ec; }

inline String NormalizePath(const char* path) {
    if(!path) return String();
    fs::path p = fs::u8path(path);
    p = p.lexically_normal();
    return String(p.string().c_str());
}
inline String NormalizePath(const char* path, const char* /*currdir*/) { return NormalizePath(path); }

inline String NormalizeUnixPath(const char* path, const char* currdir) { return NormalizePath(path); }
inline String NormalizeCpmPath(const char* path, const char* currdir) { return NormalizePath(path); }

inline bool PathIsEqual(const char* p1, const char* p2) {
    if(!p1 || !p2) return p1 == p2;
    String a = NormalizePath(p1);
    String b = NormalizePath(p2);
#ifdef _WIN32
    // case-insensitive on Windows
    std::string sa = (std::string)a; std::string sb = (std::string)b;
    std::transform(sa.begin(), sa.end(), sa.begin(), [](unsigned char c){ return (char)std::tolower(c); });
    std::transform(sb.begin(), sb.end(), sb.begin(), [](unsigned char c){ return (char)std::tolower(c); });
    return sa == sb;
#else
    return a == b;
#endif
}

enum { FINDALLFILES = 1, FINDALLFOLDERS = 2 };

inline Vector<String> FindAllPaths(const String& dir, const char* patterns = "*", dword opt = FINDALLFILES) {
    Vector<String> out;
    fs::path root((std::string)dir);
    if(!fs::exists(root) || !fs::is_directory(root)) return out;
    for(auto& e : fs::directory_iterator(root)) {
        bool is_dir = e.is_directory();
        bool accept = ((opt & FINDALLFOLDERS) && is_dir) || ((opt & FINDALLFILES) && e.is_regular_file());
        if(!accept) continue;
        std::string name = e.path().filename().string();
        if(patterns && !PatternMatchMulti(patterns, name.c_str())) continue;
        out.Add(String(e.path().string().c_str()));
    }
    return out;
}

inline String GetSymLinkPath(const char* linkpath) {
    std::error_code ec;
    fs::path p = fs::u8path(linkpath);
    if(!fs::is_symlink(p, ec)) return String();
    fs::path t = fs::read_symlink(p, ec);
    if(ec) return String();
    return String(t.string().c_str());
}

inline String GetTempPath() {
    std::error_code ec; fs::path p = fs::temp_directory_path(ec); if(ec) return String("/tmp"); return String(p.string().c_str());
}
inline String GetTempFileName(const char* prefix = NULL) {
    fs::path dir = fs::temp_directory_path();
    std::string pre = prefix ? prefix : "tmp";
    Uuid u = Uuid::Create();
    std::string uname = pre + std::string("-") + u.ToString().ToStd();
    fs::path name = dir / fs::path(uname);
    return String(name.string().c_str());
}

inline String GetFileOnPath(const char* file, const char* paths, bool current = true, const char* curdir = NULL) {
    if(current) {
        String base = curdir ? String(curdir) : GetCurrentDirectory();
        String p = AppendFileName(base, file);
        if(FileExists(p.Begin())) return p;
    }
    if(!paths) return String();
    String ps(paths);
    int start = 0;
    for(int i = 0; i <= ps.GetLength(); ++i) {
        if(i == ps.GetLength() || ps[i] == ';') {
            String one = ps.Mid(start, i - start);
            if(!one.IsEmpty()) {
                String p = AppendFileName(one, file);
                if(FileExists(p.Begin())) return p;
            }
            start = i + 1;
        }
    }
    return String();
}

inline int GetTimeZone() {
    std::time_t now = std::time(nullptr);
    std::tm lt{}, gt{};
#ifdef _WIN32
    localtime_s(&lt, &now);
    gmtime_s(&gt, &now);
#else
    localtime_r(&now, &lt);
    gmtime_r(&now, &gt);
#endif
    std::time_t l = std::mktime(&lt);
    std::time_t g = std::mktime(&gt); // interprets as local, yields shifted
    long diff = (long)std::difftime(l, g); // seconds local - UTC
    return (int)(diff / 60);
}
inline String GetTimeZoneText() {
    int m = GetTimeZone();
    int sign = m >= 0 ? +1 : -1; m = std::abs(m);
    int hh = m / 60, mm = m % 60;
    std::ostringstream os; os << (sign>0?'+':'-') << std::setw(2) << std::setfill('0') << hh << ':' << std::setw(2) << std::setfill('0') << mm;
    return String(os.str().c_str());
}
inline int ScanTimeZoneText(const char* s) {
    if(!s) return 0;
    while(std::isspace((unsigned char)*s)) ++s;
    if(std::strncmp(s, "UTC", 3) == 0) { s += 3; }
    if(*s == 'Z' || *s == 'z') return 0;
    int sign = 1;
    if(*s == '+') { sign = 1; ++s; }
    else if(*s == '-') { sign = -1; ++s; }
    int hh = 0, mm = 0;
    const char* p = s;
    while(std::isdigit((unsigned char)*p)) { hh = hh*10 + (*p - '0'); ++p; }
    if(*p == ':') { ++p; }
    while(std::isdigit((unsigned char)*p)) { mm = mm*10 + (*p - '0'); ++p; }
    return sign * (hh*60 + mm);
}
inline int ScanTimeZone(const char* s) { return ScanTimeZoneText(s); }

inline String GetFileOnPath(const String& file, const char* paths, bool current = true, const char* curdir = NULL) {
    return GetFileOnPath(file.Begin(), paths, current, curdir);
}

inline bool CreateSymLink(const char* target, const char* linkpath, bool directory = false) {
    std::error_code ec;
    if(directory) fs::create_directory_symlink(fs::u8path(target), fs::u8path(linkpath), ec);
    else fs::create_symlink(fs::u8path(target), fs::u8path(linkpath), ec);
    return !ec;
}

// --- File attribute helpers ---
inline bool FileIsSymlink(const char* path) { std::error_code ec; return fs::is_symlink(fs::u8path(path), ec); }
inline bool FileIsHidden(const char* path) {
    const char* base = GetFileNamePos(path);
    return base && base[0] == '.';
}
inline bool FileCanRead(const char* path) {
    std::error_code ec; auto st = fs::status(fs::u8path(path), ec); if(ec) return false;
    auto p = st.permissions();
    return (p & (fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read)) != fs::perms::none;
}
inline bool FileCanWrite(const char* path) {
    std::error_code ec; auto st = fs::status(fs::u8path(path), ec); if(ec) return false;
    auto p = st.permissions();
    return (p & (fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write)) != fs::perms::none;
}
inline bool FileCanExecute(const char* path) {
    std::error_code ec; auto st = fs::status(fs::u8path(path), ec); if(ec) return false;
    auto p = st.permissions();
#ifdef _WIN32
    // Approximate: Windows executables by extension
    String ext = GetFileExt(path);
    return ext == "exe" || ext == "bat" || ext == "cmd" || ext == "com";
#else
    return (p & (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec)) != fs::perms::none;
#endif
}
inline bool FileIsExecutable(const char* path) { return FileCanExecute(path); }
inline bool FileIsReadOnly(const char* path) { return FileExists(path) && !FileCanWrite(path); }
inline bool SetReadOnly(const char* path, bool ro = true) {
    std::error_code ec; auto p = fs::status(fs::u8path(path), ec).permissions(); if(ec) return false;
    if(ro) p &= ~(fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write);
    else p |= (fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write);
    fs::permissions(fs::u8path(path), p, ec);
    return !ec;
}
