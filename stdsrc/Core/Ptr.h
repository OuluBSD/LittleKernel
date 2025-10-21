// Simplified non-owning Ptr<T> wrapper (no reference counting)

template <class T>
class Ptr {
    T* p = nullptr;
public:
    using Type = T;

    T*       operator->() const { return p; }
    T*       operator~()  const { return p; }
    operator T*()  const { return p; }

    Ptr& operator=(T* v) { p = v; return *this; }
    Ptr& operator=(const Ptr& v) { p = v.p; return *this; }

    Ptr() = default;
    explicit Ptr(T* v) : p(v) {}
    Ptr(const Ptr& v) : p(v.p) {}

    void Clear() { p = nullptr; }
    void SetNull() { p = nullptr; }
    bool IsEmpty() const { return p == nullptr; }
    T*   Get() const { return p; }

    String ToString() const { return FormatPtr(p); }

    friend bool operator==(const Ptr& a, const T* b) { return a.p == b; }
    friend bool operator==(const T* a, const Ptr& b) { return a == b.p; }
    friend bool operator==(const Ptr& a, const Ptr& b){ return a.p == b.p; }
    friend bool operator!=(const Ptr& a, const T* b) { return a.p != b; }
    friend bool operator!=(const T* a, const Ptr& b) { return a != b.p; }
    friend bool operator!=(const Ptr& a, const Ptr& b){ return a.p != b.p; }

    friend bool operator==(const Ptr& a, std::nullptr_t) { return a.p == nullptr; }
    friend bool operator!=(const Ptr& a, std::nullptr_t) { return a.p != nullptr; }
};
