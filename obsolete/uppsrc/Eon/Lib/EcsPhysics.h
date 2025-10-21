#ifndef _EonLib_EcsPhysics_h_
#define _EonLib_EcsPhysics_h_


#define FYS_ECS_CLS_LIST(f) \
	FYS_CLS(System, f)

#define FYS_CLS(x, f) struct f##x;
	#define FYS_SYS(x) FYS_ECS_CLS_LIST(x)
		FYS_FYSSYS_LIST
	#undef FYS_SYS
#undef FYS_CLS


#define FYS_CLS(x, f) \
struct f##x : x##T<f##Fys> {\
	f##x(VfsValue& n) : Base(n) {} \
	void Visit(Vis& v) override {VIS_THIS(Base);} \
};
	#define FYS_SYS(x) FYS_ECS_CLS_LIST(x)
		FYS_FYSSYS_LIST
	#undef FYS_SYS
#undef FYS_CLS


#endif
