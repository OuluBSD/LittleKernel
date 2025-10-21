#include "Ecs.h"


NAMESPACE_UPP


void Easing::Visit(Vis& v) {
	v
	 VISN(target_position)
	 VISN(target_orientation)
	 VIS_(position_easing_factor)
	 VIS_(orientation_easing_factor);
	
	VISIT_COMPONENT
}

bool Easing::Initialize(const WorldState& ws) {
	GetEngine().AddUpdated("default_easing", this);
	return true;
}

void Easing::Uninitialize() {
	GetEngine().RemoveUpdated("default_easing", this);
}

INITBLOCK {
	Engine::AddNameUpdater("default_easing", [](VfsValueExt& c) {
		Easing* easing = CastPtr<Easing>(&c);
		if (!easing)
			return;
		Ptr<Transform> transform = easing->GetEntity()->val.Find<Transform>();
		if (!transform)
			return;
		TODO
	});
}


END_UPP_NAMESPACE
