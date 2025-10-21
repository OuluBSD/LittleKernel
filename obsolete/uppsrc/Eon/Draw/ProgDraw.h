#ifndef _Eon_Draw_ProgDraw_h_
#define _Eon_Draw_ProgDraw_h_


class ProgPainter;

class ProgDraw : public DrawProxy
{
	One<ProgPainter> d;
	
	Size GetFrameSize() const;
	
public:
	DrawCommand cmd_screen_begin, cmd_screen_end;
	DrawCommand render_begin, render_end;
	
	void LinkRender();
	
public:
	ProgDraw();
	ProgDraw(void* hash, Size sz);
	ProgDraw(void* hash, int w, int h);
	
	void Realize(void* hash, Size sz);
	void Create(void* hash, Size sz);
	void Create(void* hash, Size sz, DrawCommand& sub_begin, DrawCommand& sub_end);
	void Clear();
	void Finish();
	void DetachTo(ProgPainter& pp);
	
	operator Image() const;
	
	ProgPainter& GetPainter();
	
	String Dump() const;
	
	
	
};


#endif
