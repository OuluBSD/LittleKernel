// Minimal JSON value and parser/writer

class Json {
public:
    enum Type { NUL, BOOL, NUMBER, STRING, ARRAY, OBJECT };

private:
    Type type = NUL;
    bool b = false;
    double num = 0.0;
    String str;
    Vector<Json> arr;
    struct KV {
        String key;
        One<Json> value;
        KV() = default;
        KV(const KV& o) { key = o.key; if(o.value) value.Attach(new Json(*o.value)); }
        KV& operator=(const KV& o) { if(this!=&o){ key=o.key; if(o.value) value.Attach(new Json(*o.value)); else value.Clear(); } return *this; }
    };
    Vector<KV> obj;

    static void SkipWs(const char*& p) { while(*p && (unsigned char)*p <= ' ') ++p; }

    static bool ParseLiteral(const char*& p, const char* lit) {
        const char* q = lit; while(*q) { if(*p++ != *q++) return false; } return true;
    }
    static bool Hex(char c, int& v) { if(c>='0'&&c<='9'){v=c-'0';return true;} if(c>='a'&&c<='f'){v=c-'a'+10;return true;} if(c>='A'&&c<='F'){v=c-'A'+10;return true;} return false; }
    static String ParseString(const char*& p) {
        String out; if(*p != '"') return out; ++p;
        while(*p && *p != '"') {
            char c = *p++;
            if(c == '\\') {
                char e = *p++;
                switch(e) {
                case '"': out.Cat('"'); break; case '\\': out.Cat('\\'); break; case '/': out.Cat('/'); break;
                case 'b': out.Cat('\b'); break; case 'f': out.Cat('\f'); break; case 'n': out.Cat('\n'); break;
                case 'r': out.Cat('\r'); break; case 't': out.Cat('\t'); break;
                case 'u': {
                    int h1,h2,h3,h4; if(!Hex(*p++,h1)||!Hex(*p++,h2)||!Hex(*p++,h3)||!Hex(*p++,h4)) break;
                    int code = (h1<<12)|(h2<<8)|(h3<<4)|h4;
                    if(code < 0x80) out.Cat(code);
                    else if(code < 0x800) { out.Cat(0xC0 | (code>>6)); out.Cat(0x80 | (code & 0x3F)); }
                    else { out.Cat(0xE0 | (code>>12)); out.Cat(0x80 | ((code>>6)&0x3F)); out.Cat(0x80 | (code & 0x3F)); }
                    break; }
                default: out.Cat(e); break;
                }
            } else out.Cat(c);
        }
        if(*p == '"') ++p; return out;
    }
    static bool ParseNumber(const char*& p, double& out) {
        const char* s = p; if(*p=='-'||*p=='+') ++p; while(std::isdigit((unsigned char)*p)) ++p; if(*p=='.'){ ++p; while(std::isdigit((unsigned char)*p)) ++p; } if(*p=='e'||*p=='E'){ ++p; if(*p=='+'||*p=='-') ++p; while(std::isdigit((unsigned char)*p)) ++p; }
        std::string tmp(s, p - s); try { out = std::stod(tmp); return true; } catch(...) { return false; }
    }
    static bool ParseValue(Json& j, const char*& p) {
        SkipWs(p);
        if(*p == 'n') { if(!ParseLiteral(p, "null")) return false; j.type = NUL; return true; }
        if(*p == 't') { if(!ParseLiteral(p, "true")) return false; j.type = BOOL; j.b = true; return true; }
        if(*p == 'f') { if(!ParseLiteral(p, "false")) return false; j.type = BOOL; j.b = false; return true; }
        if(*p == '"') { j.type = STRING; j.str = ParseString(p); return true; }
        if(*p == '[') {
            ++p; j.type = ARRAY; j.arr.Clear(); SkipWs(p);
            if(*p == ']') { ++p; return true; }
            while(true) { Json e; if(!ParseValue(e, p)) return false; j.arr.Add(e); SkipWs(p); if(*p == ','){ ++p; continue; } if(*p == ']'){ ++p; break; } return false; }
            return true;
        }
        if(*p == '{') {
            ++p; j.type = OBJECT; j.obj.Clear(); SkipWs(p);
            if(*p == '}') { ++p; return true; }
            while(true) { SkipWs(p); if(*p != '"') return false; String k = ParseString(p); SkipWs(p); if(*p != ':') return false; ++p; Json v; if(!ParseValue(v, p)) return false; KV kv; kv.key = k; kv.value.Attach(new Json(v)); j.obj.Add(kv); SkipWs(p); if(*p == ','){ ++p; continue; } if(*p == '}'){ ++p; break; } return false; }
            return true;
        }
        double d; if(ParseNumber(p, d)) { j.type = NUMBER; j.num = d; return true; }
        return false;
    }

