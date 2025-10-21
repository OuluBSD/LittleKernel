#include "Biography.h"

NAMESPACE_UPP


BiographyPlatformCtrl::BiographyPlatformCtrl() : p(*this), a(*this), c(*this) {
	Add(tabs.VSizePos(0,20).HSizePos());
	
	p.Ctor();
	a.Ctor();
	c.Ctor();
	
	tabs.WhenSet << THISBACK(Data);
}

void BiographyPlatformCtrl::Data() {
	int tab = tabs.Get();
	
	if      (tab == 0)	p.Data();
	else if (tab == 1)	c.Data();
	else if (tab == 2)	a.Data();
}

void BiographyPlatformCtrl::ToolMenu(Bar& bar) {
	int tab = tabs.Get();
	
	if      (tab == 0)	p.ToolMenu(bar);
	else if (tab == 1)	c.ToolMenu(bar);
	else if (tab == 2)	a.ToolMenu(bar);
}

void BiographyPlatformCtrl::Do(int fn) {
	int tab = tabs.Get();
	
	if      (tab == 0)	p.Do(fn);
	else if (tab == 1)	c.Do(fn);
	else if (tab == 2)	a.Do(fn);
}

void BiographyPlatformCtrl::SetSorting(int col) {
	
}

void BiographyPlatformCtrl::ImportJson() {
	DatasetPtrs p; GetDataset(p);
	BiographyPlatform& data = GetExt<BiographyPlatform>();
	if (LoadFromJsonFile_VisitorNodePrompt(data)) {
		PostCallback(THISBACK(Data));
	}
}


INITIALIZER_COMPONENT_CTRL(BiographyPlatform, BiographyPlatformCtrl)

END_UPP_NAMESPACE
