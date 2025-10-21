#ifndef _Core_VfsBase_Visitor_h_
#define _Core_VfsBase_Visitor_h_

struct VfsValue;

struct Visitor {
	JsonIO* json = 0;
	Stream* stream = 0;
	VersionControlSystem* vcs = 0;
	CombineHash hash;
	int mode = -1;
	int file_ver = -1;
	bool skip = false;
	enum {MODE_JSON, MODE_STREAM, MODE_HASH, MODE_VCS, MODE_RUNTIMEVISIT};
	bool storing = false;
	String error;
	void* scope = 0;
	
	typedef Visitor CLASSNAME;
	Visitor(JsonIO& j) {json = &j; mode = MODE_JSON; storing = j.IsStoring();}
	Visitor(Stream& s) {stream = &s; mode = MODE_STREAM; storing = s.IsStoring();}
	Visitor(VersionControlSystem& v) {vcs = &v; mode = MODE_VCS; storing = vcs->IsStoring();}
	Visitor(hash_t) {mode = MODE_HASH; storing = true;}
	Visitor(void*) {mode = MODE_RUNTIMEVISIT;}
	template <class T> void DoHash(T& o) {hash.Do(o);}
	bool IsLoading() const {return !storing;}
	bool IsStoring() const {return storing;}
	bool IsHashing() const {return mode == MODE_HASH;}
	bool IsError() const {return !error.IsEmpty();}
	void SetError(String s) {error = s;}
	void SetScope(void* s) {scope = s;}
	
	template<class T>
	Visitor& Visit(const char* key, T& o) {
		if      (mode == MODE_STREAM) o.Visit(*this);
		else if (mode == MODE_JSON) json->Var(key, o, THISBACK(VisitJsonItem<T>));
		else if (mode == MODE_HASH) o.Visit(*this);
		else if (mode == MODE_VCS) {
			vcs->BeginObject(key);
			o.Visit(*this);
			vcs->End();
		}
		return *this;
	}
	
	template<class T>
	Visitor& VisitT(const char* key, T& o) {
		if      (mode == MODE_STREAM) o.T::Visit(*this);
		else if (mode == MODE_JSON) json->Var(key, o, THISBACK(VisitJsonItemT<T>));
		else if (mode == MODE_HASH) o.T::Visit(*this);
		else if (mode == MODE_VCS) {
			vcs->BeginObject(key);
			o.T::Visit(*this);
			vcs->End();
		}
		return *this;
	}
	
	void ChkSerializeMagic();
	
	
	// VisitVector functions are for Vector<T> or Array<T> etc.
	
	template<class T>
	void VisitVectorSerialize(T& o) {
		ChkSerializeMagic();
		Ver(1)(1);
		int count = max(0,o.GetCount());
		(*stream) / count;
		if (!storing)
			o.SetCount(count);
		int dbg_i = 0;
		for (auto& v : o) {
			ChkSerializeMagic();
			v.Visit(*this);
			dbg_i++;
		}
	}
	
	template<class T>
	void VisitJsonItem(JsonIO& j, T& o) {
		Visitor v(j);
		o.Visit(v);
	}
	template<class T>
	void VisitJsonItemT(JsonIO& j, T& o) {
		Visitor v(j);
		o.T::Visit(v);
	}
	template<class T>
	void VisitVectorJson(const char* key, T& o) {
		json->Array(key, o, THISBACK(VisitJsonItem<typename T::value_type>), NULL, scope);
	}
	template<class T>
	void VisitVectorHash(T& o) {
		hash.Put(max(0,o.GetCount()));
		for (auto& v : o)
			v.Visit(*this);
	}
	template<class T>
	void VisitVectorVcs(String key, T& o) {
		vcs->BeginVector(key, o);
		int i = 0;
		for (auto& v : o) {
			vcs->BeginAt(i++);
			v.Visit(*this);
			vcs->End();
		}
		vcs->End();
	}
	
