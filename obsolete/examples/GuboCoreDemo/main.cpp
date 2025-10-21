#include "GuboCoreDemo.h"

using namespace Upp;

class DemoGubo : public Gubo {
    Color col = Color(0, 180, 130);
public:
    void Paint(Draw3& d) override {
        Cubf c = GetContentBox();
        Volf sz = c.GetSize();
        float dz = sz.cz;
        if(dz <= 0.0f) dz = 0.01f;
        d.DrawBox(0, 0, 0, sz.cx, sz.cy, dz, col);
    }
    void MouseEnter(Point3f, dword) override {
        col = LtRed();
        Refresh();
    }
    void MouseLeave() override {
        col = Color(0, 180, 130);
        Refresh();
    }
    void LeftDown(Point3f, dword) override { SetCapture(); }
    void LeftUp(Point3f, dword) override { ReleaseCapture(); }
};

int main() {
    TopGubo w;
    w.SetFrameBox(CubfC(0, 0, 0, 640, 360, 100));

    DemoGubo g;
    g.SizePos();
    w.Add(g);

    w.OpenMain();
    w.Run();
    return 0;
}

