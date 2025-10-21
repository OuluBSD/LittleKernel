#include <iostream>
#include <Core/Core.h>
using namespace Upp;

int main(){
    String s = "hello world";
    String enc = Base64Encode(s);
    String dec = Base64Decode(enc);
    if(dec != s){ std::cout << "FAIL\n"; return 1; }
    std::cout << "OK\n"; return 0;
}

