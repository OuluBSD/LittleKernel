#ifndef _SoftAudio_GraphNodes_h_
#define _SoftAudio_GraphNodes_h_

#include "SoftAudio.h"
#include <SoftAudio/Graph/Graph.h>
#include <string.h>
#include <atomic>

NAMESPACE_AUDIO_BEGIN

// Wraps SoftAudio::SineWave as a graph source node (mono)
class SineNode : public SAGraph::Node {
public:
    void SetFrequency(float hz) { freq_ = hz; sine_.SetFrequency(hz); }

    void Prepare(const SAGraph::ProcessContext& ctx) override {
        SAGraph::Node::Prepare(ctx);
        temp_.SetCount(ctx.block_size, 1);
        sine_.SetFrequency(freq_);
    }

    int GetInputCount() const override { return 0; }
    SAGraph::PortSpec GetOutputSpec(int) const override { SAGraph::PortSpec p; p.channels = 1; return p; }

    void Process(const SAGraph::ProcessContext& ctx, const Vector<SAGraph::Bus*>&, SAGraph::Bus& output) override {
        temp_.SetCount(ctx.block_size, 1);
        temp_.Zero();
        sine_.Tick(temp_, 0);
        output.SetSize(ctx.block_size, 1);
        memcpy(output.data.Begin(), &temp_[0], ctx.block_size * sizeof(float));
    }

    private:
    SineWave sine_;
    float freq_ = 440.0f;
    AudioFrames temp_;
public:
    bool SetParam(const String& id, double value) override {
        if(id == "freq" || id == "frequency" || id == "hz") { SetFrequency((float)value); return true; }
        return false;
    }
};

// Wraps SoftAudio::FreeVerb as a graph effect node (stereo)
class FreeVerbNode : public SAGraph::Node {
public:
    void SetMix(float m) { verb_.SetEffectMix(m); }
    void SetRoomSize(float r) { verb_.SetRoomSize(r); }
    void SetDamping(float d) { verb_.SetDamping(d); }
    void SetWidth(float w) { verb_.SetWidth(w); }

    void Prepare(const SAGraph::ProcessContext& ctx) override {
        SAGraph::Node::Prepare(ctx);
        in_.SetCount(ctx.block_size, 2);
        out_.SetCount(ctx.block_size, 2);
    }

    SAGraph::PortSpec GetInputSpec(int) const override { SAGraph::PortSpec p; p.channels = 2; return p; }
    SAGraph::PortSpec GetOutputSpec(int) const override { SAGraph::PortSpec p; p.channels = 2; return p; }

    void Process(const SAGraph::ProcessContext& ctx, const Vector<SAGraph::Bus*>& inputs, SAGraph::Bus& output) override {
        const SAGraph::Bus* in = inputs.IsEmpty() ? nullptr : inputs[0];
        output.SetSize(ctx.block_size, 2);
        if(!in) { output.Zero(); return; }
        in_.SetCount(ctx.block_size, 2);
        // copy/expand input to stereo
        if(in->channels == 1) {
            for(int i = 0; i < ctx.block_size; ++i) {
                float s = in->At(i, 0);
                in_(i,0) = s; in_(i,1) = s;
            }
        } else {
            for(int i = 0; i < ctx.block_size; ++i) {
                in_(i,0) = in->At(i,0);
                in_(i,1) = in->At(i,1);
            }
        }
        out_.SetCount(ctx.block_size, 2);
        verb_.Tick(in_, out_, 0, 0);
        // copy to graph bus
        for(int i = 0; i < ctx.block_size; ++i) {
            output.At(i,0) = out_(i,0);
            output.At(i,1) = out_(i,1);
        }
    }

    private:
    FreeVerb verb_;
    AudioFrames in_;
    AudioFrames out_;
public:
    bool SetParam(const String& id, double value) override {
        if(id == "mix") { verb_.SetEffectMix((float)value); return true; }
        if(id == "room" || id == "room_size") { verb_.SetRoomSize((float)value); return true; }
        if(id == "damp" || id == "damping") { verb_.SetDamping((float)value); return true; }
        if(id == "width") { verb_.SetWidth((float)value); return true; }
        return false;
    }
};

