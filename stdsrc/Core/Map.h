// Minimal U++-style VectorMap<K,T> implemented on top of std::vector + std::unordered_map
// This header is aggregated and wrapped into namespace Upp by Core.h

template <class K, class T>
class VectorMap {
    std::vector<std::pair<K, T>> items;      // preserves order
    std::map<K, int>              pos;       // key -> index

    void rebuild() {
        pos.clear();
        for (int i = 0; i < (int)items.size(); ++i)
            pos[items[(size_t)i].first] = i;
    }
public:
    // Basic info
    int  GetCount() const { return static_cast<int>(items.size()); }
    bool IsEmpty() const { return items.empty(); }
    void Clear() { items.clear(); pos.clear(); }
    void Reserve(int n) { items.reserve(static_cast<size_t>(n)); }
    void Shrink() { items.shrink_to_fit(); }

    // Access by index
    T&       operator[](int i) { return items[(size_t)i].second; }
    const T& operator[](int i) const { return items[(size_t)i].second; }
    const K& GetKey(int i) const { return items[(size_t)i].first; }
    K&       Key(int i) { return items[(size_t)i].first; }
    T&       GetValue(int i) { return items[(size_t)i].second; }
    const T& GetValue(int i) const { return items[(size_t)i].second; }

    // Access by key (inserts default if missing)
    T& operator[](const K& k) { return GetAdd(k); }

    // Find
    int Find(const K& k) const {
        auto it = pos.find(k);
        return it == pos.end() ? -1 : it->second;
    }
    bool Has(const K& k) const { return pos.find(k) != pos.end(); }
    bool Contains(const K& k) const { return Has(k); }
    T*   TryGet(const K& k) { int i = Find(k); return i >= 0 ? &items[(size_t)i].second : nullptr; }
    const T* TryGet(const K& k) const { int i = Find(k); return i >= 0 ? &items[(size_t)i].second : nullptr; }

    // Add
    T& Add(const K& k) {
        int i = GetCount();
        items.emplace_back(k, T{});
        pos[items.back().first] = i;
        return items.back().second;
    }
    T& Add(const K& k, const T& v) {
        int i = GetCount();
        items.emplace_back(k, v);
        pos[items.back().first] = i;
        return items.back().second;
    }
    T& Add(K&& k, T&& v) {
        int i = GetCount();
        items.emplace_back(std::move(k), std::move(v));
        pos[items.back().first] = i;
        return items.back().second;
    }
    // Insert at index
    int Insert(int i, const K& k, const T& v) {
        items.insert(items.begin() + (ptrdiff_t)i, std::make_pair(k, v));
        rebuild();
        return i;
    }
    int Insert(int i, K&& k, T&& v) {
        items.insert(items.begin() + (ptrdiff_t)i, std::make_pair(std::move(k), std::move(v)));
        rebuild();
        return i;
    }

    // Put: insert if missing, otherwise overwrite value; returns position
    int Put(const K& k, const T& v) {
        int i = Find(k);
        if(i < 0) { Add(k, v); return GetCount() - 1; }
        items[(size_t)i].second = v; return i;
    }
    int Put(K&& k, T&& v) {
        int i = Find(k);
        if(i < 0) { Add(std::move(k), std::move(v)); return GetCount() - 1; }
        items[(size_t)i].second = std::move(v); return i;
    }
    bool TrySet(const K& k, const T& v) { int i = Find(k); if(i < 0) return false; items[(size_t)i].second = v; return true; }

    // Get with auto-add
    T& GetAdd(const K& k) {
        int i = Find(k);
        if(i >= 0) return items[(size_t)i].second;
        return Add(k);
    }
    T& GetAdd(const K& k, const T& init) {
        int i = Find(k);
        if(i >= 0) return items[(size_t)i].second;
        return Add(k, init);
    }
    int FindAdd(const K& k) { int i = Find(k); if(i >= 0) return i; Add(k); return GetCount() - 1; }
    template <class Factory>
    T& Ensure(const K& k, Factory make) { int i = Find(k); if(i >= 0) return items[(size_t)i].second; items.emplace_back(k, make()); rebuild(); return items.back().second; }

    const T& Get(const K& k, const T& def) const {
        int i = Find(k);
        return i >= 0 ? items[(size_t)i].second : def;
    }

    // Remove
    int RemoveKey(const K& k) {
        int i = Find(k);
        if(i < 0) return -1;
        Remove(i);
        return i;
    }
    void Remove(int i, int count = 1) {
        if (count <= 0) return;
        int n = GetCount();
        if (i < 0) i = 0; if (i > n) return; if (i + count > n) count = n - i;
        items.erase(items.begin() + i, items.begin() + i + count);
        rebuild();
    }
    // Fast remove by swapping with last
    void SwapRemove(int i) { int n = GetCount(); if(i < 0 || i >= n) return; if(i != n-1) std::swap(items[(size_t)i], items.back()); items.pop_back(); rebuild(); }
    void Trim(int n = 0) {
        if (n < 0) n = 0; if (n >= GetCount()) return;
        items.resize(static_cast<size_t>(n));
        rebuild();
    }
    void Drop(int n = 1) { Trim(GetCount() - n); }

    // Reordering and bulk
    void Swap(int i, int j) { if(i==j) return; std::swap(items[(size_t)i], items[(size_t)j]); rebuild(); }
    void Move(int i, int j) {
        int n = GetCount(); if(i<0||i>=n||j<0) return; if(j>n) j = n; if(i==j) return;
        auto elem = std::move(items[(size_t)i]);
        items.erase(items.begin() + (ptrdiff_t)i);
        if(j > i) --j; // after erase the indices shift
        items.insert(items.begin() + (ptrdiff_t)j, std::move(elem));
        rebuild();
    }
    template <class It>
    void Append(It b, It e, bool overwrite = true) {
        for(; b != e; ++b) {
            const K& k = b->first; const T& v = b->second;
            int idx = Find(k);
            if(idx < 0) Add(k, v);
            else if(overwrite) items[(size_t)idx].second = v;
        }
    }
    void Append(const VectorMap& m, bool overwrite = true) { Append(m.begin(), m.end(), overwrite); }

    // Iteration over pairs
    auto begin() { return items.begin(); }
    auto end() { return items.end(); }
    auto begin() const { return items.begin(); }
    auto end() const { return items.end(); }

    // Views and predicates
    std::vector<K> Keys() const { std::vector<K> out; out.reserve(items.size()); for(auto& kv : items) out.push_back(kv.first); return out; }
    std::vector<T*> ValuesPtr() { std::vector<T*> out; out.reserve(items.size()); for(auto& kv : items) out.push_back(&kv.second); return out; }
    template <class Pred>
    int RemoveIf(Pred p) {
        int removed = 0;
        for(size_t i = 0; i < items.size();) {
            if(p(items[i].first, items[i].second)) { items.erase(items.begin() + (ptrdiff_t)i); ++removed; }
            else ++i;
        }
        if(removed) rebuild();
        return removed;
    }
};
