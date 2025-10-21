#ifndef _Core_App_h_
#define _Core_App_h_

typedef String ExitExc;

void    SetExitCode(int code);
int     GetExitCode();

bool  IsMainRunning();

void AppExit__();
void AppExecute__(void (*app)());

#ifdef PLATFORM_WIN32

void AppInit__(int argc, const char **argv);
void AppInitEnvironment__();

#define CONSOLE_APP_MAIN \
void ConsoleMainFn_(); \
 \
int main(int argc, char *argv[]) { \
	UPP::AppInit__(argc, (const char **)argv); \
	UPP::AppExecute__(ConsoleMainFn_); \
	UPP::AppExit__(); \
	return UPP::GetExitCode(); \
} \
 \
void ConsoleMainFn_()

#endif

#ifdef PLATFORM_POSIX

void AppInit__(int argc, const char **argv, const char **envptr);

#define CONSOLE_APP_MAIN \
void ConsoleMainFn_(); \
 \
int main(int argc, const char **argv, const char **envptr) { \
	UPP::AppInit__(argc, argv, envptr); \
	UPP::AppExecute__(ConsoleMainFn_); \
	UPP::AppExit__(); \
	return UPP::GetExitCode(); \
} \
 \
void ConsoleMainFn_()

#endif

#endif
