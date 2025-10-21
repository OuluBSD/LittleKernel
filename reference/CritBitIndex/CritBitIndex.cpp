#include <Core/Core.h>

using namespace Upp;

// Simple GetHash functor (optional for merges/derivations)
struct GetHashInt {
    uint64 operator()(const int& v) const { return (uint64)(uint32)v; }
};


void Showcase1() {
	LOG("\n\nSHOWCASE 1:");
	CritBitIndex<int, GetHashInt, 64> idx;

	Vector<uint64> keys;
	keys << 42 << 100 << 7 << 9999999 << 42; // includes duplicate 42

	Vector<int*> out;
	idx.PutBulk(keys, out);

	// Assign values matching order (duplicate should reuse pointer)
	for(int i = 0; i < out.GetCount(); i++)
		*out[i] = i + 1;

    LOG("\nTree:\n" << idx.GetTreeString());
    
	DUMP(idx.GetCount()); // should be 4 (duplicate excluded)

	if(int* p = idx.Find(100))
		LOG("100 -> " << *p);

	if(int* p = idx.Find(42))
		LOG("42 -> " << *p);

	// Remove and optimize
	bool ok = idx.Remove(7);
	DUMP(ok);
	idx.Optimize();
	DUMP(idx.GetCount());

	// Merge example
	CritBitIndex<int, GetHashInt, 64> other;
	other.Put(5, 555);
	other.Put(100, 1000); // conflict with keepThis
	idx.Merge(other, true);
	DUMP(idx.GetCount());
	if(int* p = idx.Find(100)) LOG("100(after merge keepThis) -> " << *p);
	
}

void Showcase2() {
	LOG("\n\nSHOWCASE 2:");
	CritBitIndex<int, GetHashInt, 64> idx;

    Vector<uint64> keys;
    // Crafted keys to produce both shallow and deep splits and a duplicate
    keys << 0x1F20A4B300000012ULL << 0x1F20A4B3FFFF0012ULL << 0x9C00EE7700ABCDEFULL << 0x1F20A4B300000012ULL;

    Vector<int*> out;
    idx.PutBulk(keys, out); // duplicate is ignored on insert
    for(int i = 0; i < out.GetCount(); i++)
        *out[i] = i + 1; // write through returned pointers

    // C++-style iteration over values
    int sum = 0;
    for(auto& v : idx)
        sum += v;
    DUMP(sum);

    // U++-style key+value iteration via ~idx
    int sum_kv = 0;
    for(auto kv : ~idx)
        sum_kv += kv.value;
    DUMP(sum_kv);

    // Interplay with Index<Hash>: filter out a subset of keys
    Index<uint64> deny;
    deny.Add(0x1F20A4B300000012ULL);
    int sum_filtered = 0;
    for(auto kv : ~idx)
        if(deny.Find(kv.key) < 0)
            sum_filtered += kv.value;
    DUMP(sum_filtered);

    // Print tree at different indentation levels
    LOG("\nTree (indent=0):\n" << idx.GetTreeString());
    LOG("\nTree (indent=4):\n" << idx.GetTreeString(4));

    // Remove a key, optimize, and print again
    bool ok = idx.Remove(0x1F20A4B3FFFF0012ULL);
    DUMP(ok);
    idx.Optimize();
    LOG("\nTree (after remove+opt):\n" << idx.GetTreeString());

    // Merge example
    CritBitIndex<int, GetHashInt, 64> other;
    other.Put(0x0000000000000005ULL, 555);
    other.Put(0x1F20A4B300000012ULL, 1000); // conflict with keepThis
    idx.Merge(other, true);
    DUMP(idx.GetCount());
    if(int* p = idx.Find(0x1F20A4B300000012ULL))
        LOG("kept value -> " << *p);
}

void Showcase3() {
	static constexpr int PATH_HASH_BITS = (int)(sizeof(hash_t) * 8);

	struct StrHashAccessor {
		using Hash = hash_t;
		Hash operator()(const String& s) const { return (Hash)s.GetHashValue(); }
	};

	CritBitIndex<String, StrHashAccessor, PATH_HASH_BITS> seen_path_names;
	
	String s0 = "abc";
	String s3 = "123";
	String s4 = "345";
	
	const String* s1 = seen_path_names.Put(s0.GetHashValue(), s0);
	ASSERT(*s1 == s0);
	
	seen_path_names.Put(s3.GetHashValue(), s3);
	seen_path_names.Put(s4.GetHashValue(), s4);
	
	const String* s2 = seen_path_names.Find(s0.GetHashValue());
	ASSERT(s1 == s2);
	ASSERT(*s2 == s0);
	
	
    for(auto kv : ~seen_path_names) {
        LOG(kv.key);
        LOG(kv.value);
    }
}

CONSOLE_APP_MAIN
{
    StdLogSetup(LOG_COUT|LOG_FILE);
    SetLanguage(LNG_ENGLISH);

	Showcase1(),
    Showcase2();
    Showcase3();
}
