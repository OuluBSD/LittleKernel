#include <iostream>
#include <Core/Core.h>

using namespace Upp;

static int fails = 0;
static void Check(const char* name, bool ok) {
    if(!ok) { std::cout << "FAIL: " << name << "\n"; ++fails; }
}

int main() {
    // Array: pointer stability test
    Array<int> a;
    a.Add(1); a.Add(2); a.Add(3);
    int* p2 = &a[1];
    Check("init ptr", *p2 == 2);
    for(int i=0;i<100;i++) a.Add(i+10);
    Check("after grow", *p2 == 2);
    a.Remove(0); // remove first
    Check("after remove front", *p2 == 2); // still pointing to old element (now at index 0)
    a.Remove(1, 5); // remove some others after the element
    Check("after remove many", *p2 == 2);

    // ArrayMap: pointer stability, key ops
    ArrayMap<String, int> m;
    m.Add("a", 11);
    m.Add("b", 22);
    m.Add("c", 33);
    int* pb = &m["b"]; // GetAdd creates if missing; here exists
    Check("map get", *pb == 22);
    for(int i=0;i<100;i++) m.Add(Format("k%d", i), i);
    Check("map grow", *pb == 22);
    m.RemoveKey("a");
    m.Remove(1); // remove an element after 'b'
    Check("map remove keep ptr", *pb == 22);
    m["z"] = 99;
    Check("map getadd", m["z"] == 99);

    // SetKey, TryGet, iteration
    int ib = m.Find("b");
    Check("find b", ib >= 0);
    Check("setkey", m.SetKey(ib, String("bb")) == true);
    Check("contains new key", m.Contains("bb"));
    Check("not contains old key", !m.Contains("b"));
    Check("tryget", *m.TryGet("bb") == 22);
    int count = 0; int sum = 0;
    for(auto kv : m) { (void)kv.key; sum += kv.value; ++count; }
    Check("iter count", count == m.GetCount());
    // Helpers
    Check("valuesptr size", (int)m.ValuesPtr().size() == m.GetCount());
    Check("get with default", m.Get("__missing__", -5) == -5);
    Check("tryset miss", m.TrySet("__missing__", 1) == false);
    Check("removeif none", m.RemoveIf([](const String& k, int v){ return k == "__never__"; }) == 0);
    int oldc = m.GetCount();
    int removed = m.RemoveIf([](const String& k, int){ return k.StartsWith("k"); });
    Check("removeif some", removed == oldc - m.GetCount());

    // Array helpers
    Array<String> as;
    as.Add("aa"); as.Add("bb"); as.Add("cc");
    Check("find val", as.Find("bb") == 1);
    String* pbptr = &as[1];
    Check("find ptr", as.FindPtr(pbptr) == 1);
    Check("remove ptr", as.RemovePtr(pbptr) == true);
    Check("find val miss", as.Find("bb") == -1);
    int pos = as.AddN(3);
    Check("addn pos", pos == as.GetCount()-3);
    int rem = as.RemoveIf([](const String& s){ return s.IsEmpty(); });
    Check("removeif array", rem >= 0);

    // Additional Array helpers
    int* owned = new int(77);
    Array<int> ai;
    int idxOwn = ai.AddOwned(owned);
    Check("addowned addr", &ai[idxOwn] == owned);
    ai.InsertOwned(0, new int(5));
    Check("insertowned", ai[0] == 5);
    ai.SwapRemove(0); // remove first fast
    ai.Add(1234);
    int vpop = ai.Pop();
    Check("pop value", vpop == 1234);
    ai.AddOwned(new int(555));
    int* pdet = ai.PopDetach();
    Check("popdetach", *pdet == 555);
    delete pdet;

    // Additional ArrayMap helpers
    int& ez = m.Ensure("ensureZ", [](){ return 101; });
    Check("ensure", ez == 101);
    int* ownv = new int(42);
    int ip = m.AddOwned("own", ownv);
    Check("own ptr eq", &m["own"] == ownv);
    for(auto it = m.begin(); it != m.end();) {
        auto kv = *it;
        if(kv.key == "own")
            it = m.Erase(it);
        else
            ++it;
    }
    Check("erase iter", !m.Contains("own"));
    // Swap/Move/Append
    int ibb = m.Find("bb");
    m.Move(ibb, m.GetCount());
    Check("am move end", m.GetKey(m.GetCount()-1) == "bb");
    ArrayMap<String,int> m2; m2.Add("m", 7); m2.Add("n", 8);
    m.Append(m2);
    Check("am append", m.Contains("m") && m.Contains("n"));
    m.Swap(0, m.Find("m"));
    Check("am swap", m.GetKey(0) == "m");
    m.SwapRemove(0);
    Check("am swapremove", !m.Contains("m"));

    if(fails==0) std::cout << "OK\n";
    return fails ? 1 : 0;
}
