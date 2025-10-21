#include "CtrlLib.h"

NAMESPACE_UPP
FileSel& GFileSel();

void SetFileDialogDirectory(String path) {
	FileSel& fs = GFileSel();
	LoadFromGlobal(fs, "GlobalFileSelector");
	fs.ActiveDir(path);
	StoreToGlobal(fs, "GlobalFileSelector");
}

END_UPP_NAMESPACE
