#ifndef _SoftAudioGraph_BypassNode_h_
#define _SoftAudioGraph_BypassNode_h_

#include "Node.h"

NAMESPACE_SAGRAPH_BEGIN

// Two-input selector for dry/wet bypassing.
// Input 0: dry path, Input 1: wet path. Output: chosen path.
class BypassNode : public Node {
public:
    void SetBypass(bool b) { bypass_ = b; }
    bool GetBypass() const { return bypass_; }

    int GetInputCount() const override { return 2; }
    PortSpec GetOutputSpec(int) const override { PortSpec p; p.channels = 0; return p; } // dynamic

    void Process(const ProcessContext& ctx, const Vector<Bus*>& inputs, Bus& output) override {
        const Bus* dry = inputs.GetCount() > 0 ? inputs[0] : nullptr;
        const Bus* wet = inputs.GetCount() > 1 ? inputs[1] : nullptr;
        const Bus* src = bypass_ ? dry : (wet ? wet : dry);
        int ch = src ? src->channels : 1;
        output.SetSize(ctx.block_size, ch);
        output.Zero();
        if(!src) return;
        int frames = min(ctx.block_size, src->frames);
        for(int f = 0; f < frames; ++f)
            for(int c = 0; c < ch; ++c)
                output.At(f, c) = src->At(f, c);
    }

    private:
    bool bypass_ = true; // true=dry, false=wet
public:
    bool SetParam(const String& id, double value) override {
        if(id == "bypass") { SetBypass(value >= 0.5); return true; }
        return false;
    }
};

NAMESPACE_SAGRAPH_END

#endif
