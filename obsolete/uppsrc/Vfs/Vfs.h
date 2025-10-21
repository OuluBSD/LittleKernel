#ifndef _Vfs_Backend_h_
#define _Vfs_Backend_h_

#ifndef flagLCLANG
	#ifdef PLATFORM_POSIX
	#define DYNAMIC_LIBCLANG // dynamic loading of clang experiment (does not seem to work in Win32)
	#endif
#endif
#ifdef DYNAMIC_LIBCLANG
#include <ide/clang/libclang.h>
#else
#include <clang-c/Index.h>
#endif

#include <Core/Core.h>
#include <Vfs/Core/Core.h>
#include <Esc/Esc.h>

NAMESPACE_UPP

#include "Defs.h"
#include "Node.h"
#include "Util.h"
#include "Ast.h"
#include "EonStd.h"
#include "Exporter.h"
#include "TokenStructure.h"
#include "SemanticParser.h"
#include "AstRunner.h"
#include "AstExporter.h"
#include "Compiler.h"

END_UPP_NAMESPACE

#endif
