#ifndef _GuboCtrl_GuboCtrl_h_
#define _GuboCtrl_GuboCtrl_h_

#include <CtrlCore/CtrlCore.h>
#include <GuboCore/GuboCore.h>
#include <GuboLib/GuboLib.h>

NAMESPACE_UPP

class GuboGLCtrl : public DHCtrl {
    TopGubo top;
    bool gl_ready = false;
    bool animate = true;
    struct TexNode { const RGBA* key = nullptr; Size sz; unsigned int id = 0; };
    Vector<TexNode> texcache;
   
#ifdef GUI_X11
    // X11/GLX
    struct X11GLXState { XVisualInfo* vi = nullptr; void* ctx = nullptr; } x11;
#endif
public:
    typedef GuboGLCtrl CLASSNAME;
    GuboGLCtrl();
    ~GuboGLCtrl();

    void AttachTop(TopGubo* t);
    TopGubo& Top() { return top; }

protected:
    void State(int reason) override; // manages native child window lifecycle
    void Paint(Draw& w) override;
    void Layout() override;
    void MouseMove(Point p, dword kf) override;
    void LeftDown(Point p, dword kf) override;
    void LeftUp(Point p, dword kf) override;
    void RightDown(Point p, dword kf) override;
    void RightUp(Point p, dword kf) override;
    void MouseWheel(Point p, int zdelta, dword kf) override;
    bool Key(dword key, int count) override;

private:
    bool EnsureGLContext(); // TODO: create/make-current platform GL context
    void RenderGL();        // TODO: rendering call into manager
    void ForwardMouse(int type, int code, Point p, int z, dword kf);
    void OnTick();
    void ClearTextureCache();

#ifdef GUI_X11
protected:
    // DHCtrl extension points on X11
    XVisualInfo* CreateVisual() override;
    void SetAttributes(unsigned long &ValueMask, XSetWindowAttributes &attr) override;
    void AfterInit(bool Error) override;
    void BeforeTerminate() override;
    void Resize(int w, int h) override;
#endif
};

END_UPP_NAMESPACE

#endif
