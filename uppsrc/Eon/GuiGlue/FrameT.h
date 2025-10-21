#ifndef _Eon_GuiGlue_FrameT_h_
#define _Eon_GuiGlue_FrameT_h_


template <class Dim> class HandleSystemT;
template <class Dim> class ScopeT;


template <class Dim>
class FrameT :
	public Pte<FrameT<Dim>>,
	public Dim::Container
{
public:
	using Base = FrameT<Dim>;
	using Scope = ScopeT<Dim>;
	using Frame = FrameT<Dim>;
	//using Handle = HandleT<Dim>;
	using HandleSystem = HandleSystemT<Dim>;
	using DrawT = typename Dim::DrawT;
	//using Interaction = typename Dim::Interaction;
	using Container = typename Dim::Container;
	using ContainerFrame = typename Dim::ContainerFrame;
	//using ContainerGeom = typename Dim::ContainerGeom;
	using TopContainer = typename Dim::TopContainer;
	using Sz = typename Dim::Sz;
	using Pt = typename Dim::Pt;
	using Box = typename Dim::Box;
	using CmdDraw = typename Dim::CmdDraw;
	using CmdPainter = typename Dim::CmdPainter;
	using Button = typename Dim::Button;
	
protected:
	#ifdef flagGUI
	friend class ::UPP::TopWindow;
	#endif
	friend class TopWindow;
	
	Scope*  scope = NULL;
	TopContainer* window = NULL;
	String  title;
	Button  minimize, maximize, close;
	Image   icon;
	Sz      minsize;
	bool    sizeable;
	bool    maximized;
	Box     overlapped;
	bool    holding;
	TimeCallback hold;
	Pt      dir;
	Pt      startpos;
	Box     startrect;
	int id;
	
	// deprecated
	#if 0
	bool left_down = false;
	Pt left_down_pt;
	#endif
	
public:
	typedef FrameT CLASSNAME;
	FrameT();
	
	void SetScope(Scope* h);
	void SetId(int id) {this->id = id;}
	void SetTitle(String str) {title = str;}
	void SetClient(Box r);
	
	String GetTitle() const {return title;}
	Box GetClient() const;
	
	void GripResize();
	void Maximize();
	void Overlap();
	void ToggleMaximize();
	void SyncBox();
	void StartDrag();
	void Hold();
	void ToggleMaximized();
	void Minimize();
	void Restore();
	
	bool IsMaximized() const                 { return maximized; }
	
	void Close() override;
	void Paint(DrawT& draw) override;
	void LeftDown(Pt p, dword keyflags) override;
	void LeftDouble(Pt p, dword keyflags) override;
	void LeftUp(Pt p, dword keyflags) override;
	void MouseMove(Pt p, dword keyflags) override;
	void RightDown(Pt p, dword keyflags) override;
	void MouseEnter(Pt frame_p, dword keyflags) override;
	void MouseLeave() override;
	void Layout() override;
	Image CursorImage(Pt p, dword) override;
	void CancelMode() override;
	
	void LocalMenu(Bar& bar);
	
	Scope& GetWorkArea();
	Box Margins() const;
	Box ComputeClient(Box r);
	Pt GetDragMode(Pt p);
	Image GetDragImage(Pt dir);
	
	Sz GetFrameSize() const;
	Box GetFrameBox() const;
	void SetFrameBox(Box b);
	TopContainer* GetTopContainer();
	//Interaction& GetInteraction();
	Scope& GetScope() const {return *scope;}
	
};


#ifdef flagGUI
using TopWindowFrame = FrameT<CtxUpp2D>;
#endif

#endif
