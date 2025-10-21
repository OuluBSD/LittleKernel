// Minimal Color representation (RGBA)

struct Color {
    byte r = 0, g = 0, b = 0, a = 255;
    constexpr Color() = default;
    constexpr Color(byte rr, byte gg, byte bb, byte aa = 255) : r(rr), g(gg), b(bb), a(aa) {}
    static inline Color FromRGB(int rr, int gg, int bb) { return Color((byte)rr, (byte)gg, (byte)bb); }
    static inline Color FromRGBA(int rr, int gg, int bb, int aa) { return Color((byte)rr, (byte)gg, (byte)bb, (byte)aa); }
    inline bool operator==(const Color& o) const { return r==o.r && g==o.g && b==o.b && a==o.a; }
    inline bool operator!=(const Color& o) const { return !(*this == o); }
};

using RGBA = Color;

