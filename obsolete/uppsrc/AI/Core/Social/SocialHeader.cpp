#include "Social.h"

NAMESPACE_UPP


SocialHeaderProcess::SocialHeaderProcess() {
	
}

int SocialHeaderProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int SocialHeaderProcess::GetBatchCount(int phase) const {
	switch (phase) {
		case PHASE_AUDIENCE_REACTS_SUMMARY:			return max(1, ptrs.GetCount());
		default: return 1;
	}
}

int SocialHeaderProcess::GetSubBatchCount(int phase, int batch) const {
	switch (phase) {
		case PHASE_AUDIENCE_REACTS_SUMMARY:			return 1;
		default: return 1;
	}
}

void SocialHeaderProcess::DoPhase() {
	switch (phase) {
		case PHASE_AUDIENCE_REACTS_SUMMARY:			ProcessAudienceReactsSummary();
		default: return;
	}
}

SocialHeaderProcess& SocialHeaderProcess::Get(DatasetPtrs p) {
	ASSERT(p.owner && p.profile && p.snap && p.biography && p.analysis);
	static ArrayMap<String, SocialHeaderProcess> arr;
	
	String key = (String)p.profile->val.GetPath() + ";" + (String)p.snap->val.GetPath();
	SocialHeaderProcess& ts = arr.GetAdd(key);
	ts.p = p;
	return ts;
}

void SocialHeaderProcess::ProcessAudienceReactsSummary() {
	BiographyPlatform& analysis = *p.analysis;
	Biography& biography = *p.biography;
	
	if (batch == 0 && sub_batch == 0) {
		analysis.Realize();
		ptrs.Clear();
		role_descs.Clear();
		prof_ptrs.Clear();
		for(int i = 0; i < analysis.profiles.GetCount(); i++) {
			const auto& profs = GetRoleProfile(i);
			auto& profiles = analysis.profiles[i];
			int c = min(profiles.GetCount(), profs.GetCount());
			for(int j = 0; j < c; j++) {
				ptrs << &profiles[j];
				prof_ptrs << &profs[j];
				role_descs << GetSocietyRoleDescription(i);
			}
		}
	}
	
	if (batch >= ptrs.GetCount()) {
		NextPhase();
		return;
	}
	
	BiographyProfileAnalysis& pa = *ptrs[batch];
	const RoleProfile& rp = *prof_ptrs[batch];
	if (skip_ready && pa.biography_reaction.GetCount()) {
		NextBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 3;
	args.text = role_descs[batch];
	
	args.parts.Add(rp.name, rp.profile);
	
	int cat_count = min(pa.categories.GetCount(), 10);
	int total_length = 0;
	for(int i = 0; i < cat_count; i++) {
		int priority;
		switch (i) {
			case 0:
				priority = 2; break;
			case 1:
			case 2:
			case 3:
			case 4:
				priority = 1; break;
			default:
				priority = 0; break;
		}
		int bcat_i = pa.categories.GetKey(i);
		BiographyCategory& bcat = biography.GetAdd(*p.owner, bcat_i);
		
		int top_len = 0;
		for(int j = 0; j < bcat.summaries.GetCount(); j++)
			top_len = max(top_len, bcat.summaries.GetKey(j).len);
		
		int tgt_len = max(2, top_len >> priority);
		for(int j = 0; j < bcat.summaries.GetCount(); j++) {
			const auto& key = bcat.summaries.GetKey(j);
			if (key.len == tgt_len) {
				BioYear& by = bcat.summaries[j];
				if (by.text.GetCount()) {
					String title =
						GetBiographyCategoryKey(bcat_i) + ", years " +
						IntStr(key.off) + "-" + IntStr(key.off+key.len-1);
					args.parts.Add(title, by.text + "\n");
					total_length += title.GetCount() + by.text.GetCount() + 2;
					if (total_length >= 10000) break; // limit length because GPT prompt limits
				}
			}
		}
		if (total_length >= 10000) break; // limit length because GPT prompt limits
	}
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessAudienceReactsSummary));
}

void SocialHeaderProcess::OnProcessAudienceReactsSummary(String res) {
	BiographyProfileAnalysis& pa = *ptrs[batch];
	
	pa.biography_reaction = TrimBoth(res);
	
	NextBatch();
	SetWaiting(0);
}


END_UPP_NAMESPACE
