#include <iostream>
#include <Core/Core.h>
using namespace Upp;

static int fails = 0;
static void Check(const char* name, bool ok) { if(!ok){ std::cout << "FAIL: " << name << "\n"; ++fails; } }

int main(){
    Check("Atoi", Atoi("-123")==-123);
    Check("Atoi64", Atoi64("9223372036854775807")>0);
    Check("Atof", (int)(Atof("3.14")*100)==314);
    const char* e=nullptr; int i = ScanInt("77", &e, 10); Check("ScanInt", i==77 && *e=='\0');
    const char* e2=nullptr; double d = ScanDouble("12,5", &e2, true); Check("ScanDouble comma", (int)(d*10)==125 && *e2=='\0');
    Check("FormatInt", FormatInt(1234)==String("1234"));
    Check("FormatDouble", FormatDouble(3.14159,3)==String("3.142"));

    ConvertInt ci; Value v = ci.Scan(String("42")); Check("ConvertInt Scan", v.Is<int>() && v.Get<int>()==42);
    ConvertDouble cd; Value vd = cd.Scan(String("2.5")); Check("ConvertDouble Scan", vd.Is<double>());

    Date dd = ScanDate("2025-09-15"); Check("ScanDate", dd.IsValid());
    Time tt = ScanTime("2025-09-15 10:20:30"); Check("ScanTime", tt.IsValid());

    if(fails==0) std::cout << "OK\n";
    return fails ? 1 : 0;
}

