#include <iostream>
#include <Core/Core.h>

using namespace Upp;

static int fails = 0;

static void Check(const char* name, const String& got, const String& exp) {
    if(got != exp) {
        std::cout << "FAIL: " << name << "\n  got:    [" << got.ToStd() << "]\n  expect: [" << exp.ToStd() << "]\n";
        ++fails;
    }
}

int main() {
    // Basic and positional
    Check("int,str", Format("%d, %s", 123, "TEXT"), "123, TEXT");
    Check("positional", Format("%2:s, %1:d", 123, String("TEXT")), "TEXT, 123");

    // width and alignment
    Check("zero-pad", Format("%010d", 123), "0000000123");
    Check("dyn width", Format("%0*d", 11, 123), "00000000123");
    Check("align<", Format("|%20<d|", 123), "|123                 |");
    Check("align>", Format("|%20>d|", 123), "|                 123|");
    Check("align=", Format("|%20=d|", 123), "|        123         |");

    // suffix
    Check("suffix", Format("%d`pt", 123), "123pt");

    // null mapping
    Check("null-map", Format("%[empty]~d, %[empty]~d", 123, Null), "123, empty");

    // default
    Check("default-%", Format("%", 123), "123");

    // integer bases
    Check("char", Format("%c", 65), "A");
    Check("oct", Format("%o", 123), "173");
    Check("hex", Format("%x", 123), "7b");
    Check("HEX", Format("%X", 123), "7B");

    // thousands grouping for integers (apostrophe flag)
    Check("grp-dec", Format("%'d", 1234567), "1,234,567");
    Check("grp-dec-eu", Format("%',d", 1234567), "1.234.567");

    // m formatting
    String m1 = Format("%m", 1234567.89);
    if(m1.Find("e+") < 0) { std::cout << "WARN: %m not exponential for 1234567.89 => " << m1.ToStd() << "\n"; }
    Check("m-NaN-empty", Format("%?m", std::log(-1.0)), "");
    Check("m-prec7-trim", Format("%.7m", 1234567.0), "1234567");
    Check("m-fixed", Format("%.5mf", 1234567.89), "1234567.89000");
    Check("m-exp-e", Format("%.2me", 1234567.89), "1.23e+06");
    Check("m-exp-E", Format("%.2mE", 1234567.89), "1.23E+06");

    // mapping %s
    Check("map-hit", Format("%[1:one;2:two;3:three;another]s", 2), "two");
    Check("map-def", Format("%[1:one;2:two;3:three;another]s", 20), "another");
    Check("map-mod", Format("%[3%1:one;2:two;3:three;another]s", 20), "two");

    // months/days
    if(Format("%month", 6).ToStd().find("june") == std::string::npos) { std::cout << "WARN: %month mismatch: " << Format("%month", 6).ToStd() << "\n"; }
    if(Format("%Mon", 6).ToStd().find("Jun") == std::string::npos) { std::cout << "WARN: %Mon mismatch: " << Format("%Mon", 6).ToStd() << "\n"; }
    if(Format("%DAY", 6).ToStd().find("SATURDAY") == std::string::npos) { std::cout << "WARN: %DAY mismatch: " << Format("%DAY", 6).ToStd() << "\n"; }

    // letters/roman
    Check("letters-a", Format("%a", 1), "a");
    Check("letters-A", Format("%A", 27), "AA");
    Check("roman-r", Format("%r", 8), "viii");
    Check("roman-R", Format("%R", 1231), "MCCXXXI");

    // tw
    Check("tw0", Format("%tw", 0), "12");
    Check("tw15", Format("%tw", 15), "3");
    Check("tw0pad", Format("%0tw", 15), "03");

    if(fails==0) {
        std::cout << "OK\n";
    }
    return fails ? 1 : 0;
}
