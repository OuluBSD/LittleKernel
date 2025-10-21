// Minimal language helpers and macros (stubs for source compatibility)

#ifndef LNG_
#define LNG_(a,b,c,d)   ( (((a - 'A' + 1) & 31) << 15) | (((b - 'A' + 1) & 31) << 10) | \
                         (((c - 'A' + 1) & 31) << 5)  | (((d - 'A' + 1) & 31) << 0) )
#define LNGC_(a,b,c,d,cs) ( LNG_(a,b,c,d) | ((cs & 255) << 20) )
#endif

inline int     LNGFromText(const char* /*s*/) { return 0; }
inline String  LNGAsText(int /*d*/) { return String("en-US"); }

// Prefer i18n.h stubs where present; keep only distinct helpers here
inline void    SetLanguage(int /*lang*/) {}
inline void    SetLanguage(const char* /*s*/) {}
inline String  GetCurrentLanguageString() { return String("en-US"); }
inline int     GetSystemLNG() { return 0; }

inline String  GetLangName(int /*language*/) { return String("English"); }
inline String  GetNativeLangName(int /*language*/) { return String("English"); }
