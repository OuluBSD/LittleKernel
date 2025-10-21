#ifndef _Vfs_Ecs_Exchange_h_
#define _Vfs_Ecs_Exchange_h_


class ExchangeSourceProvider;
class ExchangeSideSourceProvider;
class ExchangePoint;


class ExchangeBase
{
	bool fail = false;
	
public:
	ExchangeBase();
	virtual ~ExchangeBase();
	
	bool IsFail() const {return fail;}
	void SetFail() {fail = true;}
	void ClearFail() {fail = false;}
};


struct ExchangeProviderCookie : Pte<ExchangeProviderCookie> {};
typedef Ptr<ExchangeProviderCookie> CookiePtr;


template <class R>
class ExchangeProviderT {
	using ExchangeProviderTmpl = ExchangeProviderT<R>;
	
private:
	ExchangePoint* expt = 0;
	R dst = 0;
	
protected:
	friend class ExchangeSinkProvider;
	friend class ExchangeSourceProvider;
	friend class ExchangeSideSinkProvider;
	friend class ExchangeSideSourceProvider;
	
	int IsLink(R r) const {return r == dst;}
	
	void SetLink(ExchangePoint* expt, R dst) {
		ASSERT_(!this->dst, "Link is already set");
		this->expt = expt;
		this->dst = dst;
	}
	
	
public:
	void Visit(Vis& vis) {(vis & expt) & dst;}
	
	
	void				ClearLink() {expt = 0; dst = 0;}
	ExchangePoint*		GetExPt() const {return expt;}
	R					GetLink() const {return dst;}
	
};


class ExchangeProviderBase : public Pte<ExchangeProviderBase>
{
public:
	virtual String GetConfigString() {return String();}
	virtual TypeCls GetTypeCls() const = 0;
	
};


class ExchangeSinkProvider :
	public ExchangeProviderBase	
{
	
protected:
	friend class ExchangeSourceProvider;
	
	using ExProv = ExchangeProviderT<ExchangeSourceProvider*>;
	
	ExProv base;
	
public:
	using SinkProv = Ptr<ExchangeSinkProvider>;
	using SourceProv = ExchangeSourceProvider*;
	using Cookie = CookiePtr;
	
protected:
	friend class ExchangePoint;
	
	void SetSource(ExchangePoint* expt, ExchangeSourceProvider* src) {base.SetLink(expt, src);}
	int IsSource(ExchangeSourceProvider* src) {return base.IsLink(src);}
	
	virtual void OnLink(SourceProv src, Cookie src_c, Cookie sink_c) {}
	
public:
	ExchangeSinkProvider();
	virtual ~ExchangeSinkProvider();
	
	void						ClearLink() {base.ClearLink();}
	void						Visit(Vis& vis) {vis("base", base, VISIT_NODE);}
	ExchangePoint*				GetExPt() const {return base.GetExPt();}
	ExchangeSourceProvider*		GetSinkLink() const {return base.GetLink();}
	
};

typedef Ptr<ExchangeSinkProvider> ExchangeSinkProviderPtr;


class ExchangeSourceProvider :
	public ExchangeProviderBase
{
public:
	using ExProv = ExchangeProviderT<ExchangeSinkProviderPtr>;
	
private:
	ExProv base;
	
	static bool print_debug;
	
public:
	using SinkProv = ExchangeSinkProviderPtr;
	using SourceProv = Ptr<ExchangeSourceProvider>;
	using Cookie = CookiePtr;
	
protected:
	friend class ExchangePoint;
	
	void SetSink(ExchangePoint* expt, ExchangeSinkProviderPtr sink) {base.SetLink(expt, sink);}
	int IsSink(ExchangeSinkProviderPtr sink) {return base.IsLink(sink);}
	virtual void OnLink(SinkProv sink, Cookie src_c, Cookie sink_c) {}
	
public:
	ExchangeSourceProvider();
	virtual ~ExchangeSourceProvider();
	
	virtual bool				Accept(SinkProv sink, Cookie& src_c, Cookie& sink_c) {return true;}
	void						Link(ExchangePoint* expt, SinkProv sink, Cookie& src_c, Cookie& sink_c);
	void						ClearLink() {base.ClearLink();}
	void						Visit(Vis& vis) {base.Visit(vis);}
	ExchangePoint*				GetExPt() const {return base.GetExPt();}
	ExchangeSinkProvider*		GetSourceLink() const {return base.GetLink();}
	
};

typedef Ptr<ExchangeSourceProvider> ExchangeSourceProviderPtr;








class ExchangeSideSinkProvider :
	public ExchangeProviderBase
{
	
protected:
	friend class ExchangeSideSourceProvider;
	
	using ExProv = ExchangeProviderT<ExchangeSideSourceProvider*>;
	
	ExProv base;
	
public:
	using SideSinkProv = Ptr<ExchangeSideSinkProvider>;
	using SideSourceProv = ExchangeSideSourceProvider*;
	using Cookie = CookiePtr;
	
protected:
	friend class ExchangePoint;
	
	void SetSideSource(ExchangePoint* expt, ExchangeSideSourceProvider* src) {base.SetLink(expt, src);}
	int IsSideSource(ExchangeSideSourceProvider* src) {return base.IsLink(src);}
	
	virtual void OnLink(SideSourceProv src, Cookie src_c, Cookie sink_c) {}
	
public:
	ExchangeSideSinkProvider();
	virtual ~ExchangeSideSinkProvider();
	
	void							ClearLink() {base.ClearLink();}
	void							Visit(Vis& vis) {base.Visit(vis);}
	ExchangePoint*					GetExPt() const {return base.GetExPt();}
	ExchangeSideSourceProvider*		GetSideSinkLink() const {return base.GetLink();}
	
};

