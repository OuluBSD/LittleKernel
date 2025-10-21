#ifndef _SoftAudioGraph_Port_h_
#define _SoftAudioGraph_Port_h_

#include <Core/Core.h>

#ifndef NAMESPACE_SAGRAPH_BEGIN
#define NAMESPACE_SAGRAPH_BEGIN NAMESPACE_UPP namespace SAGraph {
#define NAMESPACE_SAGRAPH_END   END_UPP_NAMESPACE }
#endif

NAMESPACE_SAGRAPH_BEGIN

struct PortSpec {
    int channels = 0;   // 0 means dynamic (match connection or graph default)
};

NAMESPACE_SAGRAPH_END

#endif
