// Minimal Base64 encode/decode helpers and Stream adapters

inline String Base64Encode(const void* data, size_t len) {
    static const char* tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    String out; out.Reserve((int)(((len + 2) / 3) * 4));
    for(size_t i=0; i<len; i+=3){ unsigned v = p[i] << 16; if(i+1<len) v |= p[i+1] << 8; if(i+2<len) v |= p[i+2];
        out.Cat(tbl[(v>>18)&63]); out.Cat(tbl[(v>>12)&63]); out.Cat(i+1<len ? tbl[(v>>6)&63] : '='); out.Cat(i+2<len ? tbl[v&63] : '='); }
    return out;
}

inline String Base64Encode(const String& s) { return Base64Encode(s.Begin(), (size_t)s.GetLength()); }

inline bool Base64Decode(const String& in, std::vector<unsigned char>& out) {
    int T[256]; for(int i=0;i<256;i++) T[i]=-1;
    const char* AL = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for(int i=0;i<64;i++) T[(unsigned char)AL[i]] = i;
    int val = 0, valb = -8; out.clear(); out.reserve(in.GetLength());
    for(int i=0;i<in.GetLength();++i){ int c = in[i]; if(std::isspace(c)) continue; if(c=='=') break; int d = c>=0 && c<256 ? T[c] : -1; if(d==-1) return false; val = (val<<6) + d; valb += 6; if(valb>=0){ out.push_back(char((val>>valb)&0xFF)); valb-=8; } }
    return true;
}

inline String Base64Decode(const String& in) {
    std::vector<unsigned char> v; if(!Base64Decode(in, v)) return String(); return String(reinterpret_cast<const char*>(v.data()), (int)v.size());
}

inline bool Base64Encode(Stream& in, Stream& out) {
    std::vector<unsigned char> chunk(3072); // multiple of 3
    while(true){ size_t n = in.Get(chunk.data(), chunk.size()); if(n==0) break; String e = Base64Encode(chunk.data(), n); out.Put(e.Begin(), e.GetLength()); }
    return true;
}
inline bool Base64Decode(Stream& in, Stream& out) {
    std::vector<char> all; std::vector<char> tmp(4096);
    while(true){ size_t n = in.Get(tmp.data(), tmp.size()); if(n==0) break; all.insert(all.end(), tmp.begin(), tmp.begin()+n); }
    String s(all.data(), (int)all.size()); std::vector<unsigned char> dec; if(!Base64Decode(s, dec)) return false; out.Put(dec.data(), dec.size()); return true;
}

