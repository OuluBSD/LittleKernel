#include "Social.h"

NAMESPACE_UPP


ArrayMap<hash_t, SocialNeedsProcess>& __SocialNeedsProcesss() {
	static ArrayMap<hash_t, SocialNeedsProcess> map;
	return map;
}


SocialNeedsProcess::SocialNeedsProcess() {
	
}

SocialNeedsProcess& SocialNeedsProcess::Get(Profile& e, BiographyPerspectives& snap) {
	String t = (String)e.val.GetPath() + ": " + (String)e.val.GetPath();
	hash_t h = t.GetHashValue();
	ArrayMap<hash_t, SocialNeedsProcess>& map = __SocialNeedsProcesss();
	int i = map.Find(h);
	if (i >= 0)
		return map[i];
	
	SocialNeedsProcess& ls = map.Add(h);
	TODO
	#if 0
	ASSERT(e.owner);
	ls.owner = e.owner;
	ls.profile = &e;
	ls.snap = &snap;
	ls.analysis = &snap.analysis;
	ls.biography = &snap.data;
	#endif
	return ls;
}

int SocialNeedsProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int SocialNeedsProcess::GetBatchCount(int phase) const {
	
	if (phase == PHASE_PACK_ROLE_REACTIONS) {
		return analysis->profiles.GetCount();
	}
	else if (phase == PHASE_PACK_PLATFORM_REACTIONS) {
		return PLATFORM_COUNT;
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTIONS) {
		return PLATFORM_COUNT;
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTION_REFINEMENTS) {
		return PLATFORM_COUNT;
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTION_TRANSLATED) {
		return PLATFORM_COUNT;
	}
	return 1;
}

int SocialNeedsProcess::GetSubBatchCount(int phase, int batch) const {
	if (phase == PHASE_PACK_ROLE_REACTIONS) {
		return max(1, ranges.GetCount());
	}
	else if (phase == PHASE_PACK_PLATFORM_REACTIONS) {
		return max(1, ranges.GetCount());
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTION_REFINEMENTS) {
		return PLATDESC_LEN_COUNT;
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTION_TRANSLATED) {
		return PLATDESC_MODE_COUNT * PLATDESC_LEN_COUNT;
	}
	return 1;
}

void SocialNeedsProcess::DoPhase() {
	TODO
	#if 0
	MetaDatabase& db = MetaDatabase::Single();
	LeadData& sd = db.lead_data;
	
	//sa = &sda.GetLeadEntityAnalysis(owner->name);
	int lng_i = db.GetLanguageIndex();

	
	if (phase == PHASE_PACK_ROLE_REACTIONS) {
		ProcessRoleReactions();
	}
	else if (phase == PHASE_PACK_PLATFORM_REACTIONS) {
		ProcessPlatformReactions();
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTIONS) {
		ProcessPlatformDescriptions();
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTION_REFINEMENTS) {
		ProcessPlatformDescriptionRefinements();
	}
	else if (phase == PHASE_PLATFORM_DESCRIPTION_TRANSLATED) {
		ProcessPlatformDescriptionTranslated();
	}
	#endif
}

void SocialNeedsProcess::OnBatchError() {
	LOG("error: OnBatchError");
	Sleep(2000);
	SetWaiting(0);
	NextBatch();
}

void SocialNeedsProcess::ProcessRoleReactions() {
	Biography& biography = *this->biography;
	BiographyPlatform& analysis = *this->analysis;
	int role_i = batch;
	
	if (role_i >= analysis.profiles.GetCount()) {
		NextPhase();
		return;
	}
	
	if (analysis.GetRequiredRoles().Find(role_i) < 0) {
		NextBatch();
		return;
	}
	
	if (role_i >= analysis.roles.GetCount())
		analysis.roles.SetCount(role_i+1);
	BiographyRoleAnalysis& role = analysis.roles[role_i];
	const auto& profs = GetRoleProfile(role_i);
	auto& profiles = analysis.profiles[role_i];
	int c = min(profiles.GetCount(), profs.GetCount());
	
	if (sub_batch == 0) {
		ranges.Clear();
		CreateRange(0, c);
		//DUMPC(ranges);
	}
	if (sub_batch >= ranges.GetCount()) {
		NextBatch();
		return;
	}
	if (skip_ready && role.merged_biography_reactions.GetCount() && role.merged_biography_reactions[0].GetCount()) {
		NextBatch();
		return;
	}
	int range_i = ranges.GetCount() - 1 - sub_batch;
	
	if (role.merged_biography_reactions.GetCount() != ranges.GetCount())
		role.merged_biography_reactions.Clear();
	role.merged_biography_reactions.SetCount(ranges.GetCount());
	const String& range_output = role.merged_biography_reactions[range_i];
	
	if (skip_ready && range_output.GetCount()) {
		NextSubBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 4;
	Range& range = ranges[range_i];
	if (range.input[0] >= 0) {
		const String& r0 = role.merged_biography_reactions[range.input[0]];
		args.parts.Add("<merged reaction #1>", r0);
	}
	else {
		int begin = range.off;
		int len_2 = range.len / 2;
		int end = begin + len_2;
		for(int i = begin; i < end; i++) {
			BiographyProfileAnalysis& pa = profiles[i];
			args.parts.Add(Capitalize(profs[i].name), pa.biography_reaction);
		}
	}
	
	if (range.input[1] >= 0) {
		const String& r0 = role.merged_biography_reactions[range.input[1]];
		args.parts.Add("<merged reaction #2>", r0);
	}
	else {
		int len_2 = range.len / 2;
		int begin = range.off + len_2;
		int end = begin + range.len - len_2;
		for(int i = begin; i < end; i++) {
			BiographyProfileAnalysis& pa = profiles[i];
			args.parts.Add(Capitalize(profs[i].name), pa.biography_reaction);
		}
	}
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessRoleReactions));
}

