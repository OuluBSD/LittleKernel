// U++-compatible String wrapper implemented on top of std::string
// This header is aggregated and wrapped into namespace Upp by Core.h

class WString; // forward declaration

class String : public std::string {
public:
    using Base = std::string;
    using Base::basic_string; // inherit std::string constructors

    // Interop with std::string
    String(const std::string& s) : Base(s) {}
    String(std::string&& s) noexcept : Base(std::move(s)) {}

    // U++-style helpers
    void Clear() { this->clear(); }
    int  GetLength() const { return static_cast<int>(this->size()); }
    bool IsEmpty() const { return this->empty(); }
    bool Is() const { return !this->empty(); }

    const char* Begin() const { return this->data(); }
    const char* End() const { return this->data() + this->size(); }

    int operator[](int i) const { return static_cast<unsigned char>(Base::operator[](static_cast<size_t>(i))); }

    // Append
    void Cat(int c) { this->push_back(static_cast<char>(c)); }
    void Cat(const char* s, int len) { if(len > 0) this->append(s, static_cast<size_t>(len)); }
    void Cat(const char* s) { if(s) this->append(s); }
    void Cat(const String& s) { this->append(s.data(), s.size()); }
    void Cat(int c, int count) { if(count > 0) this->append(static_cast<size_t>(count), static_cast<char>(c)); }

    // Operator sugar
    String& operator+=(char c) { this->push_back(c); return *this; }
    String& operator+=(const char* s) { if(s) this->append(s); return *this; }
    String& operator+=(const String& s) { this->append(s); return *this; }

    // Insert
    void Insert(int pos, int c) { this->insert(this->begin() + static_cast<ptrdiff_t>(pos), static_cast<char>(c)); }
    void Insert(int pos, const char* s, int count) { this->insert(static_cast<size_t>(pos), s, static_cast<size_t>(count)); }
    void Insert(int pos, const String& s) { this->insert(static_cast<size_t>(pos), s); }

    // Trim and reserve
    void Trim(int pos) { if(pos < 0) pos = 0; if(static_cast<size_t>(pos) < this->size()) this->resize(static_cast<size_t>(pos)); }
    void TrimLast(int count = 1) { if(count <= 0) return; if(this->size() >= static_cast<size_t>(count)) this->resize(this->size() - static_cast<size_t>(count)); else this->clear(); }
    void Reserve(int r) { this->reserve(static_cast<size_t>(r)); }
    void Shrink() { this->shrink_to_fit(); }

    // Compare / equality
    int  Compare(const String& s) const { return this->compare(s); }
    int  Compare(const char* s) const { return this->compare(s ? s : ""); }
    bool IsEqual(const String& s) const { return *this == s; }
    bool IsEqual(const char* s) const { return this->compare(s ? s : "") == 0; }

    // Slices
    String Mid(int pos, int length) const { return String(this->substr(static_cast<size_t>(pos), static_cast<size_t>(length))); }
    String Mid(int pos) const { return String(this->substr(static_cast<size_t>(pos))); }
    String Right(int count) const { return count <= 0 ? String() : String(this->substr(this->size() - static_cast<size_t>(count))); }
    String Left(int count) const { return count <= 0 ? String() : String(this->substr(0, static_cast<size_t>(count))); }

    // Find helpers
    int Find(int chr, int from = 0) const {
        size_t p = this->find(static_cast<char>(chr), static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int ReverseFind(int chr, int from) const {
        if(from < 0) return -1;
        if(from >= GetLength()) from = GetLength() - 1;
        size_t p = this->rfind(static_cast<char>(chr), static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int ReverseFind(int chr) const {
        size_t p = this->rfind(static_cast<char>(chr));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }

    int Find(const char* s, int from = 0) const {
        size_t p = this->find(s ? s : "", static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int Find(const String& s, int from = 0) const {
        size_t p = this->find(s, static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int FindAfter(const char* s, int from = 0) const {
        if(!s) return -1;
        int n = static_cast<int>(std::char_traits<char>::length(s));
        int q = Find(s, from);
        return q < 0 ? -1 : q + n;
    }
    int FindAfter(const String& s, int from = 0) const {
        int q = Find(s, from);
        return q < 0 ? -1 : q + static_cast<int>(s.size());
    }

    bool StartsWith(const char* s, int len) const {
        if(!s) return false; if(len < 0) return false; if(static_cast<size_t>(len) > this->size()) return false;
        return this->compare(0, static_cast<size_t>(len), s, static_cast<size_t>(len)) == 0;
    }
    bool StartsWith(const char* s) const {
        if(!s) return false; size_t len = std::char_traits<char>::length(s);
        return StartsWith(s, static_cast<int>(len));
    }
    bool StartsWith(const String& s) const { return StartsWith(s.data(), static_cast<int>(s.size())); }

    bool EndsWith(const char* s, int len) const {
        if(!s) return false; if(len < 0) return false; if(static_cast<size_t>(len) > this->size()) return false;
        return this->compare(this->size() - static_cast<size_t>(len), static_cast<size_t>(len), s, static_cast<size_t>(len)) == 0;
    }
    bool EndsWith(const char* s) const {
        if(!s) return false; size_t len = std::char_traits<char>::length(s);
        return EndsWith(s, static_cast<int>(len));
    }
    bool EndsWith(const String& s) const { return EndsWith(s.data(), static_cast<int>(s.size())); }

    // Conversions
    std::string ToStd() const { return *this; }
    const String& ToString() const { return *this; }
    WString ToWString() const;
    int GetCharCount() const { return GetLength(); }
    static String GetVoid() { return String(); }
    bool IsVoid() const { return false; }
};
