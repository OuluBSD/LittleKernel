#include <Core/Core.h>
using namespace Upp;

CONSOLE_APP_MAIN
{
    auto path = AppendFileName(GetTempPath(), "logtest.txt");
    StdLogSetup(LOG_FILE|LOG_COUT, path.Begin(), false); // truncate
    ILOG("Hello " << 123);
    WLOG("Warn " << 1);
    ELOG("Error " << 2);
    String s = LoadFileAsString(path.Begin());
    if(s.Find("Hello")<0 || s.Find("Warn")<0 || s.Find("Error")<0) { printf("FAIL1\n"); return 1; }
    // Test printf-style and filter: only WARN+
    StdLogSetup(LOG_FILE|LOG_COUT, path.Begin(), false); // truncate
    LogSetLevel(2);
    ILOG("Info suppressed");
    WLOGF("Warnf %d", 77);
    ELOGF("Errorf %s", "ok");
    String s2 = LoadFileAsString(path.Begin());
    if(s2.Find("Info suppressed")>=0 || s2.Find("Warnf 77")<0 || s2.Find("Errorf ok")<0) { printf("FAIL2\n"); return 1; }
    printf("OK\n");
    return 0;
}