// Sink to a file via SoftAudio::FileWaveOut
class FileOutNode : public SAGraph::Node {
public:
    void Open(String path, int channels = 2, FileWrite::FILE_TYPE type = FileWrite::FILE_WAV, Audio::AudioFormat fmt = Audio::AUDIO_SINT16) {
        if(open_) Close();
        out_.OpenFile(path, channels, type, fmt);
        open_ = true;
        channels_ = channels;
    }
    void Close() { if(open_) { out_.CloseFile(); open_ = false; } }
    ~FileOutNode() { Close(); }

    SAGraph::PortSpec GetInputSpec(int) const override { SAGraph::PortSpec p; p.channels = channels_ ? channels_ : 2; return p; }
    int GetOutputCount() const override { return 0; }

    void Process(const SAGraph::ProcessContext& ctx, const Vector<SAGraph::Bus*>& inputs, SAGraph::Bus&) override {
        if(inputs.IsEmpty()) return;
        const SAGraph::Bus* in = inputs[0];
        frames_.SetCount(ctx.block_size, in->channels > 1 ? in->channels : 1);
        // copy interleaved
        for(int i = 0; i < ctx.block_size; ++i)
            for(int c = 0; c < frames_.GetChannelCount(); ++c)
                frames_(i, c) = in->At(i, min(c, in->channels - 1));
        out_.Tick(frames_);
    }

    private:
    FileWaveOut out_;
    bool open_ = false;
    int channels_ = 2;
    AudioFrames frames_;
public:
    bool SetParam(const String& id, double value) override {
        if(id == "channels") { channels_ = (int)value; return true; }
        return false;
    }
};

// Wraps SoftAudio::Compressor as a graph effect node (stereo)
class CompressorNode : public SAGraph::Node {
public:
    void Prepare(const SAGraph::ProcessContext& ctx) override {
        SAGraph::Node::Prepare(ctx);
        in_.SetCount(ctx.block_size, 2);
        out_.SetCount(ctx.block_size, 2);
    }

    SAGraph::PortSpec GetInputSpec(int) const override { SAGraph::PortSpec p; p.channels = 2; return p; }
    SAGraph::PortSpec GetOutputSpec(int) const override { SAGraph::PortSpec p; p.channels = 2; return p; }

    void Process(const SAGraph::ProcessContext& ctx, const Vector<SAGraph::Bus*>& inputs, SAGraph::Bus& output) override {
        output.SetSize(ctx.block_size, 2);
        const SAGraph::Bus* in = inputs.IsEmpty() ? nullptr : inputs[0];
        if(!in) { output.Zero(); return; }
        int frames = ctx.block_size;
        // Prepare input frames
        if(in->channels == 1) {
            for(int i = 0; i < frames; ++i) {
                float s = in->At(i, 0);
                in_(i,0) = s; in_(i,1) = s;
            }
        } else {
            for(int i = 0; i < frames; ++i) {
                in_(i,0) = in->At(i,0);
                in_(i,1) = in->At(i,1);
            }
        }
        // Process per-sample using Tick2
        for(int i = 0; i < frames; ++i) {
            float l = in_(i,0);
            float r = in_(i,1);
            float ol = comp_.Tick2(l, r, 0);
            float or_ = comp_.Tick2(l, r, 1);
            output.At(i,0) = ol;
            output.At(i,1) = or_;
        }
    }

