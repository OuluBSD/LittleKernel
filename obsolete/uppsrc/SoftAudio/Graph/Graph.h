#ifndef _SoftAudioGraph_Graph_h_
#define _SoftAudioGraph_Graph_h_

#include <Core/Core.h>

#define NAMESPACE_SAGRAPH_BEGIN NAMESPACE_UPP namespace SAGraph {
#define NAMESPACE_SAGRAPH_END   END_UPP_NAMESPACE }

#include "Port.h"
#include "ProcessContext.h"
#include "Node.h"
#include "Connection.h"
#include "GraphCore.h"
#include "GraphPlayer.h"
#include "GainNode.h"
#include "MixerNode.h"
#include "RouterNode.h"
#include "BypassNode.h"
#include "SplitterNode.h"

#endif
