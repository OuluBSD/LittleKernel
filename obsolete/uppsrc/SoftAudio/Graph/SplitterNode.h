#ifndef _SoftAudioGraph_SplitterNode_h_
#define _SoftAudioGraph_SplitterNode_h_

#include "Node.h"

NAMESPACE_SAGRAPH_BEGIN

// Explicit pass-through node that can be used as a named fan-out point.
class SplitterNode : public Node {
public:
    void Process(const ProcessContext& ctx, const Vector<Bus*>& inputs, Bus& output) override {
        const Bus* in = inputs.IsEmpty() ? nullptr : inputs[0];
        int ch = in ? in->channels : (output.channels ? output.channels : 1);
        output.SetSize(ctx.block_size, ch);
        output.Zero();
        if(!in) return;
        int frames = min(ctx.block_size, in->frames);
        for(int f = 0; f < frames; ++f)
            for(int c = 0; c < ch; ++c)
                output.At(f, c) = in->At(f, c);
    }
};

NAMESPACE_SAGRAPH_END

#endif

