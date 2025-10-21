#ifndef _Shell_Shell_h_
#define _Shell_Shell_h_


#include <AI/Core/Base/Base.h>
#include <Geometry/Geometry.h>
#include <Vfs/Runtime/Runtime.h>
#include <Eon/Eon.h>
#include <Eon/Draw/Draw.h>
#include <Eon/Lib/Lib.h>
#include <api/Graphics/Graphics.h>


#ifdef flagGUI
#include <Eon/Ctrl/EonCtrl.h>

#if defined flagUWP && defined flagDX12
	#include <EcsWin/EcsWin.h>
#endif
#endif

#ifdef flagGUI
	#include <GuboLib/GuboLib.h>
#endif

#ifdef flagGUI
	#include <EcsCtrl/EcsCtrl.h>
#endif

#ifdef flagVR
	#include <EcsVR/EcsVR.h>
#endif

#ifdef flagGUBO
	#include <GuboSuite/GuboSuite.h>
#endif



NAMESPACE_UPP

void MachineEcsInit(Engine& mach);
void EngineEcsInit(Engine& eng);
void BindEcsToSerial();
void DebugMain(String script_content, String eon_file, VectorMap<String,Value>& args, MachineVerifier* ver=0, bool dbg_ref_visits=false, uint64 dbg_ref=0);

template <class T> void DefaultCreate() {TODO /*GetActiveEngine().Get<EntityStore>()->GetRoot()->Create<T>();*/}
template <class T> void DefaultCreateOnStart() {TODO /*Engine::WhenPreFirstUpdate << callback(DefaultCreate<T>);*/}

END_UPP_NAMESPACE

Upp::Engine& ShellMainEngine();
void ShellMain(bool skip_eon);

#endif


