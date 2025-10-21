template <class T, class... Args>
T& Single(Args... args) {
	static T o(args...);
	return o;
}

template <class T> // Workaround for GCC bug - specialization needed...
T& Single() {
	static T o;
	return o;
}

int RegisterTypeNo__(const char *type);

template <class T>
int StaticTypeNo() {
	static int typeno = RegisterTypeNo__(typeid(T).name());
	return typeno;
}

class Any : Moveable<Any> {
	struct BaseData {
		int      typeno;
		
		virtual ~BaseData() {}
	};

	template <class T>
	struct Data : BaseData {
		T        data;

		template <class... Args>
		Data(Args&&... args) : data(std::forward<Args>(args)...) { typeno = StaticTypeNo<T>(); }
	};

	BaseData *ptr;

	void Chk() const                              { ASSERT(ptr != (void *)1); }
	void Pick(Any&& s)                            { ptr = s.ptr; const_cast<Any&>(s).ptr = NULL; }

public:
	template <class T, class... Args> T& Create(Args&&... args) { Clear(); Data<T> *x = new Data<T>(std::forward<Args>(args)...); ptr = x; return x->data; }
	template <class T> bool Is() const            { return ptr && ptr->typeno == StaticTypeNo<T>(); }
	template <class T> T& Get()                   { ASSERT(Is<T>()); Chk(); return ((Data<T>*)ptr)->data; }
	template <class T> const T& Get() const       { ASSERT(Is<T>()); Chk(); return ((Data<T>*)ptr)->data; }

	void Clear()                                  { if(ptr) delete ptr; ptr = NULL; }

	bool IsEmpty() const                          { return ptr == NULL; }

	void operator=(Any&& s)                       { if(this != &s) { Clear(); Pick(pick(s)); } }
	Any(Any&& s)                                  { Pick(pick(s)); }
	
	Any(const Any& s) = delete;
	void operator=(const Any& s) = delete;

	Any()                                         { ptr = NULL; }
	~Any()                                        { Clear(); }
};

class Bits : Moveable<Bits> {
	int         alloc;
	dword      *bp;
	
	void Expand(int q);
	void Realloc(int nalloc);
	int  GetLast() const;

public:
	void   Clear();
	void   Set(int i, bool b = true) { ASSERT(i >= 0 && alloc >= 0); int q = i >> 5; if(q >= alloc) Expand(q);
	                                   i &= 31; bp[q] = (bp[q] & ~(1 << i)) | (b << i); }
	bool   Get(int i) const        { ASSERT(i >= 0 && alloc >= 0); int q = i >> 5;
	                                 return q < alloc ? bp[q] & (1 << (i & 31)) : false; }
	bool   operator[](int i) const { return Get(i); }

	void   Set(int i, dword bits, int count);
	dword  Get(int i, int count);
	void   Set64(int i, uint64 bits, int count);
	uint64 Get64(int i, int count);

	void   SetN(int i, bool b, int count);
	void   SetN(int i, int count)         { SetN(i, true, count); }
	
	void   Reserve(int nbits);
	void   Shrink();
	
	String ToString() const;

	dword       *CreateRaw(int n_dwords);
	const dword *Raw(int& n_dwords) const { n_dwords = alloc; return bp; }
	dword       *Raw(int& n_dwords)       { n_dwords = alloc; return bp; }
	
	void         Serialize(Stream& s);

	Bits()                                { bp = NULL; alloc = 0; }
	~Bits()                               { Clear(); }

	Bits(Bits&& b)                        { alloc = b.alloc; bp = b.bp; b.bp = NULL; }
	void operator=(Bits&& b)              { if(this != &b) { Clear(); alloc = b.alloc; bp = b.bp; b.bp = NULL; } }
	
	Bits(const Bits&) = delete;
	void operator=(const Bits&) = delete;
};

class PackedData {
	void *ptr = nullptr;
	
	template <class T>
	T Get(int ii, T def) const;

public:
	void   SetRawPtr(void *p)                   { ptr = p; }
	void  *GetRawPtr() const                    { return ptr; }

	void   SetData(int ii, const void *data, int datalen);

	template <class F>
	bool   GetData(int ii, F out) const;
	
	void   SetNull(int ii)                      { SetData(ii, NULL, 0); }

	void   SetString(int ii, const char *s)     { SetData(ii, s, (int)strlen(s)); }
	void   SetString(int ii, const String& s)   { SetData(ii, s, s.GetCount()); }
	String GetString(int ii) const              { String r; GetData(ii, [&](const char *s, int n) { r = String(s, n); }); return r; }
	
	void   SetInt(int ii, int val)              { SetData(ii, &val, sizeof(int)); }
	int    GetInt(int ii, int def) const        { return Get<int>(ii, def); }

