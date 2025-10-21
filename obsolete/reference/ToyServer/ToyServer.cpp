#include <ToyHub/ToyHub.h>
using namespace Upp;

// Local server URL: 127.0.0.1:8001
CONSOLE_APP_MAIN
{
#ifdef _DEBUG
	StdLogSetup(LOG_FILE|LOG_COUT);
	Ini::skylark_log = true;
#endif
	
	RunToyHub();
}

