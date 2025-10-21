#ifndef _api_Media_Media_h_
#define _api_Media_Media_h_


#include <Eon/Eon.h>


#ifdef flagFFMPEG
extern "C" {
	#include <libavdevice/avdevice.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/imgutils.h>
}

#define FFMPEG_VIDEOFRAME_RGBA_CONVERSION 1

#endif


#if defined flagOPENCV && defined flagLINUX
	#define HAVE_V4L2_DEVMGR	1
	//#define HAVE_V4L2_CAP		1
	//#define V4L2_SLOW		1
#elif defined flagOPENCV && (defined flagWIN32 && defined flagMSC)
	#define HAVE_WIN32_DEVMGR	1
#endif


#include "Types.h"
#include "Audio.h"
#include "Video.h"
#include "MediaStream.h"
#include "FileIn.h"
#include "MediaAtomBase.h"

#include "Capture_OpenCV.h"
#include "DeviceManager_V4L2.h"
#include "DeviceManager_Win32.h"
#include "Capture_V4L2.h"
#include "Capture_DShow.h"


#endif