	template<class T>
	Visitor& VisitVector(const char* key, T& o) {
		if      (mode == MODE_STREAM) VisitVectorSerialize<T>(o);
		else if (mode == MODE_JSON) VisitVectorJson<T>(key, o);
		else if (mode == MODE_HASH) VisitVectorHash<T>(o);
		else if (mode == MODE_VCS) VisitVectorVcs<T>(key, o);
		return *this;
	}
	
	
	
	// VisitVectorVector functions are for Vector<Vector<T>> or Array<Array<T>> etc.
	
	template<class T>
	void VisitVectorVectorSerialize(T& o) {
		ChkSerializeMagic();
		Ver(1)(1);
		int count = max(0,o.GetCount());
		(*stream) / count;
		if (!storing)
			o.SetCount(count);
		for (auto& v : o)
			VisitVectorSerialize(v);
	}
	
	template<class T>
	void VisitVectorVectorItem(JsonIO& j, T& o) {
		JsonizeArray(j, o, THISBACK(VisitJsonItem<typename T::value_type>), scope);
	}
	template<class T>
	void VisitVectorVectorJson(const char* key, T& o) {
		json->Array(key, o, THISBACK(VisitVectorVectorItem<typename T::value_type>));
	}
	template<class T>
	void VisitVectorVectorHash(T& o) {
		hash.Put(max(0,o.GetCount()));
		for (auto& v : o)
			VisitVectorHash(v);
	}
	template<class T>
	void VisitVectorVectorVcs(String key, T& o) {
		vcs->BeginVector(key, o);
		int i = 0;
		for (auto& v : o) {
			vcs->BeginAt(i++);
			VisitVectorVcs(IntStr(i++), v);
			vcs->End();
		}
		vcs->End();
	}
	
	template<class T>
	Visitor& VisitVectorVector(const char* key, T& o) {
		if      (mode == MODE_STREAM) VisitVectorVectorSerialize<T>(o);
		else if (mode == MODE_JSON) VisitVectorVectorJson<T>(key, o);
		else if (mode == MODE_HASH) VisitVectorVectorHash<T>(o);
		else if (mode == MODE_VCS) VisitVectorVectorVcs<T>(key, o);
		return *this;
	}
	
	
	
	// VisitMap functions are for those VectorMaps & ArrayMaps,
	// which has standard Jsonize function for Key class (no Visit(VfsValue&))
	
	template<class T>
	void VisitMapSerialize(T& o) {
		ChkSerializeMagic();
		using KeyType = decltype(o.PopKey());
		Ver(1)(1);
		int count = max(0,o.GetCount());
		(*stream) / count;
		if (storing) {
			for (auto it : ~o) {
				*stream / const_cast<KeyType&>(it.key);
				it.value.Visit(*this);
			}
		}
		else {
			o.Clear();
			for(int i = 0; i < count; i++) {
				KeyType key;
				*stream / key;
				o.Add(key).Visit(*this);
			}
		}
	}
	template <class T>
	void VisitFromJsonValue(T& var, const Value& x)
	{
		JsonIO io(x);
		Visitor vis(io);
		var.Visit(vis);
	}
	template <class T>
	Value VisitAsJsonValue(const T& var)
	{
		JsonIO io;
		Visitor vis(io);
		const_cast<T&>(var).Visit(vis);
		return io.GetResult();
	}
	
