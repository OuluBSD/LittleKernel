#ifndef _Core2_Defs_h_
#define _Core2_Defs_h_

// If "debug realtime errors" is defined (with DEBUG_RT)
#ifdef flagDEBUG_RT
	#define DEBUG_RT_PIPE 1
	#ifdef flagDEBUG
		#define RTLOG(x) DLOG(x)
	#else
		#define RTLOG(x) RLOG(x)
	#endif
#else
	#define DEBUG_RT_PIPE 0
	#define RTLOG(x) {}
#endif

#ifdef flagDEBUG
	#define DEFAULT_AUDIO_QUEUE_SIZE	10
#else
	#define DEFAULT_AUDIO_QUEUE_SIZE	4
#endif

#ifndef CXX2A_STATIC_ASSERT
	#if defined flagMSC && !defined flagUWP
		#define CXX2A_STATIC_ASSERT(x, y) static_assert(x, y)
	#else
		#define CXX2A_STATIC_ASSERT(x, y)
	#endif
#endif

#define ECS_SYS_CTOR_(x) \
	CLASSTYPE(x) \
	x(VfsValue& e) : System(e)

#define ECS_SYS_CTOR(x) \
	CLASSTYPE(x) \
	x(VfsValue& m) : System(m) {}
#define ECS_SYS_CTOR_DERIVED(x, derived_from) \
	CLASSTYPE(x) \
	x(VfsValue& m) : derived_from(m) {}
#define ECS_SYS_DEF_VISIT void Visit(Vis& vis) override {}
#define ECS_SYS_DEF_VISIT_(x) void Visit(Vis& vis) override {x;}
#define PREFAB_BEGIN(x) \
struct x##_ { \
\
struct x : \
	x##_, \
	::Upp::EntityPrefab<

#define PREFAB_END \
> { \
	 \
    static Components Make(::Upp::Entity& e) \
    { \
        auto components = EntityPrefab::Make(e); \
		return components; \
    } \
};

#define COMP_DEF_VISIT void Visit(Vis& vis) override {}
#define COMP_DEF_VISIT_(x) void Visit(Vis& vis) override {x;}
#define COPY_PANIC(T) void operator=(const T& t) {Panic("Can't copy " #T);}


#endif
