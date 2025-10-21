#ifndef _Core_VfsBase_VCS_h_
#define _Core_VfsBase_VCS_h_

struct NodeVisitor;

class VersionControlSystem {
	bool storing = false;
	String dir;
	bool pretty_json = true;
	
	typedef enum {
		ST_OBJECT,
		ST_VECTOR,
		ST_MAP,
		ST_MAP_KV,
		ST_AT_KV,
	} ScopeType;
	struct Scope : Moveable<Scope> {
		String key;
		ScopeType type;
		One<JsonIO> json;
		Value value;
	};
	Vector<Scope> scopes;
	
	Scope& Push(ScopeType t, String key);
	void Pop();
	void RemoveIdxFolders(int begin);
	void RealizeScopeJson(const String& key);
public:
	typedef VersionControlSystem CLASSNAME;
	VersionControlSystem();
	~VersionControlSystem();
	void Initialize(String path, bool storing);
	void Close();
	bool IsStoring() const;
	bool IsLoading() const;
	void BeginObject(String key);
	void BeginVector(String key, int count);
	void BeginMap(String key, int count);
	void BeginAt(int i);
	void BeginKV(int i);
	void BeginKeyVisit();
	void BeginValueVisit();
	void End();
	void SetPrettyJson(bool b=true) {pretty_json = b;}
	
	
	void DoBinary(String key, void* data, int len) {
		String path = GetCurrentPath(key + ".bin");
		if (storing) {
			FileOut fout(path);
			fout.Put(data, len);
		}
		else {
			FileIn fin(path);
			fin.Get(data, len);
		}
	}
	
	template <class T>
	void BeginVector(String key, T& o) {
		Push(ST_VECTOR, key);
		if (storing)
			RemoveIdxFolders(o.GetCount());
		else {
			int count = ResolveCount();
			o.SetCount(count);
		}
	}
	
	template <class T>
	void BeginMap(String key, T& o) {
		using KeyType = decltype(o.PopKey());
		Push(ST_MAP, key);
		if (storing)
			RemoveIdxFolders(o.GetCount());
		else {
			o.Clear();
			String dir = GetCurrentDirectory();
			int count = ResolveCount();
			o.Reserve(count);
			for(int i = 0; i < count; i++) {
				String path = dir + DIR_SEPS + IntStr(i) + DIR_SEPS "$key.json";
				KeyType key;
				LoadFromJsonFile(key, path);
				o.Add(pick(key));
			}
		}
	}
	
	template <class T>
	void BeginKeyStore(int i, const T& map_key) {
		String key = IntStr(i);
		Push(ST_OBJECT, key);
		RealizeScopeJson(key);
		StoreAsJsonFile(map_key, GetCurrentPath("$key.json"));
	}


	// "KV" Map requires visiting the key, while normal Map requires only jsonizing the key
	template <class T>
	void BeginMapKV(String key, T& o);

	template <class T>
	void Do(String key, T& o) {
		Scope& scope = scopes.Top();
		if (!scope.json) {
			if (storing) scope.json.Create();
			else return;
		}
		int a = key.Find(")");
		if (a >= 0) key = key.Mid(a+1);
		(*scope.json)(key, o);
	}
	String GetCurrentDirectory() const;
	String GetCurrentPath(String name) const;
	int ResolveCount() const;
};

#endif
