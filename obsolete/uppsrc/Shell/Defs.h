#ifndef _Shell_Defs_h_
#define _Shell_Defs_h_


#if defined flagMSC && !defined flagUWP
	#define CXX2A_STATIC_ASSERT(x, y) static_assert(x, y)
#else
	#define CXX2A_STATIC_ASSERT(x, y)
#endif


#define LOOP_PREFAB_BEGIN(x) \
struct x : \
	SpacePrefab<

#define LOOP_PREFAB_END \
> { \
	 \
    static Atoms Make(Space& e) \
    { \
        auto atoms = SpacePrefab::Make(e); \
		return atoms; \
    } \
};


#endif
