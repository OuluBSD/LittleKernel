#ifndef _AudioCtrl_MidiFileCtrl_h_
#define _AudioCtrl_MidiFileCtrl_h_

NAMESPACE_UPP


class MidiFileCtrl : public ComponentCtrl {
	MidiFileComponent* comp = 0;
	
public:
	typedef MidiFileCtrl CLASSNAME;
	MidiFileCtrl();
	
	void Reset();
	void Updated() override;
	void SetComponent(Component& base) override;
	
	
};


END_UPP_NAMESPACE

#endif
