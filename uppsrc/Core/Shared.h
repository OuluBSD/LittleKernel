#ifndef _Core_Shared_h_
#define _Core_Shared_h_


struct WeakBase {
	virtual void SetDeleted() = 0;
};

template <class T> class Weak;

struct RefBase {
	void* obj = NULL;
	Vector<WeakBase*> weaks;
	Atomic refs;
	virtual ~RefBase() {ASSERT(!obj);} // deleting obj requires known type, use RefTemplate
	virtual void Clear() = 0;
	virtual void Delete() = 0;
	void Inc() {refs++;}
	void Dec() {
		refs--;
		if (refs <= 0) {
			for(int i = 0; i < weaks.GetCount(); i++)weaks[i]->SetDeleted();
			Clear();
			Delete();
		}
	}
	void IncWeak(WeakBase* w) {weaks.Add(w);}
	void DecWeak(WeakBase* w) {for(int i = 0; i < weaks.GetCount(); i++) if (weaks[i] == w) {weaks.Remove(i--);}}
	int GetRefCount() const {return refs;}
protected:
	RefBase() {refs = 1;}
};

template <class T>
struct RefTemplate : RefBase {
	~RefTemplate() {Clear();}
	void Clear() override {
		if (!obj) return;
		delete ((T*)obj);
		obj = NULL;
	}
	void Delete() override {
		delete this;
	}
};





template <class T, class Base=RefTemplate<T>>
class Shared : Moveable<Shared<T>> {
	
protected:
	friend class Weak<T>;
	RefBase* r = NULL;
	T* o = NULL;

public:
	using S = Shared<T,Base>;
	using Class = T;
	template <class K> using SharedT = Shared<K,Base>;
	
	Shared() {}
	Shared(Shared&& s) {r = s.r; s.r = NULL; o = s.o; s.o = NULL;}
	Shared(const Shared& o) {*this = o;}
	Shared(T* o, RefBase* r) : o(o), r(r) {
		ASSERT(r->GetRefCount() > 0);
	}
	Shared(T* o) : o(o) {
		Base* r = new Base;
		r->obj = o;
		ASSERT(r->GetRefCount() > 0);
	}
	Shared(const Nuller&) {}
	~Shared() { Clear(); }
	
	template <class C> C& Create() {
		static_assert(std::is_base_of<T,C>::value, "Class is not the base of T");
		Clear(); r = new RefTemplate<C>();
		C* c = new C();
		o = c; r->obj = o;
		return *c;
	}
	void Create() { Clear(); r = new RefTemplate<T>(); o = new T(); r->obj = o;}
	template<class K> void CreateAbstract() { Clear(); r = new RefTemplate<T>(); o = new K(); r->obj = o;}
	void Clear() { if (r) { r->Dec(); r = NULL; o = NULL;} }
	void operator=(const Shared& s) {if (r == s.r) return; SetPtr(s.o, s.r);}
	bool IsEmpty() const { return r == NULL; }
	int GetRefCount() const {return r ? r->GetRefCount() : 0;}
	T* operator->() {if (r) return o; return NULL;}
	T* operator->() const {if (r) return o; return NULL;}
	T* Get() const {if (r) return o; return NULL;}
	operator bool() const {return !IsEmpty();}
	T& operator*() const {return *Get();}
	bool operator==(const T* ptr) const {if (r) return o == ptr; return false;}
	bool operator!=(const T* ptr) const {if (r) return o != ptr; return true;}
	bool operator==(const Shared& s) const {if (r && s.r) return o == s.o; return false;}
	bool operator!=(const Shared& s) const {if (r && s.r) return o != s.o; return true;}
	template <class K> SharedT<K> As() {
		static_assert(std::is_base_of<T, K>() || std::is_base_of<K, T>(), "K -> T or T -> K inheritance is required");
		
		if (o) {
			K* ptr = CastPtr<K>(o);
			if (ptr) {
				SharedT<K> s;
				s.SetPtr(ptr, r);
				return s;
			}
		}
		return SharedT<K>();
	}
	void SetPtr(T* o, RefBase* r) {
		S tmp; Swap(*this, tmp); // don't unref until new ref!!!111!1
		this->o = o;
		this->r = r;
		if (r) r->Inc();
	}
	Shared& WrapObject(T* obj) {
		Clear();
		if (obj) {
			r = new RefTemplate<T>();
			r->obj = obj;
			o = obj;
		}
		return *this;
	}
	const Base* GetBase() const {return r;}
	String ToString() const {return r ? o->ToString() : "";}
	
};




template <class T> inline Shared<T> MakeShared() {return Shared<T>(new T());}
template <class T, class P> inline Shared<T> MakeSharedBase() {Shared<P> s; s.Create(); return s.template As<T>();}

template <class T, class A0> inline Shared<T> MakeShared(A0 a0) {return Shared<T>(new T(a0));}
template <class T, class A0, class A1> inline Shared<T> MakeShared(A0 a0, A1 a1) {return Shared<T>(new T(a0, a1));}
template <class T, class A0, class A1, class A2> inline Shared<T> MakeShared(A0 a0, A1 a1, A2 a2) {return Shared<T>(new T(a0, a1, a2));}
template <class T, class A0, class A1, class A2, class A3> inline Shared<T> MakeShared(A0 a0, A1 a1, A2 a2, A3 a3) {return Shared<T>(new T(a0, a1, a2, a3));}
template <class T, class A0, class A1, class A2, class A3, class A4> inline Shared<T> MakeShared(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4) {return Shared<T>(new T(a0, a1, a2, a3, a4));}
template <class T, class A0, class A1, class A2, class A3, class A4, class A5> inline Shared<T> MakeShared(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {return Shared<T>(new T(a0, a1, a2, a3, a4, a5));}
template <class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6> inline Shared<T> MakeShared(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {return Shared<T>(new T(a0, a1, a2, a3, a4, a5, a6));}



template <class T>
class Weak : public WeakBase {
	struct RefBase* r = NULL;
	T* o = NULL;
	
public:
	Weak() {}
	Weak(const Weak& w) : r(w.r), o(w.o) {if (r) r->IncWeak(this);}
	Weak(const Shared<T>& s) : r(s.r), o(s.o) {if (r) r->IncWeak(this);}
	~Weak() {Clear();}
	
	void SetDeleted() override {r = NULL;}
	void Clear() {if (r) r->DecWeak(this); r = NULL; o = NULL;}
	void operator=(const Shared<T>& s) { Clear(); r = s.r; o = s.o; if (r) r->IncWeak(this);}
	void operator=(Weak<T>&& p) { Clear(); r = p.Get().r; o = p.Get().o; if (r) r->IncWeak(this);}
	void operator=(const Weak<T>& p) { Clear(); r = p.r; o = p.o; if (r) r->IncWeak(this);}
	bool IsEmpty() const { return r == NULL; }
	T* operator->() {return o;}
	T* Get() {return o;}
	operator bool() {return !IsEmpty();}
	Shared<T> Enter() const {Shared<T> s; s.r = r; s.o = o; if (s.r) s.r->Inc(); return s;}
};


template <class T>
class EnableSharedFromThis {
	Weak<T> weak;
	
public:
	virtual ~EnableSharedFromThis() = default;
	
	void InitWeak(const Shared<T>& s) {ASSERT(weak.IsEmpty()); weak = s;}
	
	bool HasWeak() const {return !weak.IsEmpty();}
	
	Shared<T> GetSharedFromThis() {
		ASSERT(!weak.IsEmpty());
		return weak.Enter();
	}
	
	template <class V>
	Shared<V> AsShared() {return GetSharedFromThis().template As<V>();}
};


#endif