	template<class T>
	void VisitMapJson(String key, T& map) {
		using K = decltype(map.PopKey());
		using V = typename T::value_type;
		if (!storing) {
			map.Clear();
			const Value& va = this->json->Get()[key];
			map.Reserve(va.GetCount());
			for(int i = 0; i < va.GetCount(); i++) {
				K key;
				V value;
				LoadFromJsonValue(key, va[i]["key"]);
				VisitFromJsonValue(value, va[i]["value"]);
				map.Add(key, pick(value));
			}
		}
		else  {
			Vector<Value> va;
			va.SetCount(map.GetCount());
			for(int i = 0; i < map.GetCount(); i++) {
				ValueMap item;
				JsonIO json;
				item.Add("key", StoreAsJsonValue(map.GetKey(i)));
				item.Add("value", VisitAsJsonValue(map[i]));
				va[i] = item;
			}
			this->json->Set(key, ValueArray(pick(va)));
		}
	}
	template<class T>
	void VisitMapHash(T& o) {
		hash.Put(max(0,o.GetCount()));
		for (auto it : ~o) {
			hash.Do(it.key);
			it.value.Visit(*this);
		}
	}
	template<class T>
	void VisitMapVcs(String key, T& o) {
		vcs->BeginMap(key, o);
		int i = 0;
		for (auto v : ~o) {
			if (storing)
				vcs->BeginKeyStore(i++, v.key);
			else
				vcs->BeginAt(i++);
			v.value.Visit(*this);
			vcs->End();
		}
		vcs->End();
	}
	template<class T>
	Visitor& VisitMap(const char* key, T& o) {
		if      (mode == MODE_STREAM) VisitMapSerialize<T>(o);
		else if (mode == MODE_JSON) VisitMapJson<T>(key, o);
		else if (mode == MODE_HASH) VisitMapHash<T>(o);
		else if (mode == MODE_VCS) VisitMapVcs<T>(key, o);
		return *this;
	}
	
	
	template<class T>
	void VisitMapMapSerialize(T& o) {
		ChkSerializeMagic();
		using KeyType = decltype(o.PopKey());
		Ver(1)(1);
		int count = max(0,o.GetCount());
		(*stream) / count;
		if (storing) {
			for (auto it : ~o) {
				*stream / const_cast<KeyType&>(it.key);
				VisitMapSerialize(it.value);
			}
		}
		else {
			o.Clear();
			for(int i = 0; i < count; i++) {
				KeyType key;
				*stream / key;
				VisitMapSerialize(o.Add(key));
			}
		}
	}
	template<class T>
	void VisitMapMapJson(String key, T& map) {
		using K = decltype(map.PopKey());
		using V = typename T::value_type;
		if (!storing) {
			map.Clear();
			const Value& va = this->json->Get()[key];
			map.Reserve(va.GetCount());
			for(int i = 0; i < va.GetCount(); i++) {
				K key;
				LoadFromJsonValue(key, va[i]["key"]);
				auto& value = map.Add(key);
				JsonIO jio(va[i]);
				Visitor vis(jio);
				vis.VisitMapJson("value", value);
			}
		}
		else  {
			Vector<Value> va;
			va.SetCount(map.GetCount());
			for(int i = 0; i < map.GetCount(); i++) {
				JsonIO json;
				Visitor vis(json);
				vis.VisitMapJson("value", map[i]);
				ValueMap item = json.GetResult();
				ASSERT(item.GetCount());
				item.Add("key", StoreAsJsonValue(map.GetKey(i)));
				va[i] = item;
			}
			this->json->Set(key, ValueArray(pick(va)));
		}
	}
	template<class T>
	void VisitMapMapHash(T& o) {
		hash.Put(max(0,o.GetCount()));
		for (auto it0 : ~o) {
			hash.Do(it0.key);
			for (auto it1 : ~it0.value) {
				hash.Do(it1.key);
				it1.value.Visit(*this);
			}
		}
	}
	template<class T>
	void VisitMapMapVcs(String key, T& o) {
		vcs->BeginMap(key, o);
		int i = 0;
		for (auto v : ~o) {
			String name = IntStr(i);
			if (storing)
				vcs->BeginKeyStore(i++, v.key);
			else
				vcs->BeginAt(i++);
			VisitMapVcs(name, v.value);
			vcs->End();
		}
		vcs->End();
	}
	template<class T>
	Visitor& VisitMapMap(const char* key, T& o) {
		if      (mode == MODE_STREAM) VisitMapMapSerialize<T>(o);
		else if (mode == MODE_JSON) VisitMapMapJson<T>(key, o);
		else if (mode == MODE_HASH) VisitMapMapHash<T>(o);
		else if (mode == MODE_VCS) VisitMapMapVcs<T>(key, o);
		return *this;
	}
	
	
	
