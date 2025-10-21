#ifndef _Vfs_Ecs_Realtime_h_
#define _Vfs_Ecs_Realtime_h_


template<class T> class OffsetGen;


template<class T>
struct OffsetLoop {
	using limits = std::numeric_limits<T>;
	using Gen = OffsetGen<T>;
	
	Gen* gen;
	T value;
	
	
	//OffsetLoop() {}
	//OffsetLoop(T value) : value(value) {}
	OffsetLoop(Gen* g, T value) : gen(g), value(value) {}
	OffsetLoop(Gen* g) : gen(g), value(0) {}
	OffsetLoop(Gen& g) : gen(&g), value(0) {}
	OffsetLoop(const OffsetLoop& o) {*this = o;}
	
	OffsetLoop& operator=(const OffsetLoop& o) {gen = o.gen; value = o.value; return *this;}
	
	void Clear() {value = 0;}
	bool operator==(const OffsetLoop& o) const {return o.value == value;}
	bool operator!=(const OffsetLoop& o) const {return o.value != value;}
	void operator+=(const OffsetLoop& o) {value += o.value;}
	void operator-=(const OffsetLoop& o) {value -= o.value;}
	bool operator< (const OffsetLoop& o) {return value <  o.value;}
	bool operator<=(const OffsetLoop& o) {return value <= o.value;}
	bool operator>=(const OffsetLoop& o) {return value >= o.value;}
	bool operator> (const OffsetLoop& o) {return value >  o.value;}
	
	OffsetLoop& operator++() {++value; return *this;}
	OffsetLoop operator++(int) {return OffsetLoop(gen, value++);}
	operator bool() const {return value;}
	operator T() const {return value;}
	
	void SetMax() {value = limits::max();}
	void SetMin() {value = limits::min();}
	void TestSetMin(OffsetLoop v) {if (v.value < value) value = v.value;}
	void TestSetMax(OffsetLoop v) {if (v.value > value) value = v.value;}
	
	String ToString() const {return UPP::AsString(value);}
	
	
	static OffsetLoop GetDifference(OffsetLoop min, OffsetLoop max) {
		OffsetLoop ret(min.gen, 0);
		if (min != max)
			ret.value =
				min.value < max.value ?
					max.value - min.value :
					ret.value = limits::max() - min.value + 1 + max.value - limits::min();
		return ret;
	}
	
	static int64 GetDifferenceI64(OffsetLoop overflow_anchor, OffsetLoop min, OffsetLoop max) {
		int64 anchor = overflow_anchor.value;
		int64 a = (int64)min.value;
		int64 b = (int64)max.value;
		if (b < anchor)
			b += (int64)limits::max() - (int64)limits::min();
		return b - a;
	}
	
};

template<class T>
class OffsetGen {
	using Offset = OffsetLoop<T>;
	
	Offset value;
public:
	
	OffsetGen() : value(this) {}
	
	Offset Create() {return ++value;}
	Offset GetCurrent() const {return value;}
	
	String ToString() const {return "gen(" + value.ToString() + ")";}
	
	//operator Offset() {return ++value;}
};


using off32 = OffsetLoop<dword>;
using off32_gen = OffsetGen<dword>;





struct RealtimeSourceConfig {
	double time_delta = 0;
	double time_total = 0;
	double sync_dt = 3.0;
	double sync_age = 0;
    dword last_sync_src_frame = 0;
	dword frames_after_sync = 0;
	dword src_frame = 0;
	bool enable_sync = false;
	bool sync = 0;
	bool render = 0;
	off32 cur_offset, prev_offset;
	
	
	RealtimeSourceConfig(off32_gen& gen) : cur_offset(gen), prev_offset(gen) {}
	
	void Update(double dt, bool buffer_full);
	//void SetOffset(off32 begin, off32 end) {begin_offset = begin; end_offset = end;}
};

using RTSrcConfig = RealtimeSourceConfig;

#define MIN_AUDIO_BUFFER_SAMPLES 1024


#endif
