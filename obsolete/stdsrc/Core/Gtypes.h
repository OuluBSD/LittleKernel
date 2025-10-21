// Minimal geometry types

struct Point {
    int x = 0, y = 0;
    constexpr Point() = default;
    constexpr Point(int xx, int yy) : x(xx), y(yy) {}
};

struct Size {
    int cx = 0, cy = 0;
    constexpr Size() = default;
    constexpr Size(int w, int h) : cx(w), cy(h) {}
};

struct Rect {
    int left = 0, top = 0, right = 0, bottom = 0;
    constexpr Rect() = default;
    constexpr Rect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
    static inline Rect FromLTWH(int l, int t, int w, int h) { return Rect(l, t, l + w, t + h); }
    inline int GetWidth() const { return right - left; }
    inline int GetHeight() const { return bottom - top; }
    inline bool IsEmpty() const { return GetWidth() <= 0 || GetHeight() <= 0; }
    inline bool Contains(Point p) const { return p.x >= left && p.x < right && p.y >= top && p.y < bottom; }
};

