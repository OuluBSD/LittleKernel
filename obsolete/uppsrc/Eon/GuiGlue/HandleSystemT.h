#ifndef _Eon_GuiGlue_HandleSystemT_h_
#define _Eon_GuiGlue_HandleSystemT_h_


template <class Dim>
class HandleSystemT :
	public System {
	
public:
	using Base = HandleSystemT<Dim>;
	using Scope = ScopeT<Dim>;
	//using Handle = HandleT<Dim>;
	using HandleSystem = HandleSystemT<Dim>;
	//using Interaction = typename Dim::Interaction;
	using Container = typename Dim::Container;
	using ContainerFrame = typename Dim::ContainerFrame;
	using TopContainer = typename Dim::TopContainer;
	using Event = typename Dim::Event;
	using EventCollection = typename Dim::EventCollection;
	using Desktop = typename Dim::Desktop;
	
	void (*set_mouse_cursor)(void*,const Image&);
	Image (*get_mouse_cursor)(void*);
	void* set_mouse_cursor_arg;
	void* get_mouse_cursor_arg;
	
private:
	Array<Scope> scopes;
	int active_pos = -1;
	bool close_machine_when_empty = false;
	
	Array<TopContainer> owned_wins;
	Mutex lock;
	
protected:
    bool Initialize(const WorldState&) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    
public:
    typedef HandleSystemT<Dim> CLASSNAME;
	HandleSystemT(Engine& m);
	
	SYS_DEF_VISIT
	
	Scope& AddScope();
	void Close();
	void CloseContainer(TopContainer* tc);
	void SetCloseMachineWhenEmpty(bool b=true) {close_machine_when_empty = b;}
	void Set_SetMouseCursor(void (*fn)(void*,const Image&), void* arg);
	void Set_GetMouseCursor(Image (*fn)(void*), void* arg);
	
	void RealizeScope();
	Scope& GetActiveScope();
	Scope& GetScope(int i);
	void DoEvents(const EventCollection& ev);
	Image OverrideCursor(const Image& m);
	Image DefaultCursor();
	
	int GetScopeCount() const {return scopes.GetCount();}
	int GetScreenCount() const {return GetScopeCount();}
	
};

#ifdef flagGUI
using WindowSystem = HandleSystemT<CtxUpp2D>;
using WindowSystemPtr = Ptr<WindowSystem>;
#endif

#endif
