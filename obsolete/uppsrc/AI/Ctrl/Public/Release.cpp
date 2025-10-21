#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP


ReleaseInfoCtrl::ReleaseInfoCtrl() {
	CtrlLayout(*this);
	
	title <<= THISBACK(OnValueChange);
	album_date <<= THISBACK(OnValueChange);
	year_of_content <<= THISBACK(OnValueChange);
}

void ReleaseInfoCtrl::Clear() {
	this->title				.Clear();
	this->album_date		.Clear();
	this->year_of_content	.Clear();
}

void ReleaseInfoCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	
	Clear();
	
	if (p.release) {
		Release& r = *p.release;
		
		title.SetData(r.title);
		album_date.SetData(r.date);
		year_of_content.SetData(r.year_of_content);
	}
	
}

void ReleaseInfoCtrl::OnValueChange() {
	DatasetPtrs p; GetDataset(p);
	
	if (p.release) {
		Release& r = *p.release;
		
		r.title = title.GetData();
		r.date = album_date.GetData();
		r.year_of_content = year_of_content.GetData();
	}
}



INITIALIZER_COMPONENT_CTRL(Release, ReleaseInfoCtrl)

END_UPP_NAMESPACE