    static void Escape(String& out, const String& s) { for(int i=0;i<s.GetLength();++i){ unsigned char c = s[i]; switch(c){ case '"': out.Cat("\\\""); break; case '\\': out.Cat("\\\\"); break; case '\b': out.Cat("\\b"); break; case '\f': out.Cat("\\f"); break; case '\n': out.Cat("\\n"); break; case '\r': out.Cat("\\r"); break; case '\t': out.Cat("\\t"); break; default: out.Cat((int)c); break; } } }
    static void Dump(String& out, const Json& j) {
        switch(j.type){
        case NUL: out.Cat("null"); break;
        case BOOL: out.Cat(j.b ? "true" : "false"); break;
        case NUMBER: out.Cat(FormatDouble(j.num, 15)); break;
        case STRING: { out.Cat('"'); Escape(out, j.str); out.Cat('"'); break; }
        case ARRAY: {
            out.Cat('[');
            for(int i=0;i<j.arr.GetCount();++i){ if(i) out.Cat(','); Dump(out, j.arr[i]); }
            out.Cat(']'); break; }
        case OBJECT: {
            out.Cat('{');
            for(int i=0;i<j.obj.GetCount();++i){ if(i) out.Cat(','); const String& k = j.obj[i].key; out.Cat('"'); Escape(out, k); out.Cat('"'); out.Cat(':'); if(j.obj[i].value) Dump(out, *j.obj[i].value); else out.Cat("null"); }
            out.Cat('}'); break; }
        }
    }

    static void DumpPretty(String& out, const Json& j, int indent, int level) {
        auto ind = [&](int l){ for(int i=0;i<l;i++) out.Cat(' '); };
        switch(j.type){
        case NUL: out.Cat("null"); break;
        case BOOL: out.Cat(j.b ? "true" : "false"); break;
        case NUMBER: out.Cat(FormatDouble(j.num, 15)); break;
        case STRING: { out.Cat('"'); Escape(out, j.str); out.Cat('"'); break; }
        case ARRAY: {
            out.Cat('[');
            if(j.arr.GetCount()) { out.Cat('\n'); }
            for(int i=0;i<j.arr.GetCount();++i){ if(i){ out.Cat(','); out.Cat('\n'); } ind(level+indent); DumpPretty(out, j.arr[i], indent, level+indent); }
            if(j.arr.GetCount()) { out.Cat('\n'); ind(level); }
            out.Cat(']'); break; }
        case OBJECT: {
            out.Cat('{'); if(j.obj.GetCount()) { out.Cat('\n'); }
            for(int i=0;i<j.obj.GetCount();++i){ if(i){ out.Cat(','); out.Cat('\n'); } ind(level+indent); const String& k = j.obj[i].key; out.Cat('"'); Escape(out, k); out.Cat('"'); out.Cat(':'); out.Cat(' '); if(j.obj[i].value) DumpPretty(out, *j.obj[i].value, indent, level+indent); else out.Cat("null"); }
            if(j.obj.GetCount()) { out.Cat('\n'); ind(level); }
            out.Cat('}'); break; }
        }
    }

public:
    Json() = default;
    Json(std::nullptr_t) : type(NUL) {}
    Json(bool v) : type(BOOL), b(v) {}
    Json(double v) : type(NUMBER), num(v) {}
    Json(const String& s) : type(STRING), str(s) {}
    Json(const char* s) : type(STRING), str(s ? s : "") {}

