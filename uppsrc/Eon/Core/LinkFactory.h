#ifndef _Eon_Core_LinkFactory_h_
#define _Eon_Core_LinkFactory_h_


class Factory {
	
public:
	
	
	// Links
	
	typedef LinkBase* (*NewFn)();
	struct LinkData : Moveable<LinkData> {
		NewFn			new_fn;
		String			name;
		LinkTypeCls		cls;
		TypeCls			rtti_cls;
	};
	typedef VectorMap<LinkTypeCls,LinkData> LinkMap;
	static LinkMap& LinkDataMap() {MAKE_STATIC(LinkMap, m); return m;}
	
	template <class T> static LinkBase* CreateLink() {return new T();}
	
	template <class T> static void RegisterLink() {
		LinkTypeCls cls = T::GetType();
		ASSERT(cls.IsValid());
		LinkData& d = LinkDataMap().GetAdd(cls);
		d.rtti_cls = AsTypeCls<T>();
		d.cls = cls;
		d.name = AsTypeName<T>();
		d.new_fn = &CreateLink<T>;
	}
	
	static LinkedList<LinkTypeCls>& GetLinkTypes() {static LinkedList<LinkTypeCls> l; return l;}
	
	static void Dump();
	//static const Vector<Link>& GetSinkLinks(LinkTypeCls src_link);
	//static void RefreshLinks(LinkData& d);
	//static LinkTypeCls GetLinkLinkType(LinkTypeCls link);
	
	
	
};


#endif