    private:
    Compressor comp_;
    AudioFrames in_;
    AudioFrames out_;
    ValueMap comp_state_;
public:
    bool SetParam(const String& id, double value) override {
        bool ok = true;
        if(id == "gain_db") comp_state_.GetAdd(".gain") = (double)value;
        else if(id == "threshold_db" || id == "th_db") comp_state_.GetAdd(".treshold") = (double)value;
        else if(id == "knee_db" || id == "knee") comp_state_.GetAdd(".knee") = (double)value;
        else if(id == "ratio") comp_state_.GetAdd(".ratio") = (double)value;
        else if(id == "attack_ms") comp_state_.GetAdd(".attack") = (double)value;
        else if(id == "release_ms") comp_state_.GetAdd(".release") = (double)value;
        else if(id == "auto_makeup") comp_state_.GetAdd(".auto.makeup") = value >= 0.5;
        else ok = false;
        if(ok) comp_.LoadState(comp_state_);
        return ok;
    }
};

// Wraps SoftAudio::Voicer with a default Simple instrument; mono output
class VoicerNode : public SAGraph::Node {
public:
    VoicerNode() { AddSimpleInstrument(); }

    void AddSimpleInstrument(int group = 0) {
        One<Instrument> ins; ins = MakeOne<Simple>();
        voicer_.AddInstrument(~ins, group);
        instruments_.Add(pick(ins));
    }

    void NoteOn(float note_number, float amplitude, int group = 0) { voicer_.NoteOn(note_number, amplitude, group); }
    void NoteOff(float note_number, float amplitude, int group = 0) { voicer_.NoteOff(note_number, amplitude, group); }

    // Accept a dummy input to allow ordering edges (e.g., from MidiInputNode)
    int GetInputCount() const override { return 1; }
    SAGraph::PortSpec GetOutputSpec(int) const override { SAGraph::PortSpec p; p.channels = 1; return p; }

    void Prepare(const SAGraph::ProcessContext& ctx) override {
        SAGraph::Node::Prepare(ctx);
        out_.SetCount(ctx.block_size, 1);
    }

    void Process(const SAGraph::ProcessContext& ctx, const Vector<SAGraph::Bus*>&, SAGraph::Bus& output) override {
        out_.SetCount(ctx.block_size, 1);
        voicer_.Tick(out_, 0);
        output.SetSize(ctx.block_size, 1);
        memcpy(output.data.Begin(), &out_[0], ctx.block_size * sizeof(float));
    }

private:
    Voicer voicer_;
    Vector< One<Instrument> > instruments_;
    AudioFrames out_;
};

// A small MIDI input node that schedules NoteOn/NoteOff events to a target VoicerNode.
class MidiInputNode : public SAGraph::Node {
public:
    enum Kind { NOTE_ON, NOTE_OFF };
    struct Ev {
        unsigned long long frame;
        Kind kind;
        float note;
        float value;
    };

    void SetTarget(VoicerNode* v) { target_ = v; }

    void EnqueueNoteOn(float note, float amp, unsigned long long at_frame) {
        One<Ev> e = new Ev{at_frame, NOTE_ON, note, amp}; events_.Add(e.Detach());
    }
    void EnqueueNoteOff(float note, float amp, unsigned long long at_frame) {
        One<Ev> e = new Ev{at_frame, NOTE_OFF, note, amp}; events_.Add(e.Detach());
    }

    // Optional helpers in seconds
    void EnqueueNoteOnSec(float note, float amp, double sec, int sample_rate) {
        EnqueueNoteOn(note, amp, (unsigned long long)(sec * sample_rate));
    }
    void EnqueueNoteOffSec(float note, float amp, double sec, int sample_rate) {
        EnqueueNoteOff(note, amp, (unsigned long long)(sec * sample_rate));
    }

    int GetOutputCount() const override { return 1; }
    SAGraph::PortSpec GetOutputSpec(int) const override { SAGraph::PortSpec p; p.channels = 1; return p; }

    void Prepare(const SAGraph::ProcessContext& ctx) override { SAGraph::Node::Prepare(ctx); }

