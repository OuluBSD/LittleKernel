#ifndef STDSRC_CORE_CORE_H
#define STDSRC_CORE_CORE_H

// Aggregator header for stdsrc/Core
// Provides U++-compatible API surface backed by STL/Boost.

// Standard library includes live here (not in leaf headers)
#include <string>
#include <cwchar>
#include <locale>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <functional>
#include <filesystem>
#include <fstream>
#include <cctype>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <any>
#include <sstream>
#include <cstdint>
#include <array>
#include <random>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <tuple>
#include <complex>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#endif

#undef small

// Namespace wrappers (deprecated in original, but used to wrap aggregated headers)
#ifndef NAMESPACE_UPP
#define NAMESPACE_UPP     namespace Upp {
#define END_UPP_NAMESPACE }
#define UPP               Upp
#endif

// Token pasting helper for macros
#ifndef UPP_COMBINE
#define UPP_COMBINE__(a,b) a##b
#define UPP_COMBINE(a,b) UPP_COMBINE__(a,b)
#endif

// Bring in U++-style platform/CPU/compiler macros
#include "config.h"

// Fallback minimal platform macros if config.h did not set them
#if defined(_WIN32)
#  ifndef PLATFORM_WIN32
#    define PLATFORM_WIN32 1
#  endif
#else
#  ifndef PLATFORM_POSIX
#    define PLATFORM_POSIX 1
#  endif
#  ifndef flagPOSIX
#    define flagPOSIX 1
#  endif
#endif

// Directory separator macros (compat with U++ Core)
#ifndef DIR_SEP
  #ifdef _WIN32
    #define DIR_SEP  '\\'
    #define DIR_SEPS "\\"
  #else
    #define DIR_SEP  '/'
    #define DIR_SEPS "/"
  #endif
#endif

// Minimal forward declarations/types sometimes present in U++ Core
struct Nuller {};

// Aggregate public headers (no system includes inside these)
NAMESPACE_UPP
#include "Defs.h"
#include "String.h"
#include "WString.h"
#include "Vcont.h"
#include "Index.h"
#include "Map.h"
#include "One.h"
#include "Hash.h"
#include "Value.h"
#include "Util.h"
#include "Profile.h"
#include "Uuid.h"
#include "TimeDate.h"
#include "Stream.h"
#include "FileStream.h"
#include "Path.h"
#include "Algo.h"
#include "Sort.h"
#include "Tuple.h"
#include "Complex.h"
#include "Color.h"
#include "Gtypes.h"
#include "Array.h"
#include "ArrayMap.h"
#include "i18n.h"
#include "Lang.h"
#include "Ptr.h"
#include "Function.h"
#include "Callback.h"
#include "Format.h"
#include "Convert.h"
#include "TextIO.h"
#include "JSON.h"
#include "XML.h"
#include "Base64.h"
#include "Log.h"
#include "App.h"

END_UPP_NAMESPACE

// Convenience: match common free helper used by logging/formatting
inline std::string to_string(const UPP::String& s) { return std::string(s); }

// std::hash specializations for Upp::String/WString to enable unordered containers
namespace std {
template <> struct hash<Upp::String> {
    size_t operator()(const Upp::String& s) const noexcept {
        return std::hash<std::string>{}(static_cast<const std::string&>(s));
    }
};
template <> struct hash<Upp::WString> {
    size_t operator()(const Upp::WString& s) const noexcept {
        return std::hash<std::wstring>{}(static_cast<const std::wstring&>(s));
    }
};
}

// Console app macro for quick main() definition
#ifndef CONSOLE_APP_MAIN
#define CONSOLE_APP_MAIN int main(int /*argc*/, const char** /*argv*/)
#endif


#endif // STDSRC_CORE_CORE_H
