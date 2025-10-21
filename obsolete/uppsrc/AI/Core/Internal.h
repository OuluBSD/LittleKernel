#ifndef _AI_Core_Internal_h_
#define _AI_Core_Internal_h_

#include <Vfs/Runtime/Runtime.h>
#ifdef flagAUDIO
	#include <Sound/Sound.h>
#endif
#include <plugin/bz2/bz2.h>
#include <plugin/png/png.h>
#include <plugin/jpg/jpg.h>
#ifdef flagCURL
	#define HAVE_OPENAI 1
	#include <plugin/openai/openai.h>
#else
	#define HAVE_OPENAI 0
#endif
#include <ide/AiProvider.h>
#include <Esc/Esc.h>

#ifdef flagV1
#error V1 flag is set
#endif


struct Ide;
struct CurrentFileClang;
struct CurrentFileContext;

#endif
