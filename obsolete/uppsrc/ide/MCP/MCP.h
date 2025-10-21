#ifndef _ide_MCP_MCP_h_
#define _ide_MCP_MCP_h_

#include <Core/Core.h>
#include <CtrlLib/CtrlLib.h>
#include <MCP/MCP.h>

#define NAMESPACE_UPP namespace Upp {
#define END_UPP_NAMESPACE }


NAMESPACE_UPP

// Headless MCP core is included via <MCP/MCP.h>

bool StartMcpServer(const McpConfig& cfg);
void StopMcpServer();
bool McpIsRunning();

#include "Server.h"
#include "WorkspaceBridge.h"
#include "Index.h"
#include "Index.h"
#include "Log.h"

END_UPP_NAMESPACE

#endif