    static Json Parse(const String& s) { const char* p = s.Begin(); Json j; if(!ParseValue(j, p)) return Json(); return j; }
    String ToString() const { String out; Dump(out, *this); return out; }
    String ToPrettyString(int indent = 2) const { String out; DumpPretty(out, *this, indent, 0); return out; }

    // Simple path accessor: e.g., "a.b[2].c"
    const Json* Ptr(const String& path) const {
        const Json* cur = this;
        int i = 0; String token; while(i < path.GetLength()){
            if(path[i] == '.') { ++i; continue; }
            if(path[i] == '[') {
                ++i; int idx = 0; while(i < path.GetLength() && std::isdigit((unsigned char)path[i])) { idx = idx*10 + (path[i]-'0'); ++i; }
                while(i < path.GetLength() && path[i] != ']') ++i; if(i < path.GetLength()) ++i;
                if(!cur || cur->type != ARRAY || idx < 0 || idx >= cur->arr.GetCount()) return nullptr; cur = &cur->arr[idx];
                continue;
            }
            token.Clear(); while(i < path.GetLength() && path[i] != '.' && path[i] != '[') token.Cat(path[i++]);
            if(cur->type != OBJECT) return nullptr; int k = -1; for(int j=0;j<cur->obj.GetCount();++j) if(cur->obj[j].key == token){ k=j; break; }
            if(k < 0) return nullptr; cur = cur->obj[k].value ? cur->obj[k].value.Get() : nullptr;
        }
        return cur;
    }
    Json* Ptr(const String& path) { return const_cast<Json*>(static_cast<const Json*>(this)->Ptr(path)); }

    // JSON Pointer (RFC6901) utilities
    static Vector<String> SplitPointer(const String& ptr) {
        Vector<String> toks;
        if(ptr.IsEmpty() || ptr[0] != '/') return toks;
        String cur; for(int i=1;i<ptr.GetLength();++i){ char c = ptr[i]; if(c == '/') { toks.Add(cur); cur.Clear(); }
            else if(c == '~' && i+1 < ptr.GetLength()) { char n = ptr[++i]; if(n == '0') cur.Cat('~'); else if(n == '1') cur.Cat('/'); else { cur.Cat('~'); cur.Cat(n); } }
            else cur.Cat(c);
        }
        toks.Add(cur);
        return toks;
    }
    const Json* Pointer(const String& ptr) const {
        if(ptr.IsEmpty()) return this;
        Vector<String> toks = SplitPointer(ptr); if(toks.IsEmpty()) return nullptr;
        const Json* cur = this;
        for(int ti=0; ti<toks.GetCount(); ++ti){ const String& tk = toks[ti]; if(!cur) return nullptr;
            if(cur->type == OBJECT) { int i = -1; for(int j=0;j<cur->obj.GetCount();++j) if(cur->obj[j].key == tk){ i=j; break; } if(i<0) return nullptr; cur = cur->obj[i].value ? cur->obj[i].value.Get() : nullptr; }
            else if(cur->type == ARRAY) { if(tk == "-") return nullptr; const char* p = tk.Begin(); const char* e = nullptr; int idx = ScanInt(p, &e, 10); if(e != tk.End()) return nullptr; if(idx < 0 || idx >= cur->arr.GetCount()) return nullptr; cur = &cur->arr[idx]; }
            else return nullptr;
        }
        return cur;
    }
    Json* Pointer(const String& ptr) { return const_cast<Json*>(static_cast<const Json*>(this)->Pointer(ptr)); }

    void MakeNull()   { type = NUL; str.Clear(); arr.Clear(); obj.Clear(); }
    void MakeBool(bool v) { type = BOOL; b = v; str.Clear(); arr.Clear(); obj.Clear(); }
    void MakeNumber(double v) { type = NUMBER; num = v; str.Clear(); arr.Clear(); obj.Clear(); }
    void MakeString(const String& s) { type = STRING; str = s; arr.Clear(); obj.Clear(); }
    void MakeArray()  { type = ARRAY; arr.Clear(); obj.Clear(); }
    void MakeObject() { type = OBJECT; obj.Clear(); arr.Clear(); }

