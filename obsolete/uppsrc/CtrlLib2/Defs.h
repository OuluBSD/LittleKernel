#ifndef _CtrlLib_Defs_h_
#define _CtrlLib_Defs_h_


#ifndef flagGUI
	#error <LocalCtrl/CtrlLib.h> should only be included, when GUI flag is set.
#endif


#if defined flagOGL
	#if UPP_OLD_VERSION || (defined flagWIN32 && defined flagGCC)
		#define GLEW_STATIC 1
		#include <plugin/glew/glew.h>
	#else
		#include <GL/glew.h>
	#endif
	
	#if HAVE_GLUT
		#include <GL/freeglut.h>
	#endif
#endif


#include <Core2/Core.h>

#include <CtrlLib/CtrlLib.h>


#ifdef GUIPLATFORM_VIRTUALGUI_INCLUDE
	#include GUIPLATFORM_VIRTUALGUI_INCLUDE
#endif


#endif