	// VisitMapKV functions are for those VectorMaps & ArrayMaps,
	// which has custom Key class with Visit(VfsValue&) instead of Jsonzie(JsonIO&)
	
	template<class T>
	void VisitMapKVSerialize(T& o) {
		ChkSerializeMagic();
		using KeyType = decltype(o.PopKey());
		Ver(1)(1);
		int count = max(0,o.GetCount());
		(*stream) / count;
		if (storing) {
			for (auto it : ~o) {
				const_cast<KeyType&>(it.key).Visit(*this);
				it.value.Visit(*this);
			}
		}
		else {
			o.Clear();
			for(int i = 0; i < count; i++) {
				KeyType key;
				key.Visit(*this);
				o.Add(key).Visit(*this);
			}
		}
	}
	template<class T>
	void VisitMapKVItem(JsonIO& j, T& o) {
		Visitor v(j);
		o.Visit(v);
	}
	template<class T>
	void VisitMapKVJson(String key, T& map) {
		using K = decltype(map.PopKey());
		using V = typename T::value_type;
		if (!storing) {
			map.Clear();
			const Value& va = this->json->Get()[key];
			map.Reserve(va.GetCount());
			for(int i = 0; i < va.GetCount(); i++) {
				K key;
				V value;
				VisitFromJsonValue(key, va[i]["key"]);
				VisitFromJsonValue(value, va[i]["value"]);
				map.Add(key, pick(value));
			}
		}
		else  {
			Vector<Value> va;
			va.SetCount(map.GetCount());
			for(int i = 0; i < map.GetCount(); i++) {
				ValueMap item;
				JsonIO json;
				item.Add("key", VisitAsJsonValue(map.GetKey(i)));
				item.Add("value", VisitAsJsonValue(map[i]));
				va[i] = item;
			}
			this->json->Set(key, ValueArray(pick(va)));
		}
	}
	template<class T>
	void VisitMapKVHash(T& o) {
		hash.Put(max(0,o.GetCount()));
		for (auto& v : o)
			v.Visit(*this);
	}
	template<class T>
	void VisitMapKVVcs(const String& key, T& o) {
		using KeyType = decltype(o.PopKey());
		vcs->BeginMapKV<T>(key, o);
		int i = 0;
		for (auto v : ~o) {
			vcs->BeginKV(i++); {
				if (storing) {
					vcs->BeginKeyVisit();{
						const_cast<KeyType&>(v.key).Visit(*this);}
					vcs->End();
				}
				vcs->BeginValueVisit();{
					v.value.Visit(*this);}
				vcs->End();}
			vcs->End();
		}
		vcs->End();
	}
	template<class T>
	Visitor& VisitMapKV(const char* key, T& o) {
		if      (mode == MODE_STREAM) VisitMapKVSerialize<T>(o);
		else if (mode == MODE_JSON) VisitMapKVJson<T>(key, o);
		else if (mode == MODE_HASH) VisitMapKVHash<T>(o);
		else if (mode == MODE_VCS) VisitMapKVVcs<T>(key, o);
		return *this;
	}
		
