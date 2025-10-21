#include <iostream>
#include <Core/Core.h>
using namespace Upp;

static int fails = 0;
static void Check(const char* name, bool ok) { if(!ok){ std::cout << "FAIL: " << name << "\n"; ++fails; } }

int main(){
    // VectorMap helpers
    VectorMap<String, int> vm;
    vm.Add("a", 1); vm.Add("b", 2);
    Check("vm find a", vm.Find("a") == 0);
    Check("vm contains b", vm.Contains("b"));
    Check("vm tryget b", *vm.TryGet("b") == 2);
    vm.Put("b", 22);
    Check("vm put b", vm["b"] == 22);
    vm.Insert(1, "x", 99);
    Check("vm insert pos", vm.GetKey(1) == "x");
    auto keys = vm.Keys();
    Check("vm keys size", (int)keys.size() == vm.GetCount());
    int removed = vm.RemoveIf([](const String& k, int){ return k == "x"; });
    Check("vm removeif", removed == 1);
    vm.SwapRemove(vm.Find("a"));
    Check("vm after swapremove", !vm.Contains("a"));
    int& ez = vm.Ensure("z", [](){ return 777; });
    Check("vm ensure", ez == 777);
    // Reorder and append
    int fb = vm.Find("b");
    vm.Move(fb, vm.GetCount());
    Check("vm move to end", vm.GetKey(vm.GetCount()-1) == "b");
    VectorMap<String,int> vm2; vm2.Add("m", 5); vm2.Add("n", 6);
    vm.Append(vm2);
    Check("vm append keys", vm.Contains("m") && vm.Contains("n"));
    vm.Swap(0, vm.Find("m"));
    Check("vm swap", vm.GetKey(0) == "m");

    // Index helpers
    Index<String> ix;
    ix.Add("k1"); ix.Add("k2");
    int pos = ix.FindAdd("k3");
    Check("ix findadd", ix[pos] == "k3");
    ix.Insert(1, "kin");
    Check("ix insert", ix[1] == "kin");
    ix.Set(1, String("set"));
    Check("ix set", ix[1] == "set");
    int idx = -1; Check("ix removekey with idx", ix.RemoveKey("set", idx) && idx == 1);
    auto cnt = ix.RemoveIf([](const String& s){ return s.StartsWith("k"); });
    Check("ix removeif any", cnt > 0);
    ix.Add("x1"); ix.Add("x2");
    ix.Move(0, ix.GetCount());
    Check("ix move end", ix[ix.GetCount()-1] == "x1");
    int px = ix.Find("x2"); ix.Swap(px, ix.GetCount()-1);
    Check("ix swap", ix[ix.GetCount()-1] == "x2");

    if(fails==0) std::cout << "OK\n"; return fails?1:0;
}
