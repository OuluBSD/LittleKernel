#ifndef _AI_Core_AiChat_h_
#define _AI_Core_AiChat_h_





struct AiChatComponent : Component {
	//OmniThread omni;
	
	void Visit(Vis& v) override;
	String GetName() const override {return "AI: Chat";}
	
	CLASSTYPE(AiChatComponent)
	AiChatComponent(VfsValue& owner);
	~AiChatComponent();
	
	
};

INITIALIZE(AiChatComponent);



INITIALIZE_VALUECOMPONENT(AiStageExample);





#endif
