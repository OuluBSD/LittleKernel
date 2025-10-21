#ifndef _SoftAudioGraph_ProcessContext_h_
#define _SoftAudioGraph_ProcessContext_h_

#include <Core/Core.h>

#ifndef NAMESPACE_SAGRAPH_BEGIN
#define NAMESPACE_SAGRAPH_BEGIN NAMESPACE_UPP namespace SAGraph {
#define NAMESPACE_SAGRAPH_END   END_UPP_NAMESPACE }
#endif

NAMESPACE_SAGRAPH_BEGIN

struct ProcessContext {
    int sample_rate = 44100;
    int block_size = 512;
    unsigned long long frame_cursor = 0; // total frames processed so far
};

NAMESPACE_SAGRAPH_END

#endif