	Visitor& Ver(int version, bool forced=false) {
		if (mode == MODE_STREAM) {
			*stream % version;
			file_ver = version;
			skip = false;
		}
		else if (forced && mode == MODE_JSON) {
			file_ver = version;
			(*json)(".version", file_ver);
			skip = false;
		}
		else if (forced && mode == MODE_HASH) {
			file_ver = version;
			hash.Do(file_ver);
			skip = false;
		}
		else if (forced && mode == MODE_VCS) {
			file_ver = version;
			vcs->Do(".version", file_ver);
			skip = false;
		}
		return *this;
	}
	Visitor& operator()(int version) {
		if (mode != MODE_STREAM) return *this;
		skip = file_ver < version;
		return *this;
	}
	template <class T> Visitor& operator()(const char* key, T& o) {
		if (skip) return *this;
		switch (mode) {
		case MODE_JSON: (*json)(key,o); return *this;
		case MODE_STREAM:
			ChkSerializeMagic();
			*stream % o;
			return *this;
		case MODE_HASH: DoHash<T>(o); return *this;
		case MODE_VCS: vcs->Do(key, o); return *this;
		default: return *this;
		}
	}
	
	Visitor& VisitBinary(const char* key, void* data, int size) {
		if (skip) return *this;
		if (storing) {
			switch (mode) {
			case MODE_JSON: {
				String s;
				(*json)(key, s);
				String ds = Decode64(s);
				size = min(size, ds.GetCount());
				memcpy(data, ds.Begin(), size);
				return *this;
			}
			case MODE_STREAM: stream->Get(data, size); return *this;
			case MODE_HASH: hash.DoBinary(data,size); return *this;
			case MODE_VCS: vcs->DoBinary(key, data,size); return *this;
			default: return *this;
			}
		}
		else {
			switch (mode) {
			case MODE_JSON: {
				String s = Encode64(data,size);
				(*json)(key, s);
				return *this;
			}
			case MODE_STREAM: stream->Get(data, size); return *this;
			case MODE_HASH: return *this;
			case MODE_VCS: vcs->DoBinary(key, data,size); return *this;
			default: return *this;
			}
		}
	}
	
	template <class T>
	Visitor& operator&(T o) {
		if (o) {
			TODO // runtime-visit a single pointer
		}
		return *this;
	}
	
	template <class T>
	Visitor& operator&&(T& o) {
		if (o.GetCount()) {
			TODO // runtime-visit a vector of pointers
		}
		return *this;
	}
	
	template <class T>
	Visitor& operator^(T& o) {
		if (o) {
			TODO // runtime-visit a vector of vector of pointers
		}
		return *this;
	}
	
	template <class T>
	Visitor& operator|(T& o) {
		TODO // runtime-visit a single item
		return *this;
	}
	
	template <class T>
	Visitor& operator||(T& o) {
		if (!o.IsEmpty()) {
			TODO // runtime-visit a vector
		}
		return *this;
	}
	
	template <class T>
	Visitor& operator>(T& o) {
		if (!o.IsEmpty()) {
			TODO // runtime-visit a map of pointers
		}
		return *this;
	}
	
	
	template<class T> Visitor& operator()(const char* key, T& o, int) {return VisitVector(key, o);}
	template<class T> Visitor& operator()(const char* key, T& o, int, int) {return VisitMap(key, o);}
	template<class T> Visitor& operator()(const char* key, T& o, int, int, int) {return VisitMapKV(key, o);}
	template<class T> Visitor& operator()(const char* key, T& o, int, int, int, int) {return VisitVectorVector(key, o);}
	template<class T> Visitor& operator()(const char* key, T& o, int, int, int, int, int) {return Visit(key, o);}
	template<class T> Visitor& operator()(const char* key, T& o, int, int, int, int, int, int) {return VisitMapMap(key, o);}
	
	#define VISIT_VECTOR 0
	#define VISIT_MAP 0,0
	#define VISIT_MAP_KV 0,0,0
	#define VISIT_VECTOR_VECTOR 0,0,0,0
	#define VISIT_NODE 0,0,0,0,0
	#define VISIT_MAPMAP 0,0,0,0,0,0
};

