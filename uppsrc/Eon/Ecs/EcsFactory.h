#ifndef _Eon_Ecs_EcsFactory_h_
#define _Eon_Ecs_EcsFactory_h_


struct ComponentFactory {
	typedef Component* (*NewFn)(VfsValue&);
	struct CompData : Moveable<CompData> {
		NewFn new_fn;
		String name;
		String eon_id;
		TypeCls rtti_cls;
	};
	
	typedef VectorMap<TypeCls,CompData> CompMap;
	static CompMap& CompDataMap() {MAKE_STATIC(CompMap, m); return m;}
	static Index<String>& CompEonIds() {MAKE_STATIC(Index<String>, idx); return idx;}
	
	template <class T> static Component* CreateComp(VfsValue& n) {return new T(n);}
	
	static Component* CreateComponent(VfsValue& n, TypeCls type);
	
	template <class T> static void Register(String eon_id) {
		TypeCls cls = AsTypeCls<T>();
		ASSERT(CompDataMap().Find(cls) < 0);
		int a = CompEonIds().GetCount();
		int b = CompDataMap().GetCount();
		ASSERT(a == b);
		CompEonIds().Add(eon_id);
		CompData& d = CompDataMap().Add(cls);
		d.eon_id = eon_id;
		d.rtti_cls = cls;
		d.name = AsTypeName<T>();
		d.new_fn = &CreateComp<T>;
	}
	
	static void Dump();
	
	template <class T>
	static String GetComponentName() {
		for (CompData& c : CompDataMap().GetValues()) {
			if (c.new_fn == &CreateComp<T>)
				return c.name;
		}
		return "";
	}
	
	static String GetComponentName(TypeCls type);
	static TypeCls GetComponentType(String name);
	
};

#define REG_EXT(type, subcomp, sink_d,sink_v, side_d,side_v, src_d,src_v) {\
	TypeExtCls c; \
	c.sink.dev = DevCls::sink_d; \
	c.sink.val = ValCls::sink_v; \
	c.side.dev = DevCls::side_d; \
	c.side.val = ValCls::side_v; \
	c.src.dev = DevCls::src_d; \
	c.src.val = ValCls::src_v; \
	c.sub = SubTypeCls::subcomp; \
	ComponentFactory::Register<type>(c, ValDevCls()); \
}

#define REG_EXT_(type, subcomp, sink_d,sink_v, side_d,side_v, src_d,src_v, sc_d,sc_v, is_multi_conn) {\
	TypeExtCls c; \
	c.sink.dev = DevCls::sink_d; \
	c.sink.val = ValCls::sink_v; \
	c.side.dev = DevCls::side_d; \
	c.side.val = ValCls::side_v; \
	c.src.dev = DevCls::src_d; \
	c.src.val = ValCls::src_v; \
	c.sub = SubTypeCls::subcomp; \
	c.multi_conn = is_multi_conn; \
	ValDevCls side_vd(DevCls::sc_d, ValCls::sc_v); \
	ComponentFactory::Register<type>(c, side_vd); \
}


#endif
