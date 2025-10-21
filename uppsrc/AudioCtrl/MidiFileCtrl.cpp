#include "AudioCtrl.h"

NAMESPACE_UPP

INITBLOCK_(MidiFileCtrl) {
	MakeComponentCtrlFactory<MidiFileComponent, MidiFileCtrl>();
}


MidiFileCtrl::MidiFileCtrl() {
	
}

void MidiFileCtrl::Reset() {
	
}

void MidiFileCtrl::Updated() {
	if (IsModified()) {
		LOG("MidiFileCtrl::Updated");
		ClearModify();
	
	}
}

void MidiFileCtrl::SetComponent(Component& base) {
	MidiFileComponent* new_comp = CastPtr<MidiFileComponent>(&base);
	if (new_comp && new_comp == comp)
		return;
	
	Reset();
	comp = new_comp;
	
	if (!comp) {
		LOG("MidiFileCtrl: error: unexpected component");
		return;
	}
	
	SetModify();
}


END_UPP_NAMESPACE
