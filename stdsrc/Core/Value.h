// Minimal Value variant implemented on top of std::any
// This is a simplified shim to cover common cases (int, double, bool, String, WString, const char*)

class Value {
    std::any a;
    bool null = true;
public:
    // Construction
    Value() = default;
    Value(std::nullptr_t) : a(), null(true) {}
    Value(const char* s) : a(String(s ? s : "")), null(false) {}
    Value(const String& s) : a(s), null(false) {}
    Value(const WString& ws) : a(ws), null(false) {}
    Value(bool b) : a(b), null(false) {}
    Value(int v) : a(v), null(false) {}
    Value(long long v) : a((long long)v), null(false) {}
    Value(double v) : a(v), null(false) {}

    template <class T>
    Value(const T& x) : a(x), null(false) {}

    // Null handling
    static Value GetVoid() { return Value(); }
    bool IsVoid() const { return null; }
    bool IsNull() const { return null; }
    void Clear() { a.reset(); null = true; }

    // Assignment
    template <class T>
    Value& operator=(const T& x) { a = x; null = false; return *this; }

    // Access
    template <class T>
    const T& Get() const { return std::any_cast<const T&>(a); }
    template <class T>
    T& Get() { return std::any_cast<T&>(a); }
    template <class T>
    bool Is() const { return !null && a.type() == typeid(T); }

    template <class T>
    bool TryGet(T& out) const noexcept {
        if(Is<T>()) { out = std::any_cast<const T&>(a); return true; }
        return false;
    }
    template <class T>
    T GetOrDefault(const T& def = T{}) const {
        if(Is<T>()) return std::any_cast<const T&>(a);
        return def;
    }
    bool IsNumber() const { return Is<int>() || Is<long long>() || Is<double>(); }
    bool IsStringLike() const { return Is<String>() || Is<WString>() || Is<const char*>(); }

    // String conversion
    String ToString() const {
        if(null) return String();
        if(a.type() == typeid(String)) return std::any_cast<const String&>(a);
        if(a.type() == typeid(WString)) return std::any_cast<const WString&>(a).ToString();
        if(a.type() == typeid(std::string)) return String(std::any_cast<const std::string&>(a).c_str());
        if(a.type() == typeid(const char*)) return String(std::any_cast<const char*>(a));
        if(a.type() == typeid(bool)) return std::any_cast<bool>(a) ? String("true") : String("false");
        if(a.type() == typeid(int)) return String(std::to_string(std::any_cast<int>(a)).c_str());
        if(a.type() == typeid(long long)) return String(std::to_string(std::any_cast<long long>(a)).c_str());
        if(a.type() == typeid(double)) return String(std::to_string(std::any_cast<double>(a)).c_str());
        return String("<value>");
    }

    friend bool operator==(const Value& x, const Value& y) {
        if(x.null != y.null) return false;
        if(x.null) return true;
        if(x.a.type() == y.a.type()) {
            if(x.Is<int>()) return std::any_cast<int>(x.a) == std::any_cast<int>(y.a);
            if(x.Is<long long>()) return std::any_cast<long long>(x.a) == std::any_cast<long long>(y.a);
            if(x.Is<double>()) return std::any_cast<double>(x.a) == std::any_cast<double>(y.a);
            if(x.Is<bool>()) return std::any_cast<bool>(x.a) == std::any_cast<bool>(y.a);
            if(x.Is<String>()) return std::any_cast<String>(x.a) == std::any_cast<String>(y.a);
            if(x.Is<WString>()) return std::any_cast<WString>(x.a) == std::any_cast<WString>(y.a);
        }
        // Fallback to string compare for differing numeric types
        if(x.IsNumber() && y.IsNumber()) return x.ToString() == y.ToString();
        return false;
    }
    friend bool operator!=(const Value& x, const Value& y) { return !(x == y); }
};
