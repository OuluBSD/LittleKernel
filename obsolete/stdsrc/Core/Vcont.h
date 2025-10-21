// Minimal U++-style Vector wrapper on top of std::vector
// This header is aggregated and wrapped into namespace Upp by Core.h

template <class T>
class Vector {
    std::vector<T> v;
public:
    using value_type = T;

    // Basic ops
    void  Clear() { v.clear(); }
    bool  IsEmpty() const { return v.empty(); }
    int   GetCount() const { return static_cast<int>(v.size()); }
    void  Reserve(int n) { v.reserve(static_cast<size_t>(n)); }
    void  Shrink() { v.shrink_to_fit(); }

    // Indexing
    T&       operator[](int i) { return v[static_cast<size_t>(i)]; }
    const T& operator[](int i) const { return v[static_cast<size_t>(i)]; }

    // Insert/append
    int  Add(const T& x) { v.push_back(x); return GetCount() - 1; }
    T&   Add() { v.emplace_back(); return v.back(); }
    void Insert(int i, const T& x) { v.insert(v.begin() + static_cast<ptrdiff_t>(i), x); }
    void Insert(int i, int count, const T& x) { v.insert(v.begin() + static_cast<ptrdiff_t>(i), static_cast<size_t>(count), x); }

    // Remove
    void Remove(int i, int count = 1) {
        if(count <= 0) return;
        auto b = v.begin() + static_cast<ptrdiff_t>(i);
        auto e = b + static_cast<ptrdiff_t>(count);
        if(b < v.begin()) b = v.begin();
        if(e > v.end()) e = v.end();
        if(b < e) v.erase(b, e);
    }

    // Misc
    T&       Top() { return v.back(); }
    const T& Top() const { return v.back(); }

    // Iteration (for-range compatibility)
    auto begin() { return v.begin(); }
    auto end() { return v.end(); }
    auto begin() const { return v.begin(); }
    auto end() const { return v.end(); }
};

