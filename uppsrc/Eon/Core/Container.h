#ifndef _Eon_Core_Container_h_
#define _Eon_Core_Container_h_


class EnvState : public VfsValueExt
{
	VectorMap<int, Value> data;
	String name;
	
public:
	CLASSTYPE(EnvState)
	EnvState(VfsValue& v) : VfsValueExt(v) {}
	
	String GetName() const override {return name;}
	void Visit(Vis& v) override{_VIS_(data) VIS_(name);}
	
	void SetName(String s) {name = s;}
	
	
	bool&	SetBool(int key, bool b);
	int&	SetInt(int key, int i);
	
	bool&	GetBool(int key);
	int&	GetInt(int key);
	
	template <class T>
	T& Set(int key) {
		Value& o = data.GetAdd(key);
		if (o.Is<T>())
			return const_cast<T&>(o.Get<T>());
		else {
			T& obj = CreateRawValue<T>(o);
			obj = Null;
			return obj;
		}
	}
	
	template <class T>
	T& Set(int key, const T& def) {
		Value& o = data.GetAdd(key);
		if (o.Is<T>())
			return const_cast<T&>(o.Get<T>());
		else {
			T& obj = CreateRawValue<T>(o);
			obj = def;
			return obj;
		}
	}
	
	template <class T>
	T* Get(int key) {
		int i = data.Find(key);
		if (i < 0)
			return 0;
		Value& o = data[i];
		if (o.Is<T>())
			return &const_cast<T&>(o.Get<T>());
		return 0;
	}
};

using EnvStatePtr			= Ptr<EnvState>;


String Demangle(const char* name);


class TypeId : Moveable<TypeId>
{
	const std::type_info* info = 0;
public:
	TypeId() {}
	TypeId(const TypeId& id) : info(id.info) {}
	
	
    hash_t GetHashValue() const { return info ? (hash_t)std::type_index(*info).hash_code() : 0; }
    char const* name() const { return info ? info->name() : ""; }
    String DemangledName() const {return Demangle(name());}
	String CleanDemangledName() const {
		String s(DemangledName());
		if (s.Find("") == 0)
			s = s.Mid(6);
		return s;
	}
	
	void operator=(const TypeId& id) {info = id.info;}
    bool operator==(TypeId const& other) const {return GetHashValue() == other.GetHashValue();}
    bool operator!=(TypeId const& other) const {return GetHashValue() != other.GetHashValue();}
    bool operator<(TypeId const& other) const {return GetHashValue() < other.GetHashValue();}
    
};

#endif
