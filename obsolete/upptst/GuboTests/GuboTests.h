#ifndef _GuboTests_GuboTests_h_
#define _GuboTests_GuboTests_h_

#include <GuboLib/GuboLib.h>
#include <DesktopSuite/DesktopSuite.h>
using namespace Upp;


class GuboTester : public TopGubo {
	TimeStop ts;
	float phase_seconds = 3.0;
	
public:
	typedef GuboTester CLASSNAME;
	GuboTester();
	
	
	void Paint(Draw3D& d) override;
	
	
};

#endif
