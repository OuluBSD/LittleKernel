#ifndef _Sound_Types_h_
#define _Sound_Types_h_

NAMESPACE_UPP


#define LOG_SOUND_ERRORS

#ifdef LOG_SOUND_ERRORS
  #define CHECK_ERROR(STREAM) if((STREAM).IsError()) LOG(__FILE__<<" (line "<<__LINE__<<"): "<<(STREAM).GetError());
#else
  #define CHECK_ERROR(STREAM)
#endif
#define CHECK_ERR CHECK_ERROR(*this)

enum SampleFormat : int {
	SND_FLOAT32 = paFloat32,
	SND_INT32 = paInt32,
	SND_INT24 = paInt24,
	SND_INT16 = paInt16,
	SND_INT8  = paInt8,
	SND_UINT8 = paUInt8,
	SND_UNKNOWN = -1
};

inline String GetSampleFormatString(SampleFormat fmt) {
	switch (fmt) {
		case SND_FLOAT32: return "float";
		case SND_INT32:   return "int32";
		case SND_INT24:   return "int24";
		case SND_INT16:   return "int16";
		case SND_INT8:    return "int8";
		case SND_UINT8:   return "uint8";
		default:          return "unknown";
	}
}

template <class T> SampleFormat GetSampleFormat();
template <> inline SampleFormat GetSampleFormat<float>() {return SND_FLOAT32;}
template <> inline SampleFormat GetSampleFormat<int32>() {return SND_INT32;}
template <> inline SampleFormat GetSampleFormat<int16>() {return SND_INT16;}
template <> inline SampleFormat GetSampleFormat<int8>() {return SND_INT8;}
template <> inline SampleFormat GetSampleFormat<uint8>() {return SND_UINT8;}

template <class T> double SampleToDouble(T v) {return (double)v / (double)std::numeric_limits<T>::max();}
template <> inline double SampleToDouble<float>(float f) {return f;}
template <> inline double SampleToDouble<uint8>(uint8 v) {return ((int)v - 128) / 128.0;}

enum StreamFlags {
	SND_NOFLAG    = paNoFlag,
	SND_NOCLIP    = paClipOff,
	SND_NODITHER  = paDitherOff,
	SND_NEVERDROP = paNeverDropInput,
	SND_CALLBACK_PREFILLED = paPrimeOutputBuffersUsingStreamCallback,
	SND_PLATFORM_SPECIFIC  = paPlatformSpecificFlags
};

enum StreamCallbackResult {
	SND_CONTINUE = paContinue,
	SND_COMPLETE = paComplete,
	SND_ABORT = paAbort
};

enum StreamCallbackFlags {
	SND_INPUT_UNDERFLOW = paInputUnderflow,
	SND_INPUT_OVERFLOW = paInputOverflow,
	SND_OUTPUT_UNDERFLOW = paOutputUnderflow,
	SND_OUTPUT_OVERFLOW = paOutputOverflow,
	SND_PRIMING_OUTPUT = paPrimingOutput
};

struct StreamTimeInfo {
	double InputAdc, Current, OutputDac;
	StreamTimeInfo(const PaStreamCallbackTimeInfo* timeinfo):
	  InputAdc(timeinfo->inputBufferAdcTime),
	  OutputDac(timeinfo->outputBufferDacTime),
	  Current(timeinfo->currentTime){}
	operator const PaStreamCallbackTimeInfo*(){
		return reinterpret_cast<const PaStreamCallbackTimeInfo*>(this);
	}
};

struct StreamCallbackArgs {
	const void* input;
	void* output;
	void* data;
	int state;
	unsigned long fpb;
	StreamTimeInfo timeinfo;
	unsigned long flags;
	StreamCallbackArgs(const void *input, void *output, unsigned long fpb,
	                   StreamTimeInfo timeinfo, unsigned long flags,
	                   void* data) : input(input),output(output),fpb(fpb),timeinfo(timeinfo),
	                                 flags(flags),data(data),state(SND_CONTINUE){};
};

struct StreamCallbackData {
	Callback1<StreamCallbackArgs&> func;
	Callback1<void*> finish;
	void* data;
	StreamCallbackData(){}
	StreamCallbackData(Callback1<StreamCallbackArgs&> function,
	                   Callback1<void*> whenfinish ,void* userdata):
	                          func(function),finish(whenfinish),data(userdata){}
};

struct StreamParameters : PaStreamParameters {
	StreamParameters(){Zero();}
	StreamParameters(Nuller){Zero();}
	StreamParameters(const StreamParameters& p) {
		memcpy((PaStreamParameters*)this, (const PaStreamParameters*)&p, sizeof(*this));
	}
	StreamParameters(int dev,int channels,SampleFormat format,PaTime latency,void* APISpecificInfo=NULL){
		this->device = dev;
		this->channelCount = channels;
		this->sampleFormat = format;
		this->suggestedLatency = latency;
		this->hostApiSpecificStreamInfo = APISpecificInfo;
	}
	void Zero() {memset((PaStreamParameters*)this, 0, sizeof(*this));}
	bool IsNullInstance()const{return channelCount == 0;}
};


END_UPP_NAMESPACE

#endif
