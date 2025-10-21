#include "PlayEditor.h"

#ifdef flagMAIN
using namespace Upp;

GUI_APP_MAIN {
	
	Upp::PlayEditor e;
	
	if (CommandLine().GetCount() && !e.LoadFile(CommandLine()[0]))
		return;
	
	e.Run();
	
	e.SaveFile();
	
}

#endif