void SocialNeedsProcess::OnProcessRoleReactions(String res) {
	Biography& biography = *this->biography;
	BiographyPlatform& analysis = *this->analysis;
	int role_i = batch;
	int range_i = ranges.GetCount() - 1 - sub_batch;
	
	if (role_i >= analysis.roles.GetCount())
		analysis.roles.SetCount(role_i+1);
	BiographyRoleAnalysis& role = analysis.roles[role_i];
	String& s = role.merged_biography_reactions[range_i];
	
	s = TrimBoth(res);
	if (s.Left(1) == "\"") s = s.Mid(1);
	if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
	
	NextSubBatch();
	SetWaiting(0);
}

int SocialNeedsProcess::CreateRange(int off, int len) {
	if (len <= 1) return -1;
	int pos = ranges.GetCount();
	Range& range = ranges.Add();
	range.off = off;
	range.len = len;
	int len_2 = len / 2;
	if (len_2 == 0) return pos;
	ranges[pos].input[1] = CreateRange(off + len_2, len - len_2);
	ranges[pos].input[0] = CreateRange(off + 0, len_2);
	return pos;
}

void SocialNeedsProcess::ProcessPlatformReactions() {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	
	if (plat_i >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	
	TODO
	#if 0
	const Platform& plat = GetPlatforms()[plat_i];
	const PlatformAnalysis& pa = MetaDatabase::Single().GetAdd(plat);
	ASSERT(pa.roles.GetCount());
	
	if (sub_batch == 0) {
		ranges.Clear();
		CreateRange(0, pa.roles.GetCount());
		//DUMPC(ranges);
	}
	if (sub_batch >= ranges.GetCount()) {
		NextBatch();
		return;
	}
	int range_i = ranges.GetCount() - 1 - sub_batch;
	
	if (plat_i >= analysis.platforms.GetCount())
		analysis.platforms.SetCount(plat_i+1);
	
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	if (plat_anal.packed_reactions.GetCount() != ranges.GetCount())
		plat_anal.packed_reactions.Clear();
	plat_anal.packed_reactions.SetCount(ranges.GetCount());
	const String& range_output = plat_anal.packed_reactions[range_i];
	
	if (skip_ready && range_output.GetCount()) {
		NextSubBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 5;
	Range& range = ranges[range_i];
	if (range.input[0] >= 0) {
		const String& r0 = plat_anal.packed_reactions[range.input[0]];
		args.parts.Add("<merged reaction #1>", r0);
	}
	else {
		int begin = range.off;
		int len_2 = range.len / 2;
		int end = begin + len_2;
		for(int i = begin; i < end; i++) {
			int role_i = pa.roles[i];
			const BiographyRoleAnalysis& role = analysis.roles[role_i];
			args.parts.Add(Capitalize(GetSocietyRoleDescription(role_i)), role.merged_biography_reactions[0]);
		}
	}
	
	if (range.input[1] >= 0) {
		const String& r0 = plat_anal.packed_reactions[range.input[1]];
		args.parts.Add("<merged reaction #2>", r0);
	}
	else {
		int len_2 = range.len / 2;
		int begin = range.off + len_2;
		int end = begin + range.len - len_2;
		for(int i = begin; i < end; i++) {
			int role_i = pa.roles[i];
			const BiographyRoleAnalysis& role = analysis.roles[role_i];
			args.parts.Add(Capitalize(GetSocietyRoleDescription(role_i)), role.merged_biography_reactions[0]);
		}
	}
	
	SetWaiting(1);
	AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessPlatformReactions));
	#endif
}

void SocialNeedsProcess::OnProcessPlatformReactions(String res) {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	int range_i = ranges.GetCount() - 1 - sub_batch;
	if (plat_i >= analysis.platforms.GetCount())
		analysis.platforms.SetCount(plat_i+1);
	
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	String& s = plat_anal.packed_reactions[range_i];
	
	s = TrimBoth(res);
	if (s.Left(1) == "\"") s = s.Mid(1);
	if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
	
	NextSubBatch();
	SetWaiting(0);
}

