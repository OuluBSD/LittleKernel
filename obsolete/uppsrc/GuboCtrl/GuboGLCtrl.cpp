#include "GuboCtrl.h"

#ifdef GUI_X11
#include <GL/glx.h>
#include <GL/gl.h>
#endif
#include <cmath>
#include <Eon/Draw/ProgPainter.h>

using namespace Upp;

NAMESPACE_UPP

GuboGLCtrl::GuboGLCtrl() {
    BackPaint(EXCLUDEPAINT);
    top.SetFrameBox(CubfC(0,0,0,100,100,100));
    top.CreateGeom3DComponent();
    // Drive periodic redraw for animations / responsiveness
    SetTimeCallback(16, THISBACK(OnTick), this);
}

GuboGLCtrl::~GuboGLCtrl() {
}

void GuboGLCtrl::AttachTop(TopGubo* t) {
    if (!t) return;
    // Replace internal top with provided one
    top = *t; // shallow copy OK if Pte; adjust if needed
}

bool GuboGLCtrl::EnsureGLContext() {
    if (gl_ready)
        return true;
    // TODO: create platform GL context for GetHWND()/native child
    gl_ready = true; // mark as ready for now
    return gl_ready;
}

void GuboGLCtrl::RenderGL() {
    Size sz = GetSize();
#ifdef GUI_X11
    if (x11.ctx) {
        glXMakeCurrent(Xdisplay, hwnd, (GLXContext)x11.ctx);
        glViewport(0, 0, sz.cx, sz.cy);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Basic fixed-function orthographic 3D for command replay
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // Y-down ortho to match UI coordinates, generous depth range
        glOrtho(0, sz.cx, sz.cy, 0, -10000, 10000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        // Ensure draw commands are up to date
        if (top.IsPendingLayout())
            top.DeepLayout();
        top.Redraw(false);

        // Simple texture cache (per control lifetime)
        auto get_tex = [&](const Image& img)->GLuint {
            const RGBA* key = img.Begin();
            Size isz = img.GetSize();
            for (const auto& n : texcache)
                if (n.key == key && n.sz == isz)
                    return (GLuint)n.id;
            GLuint tex = 0;
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, isz.cx, isz.cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, key);
            glBindTexture(GL_TEXTURE_2D, 0);
            TexNode& t = texcache.Add();
            t.key = key; t.sz = isz; t.id = tex;
            return tex;
        };

        // Replay a subset of 3D commands (BOX_OP, LINE_OP, ELLIPSE_OP, POLYLINE_OP, ARC_OP, IMAGE_OP, POLY_POLY_POLYGON_OP outline)
        const DrawCommand3* begin = &top.GetCommandBegin();
        const DrawCommand3* end   = &top.GetCommandEnd();
        for (const DrawCommand3* it = begin ? begin->next : nullptr; it && it != end; it = it->next) {
            switch (it->type) {
            case DRAW3_BOX_OP: {
                // Draw front face of box at z depth with given color
                float x = it->pt.x;
                float y = it->pt.y;
                float z = it->pt.z;
                float w = it->sz.cx;
                float h = it->sz.cy;
                const Color& c = it->color;
                glColor4ub(c.GetR(), c.GetG(), c.GetB(), 255);
                glBegin(GL_QUADS);
                    glVertex3f(x,     y,     z);
                    glVertex3f(x+w,   y,     z);
                    glVertex3f(x+w,   y+h,   z);
                    glVertex3f(x,     y+h,   z);
                glEnd();
                break;
            }
            case DRAW3_TEXT_OP: {
                if (!it->wtxt.IsEmpty()) {
                    const WString& ws = it->wtxt;
                    Font f = it->fnt;
                    Color ink = it->color;
                    Size tsz = GetTextSize(ws, f);
                    if (tsz.cx <= 0 || tsz.cy <= 0) break;
                    // Render text into Image via U++ Draw
                    ImageDraw id(tsz);
                    Draw& d = id.Alpha(); // ensure alpha channel
                    d.DrawRect(tsz, Color(0,0,0,0));
                    id.DrawText(0, 0, 0, ws.Begin(), f, ink, ws.GetCount(), nullptr);
                    Image img = id;
                    GLuint tex = get_tex(img);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, tex);
                    glColor4ub(255,255,255,255);
                    float x = it->pt.x;
                    float y = it->pt.y;
                    float z = it->pt.z;
                    float w = (float)tsz.cx;
                    float h = (float)tsz.cy;
                    glPushMatrix();
                    glTranslatef(x, y, z);
                    glRotatef(it->angle, 0.f, 0.f, 1.f); // assumes degrees
                    glBegin(GL_QUADS);
                        glTexCoord2f(0.f, 0.f); glVertex3f(0.f, 0.f, 0.f);
                        glTexCoord2f(1.f, 0.f); glVertex3f(w,   0.f, 0.f);
                        glTexCoord2f(1.f, 1.f); glVertex3f(w,   h,   0.f);
                        glTexCoord2f(0.f, 1.f); glVertex3f(0.f, h,   0.f);
                    glEnd();
                    glPopMatrix();
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glDisable(GL_TEXTURE_2D);
                }
                break;
            }
            case DRAW3_LINE_OP: {
                const Color& c = it->color;
                glLineWidth(std::max(1.0f, it->width));
                glColor4ub(c.GetR(), c.GetG(), c.GetB(), 255);
                glBegin(GL_LINES);
                    glVertex3f(it->pt.x,  it->pt.y,  it->pt.z);
                    glVertex3f(it->pt2.x, it->pt2.y, it->pt2.z);
                glEnd();
                break;
            }
            case DRAW3_ELLIPSE_OP: {
                // Approximate ellipse by polygon in XY plane at Z depth
                const Color& c = it->color;
                float x = it->r.left;
                float y = it->r.top;
                float w = it->r.GetWidth();
                float h = it->r.GetHeight();
                float z = it->r.near; // reuse near as z
                int segments = 48;
                glColor4ub(c.GetR(), c.GetG(), c.GetB(), 255);
                glBegin(GL_TRIANGLE_FAN);
                    glVertex3f(x + w*0.5f, y + h*0.5f, z);
                    for (int i = 0; i <= segments; ++i) {
                        float a = (float)i / segments * 6.28318530718f;
                        float px = x + w*0.5f + cosf(a) * (w*0.5f);
                        float py = y + h*0.5f + sinf(a) * (h*0.5f);
                        glVertex3f(px, py, z);
                    }
                glEnd();
                break;
            }
            case DRAW3_POLY_POLYLINE_OP: {
                if (!it->points.IsEmpty() && !it->ints.IsEmpty()) {
                    const Color& c = it->color;
                    glLineWidth(std::max(1.0f, it->width));
                    glColor4ub(c.GetR(), c.GetG(), c.GetB(), 255);
                    int offset = 0;
                    for (int k = 0; k < it->ints.GetCount(); ++k) {
                        int cnt = it->ints[k];
                        if (cnt <= 1 || offset + cnt > it->points.GetCount()) {
                            offset += cnt;
                            continue;
                        }
                        glBegin(GL_LINE_STRIP);
                        for (int i = 0; i < cnt; ++i) {
                            const Point3f& p = it->points[offset + i];
                            glVertex3f(p.x, p.y, p.z);
                        }
                        glEnd();
                        offset += cnt;
                    }
                }
                break;
            }
            case DRAW3_ARC_OP: {
                // Approximate arc on ellipse defined by it->r, from start to end
                const Color& c = it->color;
                float cx = it->r.left + it->r.GetWidth()*0.5f;
                float cy = it->r.top  + it->r.GetHeight()*0.5f;
                float rx = it->r.GetWidth()*0.5f;
                float ry = it->r.GetHeight()*0.5f;
                float z  = it->r.near; // reuse near as z
                auto angle_of = [&](const Point3f& p){ return atan2f((p.y - cy)/std::max(ry, 0.0001f), (p.x - cx)/std::max(rx, 0.0001f)); };
                float a0 = angle_of(it->pt);
                float a1 = angle_of(it->pt2);
                // Normalize sweep to be positive direction
                if (a1 < a0) a1 += 6.28318530718f;
                int segments = std::max(8, (int)((a1 - a0) * 24));
                glLineWidth(std::max(1.0f, it->width));
                glColor4ub(c.GetR(), c.GetG(), c.GetB(), 255);
                glBegin(GL_LINE_STRIP);
                    for (int i = 0; i <= segments; ++i) {
                        float a = a0 + (a1 - a0) * (float)i / (float)segments;
                        float px = cx + cosf(a) * rx;
                        float py = cy + sinf(a) * ry;
                        glVertex3f(px, py, z);
                    }
                glEnd();
                break;
            }
            case DRAW3_IMAGE_OP: {
                if (!it->img.IsEmpty()) {
                    Size isz = it->img.GetSize();
                    Rect src = it->rect;
                    if (src.IsEmpty()) src = Rect(isz);
                    float x = it->pt.x;
                    float y = it->pt.y;
                    float z = it->pt.z;
                    float w = it->sz.cx;
                    float h = it->sz.cy;
                    GLuint tex = get_tex(it->img);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, tex);
                    // Compute texcoords from src rect
                    float u0 = (float)src.left / (float)isz.cx;
                    float v0 = (float)src.top  / (float)isz.cy;
                    float u1 = (float)src.right / (float)isz.cx;
                    float v1 = (float)src.bottom/ (float)isz.cy;
                    glColor4ub(255,255,255,255);
                    glBegin(GL_QUADS);
                        glTexCoord2f(u0, v0); glVertex3f(x,     y,     z);
                        glTexCoord2f(u1, v0); glVertex3f(x + w, y,     z);
                        glTexCoord2f(u1, v1); glVertex3f(x + w, y + h, z);
                        glTexCoord2f(u0, v1); glVertex3f(x,     y + h, z);
                    glEnd();
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glDisable(GL_TEXTURE_2D);
                }
                break;
            }
            case DRAW3_POLY_POLY_POLYGON_OP: {
                // Try fill for single-contour polygons; otherwise draw outline.
                int total_sub = 0; for (int v : it->disjunct_polygon_counts) total_sub += v;
                bool single = (total_sub == 1 && it->subpolygon_counts.GetCount() == 1);
                int offset = 0;
                int suboffset = 0;
                if (single) {
                    int cnt = it->subpolygon_counts[0];
                    if (cnt > 2 && cnt <= it->points.GetCount()) {
                        Vector<Pointf> contour;
                        contour.SetCount(cnt);
                        float z = it->points[offset].z;
                        for (int i = 0; i < cnt; ++i) {
                            const Point3f& p = it->points[offset + i];
                            contour[i] = Pointf(p.x, p.y);
                        }
                        Vector<float> tris;
                        if (TriangulatePointf::Process(contour, tris) && (tris.GetCount() % 6) == 0) {
                            const Color& c = it->color;
                            glColor4ub(c.GetR(), c.GetG(), c.GetB(), 255);
                            glBegin(GL_TRIANGLES);
                            for (int i = 0; i < tris.GetCount(); i += 6) {
                                glVertex3f(tris[i+0], tris[i+1], z);
                                glVertex3f(tris[i+2], tris[i+3], z);
                                glVertex3f(tris[i+4], tris[i+5], z);
                            }
                            glEnd();
                            // Outline if requested
                            const Color& outline = it->outline;
                            if (!outline.IsNullInstance()) {
                                glLineWidth(std::max(1.0f, it->width));
                                glColor4ub(outline.GetR(), outline.GetG(), outline.GetB(), 255);
                                glBegin(GL_LINE_LOOP);
                                for (int i = 0; i < cnt; ++i) {
                                    const Point3f& p = it->points[offset + i];
                                    glVertex3f(p.x, p.y, p.z);
                                }
                                glEnd();
                            }
                            break;
                        }
                    }
                }
                // Outline fallback (multi-contour or triangulation failed)
                {
                    const Color& outline = it->outline.IsNullInstance() ? it->color : it->outline;
                    glLineWidth(std::max(1.0f, it->width));
                    glColor4ub(outline.GetR(), outline.GetG(), outline.GetB(), 255);
                    for (int d = 0; d < it->disjunct_polygon_counts.GetCount(); ++d) {
                        int subcnt = it->disjunct_polygon_counts[d];
                        for (int s = 0; s < subcnt; ++s) {
                            if (suboffset >= it->subpolygon_counts.GetCount()) break;
                            int cnt = it->subpolygon_counts[suboffset++];
                            if (cnt <= 1 || offset + cnt > it->points.GetCount()) {
                                offset += cnt; continue;
                            }
                            glBegin(GL_LINE_LOOP);
                            for (int i = 0; i < cnt; ++i) {
                                const Point3f& p = it->points[offset + i];
                                glVertex3f(p.x, p.y, p.z);
                            }
                            glEnd();
                            offset += cnt;
                        }
                    }
                }
                break;
            }
            default:
                break;
            }
        }
        glXSwapBuffers(Xdisplay, hwnd);
    }
