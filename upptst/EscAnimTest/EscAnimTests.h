#ifndef _EscAnimTests_EscAnimTests_h_
#define _EscAnimTests_EscAnimTests_h_

#include <EscAnim/EscAnim.h>
using namespace UPP;


class TestApp : public TopWindow {
	TimeCallback tc;
	int mode = -1;
	
	EscAnimContext ctx;
	
public:
	typedef TestApp CLASSNAME;
	TestApp();
	
	void Paint(Draw& d);
	
	void Clear();
	void MakeNext();
	void Make1();
	void Make2();
	
	void Iterate();
	
};


#endif
