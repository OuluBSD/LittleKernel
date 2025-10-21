#include "Draw.h"


NAMESPACE_UPP


bool EventSystem::Initialize(const WorldState& ws) {
	return true;
}

/*void EventSystem::Attach(Serial::EcsEventsBase* b) {
	ASSERT(b);
	EventStateBase::AddBinder(this);
	serial = b;
}*/

bool EventSystem::Start() {
	return true;
}

void EventSystem::Update(double dt) {
	
}

void EventSystem::Stop() {
	
}

void EventSystem::Uninitialize() {
	#ifdef flagSCREEN
	EventStateBase::RemoveBinder(this);
	#endif
}


END_UPP_NAMESPACE

