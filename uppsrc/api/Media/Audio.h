#ifndef _IMedia_Audio_h_
#define _IMedia_Audio_h_

NAMESPACE_UPP


template <class Backend>
struct AudioFrameT : PacketBufferBase {
	
	//RTTI_DECL1(AudioFrameT, PacketBufferBase)
	virtual ~AudioFrameT() {}
	
	
	ValueFormat fmt;
	
	
	const ValueFormat&		GetFormat() const {return fmt;}
};


template <class Backend>
struct AudioInputFrameT : AudioFrameT<Backend> {
	
	using Base = AudioFrameT<Backend>;
	//RTTI_DECL1(AudioInputFrameT, Base)
	
	
};



















END_UPP_NAMESPACE

#endif
