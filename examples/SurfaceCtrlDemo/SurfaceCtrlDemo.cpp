#include "SurfaceCtrlDemo.h"
#include <Eon/Draw/Draw.h>

using namespace Upp;

static void Replay2DDrawCommands(Draw& w, DrawCommand* begin, DrawCommand* end) {
    if (!begin || !end)
        return;
    int clip_depth = 0;
    int op_depth = 0;
    for (DrawCommand* it = begin; it; it = it->next) {
        if (it == end)
            break;
        switch (it->type) {
        case DRAW_BEGIN_OP:
            w.BeginOp();
            op_depth++;
            break;
        case DRAW_END_OP:
            if (op_depth > 0) { w.EndOp(); op_depth--; }
            break;
        case DRAW_OFFSET_OP:
            w.Offset(it->pt);
            break;
        case DRAW_CLIP_OP:
            w.Clip(it->r);
            break;
        case DRAW_CLIPOFF_OP:
            w.Clipoff(it->r);
            clip_depth++;
            break;
        case DRAW_EXCLUDE_CLIP_OP:
            w.ExcludeClip(it->r);
            break;
        case DRAW_INTERSECT_CLIP_OP:
            w.IntersectClip(it->r);
            break;
        case DRAW_RECT_OP:
            w.DrawRect(Rect(it->pt, it->sz), it->color);
            break;
        case DRAW_LINE_OP:
            w.DrawLine(it->pt, it->pt2, it->width, it->color);
            break;
        case DRAW_POLY_POLYLINE_OP:
            if (!it->points.IsEmpty() && !it->ints.IsEmpty())
                w.DrawPolyPolyline(it->points.Begin(), it->points.GetCount(), it->ints.Begin(), it->ints.GetCount(), it->width, it->color, it->doxor);
            break;
        case DRAW_POLY_POLY_POLYGON_OP:
            if (!it->points.IsEmpty() && !it->subpolygon_counts.IsEmpty() && !it->disjunct_polygon_counts.IsEmpty())
                w.DrawPolyPolyPolygon(it->points.Begin(), it->points.GetCount(), it->subpolygon_counts.Begin(), it->subpolygon_counts.GetCount(), it->disjunct_polygon_counts.Begin(), it->disjunct_polygon_counts.GetCount(), it->color, it->width, it->outline, it->pattern, it->doxor);
            break;
        case DRAW_ARC_OP:
            w.DrawArc(it->r, it->pt, it->pt2, it->width, it->color);
            break;
        case DRAW_ELLIPSE_OP:
            w.DrawEllipse(it->r, it->color, it->width, it->outline);
            break;
        case DRAW_TEXT_OP:
            if (!it->wtxt.IsEmpty())
                w.DrawText(it->pt.x, it->pt.y, it->angle, it->wtxt.Begin(), it->fnt, it->color, it->ints.GetCount(), it->ints.IsEmpty() ? nullptr : it->ints.Begin());
            break;
        case DRAW_DRAWING_OP:
            if (!IsNull(it->value))
                w.DrawDrawing(it->r, (const Drawing&)it->value);
            break;
        case DRAW_PAINTING_OP:
            if (!IsNull(it->value))
                w.DrawPainting(it->r, (const Painting&)it->value);
            break;
        case DRAW_SYSDRAW_IMAGE_OP:
            if (!it->img.IsEmpty())
                w.SysDrawImage(it->pt.x, it->pt.y, it->img, it->r, it->color);
            break;
        case DRAW_IMAGE_OP:
            if (!it->img.IsEmpty())
                w.DrawImage(it->pt.x, it->pt.y, it->sz.cx, it->sz.cy, it->img, it->r, it->color);
            break;
        case DRAW_ESCAPE:
            w.Escape(it->txt);
            break;
        case DRAW_START_PAGE:
        case DRAW_END_PAGE:
        case DRAW_BIND_WINDOW:
        case DRAW_UNBIND_WINDOW:
        case DRAW_SET_SIZE:
        case DRAW_NULL:
        default:
            break;
        }
    }
    while (clip_depth-- > 0)
        w.End();
    while (op_depth-- > 0)
        w.EndOp();
}

GUI_APP_MAIN {
    MainWin win;
    win.SetRect(0, 0, 800, 500);
    win.OpenMain();
    win.Run();
}
