#ifndef _Vfs_Ecs_Interface_h_
#define _Vfs_Ecs_Interface_h_


struct AtomBase;

template <class T>
class InterfaceContainer {
	
	
public:
	using Class = T;
	
	struct Item {
		
		void ClearContainer() {
			if (val) val->Clear();
			val.Clear();
		}
		
		ValueBase& GetContainerValue() {
			ASSERT(val);
			return *val;
		}
		
		void InitializeContainer(ValDevTuple vt) {
			ClearContainer();
			
			ValDevCls vd = vt.channels[0].vd;
			ASSERT(vd.IsValid());
			ValueFormat val_fmt = GetDefaultFormat(vd);
			if (!val_fmt.IsValid()) {DUMP(val_fmt);}
			ASSERT(val_fmt.IsValid());
			
			val.Create();
			val->SetFormat(val_fmt);
		}
		
		void UninitializeContainer() {
			ClearContainer();
		}
		
		One<SimpleValue> val;
	};
	
	
	Array<Item>	items;
	
	
public:
	void Visit(Vis& vis) {}
	
	
	int GetContainerCount() const {return items.GetCount();}
	
	void ClearContainer(int i) {
		GetContainerStream(i).Clear();
	}
	
	ValueBase& GetContainerValue(int i) {
		return items[i].GetContainerValue();
	}
	
	Stream& GetContainerStream(int i) {
		return items[i].GetContainerStream();
	}
	
	void SetContainerCount(int c) {items.SetCount(c);}
	
	void InitializeContainer(int i, ValDevTuple vt) {
		items[i].InitializeContainer(vt);
	}
	
	void UninitializeContainer(int i) {
		items[i].UninitializeContainer();
	}
	
	void ClearContainers() {
		for (Item& it : items)
			it.ClearContainer();
	}
	
	void UninitializeContainers() {
		for (Item& it : items)
			it.UninitializeContainer();
	}
	
};


class InterfaceBase // No Pte because of Pte<ExchangeProviderBase>
{
public:
	virtual ~InterfaceBase() {}
	virtual AtomBase* AsAtomBase() = 0;
	AtomTypeCls GetAtomType() const;
	ValDevTuple GetSinkCls() const {return GetAtomType().iface.sink;}
	ValDevTuple GetSourceCls() const {return GetAtomType().iface.src;}
	void Visit(Vis& vis) {}
	
};


class InterfaceSink :
	public InterfaceBase,
	public ExchangeSinkProvider
{
public:
	InterfaceSink() {}
	
	
	// Catches the type for CollectInterfacesVisitor
	void Visit(Vis& vis) {
		vis.VisitT<InterfaceBase>("InterfaceBase",*this);
		vis.VisitT<ExchangeSinkProvider>("ExchangeSinkProvider",*this);
	}
	
	virtual ValueBase&			GetValue(int i) = 0;
	virtual void				ClearSink() = 0;
	virtual int					GetSinkCount() const = 0;
	
};


#ifdef flagDEBUG
void InterfaceDebugPrint(TypeCls type, String s);
#endif


class InterfaceSource :
	public InterfaceBase,
	public ExchangeSourceProvider
{
	
	
public:
	InterfaceSource() {}
	
	
	// Catches the type for CollectInterfacesVisitor
	void Visit(Vis& vis) {
		vis.VisitT<InterfaceBase>("InterfaceBase", *this);
		vis.VisitT<ExchangeSourceProvider>("ExchangeSourceProvider", *this);
	}
	
	virtual void				ClearSource() = 0;
	virtual ValueBase&			GetSourceValue(int i) = 0;
	virtual int					GetSourceCount() const = 0;
	
protected:
	
};

using InterfaceSinkPtr			= Ptr<InterfaceSink>;
using InterfaceSourcePtr		= Ptr<InterfaceSource>;
using ISinkPtr					= Ptr<InterfaceSink>;
using ISourcePtr				= Ptr<InterfaceSource>;




class DefaultInterfaceSink :
	public InterfaceSink,
	public InterfaceContainer<DefaultInterfaceSink>
{
	
protected:
	using Class = DefaultInterfaceSink;
	friend class AtomSystem;
	
	
public:
	using Container = InterfaceContainer<DefaultInterfaceSink>;
	
	DefaultInterfaceSink() {}
	
	bool Initialize(const WorldState& ws);
	void Uninitialize() {ClearLink(); UninitializeContainers();}
	
	void Visit(Vis& vis) {
		vis.VisitT<InterfaceSink>("InterfaceSink", *this);
		vis.VisitT<Container>("Container", *this);
	}
	
	//TypeCls AsTypeCls() override {return TypeId(AsTypeCls<ValDevSpec>());}
	
	ValueBase&					GetSinkValue(int i)       {return GetContainerValue(i);}
	
	virtual void				ClearSink() override {ClearContainers();}
	virtual ValueBase&			GetValue(int i) override {return GetContainerValue(i);}
	virtual int					GetSinkCount() const override {return GetContainerCount();}
	
};


class DefaultInterfaceSource :
	public InterfaceSource,
	public InterfaceContainer<DefaultInterfaceSource>
{
	
protected:
	using Class = DefaultInterfaceSource;
	friend class AtomSystem;
	
	
public:
	using Container = InterfaceContainer<DefaultInterfaceSource>;
	
	DefaultInterfaceSource() {}
	
	bool Initialize(const WorldState& ws);
	void Uninitialize() {ClearLink(); UninitializeContainers();}
	
	void Visit(Vis& vis) {
		vis.VisitT<InterfaceSource>("InterfaceSource", *this);
		vis.VisitT<Container>("Container", *this);
	}
	
	using ExPt = DefaultExchangePoint;
	using Sink = DefaultInterfaceSink;
	
	virtual void				ClearSource() override {ClearContainers();}
	virtual int					GetSourceCount() const override {return GetContainerCount();}
	ValueBase&					GetSourceValue(int i) override {return GetContainerValue(i);}
	
};

using DefaultInterfaceSourcePtr			= Ptr<DefaultInterfaceSource>;
using DefaultInterfaceSinkPtr			= Ptr<DefaultInterfaceSink>;


#endif
