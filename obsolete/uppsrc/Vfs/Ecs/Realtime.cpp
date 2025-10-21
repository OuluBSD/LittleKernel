#include "Ecs.h"
#include <Core/MathNumeric/MathNumeric.h>

NAMESPACE_UPP


void RealtimeSourceConfig::Update(double dt, bool buffer_full) {
	sync_age += dt;
	time_total += dt;
	time_delta = dt;
	
	++src_frame;
	
	if (enable_sync && sync_age >= sync_dt) {
		if (sync_age > 2 * sync_dt)
			sync_age = sync_dt;
		else
			sync_age = Modulus(sync_age, sync_dt);
		
		last_sync_src_frame = src_frame;
		
		frames_after_sync = 0;
		sync = true;
		
		render = true;
	}
	else if (!buffer_full) {
		sync = false;
		frames_after_sync = src_frame > last_sync_src_frame ? src_frame - last_sync_src_frame : 0;
		
		render = true;
	}
	else {
		render = false;
	}
}



END_UPP_NAMESPACE
