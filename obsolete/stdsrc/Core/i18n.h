// Minimal i18n shims for translation macros

struct LngEntry__ { int lang; const char* text; };

inline void AddModule(const LngEntry__*, const char* = nullptr) {}

inline const char* t_(const char* s) { return s; }
inline const char* tt_(const char* s) { return s; }

inline const char* GetENUS(const char* s) { return s; }

inline String GetLngString(const char* id) { return String(id ? id : ""); }
inline String GetLngString(int, const char* id) { return String(id ? id : ""); }

inline int GetCurrentLanguage() { return 0; }
inline void SetCurrentLanguage(int) {}

