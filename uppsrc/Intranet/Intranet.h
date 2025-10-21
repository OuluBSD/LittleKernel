#ifndef _Intranet_Intranet_h_
#define _Intranet_Intranet_h_

#include <Core/Core.h>

#ifdef PLATFORM_WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef PLATFORM_POSIX
#include <arpa/inet.h>
#endif

#ifdef flagFTP
#include <FTP/Ftp.h>
#endif

#ifdef flagGUI
	#include <CtrlLib/CtrlLib.h>
	#define LAYOUTFILE <Intranet/Intranet.lay>
	#include <CtrlCore/lay.h>
#endif

using namespace Upp;

#include "FtpClient.h"
#include "FtpServer.h"
#include "Daemon.h"

NAMESPACE_UPP



END_UPP_NAMESPACE

#endif
