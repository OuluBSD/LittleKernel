#ifndef _SoftAudioGraph_MixerNode_h_
#define _SoftAudioGraph_MixerNode_h_

#include "Node.h"
#include <math.h>

NAMESPACE_SAGRAPH_BEGIN

class MixerNode : public Node {
public:
    struct InParam {
        float gain = 1.0f; // linear gain
        float pan = 0.5f;  // 0..1 (left..right), only effective when output is stereo and input is mono
        bool  enable = true;
    };

    void SetOutputChannels(int ch) { out_channels_ = ch <= 0 ? 2 : ch; }
    int  GetOutputChannels() const { return out_channels_; }

    void SetInputCount(int n) { if(n > params_.GetCount()) params_.SetCount(n); }
    void SetInputGain(int index, float g) { Ensure(index); params_[index].gain = g; }
    void SetInputPan(int index, float p)  { Ensure(index); if(p < 0) p = 0; if(p > 1) p = 1; params_[index].pan = p; }
    void SetInputEnable(int index, bool e){ Ensure(index); params_[index].enable = e; }

    int GetInputCount() const override { return max(1, params_.GetCount()); }
    PortSpec GetOutputSpec(int) const override { PortSpec p; p.channels = out_channels_; return p; }

    void Prepare(const ProcessContext& ctx) override {
        Node::Prepare(ctx);
        out_.SetSize(ctx.block_size, out_channels_);
    }

    void Process(const ProcessContext& ctx, const Vector<Bus*>& inputs, Bus& output) override {
        output.SetSize(ctx.block_size, out_channels_);
        output.Zero();
        int n_in = inputs.GetCount();
        if(n_in <= 0) return;
        if(params_.GetCount() < n_in) params_.SetCount(n_in);
        for(int i = 0; i < n_in; ++i) {
            const Bus* in = inputs[i];
            if(!in) continue;
            const InParam& ip = params_[i];
            if(!ip.enable) continue;
            const int in_ch = in->channels;
            const float g = ip.gain;
            if(in_ch == out_channels_) {
                // 1:1 channel sum
                int frames = min(ctx.block_size, in->frames);
                for(int f = 0; f < frames; ++f)
                    for(int c = 0; c < out_channels_; ++c)
                        output.At(f, c) += in->At(f, c) * g;
            } else if(in_ch == 1 && out_channels_ == 2) {
                // Mono to stereo with pan (equal-power)
                float p = ip.pan; if(p < 0) p = 0; if(p > 1) p = 1;
                float gl = sqrtf(1.0f - p) * g;
                float gr = sqrtf(p) * g;
                int frames = min(ctx.block_size, in->frames);
                for(int f = 0; f < frames; ++f) {
                    float s = in->At(f, 0);
                    output.At(f, 0) += s * gl;
                    output.At(f, 1) += s * gr;
                }
            } else if(in_ch == 2 && out_channels_ == 1) {
                // Stereo to mono (average)
                int frames = min(ctx.block_size, in->frames);
                for(int f = 0; f < frames; ++f) {
                    float s = (in->At(f,0) + in->At(f,1)) * 0.5f * g;
                    output.At(f, 0) += s;
                }
            } else {
                // Generic: map channels by clamping
                int frames = min(ctx.block_size, in->frames);
                for(int f = 0; f < frames; ++f)
                    for(int c = 0; c < out_channels_; ++c)
                        output.At(f, c) += in->At(f, min(c, in_ch - 1)) * g;
            }
        }
    }

private:
    void Ensure(int idx) { if(idx >= params_.GetCount()) params_.SetCount(idx + 1); }

    int out_channels_ = 2;
    Vector<InParam> params_;
    Bus out_;
public:
    bool SetParam(const String& id, double value) override {
        if(id == "out_channels") { SetOutputChannels((int)value); return true; }
        if(id.StartsWith("in") && id.Find("_gain") > 0) {
            int n = ScanInt(~id + 2); // simplistic: assume 'inN_gain'
            SetInputGain(n, (float)value); return true;
        }
        if(id.StartsWith("in") && id.Find("_pan") > 0) {
            int n = ScanInt(~id + 2);
            SetInputPan(n, (float)value); return true;
        }
        return false;
    }
};

NAMESPACE_SAGRAPH_END

#endif
