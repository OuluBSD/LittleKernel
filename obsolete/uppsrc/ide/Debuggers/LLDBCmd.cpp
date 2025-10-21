#include "Debuggers.h"

#define LLOG(x)   // DLOG(x)


const char *FindTag(const char *txt, const char *tag);
const char *AfterTag(const char *txt, const char *tag);
const char *AfterHeading(const char *txt, const char *heading);
VectorMap<String, String> DataMap(const ArrayCtrl& data);
void MarkChanged(const VectorMap<String, String>& m, ArrayCtrl& data);

void LLDB::SyncFrameButtons()
{
	int ii = frame.GetIndex();
	bool lock = IdeIsDebugLock();
	frame_down.Enable(!lock && ii >= 0 && (ii < frame.GetCount() - 1 || frame.GetCount() == 1));
	frame_up.Enable(!lock && ii > 0);
}

void LLDB::Lock()
{
	IdeDebugLock();
	watches.Disable();
	locals.Disable();
	frame.Disable();
	SyncFrameButtons();
	dlock.Show();
}

void LLDB::Unlock()
{
	if(IdeDebugUnLock()) {
		watches.Enable();
		locals.Enable();
		frame.Enable();
		SyncFrameButtons();
		dlock.Hide();
	}
}

bool LLDB::Result(String& result, const String& s)
{
	int l = result.GetLength();
	result.Cat(s);
	int q = result.Find("^done\n", max(0, l - 50));
	if(q >= 0) {
		result.Trim(q);
		return true;
	}
	return false;
}

String LLDB::Cmd(const char *command, bool start)
{
	if(!dbg.IsRunning() || IdeIsDebugLock()) return Null;
	TimeStop ts;
	Lock();
	String head;
	dbg.Read(head);
	if(command) {
		LLOG("========= Cmd: " << command);
		dbg.Write(String(command) + "\n");
		PutVerbose(String() << "Command: " << command);
	}
	String result;
	int ms0 = msecs();
	while(dbg.IsRunning()) {
		String s;
		if(!dbg.Read(s)) {
			PutVerbose(result);
			PutVerbose("Debugger terminated");
			LLOG("Running: " << dbg.IsRunning());
			break;
		}
		if(!s.IsEmpty() && Result(result, s)) {
			LLOG(result);
			PutVerbose(result);
			if(start) {
				start = false;
				result = s.Mid(result.GetCount());
			}
			else
				break;
		}
		if(ms0 != msecs()) {
			ProcessEvents();
			ms0 = msecs();
		}
		
		GuiSleep(50);
		
		if(TTYQuit())
			Stop();
	}
	Unlock();
	if(command) {
		PutVerbose(String() << "Time of `" << command <<"` " << ts);
	}
	result.Replace("\\n", "\n");
	if (result.Left(2) == "~\"") result = result.Mid(2);
	if (result.Right(1) == "\"") result = result.Left(result.GetCount()-1);
	
	PutVerbose("=========== Result:");
	PutVerbose(result);
	PutVerbose("===================");
#ifdef PLATFORM_POSIX
	HostSys("xdotool key XF86Ungrab"); // force X11 to relese the mouse capture
#endif
	return result;
}

String LLDB::FastCmd(const char *command)
{
	if(!dbg.IsRunning() || IdeIsDebugLock()) return Null;
	bool lock = false;
	if(command) {
		dbg.Write(String(command) + "\n");
		PutVerbose(String() << "Fast Command: " << command);
	}
	String result;
	TimeStop ts;
	while(dbg.IsRunning()) {
		String s;
		if(TTYQuit()) {
			LLOG("TTYQuit");
			Stop();
		}
		if(!dbg.Read(s)) {
			LLOG(result);
			PutVerbose(result);
			PutVerbose("dbg terminated");
			LLOG("Running: " << dbg.IsRunning());
			break;
		}
		if(!s.IsEmpty() && Result(result, s)) {
			LLOG(result);
			LLOG("Result length: " << result.GetLength());
			if(result.GetLength() < 1000)
				PutVerbose(result);
			break;
		}
		if(s.GetCount() == 0)
			Sleep(0);
		if(ts.Elapsed() > 500) {
			if(!lock) {
				lock = true;
				Lock();
			}
			Ctrl::ProcessEvents();
		}
	}
	if(lock)
		Unlock();
#ifdef _DEBUG
	if(command) {
		PutVerbose(String() << "Time of `" << command <<"` " << ts);
	}
#endif

	result.Replace("\\n", "\n");
	if (result.Left(2) == "~\"") result = result.Mid(2);
	if (result.Right(1) == "\"") result = result.Left(result.GetCount()-1);
	
	PutVerbose("Result: " + result);
	return result;
}

void LLDB::Stop()
{
	LLOG("Stop");
	if(dbg.IsRunning())
		dbg.Kill();
}

bool LLDB::IsFinished()
{
	return !dbg.IsRunning() && !IdeIsDebugLock();
}
