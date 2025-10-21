#include <Core/Core.h>
using namespace Upp;

void Upp01();
void Std01();
void Qt01();
void Wx01();
void Juce01();
void Mfc01();
void Wtl01();
void WinRT01();

CONSOLE_APP_MAIN {
	
	Upp01();
	Std01();
	
	#ifdef HAVE_QT
	Qt01();
	#endif
	
	#ifdef HAVE_WX
	Wx01();
	#endif
	
	#ifdef HAVE_JUCE
	Juce01();
	#endif
	
	#ifdef flagMFC
	Mfc01();
	#endif
	
	#ifdef flagWTL
	Wtl01();
	#endif
	
	#ifdef flagWINRT
	WinRT01();
	#endif
}
