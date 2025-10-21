#ifndef _SoftAudioGraph_Connection_h_
#define _SoftAudioGraph_Connection_h_

#include <Core/Core.h>

#ifndef NAMESPACE_SAGRAPH_BEGIN
#define NAMESPACE_SAGRAPH_BEGIN NAMESPACE_UPP namespace SAGraph {
#define NAMESPACE_SAGRAPH_END   END_UPP_NAMESPACE }
#endif

NAMESPACE_SAGRAPH_BEGIN

struct Edge {
    int from = -1; // node index
    int from_port = 0;
    int to = -1;   // node index
    int to_port = 0;
    float gain = 1.0f; // per-connection gain
    String name;   // optional label for identification
};

NAMESPACE_SAGRAPH_END

#endif
