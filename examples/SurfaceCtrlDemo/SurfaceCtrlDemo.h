#ifndef _SurfaceCtrlDemo_SurfaceCtrlDemo_h_
#define _SurfaceCtrlDemo_SurfaceCtrlDemo_h_

#include <CtrlLib/CtrlLib.h>
#include <GuboCore/GuboCore.h>
#include <Eon/Draw/Draw.h>

using namespace Upp;

class SurfaceCtrl : public Ctrl {
    TopSurface top;
    class Shapes2D : public GeomInteraction2D {
        Color bg = Color(220, 240, 250);
    public:
        void Paint(Draw& w) override {
            Rect rc = GetContentRect();
            w.DrawRect(rc, bg);
            w.DrawEllipse(rc.CenterRect(120, 80), LtRed(), 2, Black());
            w.DrawLine(10, 10, rc.right - 10, rc.bottom - 10, 3, Blue());
            w.DrawText(12, rc.bottom - 28, "Hello Surface + CtrlLib", StdFont().Bold(), Black());
        }
        void MouseEnter(Point, dword) override { bg = Color(240, 230, 210); Refresh(); }
        void MouseLeave() override { bg = Color(220, 240, 250); Refresh(); }
    } shapes;
public:
    SurfaceCtrl() {
        top.SetFrameRect(RectC(0,0,100,100));
        top.CreateGeom2DComponent();
        shapes.SizePos();
        top.Add(shapes);
    }
    void Layout() override {
        top.SetFrameRect(GetSize());
        top.SetPendingLayout();
    }
    void Paint(Draw& w) override {
        if (top.IsPendingLayout())
            top.DeepLayout();
        top.Redraw(false);
        // Replay the recorded 2D draw commands into the Ctrl's painter
        DrawCommand& cb = top.GetCommandBegin();
        DrawCommand& ce = top.GetCommandEnd();
        Replay2DDrawCommands(w, &cb, &ce);
    }
    bool Key(dword key, int count) override {
        CtrlEvent e;
        e.type = EVENT_KEYDOWN;
        e.value = key;
        e.n = count;
        return top.Dispatch(e);
    }
    void LeftDown(Point p, dword keyflags) override { SendMouse(EVENT_MOUSE_EVENT, MOUSE_LEFTDOWN, p, 0, keyflags); }
    void LeftUp(Point p, dword keyflags) override { SendMouse(EVENT_MOUSE_EVENT, MOUSE_LEFTUP, p, 0, keyflags); }
    void RightDown(Point p, dword keyflags) override { SendMouse(EVENT_MOUSE_EVENT, MOUSE_RIGHTDOWN, p, 0, keyflags); }
    void RightUp(Point p, dword keyflags) override { SendMouse(EVENT_MOUSE_EVENT, MOUSE_RIGHTUP, p, 0, keyflags); }
    void MouseMove(Point p, dword keyflags) override { SendMouse(EVENT_MOUSEMOVE, 0, p, 0, keyflags); }
    void MouseWheel(Point p, int zdelta, dword keyflags) override { SendMouse(EVENT_MOUSEWHEEL, 0, p, zdelta, keyflags); }
private:
    void SendMouse(int type, int code, Point p, int z, dword kf) {
        CtrlEvent e;
        e.type = type;
        e.n = code;
        e.pt = p;
        e.value = kf;
        e.n = (type == EVENT_MOUSEWHEEL ? z : code);
        top.Dispatch(e);
        Refresh();
    }
};

class MainWin : public TopWindow {
    SurfaceCtrl surface;
    Button btn;
    EditString edit;
public:
    MainWin() {
        Title("SurfaceCtrl + CtrlLib widgets");
        Add(surface.HSizePos(8, 8).VSizePos(8, 36));
        Add(btn.BottomPos(4, 24).LeftPos(8, 100));
        Add(edit.BottomPos(4, 24).HSizePos(116, 8));
        btn.SetLabel("Click me");
    }
};

#endif
