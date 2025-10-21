#ifndef _Core_Recycler_h_
#define _Core_Recycler_h_


template <class T, bool keep_as_constructed=false>
class RecyclerPool {
	Vector<T*> pool;
	Mutex lock;
	bool destructed = false;
	
public:
	typedef RecyclerPool CLASSNAME;
	RecyclerPool() {}
	~RecyclerPool() {Clear(); destructed = true;}
	
	void Clear() {
		lock.Enter();
		auto t = pool.Begin();
		auto end = pool.End();
		while(t != end) {
			T* o = *t;
			if (keep_as_constructed)
				o->~T();
			MemoryFree(o);
			t++;
		}
		pool.Clear();
		lock.Leave();
	}
	void Reserve(int i) {
		lock.Enter();
		pool.Reserve(i);
		lock.Leave();
	}
	
	template <typename... Args>
	T* New(const Args&... args) {
		T* o;
		while (1) {
			if (pool.IsEmpty()) {
				o = (T*)MemoryAlloc(sizeof(T));
				new (o) T(args...);
			}
			else {
				lock.Enter();
				if (pool.IsEmpty()) {
					lock.Leave();
					continue;
				}
				o = pool.Pop();
				lock.Leave();
				if (!keep_as_constructed)
					new (o) T(args...);
			}
			break;
		}
		return o;
	}
	
	void Return(T* o) {
		if (destructed) {
			o->~T();
			MemoryFree(o);
			return;
		}
		if (!keep_as_constructed)
			o->~T();
		lock.Enter();
		pool.Add(o);
		lock.Leave();
	}
	
	static RecyclerPool& StaticPool() {MAKE_STATIC(RecyclerPool, pool); return pool;}
	
};


template <class T, bool keep_as_constructed>
class Recycler {
	
public:
	using Pool = RecyclerPool<T, keep_as_constructed> ;
	
private:
	Pool* pool = 0;
	One<T> o;
	
public:
	Recycler() {}
	Recycler(Pool& pool) : pool(&pool), o(pool.New()) {o->SetCount(0);}
	~Recycler() {Clear();}
	
	void Clear() {if (o && pool) pool->Return(o.Detach());}
	void Create(Pool& pool) {Clear(); this->pool = &pool; o = pool.New();}
	T& Get() {ASSERT(!o.IsEmpty()); return *o;}
	
	T& operator*() {return Get();}
	T* operator->() {return &Get();}
	
};


template <class T>
struct RecyclerRefBase : RefBase {
	using Pool = RecyclerPool<RecyclerRefBase<T>>;
	
	~RecyclerRefBase() {Clear();}
	void Clear() override {
		if (!RefBase::obj) return;
		T::Pool::StaticPool().Return((T*)RefBase::obj);
		RefBase::obj = 0;
	}
	void Delete() override {
		Pool::StaticPool().Return(this);
	}
	void SetObj(T* o) {
		(T*&)RefBase::obj = o;
	}
};


template <class T> using SharedRecycler = Shared<T, RecyclerRefBase<T>>;



#endif