	void   SetDword(int ii, dword val)          { SetData(ii, &val, sizeof(dword)); }
	int    GetDword(int ii, dword def) const    { return Get<dword>(ii, def); }

	void   SetInt64(int ii, int64 val)          { SetData(ii, &val, sizeof(int64)); }
	int64  GetInt64(int ii, int64 def) const    { return Get<int64>(ii, def); }

	void   SetPtr(int ii, void *val)            { SetData(ii, &val, sizeof(void *)); }
	void  *GetPtr(int ii) const                 { return Get<void *>(ii, nullptr); }
	
	void   Clear();

	Vector<String> Unpack() const;
	size_t         GetPackedSize() const;
	String         GetPacked() const           { return String((const char *)ptr, (int)GetPackedSize()); }

	PackedData() {}
	PackedData(const PackedData&) = delete;
	~PackedData();
};

/*
template <class T, int N = 1>
struct Link {
	T *link_prev[N];
	T *link_next[N];

protected:
	void LPN(int i)                      { link_prev[i]->link_next[i] = link_next[i]->link_prev[i] = (T *)this; }

public:
	NOUBSAN	T *GetPtr()                  { return (T *) this; }
	const T *GetPtr() const              { return (const T *) this; }
	T       *GetNext(int i = 0)          { return link_next[i]; }
	T       *GetPrev(int i = 0)          { return link_prev[i]; }
	const T *GetNext(int i = 0) const    { return link_next[i]; }
	const T *GetPrev(int i = 0) const    { return link_prev[i]; }

	NOUBSAN	void LinkSelf(int i = 0)     { link_next[i] = link_prev[i] = (T *)this; }
	void LinkSelfAll()                   { for(int i = 0; i < N; i++) LinkSelf(i); }
	void Unlink(int i = 0)               { link_next[i]->link_prev[i] = link_prev[i]; link_prev[i]->link_next[i] = link_next[i];
	                                       LinkSelf(i); }
	void UnlinkAll()                     { for(int i = 0; i < N; i++) Unlink(i); }
	NOUBSAN	void LinkBefore(Link *n, int i = 0)  { link_next[i] = (T *)n; link_prev[i] = link_next[i]->link_prev[i]; LPN(i); }
	NOUBSAN	void LinkAfter(Link *p, int i = 0)   { link_prev[i] = (T *)p; link_next[i] = link_prev[i]->link_next[i]; LPN(i); }

	T   *InsertNext(int i = 0)           { T *x = new T; x->LinkAfter(this, i); return x; }
	T   *InsertPrev(int i = 0)           { T *x = new T; x->LinkBefore(this, i); return x; }

	void DeleteList(int i = 0)           { while(link_next[i] != GetPtr()) delete link_next[i]; }

	bool InList(int i = 0) const         { return link_next[i] != GetPtr(); }
	bool IsEmpty(int i = 0) const        { return !InList(i); }

	Link()                               { LinkSelfAll(); }
	~Link()                              { UnlinkAll(); }

private:
	Link(const Link&);
	void operator=(const Link&);

public:
#ifdef _DEBUG
	void Dump() {
		for(T *t = GetNext(); t != this; t = t->GetNext())
			LOG(t);
		LOG("-------------------------------------");
	}
#endif
};
*/

template <int N = 1>
struct Link {
	Link *link_prev[N];
	Link *link_next[N];

protected:
	void LPN(int i)                      { link_prev[i]->link_next[i] = link_next[i]->link_prev[i] = this; }

public:
	Link       *GetNext(int i = 0)          { return link_next[i]; }
	Link       *GetPrev(int i = 0)          { return link_prev[i]; }
	const Link *GetNext(int i = 0) const    { return link_next[i]; }
	const Link *GetPrev(int i = 0) const    { return link_prev[i]; }

	void LinkSelf(int i = 0)     { link_next[i] = link_prev[i] = this; }
	void LinkSelfAll()                   { for(int i = 0; i < N; i++) LinkSelf(i); }
	void Unlink(int i = 0)               { link_next[i]->link_prev[i] = link_prev[i]; link_prev[i]->link_next[i] = link_next[i];
	                                       LinkSelf(i); }
	void UnlinkAll()                     { for(int i = 0; i < N; i++) Unlink(i); }
	void LinkBefore(Link *n, int i = 0)  { link_next[i] = n; link_prev[i] = link_next[i]->link_prev[i]; LPN(i); }
	void LinkAfter(Link *p, int i = 0)   { link_prev[i] = p; link_next[i] = link_prev[i]->link_next[i]; LPN(i); }

	bool InList(int i = 0) const         { return link_next[i] != this; }
	bool IsEmpty(int i = 0) const        { return !InList(i); }

