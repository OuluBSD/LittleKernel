#ifndef _Eon_Core_Atom_h_
#define _Eon_Core_Atom_h_


template <class T> inline SideStatus MakeSide(const AtomTypeCls& src_type, const WorldState& from, const AtomTypeCls& sink_type, const WorldState& to) {Panic("Unimplemented"); NEVER(); return SIDE_NOT_ACCEPTED;}


struct Atom :
	public AtomBase,
	public DefaultInterfaceSink,
	public DefaultInterfaceSource
{
public:
	using SinkT = DefaultInterfaceSink;
	using SourceT = DefaultInterfaceSource;
	
	
	Atom(VfsValue& n) : AtomBase(n) {}
	
	bool InitializeAtom(const WorldState& ws) override {
		return SinkT::Initialize(ws) && SourceT::Initialize(ws);
	}
	
	void UninitializeAtom() override {
		SinkT::Uninitialize();
		SourceT::Uninitialize();
	}
	
	void ClearSinkSource() override {
		SinkT::ClearSink();
		SourceT::ClearSource();
	}
	
	void Visit(Vis& vis) override {
		vis.VisitT<AtomBase>("AtomBase", *this);
		vis.VisitT<SinkT>("SinkT", *this);
		vis.VisitT<SourceT>("SourceT", *this);
	}
	
	void VisitSource(Vis& vis) override {
		vis.VisitT<SourceT>("SourceT", *this);
	}
	
	void VisitSink(Vis& vis) override {
		vis.VisitT<SinkT>("SinkT", *this);
	}

	void CopyTo(AtomBase* target) const override {
		ASSERT(target->GetType() == ((AtomBase*)this)->GetType());
	    
	    TODO
	}
	
	
	InterfaceSourcePtr GetSource() override {
		InterfaceSource* src = static_cast<InterfaceSource*>(this);
		ASSERT(src);
		return InterfaceSourcePtr(src);
	}
	
	InterfaceSinkPtr GetSink() override {
		InterfaceSink* sink = static_cast<InterfaceSink*>(this);
		ASSERT(sink);
		return InterfaceSinkPtr(sink);
	}
	
	AtomBase* AsAtomBase() override {return static_cast<AtomBase*>(this);}
	void ClearSink() override {TODO}
	void ClearSource() override {TODO}
	
	
	
};

using AtomBasePtr = Ptr<AtomBase>;

#endif
