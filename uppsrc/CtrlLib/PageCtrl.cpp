#include "CtrlLib.h"

namespace Upp {

CH_STYLE(PageCtrl, Style, StyleDefault)
{
	font = StdFont();
	pageheight = 512;
	pagewidth = 9000;
	edgew = 1;
	imagew = 32;
	separator = 18;
	body = CtrlsImg::TABB();
	for(int i = 0; i < 4; i++)
		text_color[i] = SColorText();
}

PageCtrl::Item& PageCtrl::Item::Height(int i)
{
	h = i;
	return *this;
}

PageCtrl::Item& PageCtrl::Item::Text(const String& _text)
{
	text = _text;
	owner->Layout();
	return *this;
}

PageCtrl::Item& PageCtrl::Item::SetImage(const UPP::Image& _image)
{
	image = _image;
	owner->Refresh();
	return *this;
}

PageCtrl::Item& PageCtrl::Item::Slave(Ctrl *_slave)
{
	if(slave)
		slave->Remove();
	slave = _slave;
	if (slave)
		owner->Ctrl::Add(*_slave);
	return *this;
}

PageCtrl::Item& PageCtrl::Item::SetCtrl(Ctrl *_ctrl)
{
	if(ctrl)
		ctrl->Remove();
	ctrl = _ctrl;
	owner->Layout();
	return *this;
}

PageCtrl::Item::Item()
{
	ctrl = NULL;
	h = 0;
}

PageCtrl::Item& PageCtrl::Item::SetRect(Rect r)
{
	this->rect = r;
	return *this;
}

int PageCtrl::GetTabWidth() const
{
	Size sz = GetSize();
	double wmul = style->pagewidth / 10000.0;
	int w = (int)(sz.cx * wmul);
	return w;
}

void PageCtrl::Layout()
{
	Size sz = GetSize();
	for(int i = 0; i < tab.GetCount(); i++)
		if(tab[i].ctrl)
			tab[i].ctrl->Remove();
	for(int i = 0; i < tab.GetCount(); i++)
		if(tab[i].ctrl)
			Ctrl::Add(*tab[i].ctrl);
	int th = style->separator;
	sb.SetPage(sz.cy);
	Size csz = ComputeSize();
	sb.SetTotal(csz.cy);
	
	double wmul = style->pagewidth / 10000.0;
	double xoffmul = (1.0 - wmul) * 0.5;
	int xoff = (int)(sz.cx * xoffmul);
	int w = (int)(sz.cx * wmul);
	int y = -sb;
	for(int i = 0; i < tab.GetCount(); i++) {
		Item& t = tab[i];
		Ctrl* c = t.slave;
		y += style->separator;
		if (c) {
			int h = t.h;
			if (!h)
				h = style->pageheight;
			t.rect = RectC(xoff, y, w, h);
			c->SetRect(t.rect);
		}
		else {
			t.rect = RectC(xoff, y, w, style->pageheight);
		}
		y += t.rect.Height();
	}
	
	RealizeInView();
	Refresh();
}

Size PageCtrl::ComputeSize()
{
	double mul = 10000.0 / style->pagewidth;
	Size sz(0,
		style->separator * (1 + tab.GetCount()));
	for(int i = 0; i < tab.GetCount(); i++) {
		Ctrl *q = tab[i].slave;
		if(q) {
			sz.cx = max<int>(sz.cx, (int)(tab[i].rect.Width() * mul));
			sz.cy += tab[i].rect.Height();
		}
	}
	return sz;
}

void PageCtrl::RealizeInView() {
	Rect r = GetSize();
	int i = 0;
	for (auto& t : tab) {
		if (t.rect.Intersects(r)) {
			if (in_view.Find(i) < 0) {
				in_view.Add(i);
				WhenView(i);
			}
		}
		else {
			if (in_view.Find(i) >= 0) {
				in_view.RemoveKey(i);
				WhenUnview(i);
			}
		}
		i++;
	}
}

void PageCtrl::MouseWheel(Point, int zdelta, dword)
{
	sb.Wheel(zdelta);
}

void PageCtrl::Paint(Draw& w)
{
	Size sz = GetSize();
	Rect r(sz);
	double wmul = style->pagewidth / 10000.0;
	double xoffmul = (1.0 - wmul) * 0.5;
	int xoff = (int)(sz.cx * xoffmul);
	int width = (int)(sz.cx * wmul);
	Font fnt = style->font;
	fnt.Bold();
	ChPaint(w, 0, 0, sz.cx, sz.cy, style->body);
	for(int i = 0; i < tab.GetCount(); i++) {
		auto& t = tab[i];
		if (r.Intersects(t.rect)) {
			int x = xoff - 20;
			Size txtsz = GetTextSize(t.text, fnt);
			int txty = t.rect.top - txtsz.cy-1;
			w.DrawText(x, txty, t.text, fnt, style->text_color[0]);
			
			if (style->edgew > 0) {
				Rect r = t.rect;
				r.left -= style->edgew;
				r.top -= style->edgew;
				r.right += style->edgew;
				r.bottom += style->edgew;
				w.DrawRect(r, style->text_color[0]);
				ChPaint(w, t.rect, style->body);
			}
		}
	}
}

int  PageCtrl::GetTab(Point p) const
{
	for(int i = 0; i < tab.GetCount(); i++) {
		auto& t = tab[i];
		if (t.rect.top <= p.y && p.y <= t.rect.bottom)
			return i;
	}
	return -1;
}

Vector<int> PageCtrl::GetPagesOnSight() const
{
	Rect r = GetSize();
	Vector<int> v;
	for(int i = 0; i < tab.GetCount(); i++)
		if (tab[i].rect.Intersects(r))
			v << i;
	return v;
}

int  PageCtrl::Find(const Ctrl& slave) const
{
	for(int i = 0; i < tab.GetCount(); i++)
		if(tab[i].slave == &slave)
			return i;
	return -1;
}

void PageCtrl::Set(Ctrl& slave)
{
	int i = Find(slave);
	if(i >= 0)
		Set(i);
}

void PageCtrl::Remove(Ctrl& slave)
{
	int i = Find(slave);
	if(i >= 0)
		Remove(i);
}

int PageCtrl::FindInsert(Ctrl& slave)
{
	int i = Find(slave);
	return i < 0 ? GetCount() : i;
}

PageCtrl::Item& PageCtrl::Insert(Ctrl& before_slave)
{
	return Insert(FindInsert(before_slave));
}

PageCtrl::Item& PageCtrl::Insert(Ctrl& before_slave, const char *text)
{
	return Insert(FindInsert(before_slave), text);
}

PageCtrl::Item& PageCtrl::Insert(Ctrl& before_slave, const Image& m, const char *text)
{
	return Insert(FindInsert(before_slave), m, text);
}

PageCtrl::Item& PageCtrl::Insert(Ctrl& before_slave, Ctrl& slave, const char *text)
{
	return Insert(FindInsert(before_slave), slave, text);
}

PageCtrl::Item& PageCtrl::Insert(Ctrl& before_slave, Ctrl& slave, const Image& m, const char *text)
{
	return Insert(FindInsert(before_slave), slave, m, text);
}


void PageCtrl::SetData(const Value& data)
{
	Set(data);
}

Value PageCtrl::GetData() const
{
	return Get();
}

void PageCtrl::Set(int i)
{
	if (i >= 0 && i < tab.GetCount()) {
		sb.ScrollInto(tab[i].rect.bottom);
	}
}

int PageCtrl::Get() const
{
	Size sz = GetSize();
	return GetTab(sz / 2);
}

PageCtrl::Item& PageCtrl::Add()
{
	CancelMode();
	Item& t = tab.Add();
	t.owner = this;
	Layout();
	return t;
}

PageCtrl::Item& PageCtrl::Add(const char *text)
{
	return Add().Text(text);
}

PageCtrl::Item& PageCtrl::Add(const Image& m, const char *text)
{
	return Add().Text(text).SetImage(m);
}

PageCtrl::Item& PageCtrl::Add(Ctrl& slave, const char *text)
{
	return Add(text).Slave(&slave);
}

PageCtrl::Item& PageCtrl::Add(Ctrl& slave, const Image& m, const char *text)
{
	return Add(slave, text).SetImage(m);
}

PageCtrl::Item& PageCtrl::Insert(int i)
{
	CancelMode();
	int sel = Get();
	int c = i < sel ? sel + 1 : sel;
	PageCtrl::Item& m = tab.Insert(i);
	m.owner = this;
	Layout();
	sel = -1;
	Set(c);
	return m;
}

PageCtrl::Item& PageCtrl::Insert(int i, const char *text)
{
	return Insert(i).Text(text);
}

PageCtrl::Item& PageCtrl::Insert(int i, const Image& m, const char *text)
{
	return Insert(i).Text(text).SetImage(m);
}

PageCtrl::Item& PageCtrl::Insert(int i, Ctrl& slave, const char *text)
{
	return Insert(i, text).Slave(&slave);
}

PageCtrl::Item& PageCtrl::Insert(int i, Ctrl& slave, const Image& m, const char *text)
{
	return Insert(i, slave, text).SetImage(m);
}


void PageCtrl::Clear()
{
	CancelMode();
	for(int i = 0; i < tab.GetCount(); i++) {
		if(tab[i].ctrl)
			tab[i].ctrl->Remove();
		if(tab[i].slave)
			tab[i].slave->Remove();
	}
	tab.Clear();
	Layout();
	accept_current = false;
	WhenSet();
}

void PageCtrl::Remove(int i)
{
	CancelMode();
	if(tab[i].ctrl)
		tab[i].ctrl->Remove();
	if(tab[i].slave)
		tab[i].slave->Remove();
	int sel = Get();
	int c = i < sel ? sel - 1 : sel;
	tab.Remove(i);
	Layout();
	sel = -1;
	if(tab.GetCount())
		Set(minmax(c, 0, tab.GetCount() - 1));
	else {
		accept_current = false;
		WhenSet();
	}
}

void PageCtrl::Go(int d)
{
	if(IsEditable() && tab.GetCount()) {
		int sel = Get();
		int i = sel + d;
		if(i < 0)
			i = tab.GetCount() - 1;
		if(i >= tab.GetCount())
			i = 0;
		Set(i);
	}
}

bool PageCtrl::Key(dword key, int repcnt)
{
	switch(key) {
#ifdef PLATFORM_COCOA
	case K_ALT|K_TAB:
	case K_OPTION|K_TAB:
#endif
	case K_CTRL_TAB:
		GoNext();
		Action();
		return true;
#ifdef PLATFORM_COCOA
	case K_SHIFT|K_ALT|K_TAB:
	case K_SHIFT|K_OPTION|K_TAB:
#endif
	case K_SHIFT_CTRL_TAB:
		GoPrev();
		Action();
		return true;
	}
	return Ctrl::Key(key, repcnt);
}

bool PageCtrl::HotKey(dword key)
{
	switch(key) {
#ifdef PLATFORM_COCOA
	case K_ALT|K_TAB:
	case K_OPTION|K_TAB:
#endif
	case K_CTRL_TAB:
		GoNext();
		Action();
		return true;
#ifdef PLATFORM_COCOA
	case K_SHIFT|K_ALT|K_TAB:
	case K_SHIFT|K_OPTION|K_TAB:
#endif
	case K_SHIFT_CTRL_TAB:
		GoPrev();
		Action();
		return true;
	}
	return Ctrl::HotKey(key);
}

bool PageCtrl::Accept()
{
	if(tab.GetCount() == 0 || no_accept)
		return true;
	int ii = Get();
	if(accept_current)
		return !tab[ii].slave || tab[ii].slave -> Accept();
	Ptr<Ctrl> refocus = GetFocusChildDeep();
	for(int i = 0; i < tab.GetCount(); i++)
		if(tab[i].slave) {
			Set(i);
			if(!tab[i].slave->Accept())
				return false;
		}
	Set(ii);
	if(refocus)
		refocus->SetFocus();
	return true;
}

void PageCtrl::Reset()
{
	for(int i = 0; i < tab.GetCount(); i++) {
		if(tab[i].ctrl)
			tab[i].ctrl->Remove();
		if(tab[i].slave)
			tab[i].slave->Remove();
	}
	tab.Clear();
	CancelMode();
	Refresh();
	accept_current = false;
	WhenSet();
}

PageCtrl::PageCtrl()
{
	y0 = 0;
	no_accept = accept_current = false;
	NoWantFocus();
	SetStyle(StyleDefault());
	AddFrame(sb.Vert());
	sb.WhenScroll = [=] { Layout(); };
	sb.SetLine(max(1,style->pageheight / 10));
}

}
