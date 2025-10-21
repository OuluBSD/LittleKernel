#include "ide.h"

INITBLOCK {
	RegisterGlobalConfig("VfsDlg2");
}

struct VfsDlg : TopWindow {
	typedef VfsDlg CLASSNAME;
	
	virtual bool Key(dword key, int count);

	virtual int GetCurrentLine();
	
	void GoTo();
	void Ok()               { if(menv.focus.IsCursor()) Break(IDOK); }
	void ListSel();
	
	void Serialize(Stream& s);

	struct NavLine : Moveable<NavLine> {
		String path;
		Point  pos;
		bool   definition;
		
		bool operator<(const NavLine& b) const;
	};

	struct LineDisplay : Display {
		int DoPaint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style, int x) const;
		virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const;
		virtual Size GetStdSize(const Value& q) const;
	};

	VfsDlg();
	void OnTab();
	
	TabCtrl tabs;
	MetaEnvTree menv;
	MetaIndexerCtrl idxr;
	MetaTempTaskCtrl task;
	Ide* theide = 0;
};

VfsDlg::VfsDlg()
{
	//CtrlLayoutOKCancel(*this, "Vfs Navigator");
	Title("Vfs Navigator");
	
	Add(tabs.SizePos());
	tabs.Add(menv.SizePos(), "Data");
	tabs.Add(idxr.SizePos(), "Indexer");
	tabs.Add(task.SizePos(), "Temporary Task Files");
	tabs.WhenSet = THISBACK(OnTab);
	
	menv.dlgmode = true;
	//search.WhenEnter.Clear();
	Sizeable().Zoomable();
	Icon(IdeImg::Navigator());
	/*list.WhenSel << THISBACK(ListSel);
	list.WhenLeftDouble = THISBACK(Ok);
	navlines.NoHeader().NoGrid();
	navlines.AddColumn().SetDisplay(Single<LineDisplay>());
	navlines.WhenLeftDouble = THISBACK(Ok);*/
}

void PaintTeXt(Draw& w, int& x, int y, const String& text, Font font, Color ink);

void VfsDlg::OnTab()
{
	int i = tabs.Get();
	switch (i) {
		case 0: menv.Data(); break;
		case 1: idxr.Data(); break;
		case 2: task.Data(); break;
		default: break;
	}
}

int VfsDlg::LineDisplay::DoPaint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style, int x) const
{
	w.DrawRect(r, paper);
	const NavLine& l = q.To<NavLine>();
	x += r.left;
	String p = l.path;
	int y = r.top + (r.GetHeight() - StdFont().GetCy()) / 2;
	PaintTeXt(w, x, y, GetFileName(GetFileFolder(p)) + "/", StdFont(), ink);
	PaintTeXt(w, x, y, GetFileName(p), StdFont().Bold(), ink);
	PaintTeXt(w, x, y, " (", StdFont(), ink);
	PaintTeXt(w, x, y, AsString(l.pos.y), StdFont().Bold(), ink);
	PaintTeXt(w, x, y, ")", StdFont(), ink);
	return x - r.left;
}

void VfsDlg::LineDisplay::Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const
{
	DoPaint(w, r, q, ink, paper, style, min(r.GetWidth() - GetStdSize(q).cx, 0));
}

Size VfsDlg::LineDisplay::GetStdSize(const Value& q) const
{
	NilDraw w;
	return Size(DoPaint(w, Size(999999, 999999), q, White(), White(), 0, 0), StdFont().Bold().GetCy());
}

bool VfsDlg::NavLine::operator<(const NavLine& b) const
{
	return CombineCompare(GetFileExt(b.path), GetFileExt(path)) // .h > .c
	                     (GetFileName(path), GetFileName(b.path))
	                     (path, b.path)
	                     (pos.y, b.pos.y)
	                     (pos.x, b.pos.x) < 0;
}

void VfsDlg::ListSel()
{
	/*int ii = list.GetCursor();
	if(theide && ii >= 0 && ii < litem.GetCount()) {
		const NavItem& cm = *litem[ii];
		Vector<NavLine> set;
		for(const auto& f : ~CodeIndex())
			for(const AnnotationItem& m : f.value.items) {
				if(m.id == cm.id) {
					NavLine& nl = set.Add();
					nl.path = f.key;
					nl.pos = m.pos;
					nl.definition = m.definition;
				}
				if(set.GetCount() > 1000) // sanity
					break;
			}
		Sort(set);
		navlines.Clear();
		for(const NavLine& l : set)
			navlines.Add(RawToValue(l));
		navlines.GoBegin();
	}*/
}

void VfsDlg::Serialize(Stream& s)
{
	SerializePlacement(s);
	//search.SerializeList(s);
}

bool VfsDlg::Key(dword key, int count)
{
	/*if(key == K_UP || key == K_DOWN) {
		if(list.IsCursor())
			return list.Key(key, count);
		else
			list.GoBegin();
		return true;
	}*/
	return TopWindow::Key(key, count);
}

void VfsDlg::GoTo()
{
	/*if(navlines.IsCursor()) {
		const NavLine& l = navlines.Get(0).To<NavLine>();
		theide->GotoPos(l.path, l.pos);
	}*/
}

int VfsDlg::GetCurrentLine()
{
	return theide->editor.GetCurrentLine();
}

void Ide::OpenVfsDlg()
{
	if(!editor.WaitCurrentFile())
		return;
	VfsDlg dlg;
	LoadFromGlobal(dlg, "VfsDlg");
	dlg.theide = this;
	//dlg.Search();
	if(dlg.ExecuteOK())
		dlg.GoTo();
	//dlg.search.AddHistory();
	StoreToGlobal(dlg, "VfsDlg");
}
