// Minimal UUID type and helpers

class Uuid {
    std::array<unsigned char, 16> b{};
public:
    Uuid() = default;
    static Uuid Create() {
        Uuid u;
        std::random_device rd;
        for (auto& x : u.b) x = static_cast<unsigned char>(rd());
        // Set version 4 and variant bits
        u.b[6] = (u.b[6] & 0x0F) | 0x40;
        u.b[8] = (u.b[8] & 0x3F) | 0x80;
        return u;
    }
    bool IsNull() const { for(auto x : b) if(x) return false; return true; }
    const unsigned char* Begin() const { return b.data(); }
    const unsigned char* End() const { return b.data() + b.size(); }

    String ToString() const {
        static const char* hx = "0123456789abcdef";
        String s; s.Reserve(36);
        auto emit = [&](unsigned char c){ s.Cat(hx[c>>4]); s.Cat(hx[c&15]); };
        for(int i=0;i<16;i++){
            if(i==4||i==6||i==8||i==10) s.Cat('-');
            emit(b[(size_t)i]);
        }
        return s;
    }
    static bool TryParse(const String& str, Uuid& out) {
        auto hex = [](char c)->int{
            if(c>='0'&&c<='9') return c-'0';
            if(c>='a'&&c<='f') return c-'a'+10;
            if(c>='A'&&c<='F') return c-'A'+10;
            return -1;
        };
        if(str.GetLength()!=36) return false;
        int idx=0; std::array<unsigned char,16> tmp{};
        for(int i=0;i<36;i++){
            if(i==8||i==13||i==18||i==23){ if(str[i]!='-') return false; continue; }
            int h = hex(str[i]); if(++i>=36) return false; int l = hex(str[i]);
            if(h<0||l<0) return false; tmp[(size_t)idx++] = (unsigned char)((h<<4)|l);
        }
        out.b = tmp; return true;
    }
};

