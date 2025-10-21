#include "Marketing.h"


NAMESPACE_UPP


AudienceProcess::AudienceProcess() {
	
}

int AudienceProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int AudienceProcess::GetBatchCount(int phase) const {
	switch (phase) {
		case PHASE_AUDIENCE_PROFILE_CATEGORIES:			return SOCIETYROLE_COUNT;
		default: return 1;
	}
}

int AudienceProcess::GetSubBatchCount(int phase, int batch) const {
	switch (phase) {
		case PHASE_AUDIENCE_PROFILE_CATEGORIES:			return GetRoleProfile(batch).GetCount();
		default: return 1;
	}
}

void AudienceProcess::DoPhase() {
	switch (phase) {
		case PHASE_AUDIENCE_PROFILE_CATEGORIES:			ProcessAudienceProfileCategories();
		default: return;
	}
}

AudienceProcess& AudienceProcess::Get(Profile& p, BiographyPerspectives& snap) {
	static ArrayMap<String, AudienceProcess> arr;
	
	String key = "PROFILE(" + p.name + "), REVISION(" + IntStr(snap.revision) + ")";
	AudienceProcess& ts = arr.GetAdd(key);
	TODO
	#if 0
	ts.owner = p.owner;
	ts.profile = &p;
	ts.snap = &snap;
	ASSERT(ts.owner);
	return ts;
	#endif
	return Single<AudienceProcess>();
}

void AudienceProcess::ProcessAudienceProfileCategories() {
	BiographyPlatform& analysis = *p.analysis;
	int role_i = batch;
	int prof_i = sub_batch;
	
	if (role_i >= SOCIETYROLE_COUNT) {
		NextPhase();
		return;
	}
	
	analysis.Realize();
	if (analysis.GetRequiredRoles().Find(role_i) < 0) {
		NextBatch();
		return;
	}
	
	const Array<RoleProfile>& profs = GetRoleProfile(role_i);
	
	if (prof_i >= profs.GetCount()) {
		NextBatch();
		return;
	}
	const RoleProfile& prof = profs[prof_i];
	
	const BiographyProfileAnalysis& pa = analysis.profiles[role_i][prof_i];
	
	if (skip_ready && pa.categories.GetCount()) {
		NextSubBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 2;
	args.text = GetSocietyRoleDescription(batch);
	args.parts.Add(prof.name, prof.profile);
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessAudienceProfileCategories));
}

void AudienceProcess::OnProcessAudienceProfileCategories(String res) {
	int role_i = batch;
	int prof_i = sub_batch;
	const Array<RoleProfile>& profs = GetRoleProfile(role_i);
	const RoleProfile& prof = profs[prof_i];
	BiographyPlatform& analysis = *p.analysis;
	analysis.Realize();
	BiographyProfileAnalysis& pa = analysis.profiles[role_i][prof_i];
	
	pa.categories.Clear();
	
	res = "1. Category " + res;
	Vector<String> lines = Split(res, "\n");
	if (lines.GetCount() == 20) {
		Vector<int> rm_list;
		for(int i = 0; i < 10; i++) {
			lines[i*2] += lines[i*2+1];
			rm_list << i+1;
		}
		lines.Remove(rm_list);
	}
	for(int i = 0; i < lines.GetCount(); i++) {
		String& l = lines[i];
		RemoveLineNumber(l);
		l = TrimBoth(l);
		if (l.IsEmpty() || l[0] == '-')
			lines.Remove(i--);
		
		int a = l.Find("ategory ");
		if (a < 0) {
			// Category number was not given: try to find the category by comparing text
			if (l.Left(2) == ": ")
				l = l.Mid(2);
			int b = l.Find("-");
			String key = ToLower(TrimBoth(l.Left(b)));
			int key_i = -1;
			for(int j = 0; j < BIOCATEGORY_COUNT; j++) {
				String cmp_key = ToLower(GetBiographyCategoryKey(j));
				if (key == cmp_key) {
					key_i = j;
					break;
				}
			}
			if (key_i >= 0) {
				a = 0;
				l = "ategory " + IntStr(key_i) + ": " + l;
			}
			// Category was not found
			else continue;
		}
		a += 8;
		l = l.Mid(a);
		
		int cat_num = ScanInt(l);
		
		a = l.Find("-");
		if (a >= 0) {
			String desc = TrimBoth(l.Mid(a+1));
			desc.Replace("person #1", "the subject person");
			desc.Replace("Person #1", "The subject person");
			desc.Replace("person #2", "this person");
			desc.Replace("Person #2", "This person");
			pa.categories.Add(cat_num, desc);
		}
		else {
			pa.categories.Add(cat_num);
		}
	}
	
	
	SetWaiting(0);
	NextSubBatch();
}


END_UPP_NAMESPACE
