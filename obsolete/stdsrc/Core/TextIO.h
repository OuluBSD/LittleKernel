// Simple text I/O helpers using Stream

class TextReader {
    Stream* in = nullptr;
public:
    explicit TextReader(Stream& s) : in(&s) {}
    bool GetLine(String& out) {
        out.Clear();
        if(!in) return false;
        while(true){
            unsigned char c;
            size_t n = in->Get(&c, 1);
            if(n == 0) break;
            if(c == '\n') break;
            if(c == '\r') { // handle CRLF or CR
                unsigned char next;
                size_t m = in->Get(&next, 1);
                if(m && next != '\n') in->Seek(in->Tell() - 1);
                break;
            }
            out.Cat((int)c);
        }
        return out.GetLength() > 0 || !in->IsEof();
    }
};

class TextWriter {
    Stream* out = nullptr;
public:
    explicit TextWriter(Stream& s) : out(&s) {}
    void PutLine(const String& s) {
        if(!out) return;
        out->Put(s.Begin(), s.GetLength());
        char nl = '\n'; out->Put(&nl, 1);
    }
};

inline bool SaveFileAsString(const char* path, const String& content) {
    FileStream fs(path, FileStream::WRITE);
    if(!fs.Tell() && fs.IsEof()) { /* opened empty file */ }
    fs.Put(content.Begin(), content.GetLength());
    return true;
}

inline String LoadFileAsString(const char* path) {
    FileStream fs(path, FileStream::READ);
    size_t sz = fs.GetSize(); String out; out.Reserve((int)sz);
    std::vector<char> tmp(sz);
    size_t n = fs.Get(tmp.data(), sz);
    out.Cat(tmp.data(), (int)n);
    return out;
}

inline bool LoadFile(const char* path, String& out) {
    out = LoadFileAsString(path);
    return true;
}
inline bool SaveFile(const char* path, const String& content) {
    return SaveFileAsString(path, content);
}

inline bool AppendFileAsString(const char* path, const String& content) {
    FileStream fs; if(!fs.Open(path, FileStream::OpenMode(FileStream::WRITE|FileStream::READ))) return false;
    fs.Seek(fs.GetSize()); fs.Put(content.Begin(), content.GetLength()); return true;
}

inline bool ReadLines(const char* path, Vector<String>& lines) {
    FileStream fs(path, FileStream::READ);
    TextReader r(fs); String line;
    while(r.GetLine(line)) lines.Add(line);
    return true;
}
inline bool WriteLines(const char* path, const Vector<String>& lines, bool end_newline = true) {
    FileStream fs(path, FileStream::WRITE);
    for(int i=0;i<lines.GetCount();++i){ fs.Put(lines[i].Begin(), lines[i].GetLength()); char nl='\n'; if(end_newline || i+1<lines.GetCount()) fs.Put(&nl,1); }
    return true;
}
