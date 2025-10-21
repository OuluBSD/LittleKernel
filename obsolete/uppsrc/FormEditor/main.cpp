#include "FormEditor.h"

#ifdef flagMAIN
GUI_APP_MAIN
{
	int mode = 0;
	
	if (mode == 0)
		DockableFormEdit().Run();
	else if (mode == 1)
		FormEditWindow().Run();
	else {
		TopWindow tw;
		FormEditCtrl c;
		tw.Add(c.SizePos());
		tw.Run();
	}
}
#endif
