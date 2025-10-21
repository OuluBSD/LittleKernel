#include "Core.h"
#include <AI/Core/DataModel/DataModel.h>

NAMESPACE_UPP


void FillDataset(DatasetPtrs& p, VfsValue& n, Component* this_comp) {
	p.Clear();
	p.component = this_comp;
	//if (n.kind >= METAKIND_ECS_COMPONENT_BEGIN && n.kind <= METAKIND_ECS_COMPONENT_END) {
	if (n.owner) {
		if (n.owner->IsTypeHash<Entity>() && n.owner->ext)
			p.entity = dynamic_cast<Entity*>(&*n.owner->ext);
		for (auto& sub : n.owner->sub) {
			if (!sub.ext) continue;
			VfsValueExt* ext = &*sub.ext;
			VfsValueExtFactory::SetDatasetPtrs(p, *ext);
		}
		if (this_comp) {
			VfsValueExtFactory::SetDatasetPtrs(p, *this_comp);
		}
	}
	
	
// see SRC_TXT_HEADER_ENABLE
	VfsValue* db_src = 0;
	if (n.IsTypeHash<SrcTxtHeader>()) {
		db_src = &n;
	}
	
	if (p.entity && FindNodeEnvPtr) {
		p.env = FindNodeEnvPtr(*p.entity);
		if (p.env && !db_src) {
			hash_t cmp = AsTypeHash<SrcTxtHeader>();
			for (VfsValue& s : p.env->sub) {
				if (s.type_hash == cmp) {
					db_src = &s;
					break;
				}
			}
			if (!db_src && IdeVfsFillDatasetPtrsPtr) {
				db_src = IdeVfsFillDatasetPtrsPtr(p, AsTypeHash<SrcTxtHeader>());
			}
		}
	}
	
	if (db_src) {
		if (db_src->ext) {
			p.src = dynamic_cast<SrcTxtHeader*>(&*db_src->ext);
			ASSERT(p.src);
			p.src->RealizeData();
			db_src = &n;
			
			// UGLY, so UGLY, just filling values randomly kind of ugly, but it's correct
			// This ugliness is related to the SRC_TXT_HEADER_ENABLE hotfix...
			// Generalize the hotfix or remove it... (don't remove it, it's a good one).
			if (p.src->data)
				p.srctxt = &*p.src->data;
		}
	}
}

extern void (*FillDatasetPtr)(DatasetPtrs& p, VfsValue& n, Component* this_comp);



INITBLOCK {
	FillDatasetPtr = &FillDataset;
}

END_UPP_NAMESPACE