	Link()                               { LinkSelfAll(); }
	~Link()                              { UnlinkAll(); }

private:
	Link(const Link&);
	void operator=(const Link&);

public:
#ifdef _DEBUG
	void Dump() {
		for(auto *t = GetNext(); t != this; t = t->GetNext())
			LOG(t);
		LOG("-------------------------------------");
	}
#endif
};
/*
template <class T, int N = 1>
class LinkOwner : public Link<T, N> {
public:
	~LinkOwner()                         { Link<T, N>::DeleteList(); }
};
*/
template <class T, class K = String>
class LRUCache {
public:
	struct Maker {
		virtual K      Key() const = 0;
		virtual int    Make(T& object) const = 0;
		virtual ~Maker() {}
	};

private:
	struct Item : Moveable<Item> {
		int    prev, next;
		int    size;
		One<T> data;
		bool   flag;
	};
	
	struct Key : Moveable<Key> {
		K            key;
		String       type;
		
		bool operator==(const Key& b) const { return key == b.key && type == b.type; }
		hash_t GetHashValue() const { return CombineHash(key, type); }
	};

	Index<Key>   key;
	Vector<Item> data;
	int  head;

	int  size;
	int  count;

	int  foundsize;
	int  newsize;
	bool flag = false;
	
	const int InternalSize = 3 * (sizeof(Item) + sizeof(Key) + 24) / 2;

	void Unlink(int i);
	void LinkHead(int i);

public:
	int  GetSize() const            { return size; }
	int  GetCount() const           { return count; }

	template <class P> void AdjustSize(P getsize);

	T&       GetLRU();
	const K& GetLRUKey();
	void     DropLRU();
	void     Shrink(int maxsize, int maxcount = 30000);
	void     ShrinkCount(int maxcount = 30000)          { Shrink(INT_MAX, maxcount); }

	template <class P> int  Remove(P predicate);
	template <class P> bool RemoveOne(P predicate);

	template <class B, class A>
	T&   Get(const Maker& m, B before_make, A after_make, int& sz);
	template <class B, class A>
	T&   Get(const Maker& m, B before_make, A after_make) { int sz; return Get(m, before_make, after_make, sz); }
	T&   Get(const Maker& m)                              { return Get(m, []{}, []{}); }

	void Clear();

	void ClearCounters();
	int  GetFoundSize() const       { return foundsize; }
	int  GetNewSize() const         { return newsize; }

	LRUCache() { head = -1; size = 0; count = 0; ClearCounters(); }
};

template <class T>
struct ManagedStatic {
	T o;
	bool destructed = false;
	const char* file;
	int line;
	
	typedef ManagedStatic CLASSNAME;
	ManagedStatic(const char* f, int l);
	template <class Arg> ManagedStatic(const char* f, int l, const Arg& value);
	~ManagedStatic() {
		if (!destructed)
			Destruct();
	}
	void Destruct() {if (!destructed) {Clear(); destructed = true;}}
	void Clear() {o.Clear();}
};

template <class T> ManagedStatic<T>::ManagedStatic(const char* f, int l) : file(f), line(l) {CallInExitBlock([this]{this->Destruct();});}
template <class T> template <class Arg>
ManagedStatic<T>::ManagedStatic(const char* f, int l, const Arg& value) : file(f), line(l), o(value) {CallInExitBlock([this]{this->Destruct();});}

template <class T>
struct ManagedStaticThreadLocal {
	T o;
	bool destructed = false;
	const char* file;
	int line;
	
	typedef ManagedStaticThreadLocal CLASSNAME;
	ManagedStaticThreadLocal(const char* f, int l);
	template <class Arg> ManagedStaticThreadLocal(const char* f, int l, const Arg& value);
	~ManagedStaticThreadLocal() {if (!destructed) Destruct();}
	void Destruct() {if (!destructed) {Clear(); destructed = true;}}
	void Clear() {o.Clear();}
};

template <class T> ManagedStaticThreadLocal<T>::ManagedStaticThreadLocal(const char* f, int l) : file(f), line(l) {}
template <class T> template <class Arg>
ManagedStaticThreadLocal<T>::ManagedStaticThreadLocal(const char* f, int l, const Arg& value) : file(f), line(l), o(value) {}

#define MAKE_STATIC(t, x) static ::UPP::ManagedStatic<t> __##x(__FILE__,__LINE__); t& x = __##x.o;
#define MAKE_STATIC_(t, x, param) static ::UPP::ManagedStatic<t> __##x(__FILE__,__LINE__,param); t& x = __##x.o;
#define MAKE_STATIC_LOCAL(t, x) thread_local static ::UPP::ManagedStaticThreadLocal<t> __##x(__FILE__,__LINE__); t& x = __##x.o;

template <typename T>
class TrackChanges {
public:
	TrackChanges() {
		change_count = 0;
	}
	TrackChanges(TrackChanges<T> && o) : value(std::move(o.value)) {
		change_count = 0;
	}
	TrackChanges(const TrackChanges<T>& o) : value(o.Get()) {
		change_count = 0;
	}
	
