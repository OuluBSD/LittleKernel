#include "Core.h"

#if SINGLETHREADED

namespace Upp {

#define LLOG(x)  // DLOG(x)

static Mutex& sMutexLock()
{ // this is Mutex intended to synchronize initialization of other primitives
	static Mutex m;
	return m;
}

INITBLOCK {
	sMutexLock();
}

Thread::Thread()
{
	sMutexLock();
}

void Thread::Detach()
{
	
}

static Atomic sThreadCount;

static thread_local void (*sExit)(void);

void (*Thread::AtExit(void (*exitfn)()))()
{
	void (*prev)() = sExit;
	sExit = exitfn;
	return prev;
}

struct sThreadExitExc__ {};

void Thread::Exit()
{
	throw sThreadExitExc__();
}

bool Thread::Run(Function<void ()> _cb, bool noshutdown)
{
	return false;
}

bool Thread::RunNice(Function<void ()> cb, bool noshutdown)
{
	return true;
}

bool Thread::RunCritical(Function<void ()> cb, bool noshutdown)
{
	if(Run(cb, noshutdown)) {
		Critical();
		return true;
	}
	return false;
}

Thread::~Thread()
{
	Detach();
}

bool Thread::IsUpp()
{
	return true;
}

bool Thread::IsST() //the containing thread (of wich there may be multiple) has not run its Run() yet
{
	return true;
}

bool Thread::IsMain() //the calling thread is the Main Thread or the only one in App
{
	return true;
}

int Thread::GetCount()
{
	return 0;
}

static int sShutdown;

void Thread::BeginShutdownThreads()
{
	sShutdown++;
}

void Thread::EndShutdownThreads()
{
	sShutdown--;
}

void Thread::DumpDiagnostics()
{
	
}


static StaticMutex mtx;

static Vector<void (*)()>& sShutdownFns()
{
	static Vector<void (*)()> m;
	return m;
}

void Thread::AtShutdown(void (*shutdownfn)())
{
	Mutex::Lock __(mtx);
	sShutdownFns().Add(shutdownfn);
}

void Thread::TryShutdownThreads()
{
	for(auto fn : sShutdownFns())
		(*fn)();
}

void Thread::ShutdownThreads()
{
	BeginShutdownThreads();
	while(GetCount()) {
		TryShutdownThreads();
		Sleep(100);
	}
	EndShutdownThreads();
}

bool Thread::IsShutdownThreads()
{
	return sShutdown;
}

int Thread::Wait()
{
	return -1;
}

bool Thread::Priority(int percent)
{
	return false;
}

void Thread::Start(Function<void ()> cb, bool noshutdown)
{
	Thread t;
	t.Run(cb);
	t.Detach();
}

void Thread::StartNice(Function<void ()> cb, bool noshutdown)
{
	Thread t;
	t.Run(cb);
	t.Nice();
	t.Detach();
}

void Thread::StartCritical(Function<void ()> cb, bool noshutdown)
{
	Thread t;
	t.Run(cb);
	t.Critical();
	t.Detach();
}

void Thread::Sleep(int msec)
{
#ifdef PLATFORM_WIN32
	::Sleep(msec);
#endif
#ifdef PLATFORM_POSIX
	::timespec tval;
	tval.tv_sec = msec / 1000;
	tval.tv_nsec = (msec % 1000) * 1000000;
	nanosleep(&tval, NULL);
#endif
}


void Semaphore::Release()
{
	
}

void Semaphore::Release(int n)
{
	
}

bool Semaphore::Wait(int timeout_ms)
{
	return false;
}

Semaphore::Semaphore()
{
	
}

Semaphore::~Semaphore()
{
	
}

Mutex& sMutexLock();

bool Mutex::TryEnter()
{
	return true;
}

/* Win32 RWMutex implementation by Chris Thomasson, cristom@comcast.net */

void RWMutex::EnterWrite()
{
	
}

void RWMutex::LeaveWrite()
{
	
}

void RWMutex::EnterRead()
{
	
}

void RWMutex::LeaveRead()
{
	
}

RWMutex::RWMutex()
{
	
}

RWMutex::~RWMutex()
{
	
}

bool ConditionVariable::Wait(Mutex& m, int timeout_ms)
{
	return false;
}

void ConditionVariable::Signal()
{
	
}

void ConditionVariable::Broadcast()
{
	
}

ConditionVariable::ConditionVariable()
{
	
}

ConditionVariable::~ConditionVariable()
{
	
}


void LazyUpdate::Invalidate()
{
	
}

bool LazyUpdate::BeginUpdate() const
{
	return false;
}

void LazyUpdate::EndUpdate() const
{
	
}

LazyUpdate::LazyUpdate()
{
	
}

void SpinLock::Wait()
{
	
}

bool StartAuxThread(auxthread_t (auxthread__ *fn)(void *ptr), void *ptr)
{
	return false;
}

}

#endif
