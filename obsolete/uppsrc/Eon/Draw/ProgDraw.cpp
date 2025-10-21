#include "Draw.h"

#ifdef flagGUI
#include <CtrlCore/CtrlCore.h>
#else
#include <Painter/Painter.h>
#endif

NAMESPACE_UPP


ProgDraw::ProgDraw() /*: fb(state), shader(state)*/ {
	Create(0,Size(16,16));
}

ProgDraw::ProgDraw(void* hash, Size sz) /*: fb(state), shader(state)*/ {
	Create(hash, sz);
}

ProgDraw::ProgDraw(void* hash, int w, int h) /*: fb(state), shader(state)*/ {
	Create(hash, Size(w,h));
}

Size ProgDraw::GetFrameSize() const {
    if (!d.IsEmpty())
        return d->GetPageSize();
    return Size(0, 0);
}

void ProgDraw::Realize(void* hash, Size sz){
	if (d.IsEmpty() || d->GetPageSize() != sz)
		Create(hash, sz);
	
	d->Clear();
}

void ProgDraw::Create(void* hash, Size sz){
	Clear();
	//state.size = sz;
	
	LinkRender();
	
	d = new ProgPainter(hash, sz, cmd_screen_begin, render_begin, render_end, cmd_screen_end);
	
	d->Init(sz);
	
	DrawProxy::SetTarget(&*d);
}

void ProgDraw::Create(void* hash, Size sz, DrawCommand& sub_begin, DrawCommand& sub_end) {
	Clear();
	//state.size = sz;
	
	LinkRender();
	
	sub_begin.prev = &cmd_screen_begin;
	sub_end.next = &cmd_screen_end;
	cmd_screen_begin.next = &sub_begin;
	cmd_screen_end.prev = &sub_end;
	
	d = new ProgPainter(hash, sz, cmd_screen_begin, sub_begin, sub_end, cmd_screen_end);
	
}

void ProgDraw::LinkRender() {
	cmd_screen_begin.next = &render_begin;
	render_begin.prev = &cmd_screen_begin;
	cmd_screen_end.prev = &render_end;
	render_end.next = &cmd_screen_end;
}

void ProgDraw::Clear(){
	d.Clear();
	cmd_screen_begin.next = &cmd_screen_end;
	cmd_screen_begin.prev = 0;
	cmd_screen_end.next = 0;
	cmd_screen_end.prev = &cmd_screen_begin;
}

String ProgDraw::Dump() const {
	return cmd_screen_begin.GetQueueString();
}

void ProgDraw::Finish() {
	if (d)
		d->Link();
}

ProgDraw::operator Image() const {
    Size sz = GetFrameSize();
    if (sz.IsEmpty())
        return Image();
    #ifdef flagGUI
    ImageDraw id(sz);
    #else
    ImagePainter id(sz);
    #endif
    
    // Simple interpreter for common commands
    auto render = [&](Draw& w) {
        const DrawCommand* begin = &cmd_screen_begin;
        const DrawCommand* end = &cmd_screen_end;
        int clip_depth = 0;
        int op_depth = 0;
        for (const DrawCommand* it = begin ? begin->next : nullptr; it && it != end; it = it->next) {
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
                if (!it->img.IsEmpty()) {
                    #ifdef flagGUI
                    w.SysDrawImage(it->pt.x, it->pt.y, it->img, it->r, it->color);
                    #else
                    w.DrawImage(it->pt.x, it->pt.y, it->img, it->r, it->color);
                    #endif
                }
                break;
            case DRAW_IMAGE_OP:
                if (!it->img.IsEmpty())
                    w.DrawImage(it->pt.x, it->pt.y, it->sz.cx, it->sz.cy, it->img, it->r, it->color);
                break;
            case DRAW_ESCAPE:
                w.Escape(it->txt);
                break;
            default:
                break;
            }
        }
        while (clip_depth-- > 0)
            w.End();
        while (op_depth-- > 0)
            w.EndOp();
    };
    render(id);
    return id;
}

ProgPainter& ProgDraw::GetPainter() {
	ASSERT(d);
	return *d;
}

void ProgDraw::DetachTo(ProgPainter& pp) {
	if (cmd_screen_begin.next)
		pp.AppendPick(cmd_screen_begin.next, cmd_screen_end.prev);
}

#if 0

Size ProgDraw::GetPageSize() const {
	if (!d.IsEmpty())
		return d->GetPageSize();
	return Size(0,0);
}

Size ProgDraw::GetPageDimensions() {
	return GetPageSize(); // call virtual
}

void ProgDraw::SetDimensions(const Size& sz) {
	SetSize(sz); // call virtual
}

void ProgDraw::SetSize(Size sz) {
	d->SetSize(sz);
}

void ProgDraw::DrawLineOp(int x1, int y1, int x2, int y2, int width, Color color) {
	ASSERT(d);
	d->DrawLine(x1, y1, x2, y2, width, color);
}

void ProgDraw::DrawRectOp(int x, int y, int cx, int cy, Color color) {
	d->DrawRect(x, y, cx, cy, color);
}

void ProgDraw::DrawTextOp(int x, int y, int angle, const wchar *text, Font font,
	                     Color ink, int n, const int *dx) {
	d->DrawTextOp(x, y, angle, text, font, ink, n, dx);
}

void ProgDraw::DrawPolyPolylineOp(const Point *vertices, int vertex_count,
                                 const int *counts, int count_count,
                                 int width, Color color, Color doxor) {
	d->DrawPolyPolylineOp(
		vertices, vertex_count,
		counts, count_count,
		width, color, doxor);
}

bool ProgDraw::ClipOp(const Rect& r) {
	TODO
}

void ProgDraw::EndOp() {
	d->End();
}

void ProgDraw::DrawImage(int x, int y, Image img, byte alpha) {
	TODO
}

Draw& ProgDraw::Alpha() {
	TODO
}

bool ProgDraw::ClipoffOp(const Rect& r) {
	TODO
}

dword ProgDraw::GetInfo() const {
	TODO
}

void ProgDraw::BeginOp() {
	TODO
}

void ProgDraw::OffsetOp(Point p) {
	TODO
}

bool ProgDraw::ExcludeClipOp(const Rect& r) {
	TODO
}

bool ProgDraw::IntersectClipOp(const Rect& r) {
	TODO
}

bool ProgDraw::IsPaintingOp(const Rect& r) const {
	TODO
}

void ProgDraw::DrawPolyPolyPolygonOp(const Point *vertices, int vertex_count,
                                   const int *subpolygon_counts, int scc,
                                   const int *disjunct_polygon_counts, int dpcc,
                                   Color color, int width, Color outline,
                                   uint64 pattern, Color doxor) {
	TODO
}

void ProgDraw::DrawArcOp(const Rect& rc, Point start, Point end, int width, Color color) {
	TODO
}

void ProgDraw::DrawEllipseOp(const Rect& r, Color color, int pen, Color pencolor) {
	TODO
}

#endif



END_UPP_NAMESPACE
