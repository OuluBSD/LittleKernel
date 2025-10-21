// Minimal in-memory Stream shims

class Stream {
public:
    virtual ~Stream() = default;
    virtual size_t Get(void* dst, size_t n) = 0;
    virtual void   Put(const void* src, size_t n) = 0;
    virtual int    Get() { unsigned char b; return Get(&b, 1) == 1 ? (int)b : -1; }
    virtual void   Put(int c) { unsigned char b = (unsigned char)c; Put(&b, 1); }
    virtual bool   IsEof() const = 0;
    virtual size_t GetSize() const = 0;
    virtual void   Seek(size_t pos) = 0;
    virtual size_t Tell() const = 0;
};

class MemoryStream : public Stream {
    std::vector<unsigned char> buf;
    size_t pos = 0;
public:
    using Stream::Put;
    using Stream::Get;
    MemoryStream() = default;
    explicit MemoryStream(const String& s) : buf(s.begin(), s.end()), pos(0) {}

    size_t Get(void* dst, size_t n) override {
        size_t rem = buf.size() - std::min(buf.size(), pos);
        size_t take = std::min(rem, n);
        if(take) std::memcpy(dst, buf.data() + pos, take);
        pos += take;
        return take;
    }
    void Put(const void* src, size_t n) override {
        if(pos + n > buf.size()) buf.resize(pos + n);
        std::memcpy(buf.data() + pos, src, n);
        pos += n;
    }
    bool IsEof() const override { return pos >= buf.size(); }
    size_t GetSize() const override { return buf.size(); }
    void Seek(size_t p) override { pos = std::min(p, buf.size()); }
    size_t Tell() const override { return pos; }

    String GetResultString() const { return String(reinterpret_cast<const char*>(buf.data()), (int)buf.size()); }
};
