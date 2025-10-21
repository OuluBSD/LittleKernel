#include <Core/Core.h>

using namespace Upp;

struct GetHashInt {
    uint64 operator()(const int& v) const { return (uint64)(uint32)v; }
};

static void LogTreeNoDoubleNl(const String& s)
{
    if(!s.IsEmpty() && s[s.GetCount() - 1] == '\n')
        LOG(s.Left(s.GetCount() - 1));
    else
        LOG(s);
}

CONSOLE_APP_MAIN
{
    StdLogSetup(LOG_FILE|LOG_COUT);

    // Basic build and iterator sum
    CritBitIndex<int, GetHashInt, 64> idx;
    Vector<uint64> build_keys;
    build_keys << 0x1F20A4B300000012ULL << 0x1F20A4B3FFFF0012ULL << 0x9C00EE7700ABCDEFULL;
    Vector<int*> out;
    idx.PutBulk(build_keys, out);
    for(int i = 0; i < out.GetCount(); i++)
        *out[i] = i + 1; // {1,2,3}
    int sum = 0;
    for(auto& v : idx) sum += v;
    DUMP(sum); // 1+2+3=6

    // 1) Tree dump basics
    CritBitIndex<int, GetHashInt, 64> t;
    LogTreeNoDoubleNl(t.GetTreeString());
    int* p = t.Put(0x12);
    *p = 1;
    LogTreeNoDoubleNl(t.GetTreeString());
    t.Put(0x80000000000000FFULL, 2);
    LogTreeNoDoubleNl(t.GetTreeString());
    LogTreeNoDoubleNl(t.GetTreeString(4));

    // 2) Tree dump after Remove/Optimize
    bool ok = t.Remove(0x80000000000000FFULL);
    DUMP(ok);
    t.Optimize();
    LogTreeNoDoubleNl(t.GetTreeString());

    // 3) Iterator: basic / 4) edge (works when empty)
    int count = 0, sum2 = 0;
    for(auto& v : t) { count++; sum2 += v; }
    DUMP(count);
    DUMP(sum2);
    const auto& ct = t;
    int csum = 0;
    for(const auto& v : ct) csum += v;
    DUMP(csum);
    CritBitIndex<int, GetHashInt, 64> empty;
    int count2 = 0;
    for(auto& v : empty) { (void)v; count2++; }
    DUMP(count2);

    // 5) Iterator & duplicates
    int* p0 = t.Put(0x12);
    int* p1 = t.Put(0x12);
    LOG(String().Cat() << "dup_ok = " << (p0 == p1));
    LOG(String().Cat() << "final_count = " << t.GetCount());

    // 6) ~idx key+value iteration basics and mutation
    CritBitIndex<int, GetHashInt, 64> u;
    Vector<uint64> ukeys; ukeys << 10 << 20 << 30 << 40;
    Vector<int*> uout; u.PutBulk(ukeys, uout);
    for(int i = 0; i < uout.GetCount(); i++) *uout[i] = i + 1; // {1,2,3,4}

    int kv_count = 0, kv_sum = 0;
    bool ptr_ok = true;
    for(auto kv : ~u) {
        kv_count++;
        kv_sum += kv.value;
        ptr_ok = ptr_ok && (&kv.value == u.Find(kv.key));
        kv.value += 1; // mutate via kv
    }
    DUMP(kv_count); // 4
    DUMP(kv_sum);   // 1+2+3+4 = 10
    DUMP(ptr_ok);

    int sum_after_mut = 0;
    for(auto& v : u) sum_after_mut += v;
    DUMP(sum_after_mut); // 2+3+4+5 = 14

    // 7) Const ~ iteration
    const auto& cu = u;
    int csum_kv = 0;
    for(auto kv : ~cu) csum_kv += kv.value;
    DUMP(csum_kv); // 14

    // 8) Index<Hash> denylist filtering
    Index<uint64> deny2; deny2.Add(20); deny2.Add(40);
    int sum_filtered2 = 0;
    for(auto kv : ~u)
        if(deny2.Find(kv.key) < 0)
            sum_filtered2 += kv.value; // keys 10 and 30
    DUMP(sum_filtered2); // (2)+(4)=6

    // 9) Invalidation behavior: remove during iteration (restart safely)
    bool rem1 = u.Remove(20);
    DUMP(rem1);
    int sum_restart = 0;
    for(auto kv : ~u) sum_restart += kv.value; // should sum 2,4,5 -> 11
    DUMP(sum_restart);

    CheckLogEtalon();
}
