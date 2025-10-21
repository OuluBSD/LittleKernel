// Minimal sorting helpers over STL

template <class It, class Cmp>
inline void Sort(It b, It e, Cmp cmp) { std::sort(b, e, cmp); }

template <class It>
inline void Sort(It b, It e) { std::sort(b, e); }

template <class Cont>
inline void Sort(Cont& c) { std::sort(c.begin(), c.end()); }

template <class Cont, class Cmp>
inline void Sort(Cont& c, Cmp cmp) { std::sort(c.begin(), c.end(), cmp); }

template <class It>
inline void StableSort(It b, It e) { std::stable_sort(b, e); }

template <class It, class Cmp>
inline void StableSort(It b, It e, Cmp cmp) { std::stable_sort(b, e, cmp); }

template <class It, class T, class Cmp>
inline It LowerBound(It b, It e, const T& v, Cmp cmp) { return std::lower_bound(b, e, v, cmp); }

template <class It, class T>
inline It LowerBound(It b, It e, const T& v) { return std::lower_bound(b, e, v); }

template <class It, class T>
inline It UpperBound(It b, It e, const T& v) { return std::upper_bound(b, e, v); }

template <class It, class T>
inline bool BinarySearch(It b, It e, const T& v) { return std::binary_search(b, e, v); }

