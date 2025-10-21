#include "Graphics.h"

NAMESPACE_UPP


template <class Gfx>
GfxBufferFieldT<Gfx>::GfxBufferFieldT() {
	buf.owner = this;
}

template <class Gfx>
bool GfxBufferFieldT<Gfx>::Initialize(AtomBase& a, const WorldState& ws) {
	if (!buf.Initialize(a, ws))
		return false;
	
	
	bool def_recv_data_only =	a.GetSink()->GetSinkCount() == 1 &&
								a.GetSink()->GetValue(0).GetFormat().IsFbo();
	add_data_states = !ws.GetBool(".recv.data", def_recv_data_only);
	
	return true;
}

template <class Gfx>
bool GfxBufferFieldT<Gfx>::ImageInitialize(bool is_win_fbo, Size screen_sz) {
	if (add_data_states) {
		data.GetAddPipeline("default");
		
		if (buf.mode == Buffer::MULTI_STEREO)
			data.is_stereo = true;
		
		buf.SetDataStateOverride(&data, true);
	}
	
	return buf.ImageInitialize(is_win_fbo, screen_sz, add_data_states);
}

template <class Gfx>
bool GfxBufferFieldT<Gfx>::PostInitialize() {
	return buf.PostInitialize();
}



GFX3D_EXCPLICIT_INITIALIZE_CLASS(GfxBufferFieldT)


END_UPP_NAMESPACE
