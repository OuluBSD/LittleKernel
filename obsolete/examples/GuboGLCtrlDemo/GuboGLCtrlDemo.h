#ifndef _GuboGLCtrlDemo_GuboGLCtrlDemo_h_
#define _GuboGLCtrlDemo_GuboGLCtrlDemo_h_

#include <CtrlLib/CtrlLib.h>
#include <GuboCtrl/GuboCtrl.h>

using namespace Upp;

class DemoGubo3D : public Upp::Gubo {
    Color col = Color(0, 180, 130);
public:
    void Paint(Draw3& d) override {
        Cubf c = GetContentBox();
        Volf sz = c.GetSize();
        float dz = sz.cz;
        if (dz <= 0.0f) dz = 1.0f;
        d.DrawBox(10, 10, dz * 0.5f, min<float>(200, sz.cx-20), min<float>(120, sz.cy-20), dz * 0.25f, col);
    }
    void MouseEnter(Point3f, dword) override { col = LtRed(); Refresh(); }
    void MouseLeave() override { col = Color(0, 180, 130); Refresh(); }
};

class MainWin : public TopWindow {
    GuboGLCtrl glctrl;
    Button btn;
public:
    MainWin() {
        Title("GuboGLCtrl + CtrlLib");
        Add(glctrl.HSizePos(8, 8).VSizePos(8, 36));
        Add(btn.BottomPos(4, 24).LeftPos(8, 100));
        btn.SetLabel("Button");

        // Add a demo 3D gubo to the embedded top container
        auto& top = glctrl.Top();
        auto* demo = new DemoGubo3D();
        demo->SizePos();
        top.Add(*demo);
    }
};

#endif
