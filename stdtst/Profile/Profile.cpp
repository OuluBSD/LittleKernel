#include <iostream>
#include <Core/Core.h>
using namespace Upp;

int main(){
    RACTIVATE_TIMING();
    {
        RTIMING("block");
        RHITCOUNT("hits/block");
    }
    TimeStop ts; for(volatile int i=0;i<100000;i++); int ms = ts.Msecs();
    if(ms >= 0) { std::cout << "OK\n"; return 0; }
    return 1;
}

