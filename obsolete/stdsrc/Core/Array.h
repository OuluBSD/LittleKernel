// Array<T>: vector of owning pointers for stable element addresses

template <class T>
class Array {
    std::vector<std::unique_ptr<T>> a;

    template <class U>
    static std::unique_ptr<T> make_ptr(U&& u) { return std::unique_ptr<T>(new T(std::forward<U>(u))); }

public:
    using value_type = T;

    Array() = default;
    Array(const Array& other) { // deep copy
        a.reserve(other.a.size());
        for(const auto& p : other.a)
            a.emplace_back(p ? std::unique_ptr<T>(new T(*p)) : nullptr);
    }
    Array(Array&&) = default;
    Array& operator=(Array&&) = default;
    Array& operator=(const Array& other) {
        if(this == &other) return *this;
        Clear();
        a.reserve(other.a.size());
        for(const auto& p : other.a)
            a.emplace_back(p ? std::unique_ptr<T>(new T(*p)) : nullptr);
        return *this;
    }

    // Basic ops
    void  Clear() { a.clear(); }
    bool  IsEmpty() const { return a.empty(); }
    int   GetCount() const { return static_cast<int>(a.size()); }
    void  Reserve(int n) { a.reserve(static_cast<size_t>(n)); }
    void  Shrink() { a.shrink_to_fit(); }

    // Indexing
    T&       operator[](int i) { return *a[static_cast<size_t>(i)]; }
    const T& operator[](int i) const { return *a[static_cast<size_t>(i)]; }
    T*       At(int i) { return (i >= 0 && i < GetCount()) ? a[static_cast<size_t>(i)].get() : nullptr; }
    const T* At(int i) const { return (i >= 0 && i < GetCount()) ? a[static_cast<size_t>(i)].get() : nullptr; }

    // Append / insert
    T&   Add() { a.emplace_back(new T()); return *a.back(); }
    int  Add(const T& x) { a.emplace_back(new T(x)); return GetCount() - 1; }
    int  Add(T&& x) { a.emplace_back(new T(std::move(x))); return GetCount() - 1; }
    int  AddN(int count) { int pos = GetCount(); if(count > 0) { a.reserve(pos + count); for(int i=0;i<count;++i) a.emplace_back(new T()); } return pos; }
    // Take ownership of raw pointer (must be new T)
    int  AddOwned(T* p) { a.emplace_back(std::unique_ptr<T>(p)); return GetCount() - 1; }
    template <class... Args>
    T&   Create(Args&&... args) { a.emplace_back(new T(std::forward<Args>(args)...)); return *a.back(); }
    void Insert(int i, const T& x) { a.insert(a.begin() + static_cast<ptrdiff_t>(i), std::unique_ptr<T>(new T(x))); }
    void Insert(int i, T&& x) { a.insert(a.begin() + static_cast<ptrdiff_t>(i), std::unique_ptr<T>(new T(std::move(x)))); }
    T&   Insert(int i) { a.insert(a.begin() + static_cast<ptrdiff_t>(i), std::unique_ptr<T>(new T())); return *a[static_cast<size_t>(i)]; }
    void InsertOwned(int i, T* p) { a.insert(a.begin() + static_cast<ptrdiff_t>(i), std::unique_ptr<T>(p)); }
    void Insert(int i, int count, const T& x) {
        if(count <= 0) return;
        a.insert(a.begin() + static_cast<ptrdiff_t>(i), static_cast<size_t>(count), nullptr);
        for(int k = 0; k < count; ++k)
            a[static_cast<size_t>(i + k)] = std::unique_ptr<T>(new T(x));
    }
    int  InsertN(int i, int count) {
        if(count <= 0) return i;
        a.insert(a.begin() + static_cast<ptrdiff_t>(i), static_cast<size_t>(count), nullptr);
        for(int k = 0; k < count; ++k)
            a[static_cast<size_t>(i + k)] = std::unique_ptr<T>(new T());
        return i;
    }

