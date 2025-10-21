#include "Intranet.h"

#ifdef flagFTP
#ifdef PLATFORM_WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

NAMESPACE_UPP

static bool trace_ftp;
#define LLOG(x)                                                                                \
	do {                                                                                       \
		if(trace_ftp)                                                                          \
			RLOG(x);                                                                           \
	} while(false)

int FtpGet(const String& path, Stream& out, const String& host, int port, const String& user,
           const String& pass, Gate2<int64, int64> progress, Callback whenwait, bool binary, bool passive, bool ssl)
{
	Ftp worker;
	worker.WhenWait = whenwait;
	worker.WhenProgress = progress;
	if(!passive)
		worker.Active();
	if(!worker.SSL(ssl).Connect(host, port, user, pass))
		return worker.GetCode();
	return worker.Get(path, out, !binary) ? 0 : worker.GetCode();
}

int FtpPut(Stream& in, const String& path, const String& host, int port, const String& user,
           const String& pass, Gate2<int64, int64> progress, Callback whenwait, bool binary, bool passive, bool ssl)
{
	Ftp worker;
	worker.WhenWait = whenwait;
	worker.WhenProgress = progress;
	if(!passive)
		worker.Active();
	if(!worker.SSL(ssl).Connect(host, port, user, pass))
		return worker.GetCode();
	return worker.Put(in, path, !binary) ? 0 : worker.GetCode();
}

END_UPP_NAMESPACE
#endif