using ExchangeSideSinkProviderPtr = Ptr<ExchangeSideSinkProvider>;

class ExchangeSideSourceProvider :
	public ExchangeProviderBase
{
public:
	using ExProv = ExchangeProviderT<ExchangeSideSinkProviderPtr>;
	
private:
	ExProv base;
	
	static bool print_debug;
	
public:
	using SideSinkProv = ExchangeSideSinkProviderPtr;
	using SideSourceProv = Ptr<ExchangeSideSourceProvider>;
	using Cookie = CookiePtr;
	
protected:
	friend class ExchangePoint;
	
	void SetSideSink(ExchangePoint* expt, ExchangeSideSinkProvider* sink) {base.SetLink(expt, sink);}
	int IsSideSink(ExchangeSideSinkProviderPtr sink) {return base.IsLink(sink);}
	virtual void OnLink(SideSinkProv sink, Cookie src_c, Cookie sink_c) {}
	
public:
	ExchangeSideSourceProvider();
	virtual ~ExchangeSideSourceProvider();
	
	virtual bool					Accept(SideSinkProv sink, Cookie& src_c, Cookie& sink_c) {return true;}
	void							Link(ExchangePoint* expt, SideSinkProv sink, Cookie& src_c, Cookie& sink_c);
	void							ClearLink() {base.ClearLink();}
	void							Visit(Vis& vis) {base.Visit(vis);}
	ExchangePoint*					GetExPt() const {return base.GetExPt();}
	ExchangeSideSinkProvider*		GetSideSourceLink() const {return base.GetLink();}
	
};

typedef Ptr<ExchangeSideSourceProvider> ExchangeSideSourceProviderPtr;





class PacketForwarder;

class FwdScope {
	static const int QUEUE_SIZE = 16;
	PacketForwarder* next[QUEUE_SIZE];
	PacketForwarder* cur;
	PacketForwarder* first;
	int read_i, write_i;
	RealtimeSourceConfig* cfg;
	bool is_failed = false;
	bool is_break = false;
	bool is_once = false;
	bool is_looped = false;
	
public:
	
	
	FwdScope() {Clear();}
	FwdScope(PacketForwarder* cb, RealtimeSourceConfig& cfg) {Clear(); SetCfg(cfg); AddNext(cb); ActivateNext();}
	FwdScope(PacketForwarder& cb, RealtimeSourceConfig& cfg) {Clear(); SetCfg(cfg); AddNext(cb); ActivateNext();}
	
	void Clear();
	void ForwardWeak();
	void Forward();
	void ForwardAddNext();
	
	void SetCfg(RealtimeSourceConfig& cfg) {this->cfg = &cfg;}
	void SetFailed(bool b=true) {is_failed = b;}
	void SetOnce(bool b=true) {is_once = b;}
	void Break(bool b=true) {is_break = b;}
	void LoopComplete(bool b=true) {is_looped = b;}
	void AddNext(PacketForwarder& cb) {AddNext(&cb);}
	void AddNext(PacketForwarder* cb);
	void ActivateNext();
	
	bool HasCurrent() const {return !is_failed && cur != 0 && !is_looped && !is_once;}
	RealtimeSourceConfig& Cfg() {ASSERT(cfg); return *cfg;}
	bool IsFailed() const {return is_failed;}
	bool IsBreak() const {return is_break;}
	bool IsOnce() const {return is_once;}
	bool IsLoopComplete() const {return is_looped;}
	int GetPos() const {return read_i-1;}
	PacketForwarder* GetCurrent() const {return cur;}
	String GetFlagString() const;
	
	void operator++(int) {ActivateNext();}
	operator bool() const {return HasCurrent();}
	
};

class PacketForwarder : public VfsValueExt
{
public:
	using VfsValueExt::VfsValueExt;
	virtual void ForwardSetup(FwdScope& fwd) {}
	virtual void ForwardAtom(FwdScope& fwd) {Panic("not implemented");}
	virtual void ForwardExchange(FwdScope& fwd) {Panic("not implemented");}
	virtual bool IsPacketStuck() {Panic("not implemented"); return true;}
	virtual bool IsLoopComplete(FwdScope& fwd) {Panic("not implemented"); return true;}
	//virtual TypeCls GetTypeCls() const = 0;
	virtual String GetSecondaryName() {return "";}
	virtual void* GetSecondaryPtr() {return 0;}
	PacketForwarder& GetPacketForwarder() {return *this;}
	String GetDynamicName() const {return GetTypeCls().GetName();}
	
};

class ExchangePoint :
	public PacketForwarder
{
	
protected:
	friend class MetaSpaceBase;
	
	ExchangeSourceProviderPtr	src;
	ExchangeSinkProviderPtr		sink;
	CookiePtr					src_cookie;
	CookiePtr					sink_cookie;
	
public:
	typedef ExchangePoint CLASSNAME;
	ExchangePoint(VfsValue& n);
	virtual ~ExchangePoint();
	
	virtual void Init(VfsValue* mdir) = 0;
	
	void Clear();
	void Set(ExchangeSourceProviderPtr src, ExchangeSinkProviderPtr sink);
	void Set(ExchangeSourceProviderPtr src, ExchangeSinkProviderPtr sink, CookiePtr sink_cookie, CookiePtr src_cookie);
	void Visit(Vis& vis) override {vis & src & sink & src_cookie & sink_cookie;}
	bool IsLoopComplete(FwdScope& fwd) override {return false;}
	
	ExchangeSourceProviderPtr Source() {return src;}
	ExchangeSinkProviderPtr Sink() {return sink;}
	CookiePtr SourceCookie() {return src_cookie;}
	CookiePtr SinkCookie() {return sink_cookie;}
	
};

typedef Ptr<ExchangePoint> ExchangePointPtr;


#endif