    // Remove
    void Remove(int i, int count = 1) {
        if(count <= 0) return;
        auto b = a.begin() + static_cast<ptrdiff_t>(i);
        auto e = b + static_cast<ptrdiff_t>(count);
        if(b < a.begin()) b = a.begin();
        if(e > a.end()) e = a.end();
        if(b < e) a.erase(b, e);
    }

    // Detach: release ownership of element at i and remove it; caller must delete
    T* Detach(int i) {
        if(i < 0 || i >= GetCount()) return nullptr;
        std::unique_ptr<T> p = std::move(a[static_cast<size_t>(i)]);
        a.erase(a.begin() + static_cast<ptrdiff_t>(i));
        return p.release();
    }

    // Pop last element by value (moves/copies), or detach pointer
    T   Pop() { T t = std::move(*a.back()); a.pop_back(); return t; }
    T*  PopDetach() { std::unique_ptr<T> p = std::move(a.back()); a.pop_back(); return p.release(); }

    // SetCount: grow with default-constructed elements or shrink by removing from end
    void SetCount(int n) {
        if(n < 0) n = 0;
        int c = GetCount();
        if(n > c) {
            a.reserve(n);
            for(int i = c; i < n; ++i)
                a.emplace_back(new T());
        }
        else if(n < c) {
            a.erase(a.begin() + n, a.end());
        }
    }

    void Swap(int i, int j) { std::swap(a[static_cast<size_t>(i)], a[static_cast<size_t>(j)]); }
    // Swap with last and remove last (fast removal)
    void SwapRemove(int i) {
        int n = GetCount(); if(i < 0 || i >= n) return; if(i != n-1) std::swap(a[(size_t)i], a.back()); a.pop_back();
    }

    // Search helpers
    int Find(const T& x) const {
        for(size_t i = 0; i < a.size(); ++i) if(*a[i] == x) return (int)i; return -1;
    }
    int FindPtr(const T* p) const {
        for(size_t i = 0; i < a.size(); ++i) if(a[i].get() == p) return (int)i; return -1;
    }
    bool RemovePtr(const T* p) { int i = FindPtr(p); if(i >= 0) { Remove(i); return true; } return false; }

    // Remove elements matching predicate p(T&)
    template <class Pred>
    int RemoveIf(Pred p) {
        int removed = 0;
        for(size_t i = 0; i < a.size();) {
            if(p(*a[i])) { a.erase(a.begin() + (ptrdiff_t)i); ++removed; }
            else ++i;
        }
        return removed;
    }

    // Misc
    T&       Top() { return *a.back(); }
    const T& Top() const { return *a.back(); }

    // Iteration over T& by transforming unique_ptr iterators
    struct iterator {
        using base_it = typename std::vector<std::unique_ptr<T>>::iterator;
        base_it it;
        iterator() = default; iterator(base_it i) : it(i) {}
        T& operator*() const { return *(*it); }
        T* operator->() const { return (*it).get(); }
        iterator& operator++() { ++it; return *this; }
        iterator operator++(int) { iterator tmp(*this); ++it; return tmp; }
        bool operator==(const iterator& o) const { return it == o.it; }
        bool operator!=(const iterator& o) const { return it != o.it; }
    };
    struct const_iterator {
        using base_it = typename std::vector<std::unique_ptr<T>>::const_iterator;
        base_it it;
        const_iterator() = default; const_iterator(base_it i) : it(i) {}
        const T& operator*() const { return *(*it); }
        const T* operator->() const { return (*it).get(); }
        const_iterator& operator++() { ++it; return *this; }
        const_iterator operator++(int) { const_iterator tmp(*this); ++it; return tmp; }
        bool operator==(const const_iterator& o) const { return it == o.it; }
        bool operator!=(const const_iterator& o) const { return it != o.it; }
    };
    iterator begin() { return iterator(a.begin()); }
    iterator end() { return iterator(a.end()); }
    const_iterator begin() const { return const_iterator(a.begin()); }
    const_iterator end() const { return const_iterator(a.end()); }
};
