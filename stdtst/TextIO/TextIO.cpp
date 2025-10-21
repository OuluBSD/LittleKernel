#include <iostream>
#include <Core/Core.h>
using namespace Upp;

int main(){
    String tmp = AppendFileName(GetTempPath(), "textio_test.txt");
    Vector<String> lines; lines.Add("one"); lines.Add("two");
    if(!WriteLines(tmp.Begin(), lines)) return 1;
    Vector<String> read; if(!ReadLines(tmp.Begin(), read)) return 1;
    if(read.GetCount()!=2 || read[0]!="one" || read[1]!="two") return 2;
    String s = LoadFileAsString(tmp.Begin());
    if(s.Find("one")<0 || s.Find("two")<0) return 3;
    SaveFileAsString(tmp.Begin(), String("alpha\n"));
    AppendFileAsString(tmp.Begin(), String("beta\n"));
    String s2 = LoadFileAsString(tmp.Begin());
    if(s2.Find("alpha")<0 || s2.Find("beta")<0) return 4;
    std::cout << "OK\n"; return 0;
}

