#include "LLM.h"

NAMESPACE_UPP


AiChatComponent::AiChatComponent(VfsValue& owner) : Component(owner) {
	
}

AiChatComponent::~AiChatComponent() {
	
}

void AiChatComponent::Visit(Vis& v) {
	v.Ver(0);
}

INITIALIZER_COMPONENT(AiChatComponent, "ecs.disposable.chat", "AI|Chat");
INITIALIZER_COMPONENT(AiStageExample, "ecs.disposable.stage", "AI|Stage");


END_UPP_NAMESPACE
