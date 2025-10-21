#include "MetaCtrl.h"

#define IMAGECLASS MetaImgs
#define IMAGEFILE <MetaCtrl/Images.iml>
#include <Draw/iml_source.h>

NAMESPACE_UPP


VfsValueExtCtrl::VfsValueExtCtrl() {
	
}

VfsValueExt& VfsValueExtCtrl::GetExt() {return *ext;}
VfsValue& VfsValueExtCtrl::GetValue() {ASSERT(ext); return ext->val;}
const VfsValue& VfsValueExtCtrl::GetValue() const {return ext->val;}
String VfsValueExtCtrl::GetFilePath() const {
	if (owner)
		return owner->GetFilePath();
	else
		return String();
}

VfsPath VfsValueExtCtrl::GetCursorPath() const {
	if (ext)
		return ext->val.GetPath();
	if (owner)
		return StrVfs(owner->GetFilePath());
	ASSERT_(0, "invalid cursor path");
	return VfsPath();
}

void VfsValueExtCtrl::ToolMenu(Bar& bar) {
	
}

END_UPP_NAMESPACE