    bool PointerSet(const String& ptr, const Json& value, bool create = true) {
        if(ptr.IsEmpty()) { *this = value; return true; }
        Vector<String> toks = SplitPointer(ptr); if(toks.IsEmpty()) return false;
        Json* cur = this;
        for(int ti=0; ti<toks.GetCount(); ++ti){ const String& tk = toks[ti]; bool last = ti == toks.GetCount()-1;
            if(cur->type == OBJECT || (create && cur->type == NUL)) {
                if(cur->type == NUL) cur->MakeObject();
                int i = -1; for(int j=0;j<cur->obj.GetCount();++j) if(cur->obj[j].key == tk){ i=j; break; }
                if(last){ if(i < 0) { KV kv; kv.key = tk; kv.value.Attach(new Json(value)); cur->obj.Add(kv); }
                          else { if(!cur->obj[i].value) cur->obj[i].value.Attach(new Json()); *cur->obj[i].value = value; }
                          return true; }
                if(i < 0) { if(!create) return false; KV kv; kv.key = tk; kv.value.Attach(new Json()); cur->obj.Add(kv); i = cur->obj.GetCount()-1; }
                if(!cur->obj[i].value) cur->obj[i].value.Attach(new Json());
                cur = cur->obj[i].value.Get();
            }
            else if(cur->type == ARRAY || (create && cur->type == NUL)) {
                if(cur->type == NUL) cur->MakeArray();
                if(tk == "-") { if(!last) return false; cur->arr.Add(value); return true; }
                const char* p = tk.Begin(); const char* e = nullptr; int idx = ScanInt(p, &e, 10); if(e != tk.End()) return false; if(idx < 0) return false;
                if(idx > cur->arr.GetCount()) return false; // cannot create gaps
                if(idx == cur->arr.GetCount()) cur->arr.Add(Json());
                if(last) { cur->arr[idx] = value; return true; }
                cur = &cur->arr[idx];
            }
            else return false;
        }
        return false;
    }
    bool PointerRemove(const String& ptr) {
        Vector<String> toks = SplitPointer(ptr); if(toks.IsEmpty()) return false;
        Json* cur = this;
        for(int ti=0; ti<toks.GetCount(); ++ti){ const String& tk = toks[ti]; bool last = ti == toks.GetCount()-1;
            if(cur->type == OBJECT) {
                int i = -1; for(int j=0;j<cur->obj.GetCount();++j) if(cur->obj[j].key == tk){ i=j; break; }
                if(i < 0) return false; if(last) { cur->obj.Remove(i); return true; } cur = cur->obj[i].value.Get();
            }
            else if(cur->type == ARRAY) {
                if(tk == "-") return false; const char* p = tk.Begin(); const char* e = nullptr; int idx = ScanInt(p, &e, 10); if(e != tk.End()) return false; if(idx < 0 || idx >= cur->arr.GetCount()) return false; if(last) { cur->arr.Remove(idx); return true; } cur = &cur->arr[idx];
            }
            else return false;
        }
        return false;
    }

    Type GetType() const { return type; }
    bool IsNull() const { return type == NUL; }
    bool IsBool() const { return type == BOOL; }
    bool IsNumber() const { return type == NUMBER; }
    bool IsString() const { return type == STRING; }
    bool IsArray() const { return type == ARRAY; }
    bool IsObject() const { return type == OBJECT; }

    bool GetBool() const { return b; }
    double GetNumber() const { return num; }
    const String& GetString() const { return str; }
    const Vector<Json>& GetArray() const { return arr; }
    const Vector<KV>& GetObject() const { return obj; }

    // Object access helpers
    int Find(const String& key) const { for(int i=0;i<obj.GetCount();++i) if(obj[i].key == key) return i; return -1; }
    const Json& operator[](int i) const { return arr[i]; }
    Json& operator[](int i) { return arr[i]; }
    const Json& operator[](const String& k) const { int i = Find(k); static Json empty; return i>=0 && obj[i].value ? *obj[i].value : empty; }
    Json& operator[](const String& k) { int i = Find(k); if(i < 0) { KV kv; kv.key = k; kv.value.Attach(new Json()); obj.Add(kv); i = obj.GetCount()-1; } if(!obj[i].value) obj[i].value.Attach(new Json()); return *obj[i].value; }
};
