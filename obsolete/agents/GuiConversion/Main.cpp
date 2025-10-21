#ifdef flagUPP
#include <CtrlLib/CtrlLib.h>
using namespace Upp;
#elif defined flagMSC
#include <windows.h>
#include <stdio.h>
#include <string.h>
#endif

void Upp01(int);
void Qt01(int);
void Wx01(int);
void Juce01(int);
void Mfc01(int);
void Wtl01(int);
void WinRT01(int);
void Vst01(int);
void Gtkmm01(int);
void Wt01(int);
void Agar01(int);
void Gtk01(int);
void Cocoa01(int);

#ifdef flagUPP
GUI_APP_MAIN
#elif defined flagMSC
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow)
#else
int main(int argc,const char** argv)
#endif
{
	int test = 0;
	
	#ifdef flagUPP
	if (CommandLine().GetCount())
		test = ScanInt(CommandLine()[0]);
	#elif defined flagMSC
	if (lpCmdLine[0] != 0)
		test = atoi(lpCmdLine);
	#else
	if (argc > 1)
		test = atoi(argv[1]);
	#endif
	
	Upp01(test);
	Qt01(test);
	Wx01(test);
	Juce01(test);
	Mfc01(test);
	Wtl01(test);
	WinRT01(test);
	Vst01(test);
	Gtkmm01(test);
	Wt01(test);
	Agar01(test);
	Gtk01(test);
	Cocoa01(test);
	
}


#ifndef HAVE_COCOA
void Cocoa01(int) {} // obj-c
#endif

