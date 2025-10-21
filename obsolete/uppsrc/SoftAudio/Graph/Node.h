#ifndef _SoftAudioGraph_Node_h_
#define _SoftAudioGraph_Node_h_

#include <Core/Core.h>
#include "ProcessContext.h"
#include "Port.h"
#include <string.h>

#ifndef NAMESPACE_SAGRAPH_BEGIN
#define NAMESPACE_SAGRAPH_BEGIN NAMESPACE_UPP namespace SAGraph {
#define NAMESPACE_SAGRAPH_END   END_UPP_NAMESPACE }
#endif

NAMESPACE_SAGRAPH_BEGIN

// Simple interleaved float buffer used between nodes.
struct Bus {
    Vector<float> data;
    int frames = 0;
    int channels = 0;

    void SetSize(int f, int ch) {
        frames = f; channels = ch; data.SetCount(f * ch);
    }
    void Zero() {
        if(!data.IsEmpty()) memset(data.Begin(), 0, data.GetCount() * sizeof(float));
    }
    inline float& At(int frame, int ch) { return data[frame * channels + ch]; }
    inline const float& At(int frame, int ch) const { return data[frame * channels + ch]; }
};

class Node {
public:
    virtual ~Node() {}
    virtual void Prepare(const ProcessContext& ctx) { ctx_ = ctx; }
    virtual void Process(const ProcessContext& ctx, const Vector<Bus*>& inputs, Bus& output) = 0;
    virtual int GetInputCount() const { return 1; }
    virtual int GetOutputCount() const { return 1; }
    virtual PortSpec GetInputSpec(int /*index*/) const { return {0}; }
    virtual PortSpec GetOutputSpec(int /*index*/) const { return {0}; }
    // Optional parameter interface for runtime control (control-thread only)
    virtual bool SetParam(const String& /*id*/, double /*value*/) { return false; }

protected:
    ProcessContext ctx_;
};

NAMESPACE_SAGRAPH_END

#endif
