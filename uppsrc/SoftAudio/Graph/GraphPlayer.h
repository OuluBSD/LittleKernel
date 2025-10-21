#ifndef _SoftAudioGraph_GraphPlayer_h_
#define _SoftAudioGraph_GraphPlayer_h_

#include "Graph.h"

NAMESPACE_SAGRAPH_BEGIN

class GraphPlayer {
public:
    GraphPlayer() {}
    GraphPlayer(Graph& g, int sample_rate, int block_size) { Attach(g, sample_rate, block_size); }

    void Attach(Graph& g, int sample_rate, int block_size) {
        g_ = &g;
        ctx_.sample_rate = sample_rate;
        ctx_.block_size = block_size;
        ctx_.frame_cursor = 0;
        prepared_ = false;
    }

    void Configure(int sample_rate, int block_size) {
        ctx_.sample_rate = sample_rate;
        ctx_.block_size = block_size;
        prepared_ = false;
    }

    void Prepare() {
        if(!g_) return;
        g_->SetSampleRate(ctx_.sample_rate);
        g_->SetBlockSize(ctx_.block_size);
        g_->Prepare(ctx_);
        prepared_ = true;
        played_frames_ = ctx_.frame_cursor;
    }

    void Reset() {
        ctx_.frame_cursor = 0;
        prepared_ = false;
        played_frames_ = 0;
    }

    void SeekFrames(unsigned long long frame) {
        ctx_.frame_cursor = frame;
        prepared_ = false;
        played_frames_ = frame;
    }

    void StepBlocks(int blocks) {
        EnsurePrepared();
        for(int i = 0; i < blocks; ++i) {
            g_->ProcessBlock();
            played_frames_ += ctx_.block_size;
        }
    }

    void PlayFrames(unsigned long long frames) {
        int blocks = (int)((frames + ctx_.block_size - 1) / ctx_.block_size);
        StepBlocks(blocks);
    }

    void PlaySeconds(double seconds) {
        unsigned long long frames = (unsigned long long)(seconds * ctx_.sample_rate);
        PlayFrames(frames);
    }

    unsigned long long GetFrameCursor() const { return played_frames_; }
    double GetTimeSeconds() const { return (double)played_frames_ / (double)max(1, ctx_.sample_rate); }

private:
    void EnsurePrepared() { if(!prepared_) Prepare(); }

    Graph* g_ = nullptr;
    ProcessContext ctx_;
    bool prepared_ = false;
    unsigned long long played_frames_ = 0;
};

NAMESPACE_SAGRAPH_END

#endif

