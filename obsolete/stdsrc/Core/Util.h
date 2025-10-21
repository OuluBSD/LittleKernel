// Minimal utility helpers compatible with common U++ usage

inline String FormatPtr(const void* p) {
    std::ostringstream os;
    os << "0x" << std::hex << (uintptr_t)p;
    return String(os.str().c_str());
}

template <class T>
inline String AsString(const T& x) {
    std::ostringstream os;
    os << x;
    return String(os.str().c_str());
}

inline String AsString(const String& s) { return s; }
inline String AsString(const WString& s) { return s.ToString(); }

