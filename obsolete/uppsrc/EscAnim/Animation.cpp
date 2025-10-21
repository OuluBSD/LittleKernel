#include "EscAnim.h"

NAMESPACE_UPP


Animation::Animation() {
	
}

void Animation::Clear() {
	scenes.Clear();
	active_scene = 0;
}

AnimScene& Animation::AddScene(String name) {
	AnimScene& s = scenes.Add();
	s.name = name;
	return s;
}

int Animation::GetKeysFromTime(double seconds) {
	int i = (int)(seconds * keys_per_second);
	return i;
}


END_UPP_NAMESPACE
