// Minimal hashing helpers compatible with U++ expectations
// This header is aggregated and wrapped into namespace Upp by Core.h

using hash_t = std::size_t;

inline std::size_t CombineHash(std::size_t a, std::size_t b) {
    // 64-bit mix (works fine on 32-bit too)
    a ^= b + 0x9e3779b97f4a7c15ull + (a << 6) + (a >> 2);
    return a;
}

template <class T>
inline hash_t GetHashValue(const T& x) {
    return std::hash<T>{}(x);
}

inline hash_t GetHashValue(const String& s) { return std::hash<std::string>{}(static_cast<const std::string&>(s)); }
inline hash_t GetHashValue(const WString& s) { return std::hash<std::wstring>{}(static_cast<const std::wstring&>(s)); }

