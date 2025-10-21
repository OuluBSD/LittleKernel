#if SINGLETHREADED
#ifdef DEPRECATED
#define thread__
#endif

#ifdef flagPROFILEMT
class Mutex;
class StaticMutex;

struct MtInspector {
	const char *name;
	int   number;
	int   locked;
	int   blocked;
	
	static MtInspector *Dumi();

	MtInspector(const char *s, int n = -1) { name = s; number = n; locked = blocked = 0; }
	~MtInspector();
};

#define PROFILEMT(mutex) \
	{ static MtInspector MK__s(__FILE__, __LINE__); mutex.Set(MK__s); }

#define PROFILEMT_(mutex, id) \
	{ static MtInspector MK__s(id); mutex.Set(MK__s); }

#else

#define PROFILEMT(mutex)
#define PROFILEMT_(mutex, id)

#endif

template<typename Res, typename... ArgTypes>
class Function<Res(ArgTypes...)>;

class Thread : NoCopy {
	int stack_size = 0;

public:
	bool       Run(Function<void ()> cb, bool noshutdown = false);
	bool       RunNice(Function<void ()> cb, bool noshutdown = false);
	bool       RunCritical(Function<void ()> cb, bool noshutdown = false);

	void       Detach();
	int        Wait();

	bool       IsOpen() const     { return 0; }

	typedef int Handle;
	typedef int Id;

	Id          GetId() const                  { return 0; }

	Handle      GetHandle() const              { return 0; }
	
	bool        Priority(int percent); // 0 = lowest, 100 = normal
	void        StackSize(int bytes)           { stack_size = bytes; }
	
	void        Nice()                         { Priority(25); }
	void        Critical()                     { Priority(150); }

	static void Start(Function<void ()> cb, bool noshutdown = false);
	static void StartNice(Function<void ()> cb, bool noshutdown = false);
	static void StartCritical(Function<void ()> cb, bool noshutdown = false);

	static void Sleep(int ms);

	static bool IsST();
	static bool IsMain();
	static bool IsUpp();
	static int  GetCount();
	static void BeginShutdownThreads();
	static void AtShutdown(void (*shutdownfn)());
	static void TryShutdownThreads();
	static void EndShutdownThreads();
	static void ShutdownThreads();
	static bool IsShutdownThreads();
	static void (*AtExit(void (*exitfn)()))();

	static void Exit();
	
	static void DumpDiagnostics();

	static Handle GetCurrentHandle()          { return 0; }
	static inline Id GetCurrentId()           { return 0; }

	Thread();
	~Thread();

private:
	void operator=(const Thread&);
	Thread(const Thread&);
};

inline void AssertST() {}

class Semaphore : NoCopy {
public:
	bool       Wait(int timeout_ms = -1);
	void       Release();
	void       Release(int n);

	Semaphore();
	~Semaphore();
};

struct MtInspector;


class Mutex : NoCopy {
protected:
	MtInspector        *mti;

	Mutex(int)         {}

	friend class ConditionVariable;

public:
	bool  TryEnter();
	void  Leave()                { }

	void  Enter()                { }

	Mutex()                      { }
	~Mutex()                     { }

	class Lock;
};

class RWMutex : NoCopy {
	
public:
	void EnterWrite();
	void LeaveWrite();

	void EnterRead();
	void LeaveRead();

	RWMutex();
	~RWMutex();
};

class ConditionVariable {
	
public:
	bool Wait(Mutex& m, int timeout_ms = -1);
	void Signal();
	void Broadcast();
	
	ConditionVariable();
	~ConditionVariable();
};


typedef bool OnceFlag;

#define ONCELOCK_(o_b_) \
for(static int o_b_ = 0; o_b_ < 1; o_b_++)

#define ONCELOCK \
for(static int ___ = 0; ___ < 1; ___++)


class Mutex::Lock : NoCopy {
	Mutex& s;

public:
	Lock(Mutex& s) : s(s) { s.Enter(); }
	~Lock()               { s.Leave(); }
};

template <class Primitive>
class StaticPrimitive_ : NoCopy {
	Primitive *primitive;
	byte       buffer[sizeof(Primitive)];
	OnceFlag   once;
	
	void Initialize() { primitive = new(buffer) Primitive; }

public:
	Primitive& Get()  { ONCELOCK_(once) Initialize(); return *primitive; }
};

class StaticMutex : StaticPrimitive_<Mutex> {
public:
	operator Mutex&()          { return Get(); }
	bool TryEnter()            { return Get().TryEnter();}
	void Enter()               { Get().Enter();}
	void Leave()               { Get().Leave(); }
};

class StaticSemaphore : StaticPrimitive_<Semaphore> {
public:
	operator Semaphore&()        { return Get(); }
	void Wait()                  { Get().Wait(); }
	void Release()               { Get().Release(); }
};

class StaticRWMutex : StaticPrimitive_<RWMutex> {
public:
	operator RWMutex&()  { return Get(); }
	void EnterRead()     { Get().EnterRead();}
	void LeaveRead()     { Get().LeaveRead(); }
	void EnterWrite()    { Get().EnterWrite();}
	void LeaveWrite()    { Get().LeaveWrite(); }
};

class StaticConditionVariable : StaticPrimitive_<ConditionVariable> {
public:
	operator ConditionVariable&() { return Get(); }
	void Wait(Mutex& m)  { Get().Wait(m); }
	void Signal()        { Get().Signal(); }
	void Broadcast()     { Get().Broadcast(); }
};

class LazyUpdate {
	mutable Mutex              mutex;
	mutable bool               dirty;

public:
	void Invalidate();
	bool BeginUpdate() const;
	void EndUpdate() const;

	LazyUpdate();
};

inline bool IsMainThread() { return true; }

struct SpinLock : Moveable<SpinLock> {
	volatile int locked;

	bool TryEnter() { return true; }
	void Leave()    { }

	void Enter()    { }
	
	void Wait();
	
	class Lock;

	SpinLock()         { locked = 0; }
};

class SpinLock::Lock : NoCopy {
	SpinLock& s;

public:
	Lock(SpinLock& s) : s(s) { s.Enter(); }
	~Lock()                  { s.Leave(); }
};

#define INTERLOCKED

struct H_l_ : Mutex::Lock {
	bool b;
	H_l_(Mutex& cs) : Mutex::Lock(cs) { b = true; }
};

#define INTERLOCKED_(cs)

#ifdef DEPRECATED
typedef Mutex CriticalSection;
typedef StaticMutex StaticCriticalSection;
#endif

#define auxthread_t void *
#define auxthread__

bool StartAuxThread(auxthread_t (auxthread__ *fn)(void *ptr), void *ptr);

#endif