void SocialNeedsProcess::ProcessPlatformDescriptions() {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	
	if (plat_i >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	
	TODO
	#if 0
	const Platform& plat = GetPlatforms()[plat_i];
	
	if (plat_i >= analysis.platforms.GetCount())
		analysis.platforms.SetCount(plat_i+1);
	
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	if (skip_ready && plat_anal.profile_description_from_biography.GetCount()) {
		NextBatch();
		return;
	}
	
	if (plat_anal.packed_reactions.IsEmpty()) {
		LOG("error: plat_anal.packed_reactions is empty");
		NextBatch();
		return;
	}
	
	const String& merged_reactions = plat_anal.packed_reactions[0];
	SocialArgs args;
	args.fn = 6;
	args.parts.Add("", merged_reactions);
	
	SetWaiting(1);
	AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessPlatformDescriptions));
	#endif
}

void SocialNeedsProcess::OnProcessPlatformDescriptions(String res) {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	if (plat_i >= analysis.platforms.GetCount())
		analysis.platforms.SetCount(plat_i+1);
	
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	
	String& s = plat_anal.profile_description_from_biography;
	s = TrimBoth(res);
	if (s.Left(1) == "\"") s = s.Mid(1);
	if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
	
	NextBatch();
	SetWaiting(0);
}

void SocialNeedsProcess::ProcessPlatformDescriptionRefinements() {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	
	if (plat_i >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	
	if (sub_batch >= PLATDESC_LEN_COUNT) {
		NextBatch();
		return;
	}
	int len_i = sub_batch;
	int mode_i = PLATDESC_MODE_FINAL;
	
	TODO
	#if 0
	const Platform& plat = GetPlatforms()[plat_i];
	
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	String s = plat_anal.descriptions[len_i][mode_i];
	if (skip_ready && s.GetCount()) {
		NextSubBatch();
		return;
	}
	
	String source;
	if(len_i == 0) {
		source = plat_anal.profile_description_from_biography;
	}
	else {
		source = plat_anal.descriptions[len_i-1][PLATDESC_MODE_FINAL];
	}
	
	const String& merged_reactions = plat_anal.packed_reactions[0];
	SocialArgs args;
	args.fn = len_i == 0 ? 7 : 8;
	args.text = source;
	args.len = GetPlatformDescriptionLength(len_i);
	
	SetWaiting(1);
	AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessPlatformDescriptionRefinements));
	#endif
}

void SocialNeedsProcess::OnProcessPlatformDescriptionRefinements(String res) {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	TODO
	#if 0
	const Platform& plat = GetPlatforms()[plat_i];
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	int len_i = sub_batch;
	int mode_i = PLATDESC_MODE_FINAL;
	String& s = plat_anal.descriptions[len_i][mode_i];
	
	s = TrimBoth(res);
	if (s.Left(1) == "\"") s = s.Mid(1);
	if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
	
	NextSubBatch();
	SetWaiting(0);
	#endif
}

void SocialNeedsProcess::ProcessPlatformDescriptionTranslated() {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	
	if (plat_i >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	int total_subbatches = PLATDESC_MODE_COUNT * PLATDESC_LEN_COUNT;
	if (sub_batch >= total_subbatches) {
		NextBatch();
		return;
	}
	
	int len_i = sub_batch / PLATDESC_MODE_COUNT;
	int mode_i = sub_batch % PLATDESC_MODE_COUNT;
	if (mode_i == PLATDESC_MODE_FINAL) {
		NextSubBatch();
		return;
	}
	
	TODO
	#if 0
	const Platform& plat = GetPlatforms()[plat_i];
	
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	String s = plat_anal.descriptions[len_i][mode_i];
	String source = plat_anal.descriptions[len_i][PLATDESC_MODE_FINAL];
	String dst_ln = mode_i == PLATDESC_MODE_FINAL_DIALECT ? "EN-EN" : "FI-FI";
	bool slightly_dialect = (mode_i == PLATDESC_MODE_FINAL_DIALECT || mode_i == PLATDESC_MODE_FINAL_TRANSLATED_DIALECT);
	
	if (skip_ready && s.GetCount()) {
		NextSubBatch();
		return;
	}
	
	const String& merged_reactions = plat_anal.packed_reactions[0];
	
	SetWaiting(1);
	AiTaskManager();
	m.Translate("EN-EN", source, dst_ln, THISBACK(OnProcessPlatformDescriptionTranslated), slightly_dialect);
	#endif
}

void SocialNeedsProcess::OnProcessPlatformDescriptionTranslated(String res) {
	BiographyPlatform& analysis = *this->analysis;
	int plat_i = batch;
	TODO
	#if 0
	const Platform& plat = GetPlatforms()[plat_i];
	PlatformBiographyPlatform& plat_anal = analysis.platforms[plat_i];
	int len_i = sub_batch / PLATDESC_MODE_COUNT;
	int mode_i = sub_batch % PLATDESC_MODE_COUNT;
	
	String& s = plat_anal.descriptions[len_i][mode_i];
	s = TrimBoth(res);
	if (s.Left(1) == "\"") s = s.Mid(1);
	if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
	
	NextSubBatch();
	SetWaiting(0);
	#endif
}


END_UPP_NAMESPACE
