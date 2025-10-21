#ifndef _Vfs_Ecs_Debugging_h_
#define _Vfs_Ecs_Debugging_h_


class StackDebugger {
	enum {
		CTOR,
		REF
	};
	struct Item : Moveable<Item> {
		int event;
		TypeCls type;
		void* ptr;
		
		bool operator!=(const Item& it) const {return event != it.event || ptr != it.ptr;}
		String ToString() const;
	};
	Vector<Item> ctors, refs;
	bool checking_unrefs = false;
	
	void Construct(const Item& it);
	void Destruct(const Item& it);
	void IncRef(const Item& it);
	void DecRef(const Item& it);
public:
	typedef StackDebugger CLASSNAME;
	StackDebugger() {}
	~StackDebugger() {Clear();}
	
	void Clear() {ctors.Clear(); refs.Clear();}
	void NonZeroRefError();
	void Dump();
	void Log(String type, const Item& it);
	void CheckUnrefs(bool b=true) {checking_unrefs = b;}
	
	
	template <class T> void Construct(T* o) {Construct(Item {{}, CTOR, AsTypeCls<T>(), o});}
	template <class T> void Destruct(T* o) {Destruct(Item {{}, CTOR, AsTypeCls<T>(), o});}
	template <class T> void IncRef(T* o, TypeCls type) {IncRef(Item {{}, REF, type, o});}
	template <class T> void DecRef(T* o, TypeCls type) {DecRef(Item {{}, REF, type, o});}
	
	
	static StackDebugger& Static() {MAKE_STATIC(StackDebugger, s); return s;}
	
};


#ifdef flagDEBUG_STACK
	#define DBG_CONSTRUCT			{SetDebugReferencing(); StackDebugger::Static().Construct(this);}
	#define DBG_DESTRUCT			{StackDebugger::Static().Destruct(this);}
	#define DBG_REF_INC				{StackDebugger::Static().IncRef(this, dbg_type);}
	#define DBG_REF_DEC				{StackDebugger::Static().DecRef(this, dbg_type);}
	#define DBG_REF_NONZERO_ERROR	{StackDebugger::Static().NonZeroRefError();}
	#define DBG_BEGIN_UNREF_CHECK	{StackDebugger::Static().CheckUnrefs();}
#else
	#define DBG_CONSTRUCT
	#define DBG_DESTRUCT
	#define DBG_REF_INC
	#define DBG_REF_DEC
	#define DBG_REF_ZERO_ERROR
	#define DBG_BEGIN_UNREF_CHECK
#endif


class RefClearVisitor : public Visitor {
	
public:
	RefClearVisitor() : Visitor(nullptr) {
		TODO //SetClearRefs();
	}
	
};


#endif
