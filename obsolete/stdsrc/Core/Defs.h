// Minimal foundational typedefs and helpers to ease U++-style source compatibility
// Aggregated by Core.h; do not include system headers here.

// Basic sized aliases
using byte  = unsigned char;
using word  = unsigned short;
using dword = unsigned int;
using qword = unsigned long long;
using int16 = short;
using int64 = long long;
using wchar = wchar_t;
using char16 = char16_t;

// Moveable markers (no-op in STL-backed variant)
template <class T>
struct Moveable {};

template <class T>
struct NoCopy { protected: NoCopy() = default; ~NoCopy() = default; NoCopy(const NoCopy&) = delete; void operator=(const NoCopy&) = delete; };

template <class T>
struct MoveableAndDeepCopyOption {};

// pick helper maps to std::move
template <class T>
constexpr T&& pick(T& t) noexcept { return static_cast<T&&>(t); }

// ASSERT macro placeholder (can hook to standard assert later)
#ifndef ASSERT
#define ASSERT(x) do { (void)sizeof(x); } while(0)
#endif

// Swap helper
template <class A, class B>
inline void Swap(A& a, B& b) { using std::swap; swap(a, b); }