template <class T>
inline void VersionControlSystem::BeginMapKV(String key, T& o) {
	using KeyType = decltype(o.PopKey());
	Push(ST_MAP_KV, key);
	if (storing)
		RemoveIdxFolders(max(0,o.GetCount()));
	else {
		o.Clear();
		String dir = GetCurrentDirectory();
		int count = ResolveCount();
		o.Reserve(count);
		for(int i = 0; i < count; i++) {
			BeginKeyVisit();
			KeyType key;
			Visitor vis(*this);
			key.Visit(vis);
			o.Add(pick(key));
			End();
		}
	}
}

template<> void Visitor::VisitVectorSerialize(Array<VfsValue>& o);

using Vis = Visitor;

#define VIS_THIS(x) v.VisitT<x>(#x, *this)
#define _VIS_(x) v(#x, x)
#define VIS_(x) (#x, x)
#define VIS0(x, y) (#x, x, y)
#define VISN(x) (#x, x, VISIT_NODE)
#define VISV(x) (#x, x, VISIT_VECTOR)
#define VISVV(x) (#x, x, VISIT_VECTOR_VECTOR)
#define VISM(x) (#x, x, VISIT_MAP)

template <> inline void Visitor::DoHash<Index<int>>(Index<int>& o) {hash.Do(o.GetKeys());}
template <> inline void Visitor::DoHash<Index<String>>(Index<String>& o) {hash.Do(o.GetKeys());}

bool MakeRelativePath(const String& includes, const String& dir, String& best_ai_dir, String& best_rel_dir);
Vector<String> FindParentUppDirectories(const String& dir);

class PtrClearVisitor : public Visitor {
	
public:
	typedef PtrClearVisitor CLASSNAME;
	PtrClearVisitor();
	
};


template <class T>
bool VisitFromJson(T& var, const char *json)
{
	try {
		Value jv;
		if (TrimLeft(String(json)).GetCount()) {
			jv = ParseJSON(json);
			if(jv.IsError()) {
				LOG("VisitFromJson: " << GetErrorText(jv));
				return false;
			}
		}
		JsonIO io(jv);
		Vis vis(io);
		var.Visit(vis);
	}
	catch(ValueTypeError e) {
		LOG("VisitFromJson: " << e);
		return false;
	}
	catch(JsonizeError e) {
		LOG("VisitFromJson: " << e);
		return false;
	}
	return true;
}

template <class T>
bool VisitFromJsonFile(T& var, const char *file = NULL)
{
	return VisitFromJson(var, LoadFile(sJsonFile(file)));
}

template <class T>
String VisitToJson(T& var)
{
	try {
		JsonIO io;
		Vis vis(io);
		var.Visit(vis);
		Value val = io.GetResult();
		return AsJSON(val);
	}
	catch (...) {
		return String();
	}
}

template <class T> inline hash_t GetVisitJsonHash(const T& o) {return VisitToJson<T>(const_cast<T&>(o)).GetHashValue();}

template <class T>
bool DoVisitToJson(T& var, String& res, bool pretty=false)
{
	try {
		JsonIO io;
		Vis vis(io);
		var.Visit(vis);
		if (vis.IsError())
			return false;
		Value val = io.GetResult();
		res = AsJSON(val, pretty);
	}
	catch (...) {
		return false;
	}
	return true;
}

template <class T>
bool VisitToJsonFile(T& var, const char *file = NULL)
{
	try {
		String json = VisitToJson(var);
		FileOut s(file);
		s << json;
		s.Close();
	}
	catch (...) {
		return false;
	}
	return true;
}

template <class T>
void VisitCopy(const T& src, T& dst) {
	StringStream ss;
	{
		Vis vis(ss);
		ASSERT(vis.IsStoring());
		const_cast<T&>(src).Visit(vis);
	}
	ss.Seek(0);
	ss.SetLoading();
	{
		Vis vis(ss);
		ASSERT(vis.IsLoading());
		dst.Visit(vis);
	}
}

#endif
