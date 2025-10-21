#ifndef _SoftAudioGraph_GainNode_h_
#define _SoftAudioGraph_GainNode_h_

#include "Node.h"

NAMESPACE_SAGRAPH_BEGIN

class GainNode : public Node {
public:
    void SetGain(float g) { gain_ = g; }
    float GetGain() const { return gain_; }

    void Prepare(const ProcessContext& ctx) override { Node::Prepare(ctx); }

    void Process(const ProcessContext& ctx, const Vector<Bus*>& inputs, Bus& output) override {
        const Bus* in = inputs.IsEmpty() ? nullptr : inputs[0];
        if(!in) { output.SetSize(ctx.block_size, output.channels ? output.channels : 1); output.Zero(); return; }
        output.SetSize(in->frames, in->channels);
        const float* s = in->data.Begin();
        float* d = output.data.Begin();
        int n = in->frames * in->channels;
        for(int i = 0; i < n; ++i)
            d[i] = s[i] * gain_;
    }

    private:
    float gain_ = 1.0f;
public:
    bool SetParam(const String& id, double value) override {
        if(id == "gain") { SetGain((float)value); return true; }
        return false;
    }
};

NAMESPACE_SAGRAPH_END

#endif
