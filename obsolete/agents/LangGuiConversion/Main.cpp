#include <CtrlLib/CtrlLib.h>
using namespace Upp;

void Upp01(int);

GUI_APP_MAIN
{
	int test = 0;
	
	if (CommandLine().GetCount())
		test = ScanInt(CommandLine()[0]);
	
	Upp01(test);
}