    void Process(const SAGraph::ProcessContext& ctx, const Vector<SAGraph::Bus*>&, SAGraph::Bus& output) override {
        // Emit zeros and dispatch events to target within this block window
        output.SetSize(ctx.block_size, 1);
        output.Zero();
        if(!target_) return;
        unsigned long long start = ctx.frame_cursor;
        unsigned long long end = start + (unsigned long long)ctx.block_size;
        // Simple linear scan; for prod consider keeping events sorted and using an index
        for(int i = read_pos_; i < events_.GetCount(); ++i) {
            const Ev& e = events_[i];
            if(e.frame >= end) break;
            if(e.frame >= start) {
                if(e.kind == NOTE_ON) target_->NoteOn(e.note, e.value);
                else target_->NoteOff(e.note, e.value);
                read_pos_ = i + 1;
            }
        }
    }

private:
    VoicerNode* target_ = nullptr;
    Array<Ev> events_;
    int read_pos_ = 0;
};

// Live PortAudio output node: writes incoming audio to the default output device.
// Real-time thread reads from an internal SPSC ring-buffer. Graph thread pushes blocks.
class LiveOutNode : public SAGraph::Node {
public:
	typedef LiveOutNode CLASSNAME;
    LiveOutNode() { }
    ~LiveOutNode() { Stop(); Close(); }

    void SetChannels(int ch) { channels_ = ch <= 0 ? 2 : ch; }
    void SetOutputDeviceIndex(int index) { device_index_ = index; }
    void SetLatency(double seconds) { latency_ = seconds; }
    static String DescribeAudioSystem() {
        using namespace Portaudio;
        (void)AudioSys();
        return AudioSys().ToString();
    }
    static Vector<String> ListOutputDeviceNames() {
        using namespace Portaudio;
        (void)AudioSys();
        Vector<String> names;
        for(const auto& d : AudioSys().GetDevices())
            if(d.output_channels > 0)
                names.Add(String().Cat() << d.index << ": " << d.name);
        return names;
    }
    static int FindOutputDeviceByName(const String& name_substr, bool ci = true) {
        using namespace Portaudio;
        (void)AudioSys();
        String needle = ci ? ToLower(name_substr) : name_substr;
        for(const auto& d : AudioSys().GetDevices()) {
            String s = ci ? ToLower(String(d.name)) : String(d.name);
            if(s.Find(needle) >= 0 && d.output_channels > 0)
                return d.index;
        }
        return -1;
    }
    bool SetOutputDeviceByName(const String& name_substr, bool ci = true) {
        int idx = FindOutputDeviceByName(name_substr, ci);
        if(idx >= 0) { device_index_ = idx; return true; }
        return false;
    }
    void SetAutoStart(bool on) { autostart_ = on; }
    void Start() { if(!running_) { stream_.Start(); running_ = true; } }
    void Stop()  { if(running_) { stream_.Stop(); running_ = false; } }
    void Close() { stream_.Close(); ring_.Clear(); }

    SAGraph::PortSpec GetInputSpec(int) const override { SAGraph::PortSpec p; p.channels = channels_ ? channels_ : 2; return p; }
    int GetOutputCount() const override { return 0; }

    void Prepare(const SAGraph::ProcessContext& ctx) override {
        SAGraph::Node::Prepare(ctx);
        block_frames_ = ctx.block_size;
        if(channels_ <= 0) channels_ = 2;
        
        // Init ring: capacity = N blocks
        int cap = channels_ * block_frames_ * ring_blocks_;
        ring_.Init(cap);
        
        // Open default PortAudio stream
        using namespace Portaudio;
        (void)AudioSys(); // ensure system is initialized
        stream_.SetSampleRate(ctx.sample_rate);
        stream_.SetFrequency(ctx.sample_rate);
        stream_.SetFlags(SND_NOFLAG);
        if(device_index_ >= 0) {
            StreamParameters inparam(Null);
            StreamParameters outparam(device_index_, channels_, SND_FLOAT32, (PaTime)latency_, nullptr);
            stream_.Open((void*)this, inparam, outparam);
        } else {
            stream_.OpenDefault((void*)this, 0, channels_, SND_FLOAT32);
        }
        stream_.WhenAction = THISBACK(OnCallback);
        if(autostart_) Start();
    }

