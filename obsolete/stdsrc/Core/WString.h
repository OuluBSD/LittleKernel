// U++-compatible WString wrapper implemented on top of std::wstring
// This header is aggregated and wrapped into namespace Upp by Core.h

class WString : public std::wstring {
public:
    using Base = std::wstring;
    using Base::basic_string; // inherit std::wstring constructors
    

    // Interop with std::wstring
    WString(const std::wstring& s) : Base(s) {}
    WString(std::wstring&& s) noexcept : Base(std::move(s)) {}

    // U++-style helpers
    void Clear() { this->clear(); }
    int  GetLength() const { return static_cast<int>(this->size()); }
    bool IsEmpty() const { return this->empty(); }
    bool Is() const { return !this->empty(); }

    const wchar_t* Begin() const { return this->data(); }
    const wchar_t* End() const { return this->data() + this->size(); }

    int operator[](int i) const { return static_cast<unsigned int>(Base::operator[](static_cast<size_t>(i))); }

    // Append
    void Cat(int c) { this->push_back(static_cast<wchar_t>(c)); }
    void Cat(const wchar_t* s, int len) { if(len > 0) this->append(s, static_cast<size_t>(len)); }
    void Cat(const wchar_t* s) { if(s) this->append(s); }
    void Cat(const WString& s) { this->append(s.data(), s.size()); }
    void Cat(int c, int count) { if(count > 0) this->append(static_cast<size_t>(count), static_cast<wchar_t>(c)); }

    // Operator sugar
    WString& operator+=(wchar_t c) { this->push_back(c); return *this; }
    WString& operator+=(const wchar_t* s) { if(s) this->append(s); return *this; }
    WString& operator+=(const WString& s) { this->append(s); return *this; }

    // Insert
    void Insert(int pos, int c) { this->insert(this->begin() + static_cast<ptrdiff_t>(pos), static_cast<wchar_t>(c)); }
    void Insert(int pos, const wchar_t* s, int count) { this->insert(static_cast<size_t>(pos), s, static_cast<size_t>(count)); }
    void Insert(int pos, const WString& s) { this->insert(static_cast<size_t>(pos), s); }

    // Trim and reserve
    void Trim(int pos) { if(pos < 0) pos = 0; if(static_cast<size_t>(pos) < this->size()) this->resize(static_cast<size_t>(pos)); }
    void TrimLast(int count = 1) { if(count <= 0) return; if(this->size() >= static_cast<size_t>(count)) this->resize(this->size() - static_cast<size_t>(count)); else this->clear(); }
    void Reserve(int r) { this->reserve(static_cast<size_t>(r)); }
    void Shrink() { this->shrink_to_fit(); }

    // Compare / equality
    int  Compare(const WString& s) const { return this->compare(s); }
    int  Compare(const wchar_t* s) const { return this->compare(s ? s : L""); }
    bool IsEqual(const WString& s) const { return *this == s; }
    bool IsEqual(const wchar_t* s) const { return this->compare(s ? s : L"") == 0; }

    // Slices
    WString Mid(int pos, int length) const { return WString(this->substr(static_cast<size_t>(pos), static_cast<size_t>(length))); }
    WString Mid(int pos) const { return WString(this->substr(static_cast<size_t>(pos))); }
    WString Right(int count) const { return count <= 0 ? WString() : WString(this->substr(this->size() - static_cast<size_t>(count))); }
    WString Left(int count) const { return count <= 0 ? WString() : WString(this->substr(0, static_cast<size_t>(count))); }