#else
    (void)sz;
#endif
}

void GuboGLCtrl::State(int reason) {
    DHCtrl::State(reason);
    if (reason == OPEN) {
        EnsureGLContext();
    } else if (reason == LAYOUTPOS || reason == LAYOUT) {
        Refresh();
    } else if (reason == CLOSE) {
        // TODO: destroy GL context
        gl_ready = false;
    }
}

void GuboGLCtrl::Layout() {
    Cubf c = CubfC(0,0,0, (float)GetSize().cx, (float)GetSize().cy, top.GetFrameBox().Depth());
    top.SetFrameBox(c);
}

void GuboGLCtrl::Paint(Draw& w) {
    if (!EnsureGLContext()) {
        w.DrawRect(GetSize(), LtGray());
        w.DrawText(8, 8, "OpenGL context not ready", StdFont(), Red());
        return;
    }
    RenderGL();
}

void GuboGLCtrl::ForwardMouse(int type, int code, Point p, int z, dword kf) {
    CtrlEvent e;
    e.type = type;
    e.n = (type == EVENT_MOUSEWHEEL ? z : code);
    e.pt = p;
    e.value = kf;
    top.Dispatch(e);
}

void GuboGLCtrl::MouseMove(Point p, dword kf) { ForwardMouse(EVENT_MOUSEMOVE, 0, p, 0, kf); }
void GuboGLCtrl::LeftDown(Point p, dword kf) { ForwardMouse(EVENT_MOUSE_EVENT, MOUSE_LEFTDOWN, p, 0, kf); }
void GuboGLCtrl::LeftUp(Point p, dword kf) { ForwardMouse(EVENT_MOUSE_EVENT, MOUSE_LEFTUP, p, 0, kf); }
void GuboGLCtrl::RightDown(Point p, dword kf) { ForwardMouse(EVENT_MOUSE_EVENT, MOUSE_RIGHTDOWN, p, 0, kf); }
void GuboGLCtrl::RightUp(Point p, dword kf) { ForwardMouse(EVENT_MOUSE_EVENT, MOUSE_RIGHTUP, p, 0, kf); }
void GuboGLCtrl::MouseWheel(Point p, int z, dword kf) { ForwardMouse(EVENT_MOUSEWHEEL, 0, p, z, kf); }

