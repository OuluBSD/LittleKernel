#ifndef _Geometry_ModelPainter_h_
#define _Geometry_ModelPainter_h_

#if 0




class ModelPainter : public Draw3 {
	DrawCommand *prev;
	DrawCommand *next;
	DrawCommand *begin;
	DrawCommand *end;
	DrawCommand *cur_begin = NULL;
	DrawCommand *cur = NULL;
	
	Vector<vec3> tmp0;
	Vector<double> angles;
	Volf sz;
	
	DrawCommand& CreateCommand();
	
	
public:
	ModelPainter(Volf sz, DrawCommand& prev, DrawCommand& begin, DrawCommand& end, DrawCommand& next);
	ModelPainter(Volf sz, ModelPainter& p, DrawCommand& begin, DrawCommand& end);
	~ModelPainter() {/*Clear();*/}
	
	void Clear();
	DrawCommand* GetBegin() const;
	DrawCommand* GetEnd() const;
	
	dword GetInfo() const override;
	Volf GetPageSize() const override;
	void StartPage() override;
	void EndPage() override;
	void BeginOp() override;
	void EndOp() override;
	void OffsetOp(Point3f p) override;
	bool ClipOp(const Cubf& r) override;
	bool ClipoffOp(const Cubf& r) override;
	bool ExcludeClipOp(const Cubf& r) override;
	bool IntersectClipOp(const Cubf& r) override;
	bool IsPaintingOp(const Cubf& r) const override;
	Cubf GetPaintCubf() const override;
	void DrawBoxOp(float x, float y, float z, float cx, float cy, float cz, Color color) override;
	void SysDrawImageOp(float x, float y, float z, const Image& img, Color color) override;
	void SysDrawImageOp(float x, float y, float z, const Image& img, const Cubf& src, Color color) override;
	void DrawImageOp(float x, float y, float z, float cx, float cy, float cz, const Image& img, const Cubf& src, Color color) override;
	void DrawDataOp(float x, float y, float z, float cx, float cy, float cz, const String& data, const char *id) override;
	void DrawLineOp(float x1, float y1, float z1, float x2, float y2, float z2, float width, Color color) override;
	void DrawPolyPolylineOp(const Point3f *vertices, int vertex_count,
	                        const int *counts, int count_count,
	                        float width, Color color, Color doxor) override;
	void DrawPolyPolyPolygonOp(const Point3f *vertices, int vertex_count,
	                           const int *subpolygon_counts, int scc,
	                           const int *disjunct_polygon_counts, int dpcc,
	                           Color color, float width, Color outline,
	                           uint64 pattern, Color doxor) override;
	void DrawArcOp(const Cubf& rc, Point3f start, Point3f end, float width, Color color) override;
	void DrawEllipseOp(const Cubf& r, Color color, float pen, Color pencolor) override;
	void DrawTextOp(float x, float y, float z, int angle, const wchar *text, Font font,
		            Color ink, int n, const int *dx) override;
	void DrawDrawingOp(const Cubf& target, const Drawing& w) override;
	void DrawPaintingOp(const Cubf& target, const Painting& w) override;
	Volf GetNativeDpi() const override;
	void BeginNative() override;
	void EndNative() override;
	int  GetCloffLevel() const override;
	void Escape(const String& data) override;
	
	
	void BindWindow(hash_t h);
	void UnbindWindow();
	
	void DrawPolyline(const vec3* pts, int pt_count, float line_width, RGBA c);
	void DrawPolygon(const Vector<vec3>& pts, RGBA c);
	
	void Offset(const Cubf& r);
	void End();
	void WindowOffset(const Cubf& r);
	void WindowEnd();
	
	
	void Link();
	void Dump();
	
	//void Attach(Ctrl& c);
	void Attach(DrawCommand& begin, DrawCommand& end);
	void AppendPick(DrawCommand* begin, DrawCommand* end);
	
};



#endif
#endif
