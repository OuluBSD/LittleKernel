#ifndef _SoftAudioGraph_RouterNode_h_
#define _SoftAudioGraph_RouterNode_h_

#include "Node.h"

NAMESPACE_SAGRAPH_BEGIN

class RouterNode : public Node {
public:
    void SetTargetChannels(int ch) { target_ch_ = ch <= 0 ? 1 : ch; }
    int  GetTargetChannels() const { return target_ch_; }

    PortSpec GetOutputSpec(int) const override { PortSpec p; p.channels = target_ch_; return p; }

    void Prepare(const ProcessContext& ctx) override {
        Node::Prepare(ctx);
        out_.SetSize(ctx.block_size, target_ch_);
    }

    void Process(const ProcessContext& ctx, const Vector<Bus*>& inputs, Bus& output) override {
        output.SetSize(ctx.block_size, target_ch_);
        output.Zero();
        const Bus* in = inputs.IsEmpty() ? nullptr : inputs[0];
        if(!in) return;
        int frames = min(ctx.block_size, in->frames);
        if(in->channels == target_ch_) {
            // Pass-through
            for(int f = 0; f < frames; ++f)
                for(int c = 0; c < target_ch_; ++c)
                    output.At(f, c) = in->At(f, c);
        } else if(in->channels == 1 && target_ch_ == 2) {
            // Mono to stereo (duplicate)
            for(int f = 0; f < frames; ++f) {
                float s = in->At(f, 0);
                output.At(f, 0) = s;
                output.At(f, 1) = s;
            }
        } else if(in->channels == 2 && target_ch_ == 1) {
            // Stereo to mono (average)
            for(int f = 0; f < frames; ++f)
                output.At(f, 0) = (in->At(f, 0) + in->At(f, 1)) * 0.5f;
        } else {
            // Generic clamp mapping
            for(int f = 0; f < frames; ++f)
                for(int c = 0; c < target_ch_; ++c)
                    output.At(f, c) = in->At(f, min(c, in->channels - 1));
        }
    }

private:
    int target_ch_ = 1;
    Bus out_;
public:
    bool SetParam(const String& id, double value) override {
        if(id == "channels" || id == "target_channels") { SetTargetChannels((int)value); return true; }
        return false;
    }
};

NAMESPACE_SAGRAPH_END

#endif
