#include "LLDBUtils.h"

#include <memory>
#include <ide/Core/Logger.h>
#include <ide/Common/CommandLineOptions.h>

using namespace Upp;

One<ILLDBUtils> LLDBUtilsFactory::Create()
{
#if defined(PLATFORM_WIN32)
	return MakeOne<LLDBWindowsUtils>();
#elif defined(PLATFORM_POSIX)
	return MakeOne<LLDBPosixUtils>();
#endif
}

#if defined(PLATFORM_WIN32)

#define METHOD_NAME UPP_METHOD_NAME("LLDBWindowsUtils")

using DeleteHandleFun = std::function<void(HANDLE)>;

void DeleteHandle(HANDLE handle);

String LLDBWindowsUtils::BreakRunning(int pid)
{
	std::unique_ptr<void, DeleteHandleFun> handle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid), &DeleteHandle);
	if(!handle)
		return String().Cat() << "Failed to open process associated with " << pid << " PID.";
	
	if(Is64BitIde() && !Is64BitProcess(handle.get())) {
		auto path = String().Cat() << GetExeFolder() << "\\" << GetExeTitle() << "32.exe";
		auto cmd = String().Cat() << path << " " << COMMAND_LINE_GDB_DEBUG_BREAK_PROCESS_OPTION << " " << pid;
		
		String out;
		if(Sys(cmd, out) == COMMAND_LINE_GDB_DEBUG_BREAK_PROCESS_FAILURE_STATUS) {
			Logd() << METHOD_NAME << cmd;
			
			return String().Cat() << "Failed to interrupt process via 32-bit TheIDE. Output from command is \"" << out << "\".";
		}
		
		return "";
	}
	
	if (!DebugBreakProcess(handle.get()))
		return String().Cat() << "Failed to break process associated with " << pid << " PID.";
	
	return "";
}

bool LLDBWindowsUtils::Is64BitIde() const
{
	return sizeof(void*) == 8;
}

bool LLDBWindowsUtils::Is64BitProcess(HANDLE handle) const
{
	BOOL is_wow_64 = FALSE;
	if(!IsWow64Process(handle, &is_wow_64)) {
		Loge() << METHOD_NAME << "Failed to check that process is under wow64 emulation layer.";
	}
	
	return !is_wow_64;
}

#undef METHOD_NAME

#elif defined(PLATFORM_POSIX)

String LLDBPosixUtils::BreakRunning(int pid)
{
	if(kill(pid, SIGINT) == -1)
		return String().Cat() << "Failed to interrupt process associated with " << pid << " PID.";
	
	return "";
}

#endif
