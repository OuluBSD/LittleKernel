#ifndef _Core_CompatExt_Compat_h_
#define _Core_CompatExt_Compat_h_





#if __GNUC__
	#define UNREACHABLE __builtin_unreachable()
#endif

#ifdef _MSC_VER
	#define UNREACHABLE __assume(0)
#endif

#define MemoryCompare memcmp
#define MemoryCopy    memcpy
#define MemoryMove    memmove

#undef TODO
#define MACROSTR(x) #x
#define COUT(x) {::UPP::String ____s; ____s << x; Cout() << ____s << "\n";}
#define COUTLOG(x) {::UPP::String ____s; ____s << x; LOG(____s); Cout() << ____s << "\n";}
#define TODO {Panic("TODO " __FILE__ ":" + UPP::IntStr(__LINE__)); throw UPP::Exc("TODO");}
#define SYNON_UNION_2(type, name0, name1) union {type name0; type name1;};
#define PANIC(msg) Panic(msg); UNREACHABLE

#define RTTI_TYPEIDCLS



template<class InputIterator, class UnaryPredicate>
InputIterator FindIf(InputIterator first, InputIterator last, UnaryPredicate pred) {
	return std::find_if(first, last, pred);
}

inline bool IsBinDigit(int c) { return c == '0' || c == '1'; }
inline bool IsHexDigit(int c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
inline int GetHexDigit(int c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c - 'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c - 'A';
	return 0;
}
inline int GetBinDigit(int c) {
	if (c == '0' || c == '1')
		return c - '0';
	return 0;
}
inline int64 BinInt64(const char *s) {
	if (!s) return 0;
	while (IsSpace(*s)) s++;
	int64 n=0, neg=0;
	switch (*s) {
		case '-': neg=1;
		case '+': s++;
	}
	if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
		s += 2;
	while (IsBinDigit(*s))
		n = 2*n - GetBinDigit(*s++);
	return neg ? n : -n;
}
inline int64 HexInt64(const char *s) {
	if (!s) return 0;
	while (IsSpace(*s)) s++;
	int64 n=0, neg=0;
	switch (*s) {
		case '-': neg=1;
		case '+': s++;
	}
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;
	while (IsHexDigit(*s))
		n = 16*n - GetHexDigit(*s++);
	return neg ? n : -n;
}


//#if !(defined flagMSC && !defined flagUWP)
inline WString FromUtf8(String s) {return s.ToWString();}
//#endif



#endif
