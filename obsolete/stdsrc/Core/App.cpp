#include "Core.h"

NAMESPACE_UPP

static char sHomeDir[_MAX_PATH + 1];
static char Argv0__[_MAX_PATH + 1];
static int exitcode;
static bool sMainRunning;
static bool NoMemoryLeaksCheck;

void  SetExitCode(int code) { exitcode = code; }
int   GetExitCode()         { return exitcode; }

bool  IsMainRunning()
{
	return sMainRunning;
}


void CommonInit()
{
	// TODO
	#if 0
#ifdef PLATFORM_WIN32
	LoadLangFiles(GetFileFolder(GetExeFilePath()));
#else
	LoadLangFiles(GetHomeDirectory());
#endif
	Vector<WString>& cmd = coreCmdLine__();
	static WString exp_cmd = "--export-tr";
	static WString brk_cmd = "--memory-breakpoint__";
	
	for(int i = 0; i < cmd.GetCount();) {
		if(cmd[i] == exp_cmd) {
			{
				i++;
				int lang = 0;
				int lang2 = 0;
				byte charset = CHARSET_UTF8;
				String fn = "all";
				if(i < cmd.GetCount())
					if(cmd[i].GetLength() == 4 || cmd[i].GetLength() == 5) {
						lang = LNGFromText(cmd[i].ToString());
						fn = cmd[i].ToString();
						int c = cmd[i][4];
						if(c >= '0' && c <= '8')
							charset = c - '0' + CHARSET_WIN1250;
						if(c >= 'A' && c <= 'J')
							charset = c - 'A' + CHARSET_ISO8859_1;
					}
				fn << ".tr";
				if(++i < cmd.GetCount() && (cmd[i].GetLength() == 4 || cmd[i].GetLength() == 5))
					lang2 = LNGFromText(cmd[i].ToString());
			#ifdef PLATFORM_WIN32
				FileOut out(GetExeDirFile(fn));
			#else
				FileOut out(GetHomeDirFile(fn));
			#endif
				if(lang) {
					if(lang2)
						SaveLngFile(out, SetLNGCharset(lang, charset), SetLNGCharset(lang2, charset));
					else
						SaveLngFile(out, SetLNGCharset(lang, charset));
				}
				else {
					Index<int> l = GetLngSet();
					for(int i = 0; i < l.GetCount(); i++)
						SaveLngFile(out, SetLNGCharset(l[i], charset));
				}
			}
			exit(0);
		}
	#if defined(_DEBUG) && defined(UPP_HEAP)
		if(cmd[i] == brk_cmd && i + 1 < cmd.GetCount()) {
			MemoryBreakpoint(atoi(cmd[i + 1].ToString()));
			cmd.Remove(i, 2);
		}
		else
			i++;
	#else
		i++;
	#endif
	}
	sMainRunning = true;
	
	#endif
}

void Exit(int code)
{
	SetExitCode(code);
	throw ExitExc();
}

void MemorySetMainBegin__() {}
void MemorySetMainEnd__() {}

void AppExecute__(void (*app)())
{
	try {
		MemorySetMainBegin__();
		(*app)();
		MemorySetMainEnd__();
	}
	catch(ExitExc) {
		return;
	}
}

#ifdef PLATFORM_POSIX

void s_ill_handler(int)
{
	CrashHook();
	Panic("Illegal instruction!");
}

void s_segv_handler(int)
{
	CrashHook();
	Panic("Invalid memory access!");
}

void s_fpe_handler(int)
{
	CrashHook();
	Panic("Invalid arithmetic operation!");
}

void AppInit__(int argc, const char **argv, const char **envptr)
{
	SetLanguage(LNG_ENGLISH);
	sSetArgv0__(argv[0]);
	for(const char *var; (var = *envptr) != 0; envptr++)
	{
		const char *b = var;
		while(*var && *var != '=')
			var++;
		String varname(b, var);
		if(*var == '=')
			var++;
		EnvMap().Add(varname.ToWString(), String(var).ToWString());
	}
	Vector<WString>& cmd = coreCmdLine__();
	for(int i = 1; i < argc; i++)
		cmd.Add(FromSystemCharset(argv[i]).ToWString());
	CommonInit();
	signal(SIGILL, s_ill_handler);
	signal(SIGSEGV, s_segv_handler);
	signal(SIGBUS, s_segv_handler);
	signal(SIGFPE, s_fpe_handler);
}
#endif

#if defined(PLATFORM_WIN32)

#ifdef _DEBUG
static BOOL WINAPI s_consoleCtrlHandler(DWORD signal) {
	if(signal == CTRL_C_EVENT) {
		extern bool NoMemoryLeaksCheck;
		NoMemoryLeaksCheck = true;
	}
    return FALSE;
}
#endif

void AppInitEnvironment__()
{
	SetLanguage(LNG_('E', 'N', 'U', 'S'));
	int nArgs;
    /* TODO
    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if(szArglist) {
		strcpy(Argv0__, FromSystemCharsetW(szArglist[0]));
		for(int i = 1; i < nArgs; i++)
			coreCmdLine__().Add(FromSystemCharsetW(szArglist[i]).ToWString());
		LocalFree(szArglist);
    }
		
	WCHAR *env = GetEnvironmentStringsW();
	for(WCHAR *ptr = env; *ptr; ptr++)
	{
		const WCHAR *b = ptr;
		if(*ptr)
			ptr++;
		while(*ptr && *ptr != '=')
			ptr++;
		WString varname = ToUtf32(b, int(ptr - b));
		if(*ptr)
			ptr++;
		b = ptr;
		while(*ptr)
			ptr++;
		EnvMap().GetAdd(ToUpper(varname)) = ToUtf32(b, int(ptr - b));
	}
	FreeEnvironmentStringsW(env);
	*/
	CommonInit();
}

void AppInit__(int argc, const char **argv)
{
	AppInitEnvironment__();

#ifdef _DEBUG
	SetConsoleCtrlHandler(s_consoleCtrlHandler, TRUE);
#endif
}
#endif

void AppExit__()
{
	//TODO Thread::ShutdownThreads();
	sMainRunning = false;
#ifdef PLATFORM_POSIX
	MemoryIgnoreLeaksBegin(); // Qt leaks on app exit...
#endif
}

END_UPP_NAMESPACE
