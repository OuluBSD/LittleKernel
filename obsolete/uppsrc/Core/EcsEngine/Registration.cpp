#include "EcsEngine.h"
#include <Vfs/Ecs/Ecs.h>

NAMESPACE_UPP

INITBLOCK {
	TypedStringHasher<Engine>("Engine");
	
	REGISTER_SYSTEM_ATOM(LinkSystem, "sys.link", "Atom|Net")
	REGISTER_SYSTEM_ATOM(PacketTracker, "sys.packet.tracker", "Atom|Net")
	
	#define ECS_CTX(dev, val, prefix) \
		VfsValueExtFactory::RegisterExchange<DefaultExchangePoint>("DefaultExchangePoint", DevCls::dev, ValCls::val);
	#define IFACE_CTX_CLS(dev, value, prefix) ECS_CTX(dev, value, prefix)
	#define IFACE(x) DEV_IFACE(x)
	IFACE_LIST
	#undef IFACE
	#undef IFACE_CTX_CLS
	#undef ECS_CTX

}

END_UPP_NAMESPACE
