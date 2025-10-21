#ifndef _GuboCore_CtrlEvent_h_
#define _GuboCore_CtrlEvent_h_

#include <Core2/GeomEvent.h>

NAMESPACE_UPP

// Alias Core2 GeomEvent as CtrlEvent so GuboCore and Ctrl-facing code
// can share a common event structure without duplication.
using CtrlEvent = GeomEvent;

END_UPP_NAMESPACE

#endif

