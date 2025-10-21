#ifndef _Eon_Script_Script_h_
#define _Eon_Script_Script_h_

#include <Vfs/Vfs.h>
#include <Vfs/Ecs/Ecs.h>
#include <Core/EcsEngine/EcsEngine.h>
#include <Eon/Core/Core.h>

NAMESPACE_UPP

// Script DSL model + loaders
#include "Def.h"
#include "Loader.h"

// Convenience: keep headers discoverable via umbrella
#include "ToyLoader.h"
#include "ShadertoyLoader.h"
#include "EcsLoader.h"
#include "EonLoader.h"
#include "Builder.h"

END_UPP_NAMESPACE

#endif

