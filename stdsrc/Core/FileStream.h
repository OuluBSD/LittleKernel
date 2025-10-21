// File-based Stream using std::fstream

class FileStream : public Stream {
    mutable std::fstream f;
public:
    enum OpenMode { READ = 1, WRITE = 2, READWRITE = 3, BINARY = 4 };

    FileStream() = default;
    FileStream(const char* path, OpenMode mode = READWRITE) { Open(path, mode); }

    bool Open(const char* path, OpenMode mode = READWRITE) {
        std::ios::openmode m = std::ios::binary;
        if(mode & READ) m |= std::ios::in;
        if(mode & WRITE) m |= std::ios::out;
        if(!(mode & READ) && (mode & WRITE)) m |= std::ios::trunc;
        f.open(path, m);
        return f.is_open();
    }
    void Close() { if(f.is_open()) f.close(); }

    size_t Get(void* dst, size_t n) override {
        f.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(n));
        return static_cast<size_t>(f.gcount());
    }
    void Put(const void* src, size_t n) override {
        f.write(reinterpret_cast<const char*>(src), static_cast<std::streamsize>(n));
    }
    bool IsEof() const override { return f.eof(); }
    size_t GetSize() const override {
        auto pos = f.tellg();
        f.seekg(0, std::ios::end);
        auto end = f.tellg();
        f.seekg(pos);
        return end >= 0 ? static_cast<size_t>(end) : 0;
    }
    void Seek(size_t pos) override { f.seekg(static_cast<std::streamoff>(pos)); f.seekp(static_cast<std::streamoff>(pos)); }
    size_t Tell() const override { auto p = f.tellg(); return p >= 0 ? static_cast<size_t>(p) : 0; }
};
