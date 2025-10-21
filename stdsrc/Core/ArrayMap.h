// ArrayMap<K, T>: ordered map with owning pointer values for stable addresses

template <class K, class T>
class ArrayMap {
    std::vector<K> keys;
    std::vector<std::unique_ptr<T>> vals;
    std::map<K, int> index; // key -> position

    void RebuildIndex() {
        index.clear();
        for(int i = 0; i < (int)keys.size(); ++i)
            index[keys[(size_t)i]] = i;
    }

public:
    using key_type = K;
    using mapped_type = T;

    ArrayMap() = default;
    ArrayMap(const ArrayMap& other) { *this = other; }
    ArrayMap(ArrayMap&&) = default;
    ArrayMap& operator=(ArrayMap&&) = default;
    ArrayMap& operator=(const ArrayMap& other) {
        if(this == &other) return *this;
        keys = other.keys;
        vals.clear(); vals.reserve(other.vals.size());
        for(const auto& p : other.vals)
            vals.emplace_back(p ? std::unique_ptr<T>(new T(*p)) : nullptr);
        RebuildIndex();
        return *this;
    }

    // Basic
    void  Clear() { keys.clear(); vals.clear(); index.clear(); }
    bool  IsEmpty() const { return keys.empty(); }
    int   GetCount() const { return static_cast<int>(keys.size()); }
    void  Reserve(int n) { keys.reserve(n); vals.reserve(n); }

    // Lookup
    int  Find(const K& k) const { auto it = index.find(k); return it == index.end() ? -1 : it->second; }
    bool Contains(const K& k) const { return Find(k) >= 0; }
    int  FindAdd(const K& k) { int i = Find(k); if(i >= 0) return i; Add(k); return GetCount()-1; }
    T*   TryGet(const K& k) { int i = Find(k); return i >= 0 ? vals[(size_t)i].get() : nullptr; }
    const T* TryGet(const K& k) const { int i = Find(k); return i >= 0 ? vals[(size_t)i].get() : nullptr; }
    const K* TryGetKey(const K& k) const { int i = Find(k); return i >= 0 ? &keys[(size_t)i] : nullptr; }

    const K& GetKey(int i) const { return keys[static_cast<size_t>(i)]; }
    K&       Key(int i) { return keys[static_cast<size_t>(i)]; }

    T&       operator[](int i) { return *vals[static_cast<size_t>(i)]; }
    const T& operator[](int i) const { return *vals[static_cast<size_t>(i)]; }

    // Add / GetAdd
    int Add(const K& k, const T& v) {
        int i = GetCount();
        keys.push_back(k);
        vals.emplace_back(new T(v));
        index[k] = i;
        return i;
    }
    T& Add(const K& k) {
        int i = GetCount();
        keys.push_back(k);
        vals.emplace_back(new T());
        index[k] = i;
        return *vals.back();
    }
    int Insert(int i, const K& k, const T& v) {
        keys.insert(keys.begin() + (ptrdiff_t)i, k);
        vals.insert(vals.begin() + (ptrdiff_t)i, std::unique_ptr<T>(new T(v)));
        RebuildIndex();
        return i;
    }
    int Put(const K& k, const T& v) { int i = Find(k); if(i < 0) i = Add(k, v); else *vals[(size_t)i] = v; return i; }
    // Take ownership of raw pointer for value
    int AddOwned(const K& k, T* p) {
        int i = GetCount();
        keys.push_back(k);
        vals.emplace_back(std::unique_ptr<T>(p));
        index[k] = i;
        return i;
    }
    int InsertOwned(int i, const K& k, T* p) {
        keys.insert(keys.begin() + (ptrdiff_t)i, k);
        vals.insert(vals.begin() + (ptrdiff_t)i, std::unique_ptr<T>(p));
        RebuildIndex();
        return i;
    }
    template <class... Args>
    T& Create(const K& k, Args&&... args) {
        int i = GetCount();
        keys.push_back(k);
        vals.emplace_back(new T(std::forward<Args>(args)...));
        index[k] = i;
        return *vals.back();
    }
    T& GetAdd(const K& k) {
        int i = Find(k);
        if(i >= 0) return *vals[(size_t)i];
        return Add(k);
    }
    T Get(const K& k, const T& def) const { int i = Find(k); return i >= 0 ? *vals[(size_t)i] : def; }
    bool TrySet(const K& k, const T& v) { int i = Find(k); if(i < 0) return false; *vals[(size_t)i] = v; return true; }
    template <class Factory>
    T& Ensure(const K& k, Factory make) { int i = Find(k); if(i >= 0) return *vals[(size_t)i]; vals.emplace_back(new T(make())); keys.push_back(k); index[k] = GetCount()-1; return *vals.back(); }

    // Remove
    void Remove(int i, int count = 1) {
        if(count <= 0) return;
        auto b1 = keys.begin() + static_cast<ptrdiff_t>(i);
        auto e1 = b1 + static_cast<ptrdiff_t>(count);
        auto b2 = vals.begin() + static_cast<ptrdiff_t>(i);
        auto e2 = b2 + static_cast<ptrdiff_t>(count);
        if(b1 < keys.begin()) b1 = keys.begin();
        if(e1 > keys.end()) e1 = keys.end();
        if(b2 < vals.begin()) b2 = vals.begin();
        if(e2 > vals.end()) e2 = vals.end();
        if(b1 < e1) keys.erase(b1, e1);
        if(b2 < e2) vals.erase(b2, e2);
        RebuildIndex();
    }
    bool RemoveKey(const K& k) {
        int i = Find(k);
        if(i < 0) return false;
        Remove(i);
        return true;
    }

