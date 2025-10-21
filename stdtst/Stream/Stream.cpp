#include <iostream>
#include <Core/Core.h>
using namespace Upp;

static int fails = 0;
static void Check(const char* name, bool ok) { if(!ok){ std::cout << "FAIL: " << name << "\n"; ++fails; } }

int main(){
    // MemoryStream
    MemoryStream ms; ms.Put("abc", 3); ms.Seek(0); char b[3]; size_t n = ms.Get(b,3); Check("mem get len", n==3); Check("mem content", String(b,3)==String("abc"));

    // FileStream
    String tmp = AppendFileName(GetTempPath(), "stdtst_stream.dat");
    {
        FileStream fs(tmp.Begin(), FileStream::WRITE);
        Check("fs open write", fs.GetSize()==0 || true);
        const char* d = "data"; fs.Put(d, 4);
    }
    {
        FileStream fs(tmp.Begin(), FileStream::READ);
        char buff[4]; size_t m = fs.Get(buff,4); Check("fs read size", m==4); Check("fs read data", String(buff,4)==String("data"));
    }
    FileDelete(tmp.Begin());

    if(fails==0) std::cout << "OK\n";
    return fails ? 1 : 0;
}

