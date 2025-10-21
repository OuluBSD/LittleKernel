#include "Persona.h"

NAMESPACE_UPP

BiographyPerspectives* Profile::FindSnapshotRevision(int i) {
	TODO
	#if 0
	for (auto& snap : snapshots)
		if (snap.revision == i)
			return &snap;
	#endif
	return 0;
}





INITIALIZER_COMPONENT(Profile, "ecs.public.profile", "Ecs|Public")

END_UPP_NAMESPACE