    // Access by key (creates if missing)
    T& operator[](const K& k) { return GetAdd(k); }

    // Change key at index; returns false if new key would duplicate existing
    bool SetKey(int i, const K& k) {
        if(i < 0 || i >= GetCount()) return false;
        auto it = index.find(k);
        if(it != index.end() && it->second != i)
            return false;
        index.erase(keys[(size_t)i]);
        keys[(size_t)i] = k;
        index[k] = i;
        return true;
    }

    // Iteration as pair-like references
    struct PairRef { const K& key; T& value; };
    struct iterator {
        ArrayMap* m = nullptr; int i = 0;
        iterator() = default; iterator(ArrayMap* mm, int ii) : m(mm), i(ii) {}
        iterator& operator++() { ++i; return *this; }
        iterator operator++(int) { iterator t(*this); ++i; return t; }
        bool operator==(const iterator& o) const { return m==o.m && i==o.i; }
        bool operator!=(const iterator& o) const { return !(*this == o); }
        PairRef operator*() const { return PairRef{ m->keys[(size_t)i], *m->vals[(size_t)i] }; }
    };
    struct const_iterator {
        const ArrayMap* m = nullptr; int i = 0;
        const_iterator() = default; const_iterator(const ArrayMap* mm, int ii) : m(mm), i(ii) {}
        const_iterator& operator++() { ++i; return *this; }
        const_iterator operator++(int) { const_iterator t(*this); ++i; return t; }
        bool operator==(const const_iterator& o) const { return m==o.m && i==o.i; }
        bool operator!=(const const_iterator& o) const { return !(*this == o); }
        PairRef operator*() const { return PairRef{ m->keys[(size_t)i], *m->vals[(size_t)i] }; }
    };
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, GetCount()); }
    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end() const { return const_iterator(this, GetCount()); }

    const std::vector<K>& Keys() const { return keys; }
    std::vector<T*> ValuesPtr() const { std::vector<T*> out; out.reserve(vals.size()); for(auto& p : vals) out.push_back(p.get()); return out; }

    // Remove elements matching predicate p(key, value)
    template <class Pred>
    int RemoveIf(Pred p) {
        int removed = 0;
        for(size_t i = 0; i < keys.size();) {
            if(p(keys[i], *vals[i])) {
                keys.erase(keys.begin() + (ptrdiff_t)i);
                vals.erase(vals.begin() + (ptrdiff_t)i);
                ++removed;
            } else {
                ++i;
            }
        }
        if(removed) RebuildIndex();
        return removed;
    }

    // Erase at iterator and return iterator to next element
    iterator Erase(iterator it) {
        if(it.m != this) return it;
        if(it.i < 0 || it.i >= GetCount()) return iterator(this, GetCount());
        keys.erase(keys.begin() + (ptrdiff_t)it.i);
        vals.erase(vals.begin() + (ptrdiff_t)it.i);
        RebuildIndex();
        return iterator(this, it.i);
    }

    // Reordering and bulk operations
    void Swap(int i, int j) {
        if(i == j) return;
        std::swap(keys[(size_t)i], keys[(size_t)j]);
        std::swap(vals[(size_t)i], vals[(size_t)j]);
        RebuildIndex();
    }
    void Move(int i, int j) {
        int n = GetCount();
        if(i < 0 || i >= n || j < 0) return;
        if(j > n) j = n;
        if(i == j) return;
        K k = std::move(keys[(size_t)i]);
        std::unique_ptr<T> v = std::move(vals[(size_t)i]);
        keys.erase(keys.begin() + (ptrdiff_t)i);
        vals.erase(vals.begin() + (ptrdiff_t)i);
        if(j > i) --j;
        keys.insert(keys.begin() + (ptrdiff_t)j, std::move(k));
        vals.insert(vals.begin() + (ptrdiff_t)j, std::move(v));
        RebuildIndex();
    }
    void SwapRemove(int i) {
        int n = GetCount();
        if(i < 0 || i >= n) return;
        if(i != n - 1) {
            std::swap(keys[(size_t)i], keys.back());
            std::swap(vals[(size_t)i], vals.back());
        }
        keys.pop_back();
        vals.pop_back();
        RebuildIndex();
    }
    template <class It>
    void Append(It b, It e, bool overwrite = true) {
        for(; b != e; ++b) {
            const K& k = b->first;
            const T& v = b->second;
            int i = Find(k);
            if(i < 0)
                Add(k, v);
            else if(overwrite)
                *vals[(size_t)i] = v;
        }
    }
    void Append(const ArrayMap& m, bool overwrite = true) {
        for(int i = 0; i < m.GetCount(); ++i) {
            const K& k = m.GetKey(i);
            const T& v = m[i];
            int p = Find(k);
            if(p < 0)
                Add(k, v);
            else if(overwrite)
                *vals[(size_t)p] = v;
        }
    }
};