    void Process(const SAGraph::ProcessContext& ctx, const Vector<SAGraph::Bus*>& inputs, SAGraph::Bus&) override {
        const SAGraph::Bus* in = inputs.IsEmpty() ? nullptr : inputs[0];
        if(!in) return;
        int frames = min(ctx.block_size, in->frames);
        
        // Ensure channels match; if mismatch, clamp or duplicate
        temp_.SetCount(frames, channels_);
        for(int f = 0; f < frames; ++f) {
            for(int c = 0; c < channels_; ++c) {
                float s = in->At(f, min(c, in->channels - 1));
                temp_(f, c) = s;
            }
        }
        ring_.Push(&temp_[0], frames * channels_);
    }

private:
    // PortAudio callback
    void OnCallback(StreamCallbackArgs& a) {
        float* out = (float*)a.output;
        int need = a.fpb * channels_;
        if(!ring_.Pop(out, need)) {
            // underflow: zero-extend remaining
            int avail = ring_.LastReadCount();
            for(int i = avail; i < need; ++i) out[i] = 0.0f;
        }
        a.state = SND_CONTINUE;
    }

    // Simple lock-free single-producer single-consumer ring buffer for float samples
    struct Ring {
        Vector<float> buf;
        std::atomic<int> head{0};
        std::atomic<int> tail{0};
        int cap = 0;
        int last_read_ = 0;
        void Init(int capacity) {
            buf.SetCount(capacity);
            cap = capacity;
            head.store(0, std::memory_order_relaxed);
            tail.store(0, std::memory_order_relaxed);
            last_read_ = 0;
        }
        void Clear() { buf.Clear(); cap = 0; head.store(0); tail.store(0); last_read_ = 0; }
        int AvailableRead() const {
            int h = head.load(std::memory_order_acquire);
            int t = tail.load(std::memory_order_acquire);
            return h >= t ? (h - t) : (cap - (t - h));
        }
        int AvailableWrite() const { return cap ? (cap - 1 - AvailableRead()) : 0; }
        bool Push(const float* data, int count) {
            if(count <= 0 || cap == 0) return false;
            // if overflow, drop oldest to make room
            if(count > AvailableWrite()) {
                int drop = count - AvailableWrite();
                int t = tail.load(std::memory_order_relaxed);
                t = (t + drop) % cap;
                tail.store(t, std::memory_order_release);
            }
            int h = head.load(std::memory_order_relaxed);
            int first = min(count, cap - h);
            memcpy(&buf[h], data, first * sizeof(float));
            int rem = count - first;
            if(rem > 0) memcpy(&buf[0], data + first, rem * sizeof(float));
            head.store((h + count) % cap, std::memory_order_release);
            return true;
        }
        bool Pop(float* dst, int count) {
            if(count <= 0 || cap == 0) { last_read_ = 0; return false; }
            int avail = AvailableRead();
            int toread = min(avail, count);
            int t = tail.load(std::memory_order_relaxed);
            int first = min(toread, cap - t);
            if(first > 0) memcpy(dst, &buf[t], first * sizeof(float));
            int rem = toread - first;
            if(rem > 0) memcpy(dst + first, &buf[0], rem * sizeof(float));
            tail.store((t + toread) % cap, std::memory_order_release);
            last_read_ = toread;
            return toread == count;
        }
        int LastReadCount() const { return last_read_; }
    };

    Portaudio::AudioDeviceStream stream_;
    Ring ring_;
    AudioFrames temp_;
    int channels_ = 2;
    int block_frames_ = 0;
    bool autostart_ = true;
    bool running_ = false;
    int ring_blocks_ = 8; // internal buffering depth
    int device_index_ = -1; // -1 = default
    double latency_ = 0.02; // seconds
};

NAMESPACE_AUDIO_END

#endif