bool GuboGLCtrl::Key(dword key, int count) {
    CtrlEvent e;
    e.type = EVENT_KEYDOWN;
    e.value = key;
    e.n = count;
    return top.Dispatch(e);
}

void GuboGLCtrl::OnTick() {
    if (animate) {
        Refresh();
        SetTimeCallback(16, THISBACK(OnTick), this);
    }
}

#ifdef GUI_X11
#ifdef GUI_X11
XVisualInfo* GuboGLCtrl::CreateVisual() {
    int attr[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 24, None };
    XVisualInfo* vi = glXChooseVisual(Xdisplay, DefaultScreen(Xdisplay), attr);
    if (!vi) {
        SetError(true);
        SetErrorMessage("GuboGLCtrl: glXChooseVisual failed");
        return nullptr;
    }
    x11.vi = vi;
    return vi;
}

void GuboGLCtrl::SetAttributes(unsigned long &ValueMask, XSetWindowAttributes &attr) {
    if (x11.vi) {
        attr.colormap = XCreateColormap(Xdisplay, RootWindow(Xdisplay, x11.vi->screen), x11.vi->visual, AllocNone);
        ValueMask |= CWColormap;
    }
}

void GuboGLCtrl::AfterInit(bool Error) {
    if (Error)
        return;
    if (x11.vi && !x11.ctx) {
        GLXContext ctx = glXCreateContext(Xdisplay, x11.vi, NULL, True);
        if (!ctx) {
            SetError(true);
            SetErrorMessage("GuboGLCtrl: glXCreateContext failed");
            return;
        }
        x11.ctx = ctx;
        glXMakeCurrent(Xdisplay, hwnd, ctx);
        gl_ready = true;
    }
}

void GuboGLCtrl::BeforeTerminate() {
    if (x11.ctx) {
        glXMakeCurrent(Xdisplay, None, NULL);
        glXDestroyContext(Xdisplay, (GLXContext)x11.ctx);
        x11.ctx = nullptr;
    }
    if (x11.vi) {
        XFree(x11.vi);
        x11.vi = nullptr;
    }
    ClearTextureCache();
}

void GuboGLCtrl::Resize(int w, int h) {
    // Adjust TopGubo depth unchanged; update width/height
    Cubf c = CubfC(0,0,0,(float)w,(float)h, top.GetFrameBox().Depth());
    top.SetFrameBox(c);
}
#endif

void GuboGLCtrl::ClearTextureCache() {
#ifdef GUI_X11
    if (!x11.ctx) { texcache.Clear(); return; }
    glXMakeCurrent(Xdisplay, hwnd, (GLXContext)x11.ctx);
    for (const auto& n : texcache) {
        if (n.id)
            glDeleteTextures(1, (const GLuint*)&n.id);
    }
    texcache.Clear();
#endif
}

END_UPP_NAMESPACE
