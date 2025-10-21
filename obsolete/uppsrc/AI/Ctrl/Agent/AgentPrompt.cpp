#include "Agent.h"

NAMESPACE_UPP

AgentPromptEdit::AgentPromptEdit() {
	CtrlLayout(*this);
	attach.SetImage(AIImgs::attachment);
	versions.SetData(1);
}

END_UPP_NAMESPACE
