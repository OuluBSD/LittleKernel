// Minimal U++-style Index<T> implemented over std::vector + std::unordered_map
// This header is aggregated and wrapped into namespace Upp by Core.h

template <class T>
class Index {
    std::vector<T> items;
    std::map<T, int> pos; // key -> index

    void rebuild() {
        pos.clear();
        for (int i = 0; i < (int)items.size(); ++i)
            pos[items[(size_t)i]] = i;
    }
public:
    // Basic info
    int  GetCount() const { return static_cast<int>(items.size()); }
    bool IsEmpty() const { return items.empty(); }
    void Clear() { items.clear(); pos.clear(); }
    void Reserve(int n) { items.reserve(static_cast<size_t>(n)); }
    void Shrink() { items.shrink_to_fit(); }

    // Access
    const T& operator[](int i) const { return items[(size_t)i]; }
    T&       operator[](int i)       { return items[(size_t)i]; }
    const T& Top() const { return items.back(); }
    T        Pop() { T x = std::move(items.back()); items.pop_back(); rebuild(); return x; }

    // Find
    int Find(const T& k) const {
        auto it = pos.find(k);
        return it == pos.end() ? -1 : it->second;
    }
    bool Has(const T& k) const { return pos.find(k) != pos.end(); }

    // Add/Put
    void Add(const T& k) {
        if (pos.find(k) != pos.end()) return; // keep unique
        int i = GetCount();
        items.push_back(k);
        pos.emplace(items.back(), i);
    }
    void Add(T&& k) {
        if (pos.find(k) != pos.end()) return;
        int i = GetCount();
        items.push_back(std::move(k));
        pos.emplace(items.back(), i);
    }
    int Insert(int i, const T& k) { if(pos.find(k)!=pos.end()) return Find(k); items.insert(items.begin()+ (ptrdiff_t)i, k); rebuild(); return i; }
    int Insert(int i, T&& k) { if(pos.find(k)!=pos.end()) return Find(k); items.insert(items.begin()+ (ptrdiff_t)i, std::move(k)); rebuild(); return i; }
    int FindAdd(const T& k) { int i = Find(k); if(i >= 0) return i; Add(k); return GetCount() - 1; }
    int FindAdd(T&& k) { int i = Find(k); if(i >= 0) return i; Add(std::move(k)); return GetCount() - 1; }
    int Put(const T& k) { return FindAdd(k); }
    int Put(T&& k) { return FindAdd(std::move(k)); }

    // Modify
    void Set(int i, const T& k) {
        if (i < 0 || i >= GetCount()) return;
        if(pos.find(k) != pos.end() && pos[k] != i) return; // keep unique
        items[(size_t)i] = k; rebuild();
    }
    void Set(int i, T&& k) {
        if (i < 0 || i >= GetCount()) return;
        auto it = pos.find(k);
        if(it != pos.end() && it->second != i) return;
        items[(size_t)i] = std::move(k); rebuild();
    }

    // Remove
    int RemoveKey(const T& k) {
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
    void Trim(int n = 0) {
        if (n < 0) n = 0; if (n >= GetCount()) return;
        items.resize(static_cast<size_t>(n));
        rebuild();
    }
    void Drop(int n = 1) { Trim(GetCount() - n); }

    // Iteration
    auto begin() { return items.begin(); }
    auto end() { return items.end(); }
    auto begin() const { return items.begin(); }
    auto end() const { return items.end(); }

    // Helpers
    bool RemoveKey(const T& k, int& idx) { idx = Find(k); if(idx < 0) return false; Remove(idx); return true; }
    void SwapRemove(int i) { int n = GetCount(); if(i < 0 || i >= n) return; if(i != n-1) std::swap(items[(size_t)i], items.back()); items.pop_back(); rebuild(); }
    template <class Pred>
    int RemoveIf(Pred p) { int removed = 0; for(size_t i=0;i<items.size();) { if(p(items[i])) { items.erase(items.begin()+(ptrdiff_t)i); ++removed; } else ++i; } if(removed) rebuild(); return removed; }
    const std::vector<T>& Keys() const { return items; }
    void Swap(int i, int j) { if(i==j) return; std::swap(items[(size_t)i], items[(size_t)j]); rebuild(); }
    void Move(int i, int j) { int n=GetCount(); if(i<0||i>=n||j<0) return; if(j>n) j=n; if(i==j) return; T elem = std::move(items[(size_t)i]); items.erase(items.begin()+(ptrdiff_t)i); if(j>i) --j; items.insert(items.begin()+(ptrdiff_t)j, std::move(elem)); rebuild(); }
};
