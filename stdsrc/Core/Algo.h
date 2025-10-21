// Minimal algorithm helpers inspired by U++ Core Algo.h

template <class T>
inline const T& Min(const T& a, const T& b) { return a < b ? a : b; }

template <class T>
inline const T& Max(const T& a, const T& b) { return a < b ? b : a; }

template <class T>
inline T Clamp(const T& x, const T& lo, const T& hi) { return x < lo ? lo : (x > hi ? hi : x); }

template <class T>
inline int sgn(const T& x) { return (x > 0) - (x < 0); }

template <class T>
inline bool InRange(const T& x, const T& lo, const T& hi) { return !(x < lo) && !(hi < x); }

