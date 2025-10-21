#include "Social.h"

NAMESPACE_UPP

#define DATASET_ITEM(a,b,e) INITIALIZER_COMPONENT(a, "ecs.justice." #b, "Ecs|Justice")
	DATASET_ITEM(Litigation,		litigation,		/*METAKIND_ECS_COMPONENT_LITIGATION,			CATEGORY_PUBLIC,*/ "Litigation")
	DATASET_ITEM(Lawyer,			lawyer,			/*METAKIND_ECS_COMPONENT_LAWYER,				CATEGORY_PUBLIC,*/ "Lawyer")
	DATASET_ITEM(Judge,				judge,			/*METAKIND_ECS_COMPONENT_JUDGE,				CATEGORY_PUBLIC,*/ "Judge")
#undef DATASET_ITEM

END_UPP_NAMESPACE
