#include "Draw.h"


NAMESPACE_UPP


void ToolSelectorKey::Visit(Vis& v) {
	
	TODO //e % type; // TypeCls to string
	
}

void ToolComponent::Visit(Vis& v) {
	v & active_tool & active_hand;
	v && tools;
	
	TODO
	#if 0
	e % title
	  % description;
	
	EtherizeRef(e, active_tool);
	EtherizeRefContainer(e, tools);
	EtherizeRef(e, active_hand);
	
	TODO // tool_type TypeCId
	#endif
}


END_UPP_NAMESPACE

