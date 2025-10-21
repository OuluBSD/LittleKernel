#include "Social.h"


NAMESPACE_UPP


void BiographyPlatform::Realize() {
	if (profiles.GetCount() < SOCIETYROLE_COUNT)
		profiles.SetCount(SOCIETYROLE_COUNT);
	for(int i = 0; i < SOCIETYROLE_COUNT; i++) {
		const auto& v = GetRoleProfile(i);
		profiles[i].SetCount(v.GetCount());
	}
	if (platforms.GetCount() < PLATFORM_COUNT)
		platforms.SetCount(PLATFORM_COUNT);
}

void BiographyPlatform::RealizePromptImageTypes() {
	for(int i = 0; i < image_types.GetCount(); i++)
		image_types[i].image_count = 0;
	
	for(int plat_i = 0; plat_i < platforms.GetCount(); plat_i++) {
		const PlatformBiographyPlatform& pba = platforms[plat_i];
		for(int i = 0; i < pba.epk_photos.GetCount(); i++) {
			const PlatformAnalysisPhoto& pap = pba.epk_photos[i];
			String group = ToLower(pba.epk_photos.GetKey(i));
			PhotoPromptGroupAnalysis& ppga = image_types.GetAdd(group);
			ppga.image_count += 1; //pap.prompts.GetCount();
		}
	}
}

Index<int> BiographyPlatform::GetRequiredRoles() const {
	Index<int> ret;
	DatasetPtrs p;
	GetDataset(p);
	if (!p.platmgr)
		return ret;
	for(int role_i0 = 0; role_i0 < SOCIETYROLE_COUNT; role_i0++) {
		bool enabled = false;
		
		for(int i = 0; i < platforms.GetCount(); i++) {
			const PlatformBiographyPlatform& pla = platforms[i];
			if (!pla.platform_enabled)
				continue;
			
			const Platform& plat = GetPlatforms()[i];
			const PlatformAnalysis& pa = p.platmgr->GetPlatform(i);
			for (int role_i1 : pa.roles)
				if (role_i0 == role_i1)
					{enabled = true; break;}
			if (enabled) break;
		}
		
		if (enabled)
			ret.FindAdd(role_i0);
	}
	return ret;
}

Index<int> BiographyPlatform::GetRequiredCategories() const {
	Index<int> roles = GetRequiredRoles();
	Index<int> cats;
	for(int role_i : roles) {
		bool enabled = false;
		for (const auto& plat_roles : profiles) {
			for (const auto& plat_profs : plat_roles) {
				for (int cat_i : plat_profs.categories.GetKeys())
					cats.FindAdd(cat_i);
			}
		}
	}
	return cats;
}

Vector<PhotoPromptLink> BiographyPlatform::GetImageTypePrompts(String image_type) {
	Vector<PhotoPromptLink> o;
	image_type = ToLower(image_type);
	for(int plat_i = 0; plat_i < platforms.GetCount(); plat_i++) {
		PlatformBiographyPlatform& pba = platforms[plat_i];
		int i = -1;
		for(int j = 0; j < pba.epk_photos.GetCount(); j++) {
			if (ToLower(pba.epk_photos.GetKey(j)) == image_type) {
				i = j;
				break;
			}
		}
		if (i >= 0) {
			PlatformAnalysisPhoto& pap = pba.epk_photos[i];
			//for(int j = 0; j < pap.prompts.GetCount(); j++) {
			//	PhotoPrompt& pp = pap.prompts[j];
			if (pap.prompts.GetCount()) {
				PhotoPrompt& pp = pap.prompts[0];
				PhotoPromptLink& ppl = o.Add();
				ppl.pap = &pap;
				ppl.pba = &pba;
				ppl.pp = &pp;
			}
		}
	}
	return o;
}

const VectorMap<String, Vector<String>>& GetMarketplaceSections() {
	static VectorMap<String, Vector<String>> m;
	if (!m.IsEmpty()) return m;
	{
		auto& v = m.Add("Labor");
		v.Add("Skilled");
		v.Add("Unskilled");
	}
	return m;
}

INITIALIZER_COMPONENT(BiographyPlatform, "ecs.private.biography.platform", "Ecs|Private")


END_UPP_NAMESPACE
