#include "enet.h"

NAMESPACE_UPP


INITBLOCK {
	DaemonBase::Register<EnetServiceServer>("EnetServer");
	DaemonBase::Register<EnetServiceClient>("EnetClient");
}


END_UPP_NAMESPACE
