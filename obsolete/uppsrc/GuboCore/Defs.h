#ifndef _GuboCore_Defs_h_
#define _GuboCore_Defs_h_




#define NAMESPACE_GUBO_NAME		Gu
#define GUBO					Gu
#define NAMESPACE_GUBO_BEGIN	\
	namespace Upp { namespace NAMESPACE_GUBO_NAME {
#define NAMESPACE_GUBO_END		}}

#if 0
#define HAVE_VIRTUALGUI 1

#if HAVE_VIRTUALGUI
	#define GUIPLATFORM_KEYCODES_INCLUDE       <VirtualGui3D/Keys.h>
	#define GUIPLATFORM_INCLUDE                <VirtualGui3D/VirtualGui3D.h>
	#define VIRTUALGUI 1
#else
	#define GUIPLATFORM_INCLUDE                <StaticInterface/Screen.h>
#endif
#endif


#if 0
#ifndef GUIPLATFORM_INCLUDE
	#error Build flags prevents GuboCore usage. Probably GUI flag is not set and GuboCore.h is included indirectly.
#else
	#include GUIPLATFORM_INCLUDE
#endif
#endif



#if defined LIBTOPSIDE && defined flagGUI
	#define HAVE_WINDOWSYSTEM 1
	#include <VirtualGui3D/VirtualGui3D.h>
#endif


#endif
