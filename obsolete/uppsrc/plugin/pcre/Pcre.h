#ifndef _plugin_pcre_pcre_h_
#define _plugin_pcre_pcre_h_

#include <Core/Core.h>

#ifdef PLATFORM_WIN32
#define PCRE_EXP_DECL extern 
#endif

#include "lib/config.h"
#include "lib/pcre.h"
#include "RegExp.h"

#endif
