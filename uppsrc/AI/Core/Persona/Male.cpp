#include "Persona.h"


NAMESPACE_UPP


#define DATASET_ITEM(a,b,e) INITIALIZER_COMPONENT(a, "ecs.male." #b, "Ecs|Male")
	DATASET_ITEM(Provider,			provider,		/*METAKIND_ECS_COMPONENT_PROVIDER,			CATEGORY_MALE,*/ "Provider")
	DATASET_ITEM(DecisionMaker,		decision_maker,	/*METAKIND_ECS_COMPONENT_DECISION_MAKER,		CATEGORY_MALE,*/ "Decision Maker")
	DATASET_ITEM(Assertions,		assertions	,	/*METAKIND_ECS_COMPONENT_ASSERTIONS,			CATEGORY_MALE,*/ "Assertions")
	DATASET_ITEM(Aggressions,		aggressions,	/*METAKIND_ECS_COMPONENT_AGGRESSIONS,			CATEGORY_MALE,*/ "Aggressions")
	DATASET_ITEM(Strength,			strength,		/*METAKIND_ECS_COMPONENT_STRENGTH,			CATEGORY_MALE,*/ "Strength")
	DATASET_ITEM(Leadership,		leadership,		/*METAKIND_ECS_COMPONENT_LEADERSHIP,			CATEGORY_MALE,*/ "Leadership")
	DATASET_ITEM(Justice,			justice,		/*METAKIND_ECS_COMPONENT_JUSTICE,				CATEGORY_MALE,*/ "Justice")
#undef DATASET_ITEM

END_UPP_NAMESPACE
