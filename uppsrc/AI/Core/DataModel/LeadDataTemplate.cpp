#include "DataModel.h"

NAMESPACE_UPP


INITIALIZER_COMPONENT(LeadDataPublisher, "ecs.marketing.lead.data.publisher", "Ecs|Marketing");

void LeadDataTemplate::Visit(Vis& v) {
	v.Ver(1)
	(1)	("templates", templates, 0)
		("author_classes", author_classes)
		("author_specialities", author_specialities)
		("profit_reasons", profit_reasons)
		("organizational_reasons", organizational_reasons)
		;
}

INITIALIZER_COMPONENT(LeadDataTemplate, "ecs.marketing.lead.data.tmpl", "Ecs|Marketing")

END_UPP_NAMESPACE