	TrackChanges<T>& operator=(TrackChanges<T> && o) {
		++change_count;
		value = std::move(o.value);
		return *this;
	}
	
	TrackChanges<T>& operator=(const TrackChanges<T>& o) {
		++change_count;
		value = o.value;
		return *this;
	}
	
	template <typename Func>
	void Set(Func func) {
		++change_count;
		func(value);
	}
	
	bool UpdateChangeCountBookmark(uint32* change_count_bookmark) const {
		uint32 new_value = *change_count_bookmark;
		uint32 prev = change_count.exchange(new_value);
		return prev != new_value;
	}
	
	const T& Get() const {
		return value;
	}
	
	T* operator->() {return &value;}
	const T* operator->() const {return &value;}
	
	T& operator*() {return value;}
	void operator++() {++change_count;}
private:
	T value;
	mutable Atomic change_count;
};


using NullOpt = std::nullopt_t;

#define null_opt std::nullopt

template <class T> using Optional = std::optional<T>;

//template <class T, class ...Args> std::optional<T> MakeOptional(Args... args) {return std::make_optional(args...);}
template <class T> std::optional<T> MakeOptional(const T& o) {return std::make_optional(o);}



template <class T> void RemoveLast(T& o) {
	int c = o.GetCount();
	if (c > 0)
		o.Remove(c-1);
}

template <class T, int I>
struct FixedArray : Moveable<FixedArray<T,I>> {
	
	static const int size = I;
	
	struct Iterator {
		T* ptr = 0;
		
		Iterator() {}
		Iterator(T* p) : ptr(p) {}
		T* operator->() const {return ptr;}
		T& operator*() const {return *ptr;}
		bool operator!=(const Iterator& i) const {return ptr != i.ptr;}
		bool operator==(const Iterator& i) const {return ptr == i.ptr;}
		void operator++() {++ptr;}
		void operator--() {--ptr;}
	};
	
	T vector[I];
	
	FixedArray() {}
	FixedArray(const Nuller& n) {for(int i = 0; i < I; i++) vector[i] = n;}
	int GetCount() const {return size;}
	T&       operator[](int i)       {ASSERT(i >= 0 && i < size); return vector[i];}
	const T& operator[](int i) const {ASSERT(i >= 0 && i < size); return vector[i];}
	void Clear() {for(int i = 0; i < I; i++) vector[i] = Null;}
	void operator=(const Nuller& n) {for(int i = 0; i < I; i++) vector[i] = n;}
	void operator=(const T& value) {for(int i = 0; i < I; i++) this->vector[i] = value;}
	T& Top() {return vector[I-1];}
	
	Iterator begin() {return Iterator(&vector[0]);}
	Iterator end() {return Iterator(&vector[0] + I);}
	
	bool IsEmpty() const {return false;}
	void SetAll(const T& o) {for(int i = 0; i < I; i++) vector[i] = o;}
	String ToString() const {return "FixedArray";}
	int ToInt() const {return I;}
	double ToDouble() const {return I;}
	operator double() const {return I;}
	
	hash_t GetHashValue() const {CombineHash c; for(int i = 0; i < I; i++) c.Put(UPP::GetHashValue<T>(vector[i])); return c;}
	T* Get() {return vector;}
	
};


struct TypeCls : Moveable<TypeCls>, std::type_index {
	TypeCls() : std::type_index(typeid(void)) {}
	TypeCls(const TypeCls& t) : std::type_index(t) {}
	TypeCls(const std::type_info& t) : std::type_index(t) {}
	operator bool() const {return *this != std::type_index(typeid(void));}
	String GetName() const {return CppDemangle(this->name());}
	hash_t GetHashValue() const {return this->hash_code();}
	String ToString() const {return this->name();}
};

template <class T> TypeCls AsTypeCls(T* o=0) {return typeid(T);}
template <class T> String AsTypeName(T* o=0) {return CppDemangle(typeid(T).name());}
// Note: AsTypeHash is considered to be persistent and cross-platform (& cross-compiler),
//       which currently requires the user to set the type string.
//       If std had demangler or mangled name was standard, this would be unnecessary.
//       Usage: call TypedStringHasher<T>("name of T") early in constructor or main.
//       Use AsTypeCls().GetHashValue() if you need easy & non-persistent hash.
template <class T> hash_t TypedStringHasher(const char* s);
template <class T> hash_t AsTypeHash(T* o=0) {return TypedStringHasher<T>(0);}
inline TypeCls AsVoidTypeCls() {return TypeCls();} // for explicit naming

template <class T> using TypeMap = ArrayMap<TypeCls, T>;

#include "Other.hpp"
