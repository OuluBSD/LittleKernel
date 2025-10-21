#include <iostream>
#include <Core/Core.h>
using namespace Upp;

static int fails = 0;
static void Check(const char* name, bool ok) {
    if(!ok) { std::cout << "FAIL: " << name << "\n"; ++fails; }
}

int main(){
    String tmpdir = GetTempPath();
    Check("have tmpdir", !tmpdir.IsEmpty());

    String f1 = AppendFileName(tmpdir, "stdtst_path_f1.txt");
    String f2 = AppendFileName(tmpdir, "stdtst_path_f2.txt");
    String f3 = AppendFileName(tmpdir, "stdtst_path_sub" DIR_SEPS "stdtst_path_f3.txt"); // overload not present; build f3 manually
    f3 = AppendFileName(AppendFileName(tmpdir, "stdtst_path_sub"), "stdtst_path_f3.txt");

    SaveFileAsString(f1.Begin(), String("hello"));
    Check("exists f1", FileExists(f1.Begin()));
    Check("length f1", GetFileLength(f1.Begin()) == 5);

    Check("copy f1->f2", FileCopy(f1.Begin(), f2.Begin()) && FileExists(f2.Begin()));
    Check("move f2->f1", FileMove(f2.Begin(), f1.Begin()) && FileExists(f1.Begin()));

    // directory create
    String subdir = AppendFileName(tmpdir, "stdtst_path_sub");
    Check("realize dir", RealizeDirectory(subdir));
    SaveFileAsString(f3.Begin(), String("x"));
    Check("exists f3", FileExists(f3.Begin()));

    // GetFileOnPath
    String got = GetFileOnPath(GetFileName(f3.Begin()), subdir.Begin(), false);
    Check("getfileonpath", PathIsEqual(got.Begin(), f3.Begin()));

    // NormalizePath & PathIsEqual
    String norm = NormalizePath(f3.Begin());
    Check("normalize path equal", PathIsEqual(norm.Begin(), f3.Begin()));

    // Cleanup
    FileDelete(f1.Begin());
    FileDelete(f2.Begin());
    FileDelete(f3.Begin());
    DirectoryDelete(subdir.Begin());

    if(fails==0) std::cout << "OK\n";
    return fails ? 1 : 0;
}