    // Find helpers
    int Find(int chr, int from = 0) const {
        size_t p = this->find(static_cast<wchar_t>(chr), static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int ReverseFind(int chr, int from) const {
        if(from < 0) return -1;
        if(from >= GetLength()) from = GetLength() - 1;
        size_t p = this->rfind(static_cast<wchar_t>(chr), static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int ReverseFind(int chr) const {
        size_t p = this->rfind(static_cast<wchar_t>(chr));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }

    int Find(const wchar_t* s, int from = 0) const {
        size_t p = this->find(s ? s : L"", static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int Find(const WString& s, int from = 0) const {
        size_t p = this->find(s, static_cast<size_t>(from));
        return p == Base::npos ? -1 : static_cast<int>(p);
    }
    int FindAfter(const wchar_t* s, int from = 0) const {
        if(!s) return -1;
        int n = static_cast<int>(std::wcslen(s));
        int q = Find(s, from);
        return q < 0 ? -1 : q + n;
    }
    int FindAfter(const WString& s, int from = 0) const {
        int q = Find(s, from);
        return q < 0 ? -1 : q + static_cast<int>(s.size());
    }

    bool StartsWith(const wchar_t* s, int len) const {
        if(!s) return false; if(len < 0) return false; if(static_cast<size_t>(len) > this->size()) return false;
        return this->compare(0, static_cast<size_t>(len), s, static_cast<size_t>(len)) == 0;
    }
    bool StartsWith(const wchar_t* s) const {
        if(!s) return false; size_t len = std::wcslen(s);
        return StartsWith(s, static_cast<int>(len));
    }
    bool StartsWith(const WString& s) const { return StartsWith(s.data(), static_cast<int>(s.size())); }

    bool EndsWith(const wchar_t* s, int len) const {
        if(!s) return false; if(len < 0) return false; if(static_cast<size_t>(len) > this->size()) return false;
        return this->compare(this->size() - static_cast<size_t>(len), static_cast<size_t>(len), s, static_cast<size_t>(len)) == 0;
    }
    bool EndsWith(const wchar_t* s) const {
        if(!s) return false; size_t len = std::wcslen(s);
        return EndsWith(s, static_cast<int>(len));
    }
    bool EndsWith(const WString& s) const { return EndsWith(s.data(), static_cast<int>(s.size())); }

    // Conversions (UTF-8 <-> wide)
    // Helpers to convert between UTF-8 and wide using platform facilities
    static std::string WideToUtf8(const std::wstring& ws) {
#ifdef _WIN32
        if(ws.empty()) return std::string();
        int n = WideCharToMultiByte(CP_UTF8, 0, ws.data(), (int)ws.size(), NULL, 0, NULL, NULL);
        std::string out; out.resize(n);
        if(n>0) WideCharToMultiByte(CP_UTF8, 0, ws.data(), (int)ws.size(), out.data(), n, NULL, NULL);
        return out;
#else
        std::mbstate_t st{}; const wchar_t* src = ws.c_str(); size_t need = wcsrtombs(NULL, &src, 0, &st);
        if(need != (size_t)-1){ std::string out; out.resize(need); st = std::mbstate_t{}; src = ws.c_str(); wcsrtombs(out.data(), &src, need, &st); return out; }
        // Fallback: encode as UTF-8 manually (assumes wchar_t is UCS-4)
        std::string out;
        auto append_cp = [&](uint32_t cp){
            if(cp <= 0x7F) out.push_back((char)cp);
            else if(cp <= 0x7FF){ out.push_back((char)(0xC0 | (cp>>6))); out.push_back((char)(0x80 | (cp & 0x3F))); }
            else if(cp <= 0xFFFF){ out.push_back((char)(0xE0 | (cp>>12))); out.push_back((char)(0x80 | ((cp>>6)&0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
            else { out.push_back((char)(0xF0 | (cp>>18))); out.push_back((char)(0x80 | ((cp>>12)&0x3F))); out.push_back((char)(0x80 | ((cp>>6)&0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
        };
        for(wchar_t wc : ws) append_cp((uint32_t)wc);
        return out;
#endif
    }
    static std::wstring Utf8ToWide(const char* s, size_t len) {
#ifdef _WIN32
        if(len == 0) return std::wstring();
        int n = MultiByteToWideChar(CP_UTF8, 0, s, (int)len, NULL, 0);
        std::wstring out; out.resize(n);
        if(n>0) MultiByteToWideChar(CP_UTF8, 0, s, (int)len, out.data(), n);
        return out;
#else
        std::mbstate_t st{}; const char* src = s; size_t need = mbsrtowcs(NULL, &src, 0, &st);
        if(need != (size_t)-1){ std::wstring out; out.resize(need); st = std::mbstate_t{}; src = s; mbsrtowcs(out.data(), &src, need, &st); return out; }
        // Fallback: manual UTF-8 decode to wchar_t (assumes wchar_t is UCS-4)
        std::wstring out;
        out.reserve(len);
        size_t i=0; while(i<len){ unsigned c = (unsigned char)s[i++];
            if(c < 0x80){ out.push_back((wchar_t)c); }
            else if((c>>5)==0x6 && i<len){ unsigned c1 = (unsigned char)s[i++]; out.push_back((wchar_t)(((c&0x1F)<<6)|(c1&0x3F))); }
            else if((c>>4)==0xE && i+1<len){ unsigned c1=(unsigned char)s[i++], c2=(unsigned char)s[i++]; out.push_back((wchar_t)(((c&0x0F)<<12)|((c1&0x3F)<<6)|(c2&0x3F))); }
            else if((c>>3)==0x1E && i+2<len){ unsigned c1=(unsigned char)s[i++], c2=(unsigned char)s[i++], c3=(unsigned char)s[i++]; uint32_t cp = ((c&0x07)<<18)|((c1&0x3F)<<12)|((c2&0x3F)<<6)|(c3&0x3F); out.push_back((wchar_t)cp); }
        }
        return out;
#endif
    }

    String ToString() const { return String(WideToUtf8(*this).c_str()); }
};

// Define String::ToWString now that WString is complete
inline WString String::ToWString() const {
    return WString(WString::Utf8ToWide(Begin(), (size_t)GetLength()));
}
