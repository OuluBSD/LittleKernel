class PageCtrl : public Ctrl {
public:
	virtual bool  Accept();
	virtual bool  Key(dword key, int count);
	virtual bool  HotKey(dword key);
	virtual void  Layout();
	virtual void  MouseWheel(Point p, int zdelta, dword keyflags);
	virtual void  Paint(Draw& draw);
	virtual Value GetData() const;
	virtual void  SetData(const Value& data);

public:
	class Item : public Pte<Item> {
	protected:
		friend class PageCtrl;
		PageCtrl  *owner;
		Ptr<Ctrl> slave;
		Ptr<Ctrl> ctrl;
		Image image;
		String text;
		Rect rect;
		int h;

	public:
		Item();
		Item&          SetRect(Rect r);
		Item&          SetCtrl(Ctrl *_ctrl);
		Item&          SetCtrl(Ctrl& c)                 { return SetCtrl(&c); }
		Item&          Slave(Ctrl *_slave);
		Item&          SetImage(const UPP::Image& m);
		Item&          Text(const String& s);
		Item&          Height(int i);
	};

	struct Style : ChStyle<Style> {
		int pagewidth; // 0-10000 multiplier / "percentage"
		int pageheight;
		int edgew, imagew, separator;
		Value body;
		Color text_color[4];
		Font font;
	};

private:
	
	Index<int>  in_view;
	Array<Item> tab;
	bool        accept_current, no_accept;
	int         y0;
	ScrollBar   sb;

	const Style *style;

	static Image Fade(int i);

	void       Go(int);
	int        FindInsert(Ctrl& slave);
	void       RealizeInView();

public:
	Event<>     WhenSet;
	Event<int>  WhenView;
	Event<int>  WhenUnview;
	
	PageCtrl::Item& Add();
	PageCtrl::Item& Add(const char *text);
	PageCtrl::Item& Add(const Image& m, const char *text);
	PageCtrl::Item& Add(Ctrl& slave, const char *text);
	PageCtrl::Item& Add(Ctrl& slave, const Image& m, const char *text);

	PageCtrl::Item& Insert(int i);
	PageCtrl::Item& Insert(int i, const char *text);
	PageCtrl::Item& Insert(int i, const Image& m, const char *text);
	PageCtrl::Item& Insert(int i, Ctrl& slave, const char *text);
	PageCtrl::Item& Insert(int i, Ctrl& slave, const Image& m, const char *text);

	void  Clear();
	void  Remove(int i);

	int   GetTab(Point p) const;
	Vector<int> GetPagesOnSight() const;

	int   GetCount() const                       { return tab.GetCount(); }
	Item& GetItem(int i)                         { return tab[i]; }
	const Item& GetItem(int i) const             { return tab[i]; }

	void Set(int i);
	int  Get() const;
	int  GetTabWidth() const;
	
	void Set(Ctrl& slave);
	int  Find(const Ctrl& slave) const;

	PageCtrl::Item& Insert(Ctrl& before_slave);
	PageCtrl::Item& Insert(Ctrl& before_slave, const char *text);
	PageCtrl::Item& Insert(Ctrl& before_slave, const Image& m, const char *text);
	PageCtrl::Item& Insert(Ctrl& before_slave, Ctrl& slave, const char *text);
	PageCtrl::Item& Insert(Ctrl& before_slave, Ctrl& slave, const Image& m, const char *text);

	void  Remove(Ctrl& slave);

	void GoNext()                                { Go(1); }
	void GoPrev()                                { Go(-1); }

	Size     ComputeSize();
	
	static const Style& StyleDefault();

	PageCtrl& SetStyle(const Style& s)            { style = &s; Refresh(); return *this; }

	void Reset();

	typedef PageCtrl CLASSNAME;

	PageCtrl();
};
