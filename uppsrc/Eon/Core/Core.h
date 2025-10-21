#ifndef _Eon_Core_Core_h_
#define _Eon_Core_Core_h_

#include <Core/MediaFormats/MediaFormats.h>
#include <Core/CompatExt/CompatExt.h>
#include <Vfs/Vfs.h>
#include <Vfs/Ecs/Ecs.h>
// too much: #include <AI/Core/Core.h>
#include <Geometry/Geometry.h>

NAMESPACE_UPP

// Core building blocks for atoms/links and helpers
#include "Types.h"
#include "Container.h"
#include "Atom.h"
#include "LinkUtil.h"
#include "LinkBase.h"
#include "Base.h"
#include "BaseAudio.h"
#include "BaseVideo.h"
#include "Network.h"
#include "LinkFactory.h"
#include "Context.h"

END_UPP_NAMESPACE

#endif
