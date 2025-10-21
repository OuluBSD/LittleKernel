#include "Content.h"

NAMESPACE_UPP


MergeProcess::MergeProcess() {
	
}

int MergeProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int MergeProcess::GetBatchCount(int phase) const {
	ASSERT(p.src);
	auto& src = p.src->Data();
	if (phase == PHASE_TRANSFER_SCRIPTS)
		return src.authors.GetCount();
	return 1;
}

int MergeProcess::GetSubBatchCount(int phase, int batch) const {
	ASSERT(p.src);
	auto& src = p.src->Data();
	if (phase == PHASE_TRANSFER_SCRIPTS) {
		if (batch >= 0 && batch < src.authors.GetCount())
			return src.authors[batch].scripts.GetCount();
	}
	return 1;
}

void MergeProcess::DoPhase() {
	ASSERT(p.src);
	const auto& src = p.src->Data();
	
	if (phase == PHASE_RESET) {
		el_transfer.Clear();
		target.Clear();
		target.Create();
		
		ASSERT(language_str.GetCount());
		target->langwords.Add(language_str); // always first?
		this->current_language = target->langwords.Find(language_str); // non-optimized
		
		this->current_ctx = ContextType::GetFromString(context_str);
		if (this->current_ctx.value == 0)
			TODO;
		
		NextPhase();
	}
	else if (phase == PHASE_LOAD) {
		LoadForAppending();
	}
	else if (phase == PHASE_TRANSFER_SCRIPTS) {
		TransferScripts();
	}
	else if (phase == PHASE_TRANSFER_OPRHANED_SCRIPTS) {
		TransferOrphanedScripts();
	}
	else if (phase == PHASE_TRANSFER_AMBIGUOUS_WORDS) {
		TransferAmbiguous();
	}
	else if (phase == PHASE_TRANSFER_CONTEXT) {
		TransferContext();
	}
	else if (phase == PHASE_TRANSFER_PHRASE_PARTS) {
		TransferPhraseParts();
	}
	else if (phase == PHASE_TRANSFER_WORDNETS) {
		TransferWordnets();
	}
	else if (phase == PHASE_TRANSFER_COUNT) {
		CountValues();
	}
	else if (phase == PHASE_WRITE) {
		Write();
	}
	else if (phase == PHASE_TRANSFER_ACTION_PHRASES) {
		TransferActionPhrases();
	}
	else if (phase == PHASE_TRANSFER_ACTION_TRANSITION) {
		TransferActionTransitions();
	}
	else {
		NextPhase();
	}
}

void MergeProcess::TransferScripts() {
	ASSERT(p.src);
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	batch_err.Clear();
	if (batch >= d0.authors.GetCount()) {
		NextPhase();
		return;
	}
	
	const auto& author0 = d0.authors[batch];
	if (sub_batch >= author0.scripts.GetCount()) {
		NextBatch();
		return;
	}
	const auto& script0 = author0.scripts[sub_batch];
	
	String script_title = author0.name + " - " + script0.title;
	hash_t ss_hash = script_title.GetHashValue();
	int ss_i0 = d0.scripts.Find(ss_hash);
	if (ss_i0 < 0) {
		NextSubBatch();
		return;
	}
	
	auto& author1 = d1.GetAddAuthor(author0.name);
	if (author1.scripts.IsEmpty())
		author1.ctx = current_ctx;
	else if (author1.ctx.value != 0)
		author1.ctx.value = 0; // reset context if author has multiple contexts
	
	auto& script1 = author1.GetAddScript(script0.title);
	script1.text = script0.text;
	script1.ctx = current_ctx;
	
	int ss_i1 = -1;
	const ScriptStruct& ss0 = d0.scripts[ss_i0];
	ScriptStruct& ss1 = d1.scripts.GetAddPos(ss_hash, ss_i1);
	ss1.author = author0.name;
	ss1.title = script0.title;
	ss1.ctx = current_ctx;
	
	TransferScript(ss0, ss1);
}

void MergeProcess::TransferOrphanedScripts() {
	ASSERT(p.src);
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	if (batch >= d0.scripts.GetCount()) {
		NextPhase();
		return;
	}
	
	hash_t ss_hash = d0.scripts.GetKey(batch);
	if (d1.scripts.Find(ss_hash) < 0) {
		const ScriptStruct& ss0 = d0.scripts[batch];
		int ss_i1 = -1;
		ScriptStruct& ss1 = d1.scripts.GetAddPos(ss_hash, ss_i1);
		TransferScript(ss0, ss1);
	}
	
	NextBatch();
}

void MergeProcess::TransferScript(const ScriptStruct& ss0, ScriptStruct& ss1) {
	ASSERT(p.src);
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	PROCESS_ASSERT(ss1.parts.IsEmpty());
	
	for (const auto& part0 : ss0.parts) {
		auto& part1 = ss1.parts.Add();
		part1.type = part0.type;
		part1.num = part0.num;
		part1.el_i = TransferElement(part0.el_i);
		part1.typeclass = TransferTypeclass(part0.typeclass);
		part1.content = TransferContent(part0.content);
		
		for (const auto& sub0 : part0.sub) {
			auto& sub1 = part1.sub.Add();
			sub1.el_i = TransferElement(sub0.el_i);
			sub1.repeat = sub0.repeat;
			sub1.repeat_ = sub0.repeat_;
			
			for (const auto& ssub0 : sub0.sub) {
				auto& ssub1 = sub1.sub.Add();
				ssub1.el_i = TransferElement(ssub0.el_i);
				
				ssub1.token_texts.SetCount(ssub0.token_texts.GetCount());
				auto tt_i1 = ssub1.token_texts.Begin();
				for (int tt_i0 : ssub0.token_texts)
					*tt_i1++ = TransferTokenText(tt_i0);
			}
		}
	}
	
	if (batch_err.GetCount()) {
		TODO // Reduce size of vectors to same as in the beginning of this function
		NextSubBatch();
		return;
	}
	
	NextSubBatch();
}

int MergeProcess::TransferElement(int el_i0) {
	if (el_i0 < 0) return -1;
	int i = el_transfer.Find(el_i0);
	if (i >= 0) return el_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	const String& el = d0.element_keys[el_i0];
	int el_i1 = d1.element_keys.FindAdd(el);
	el_transfer.Add(el_i0, el_i1);
	
	return el_i1;
}

int MergeProcess::TransferTypeclass(int tc_i0) {
	if (skip_typeclass_content)
		return -1;
	ASSERT(tc_i0 >= -1 && tc_i0 < TYPECAST_COUNT);
	if (tc_i0 >= TYPECAST_COUNT)
		return -1;
	return tc_i0;
}

ContentIdx MergeProcess::TransferContent(ContentIdx con_i0) {
	if (skip_typeclass_content)
		return -1;
	ASSERT(con_i0 >= -1 && con_i0 < CONTENT_COUNT);
	if (con_i0 >= CONTENT_COUNT)
		return -1;
	return con_i0;
}

int MergeProcess::TransferTokenText(int tt_i0) {
	if (tt_i0 < 0) return -1;
	int i = tt_transfer.Find(tt_i0);
	if (i >= 0) return tt_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	int tt_i1 = -1;
	const auto& tt0 = d0.token_texts[tt_i0];
	
	Vector<int> tokens1;
	for (int tk_i0 : tt0.tokens) {
		int tk_i1 = TransferToken(tk_i0);
		tokens1 << tk_i1;
	}
	hash_t key1 = TokenText::GetHash(tokens1);
	auto& tt1 = d1.token_texts.GetAddPos(key1, tt_i1);
	tt_transfer.Add(tt_i0, tt_i1);
	tt1.tokens <<= tokens1;
	
	d1.token_texts[tt_i1].virtual_phrase = TransferVirtualPhrase(tt0.virtual_phrase);
	
	int pp_i0 = tt0.phrase_part;
	// Find PhrasePart the hard way
	if (pp_i0 < 0) {
		CombineHash ch;
		bool fail = false;
		for (int wrd_i0 : tt0.words) {
			ch.Do(wrd_i0).Put(1);
		}
		hash_t pp_hash0 = ch;
		if (fail) {
			pp_i0 = d0.phrase_parts.Find(pp_hash0);
			if (pp_i0 >= 0) {
				LOG("warning: using tainted hash");
			}
		}
		else {
			pp_i0 = d0.phrase_parts.Find(pp_hash0);
		}
	}
	if (pp_i0 >= 0)
		d1.token_texts[tt_i1].phrase_part = TransferPhrasePart(pp_i0, &tt0);
	
	return tt_i1;
}

int MergeProcess::TransferToken(int tk_i0) {
	if (tk_i0 < 0) return -1;
	int i = token_transfer.Find(tk_i0);
	if (i >= 0) return token_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	String key = d0.tokens.GetKey(tk_i0);
	{
		WString wkey = key.ToWString();
		wkey = ToLower(wkey);
		key = wkey.ToString();
	}
	const auto& tk0 = d0.tokens[tk_i0];
	int tk_i1 = -1;
	auto& tk1 = d1.tokens.GetAddPos(key, tk_i1);
	token_transfer.Add(tk_i0, tk_i1);
	
	return tk_i1;
}

int MergeProcess::TransferWord(int wrd_i0) {
	if (wrd_i0 < 0) return -1;
	int i = word_transfer.Find(wrd_i0);
	if (i >= 0) return word_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	const auto& wrd0 = d0.words_[wrd_i0];
	String key = wrd0.text;
	String hash_str = ToLower(key.ToWString()).ToString();
	int wrd_i1 = -1;
	
	auto& lw1 = d1.langwords[current_language];
	hash_t h = hash_str.GetHashValue();
	auto& v = lw1.GetAdd(h);
	for(int i = 0; i < v.GetCount(); i++) {
		if (v.GetKey(i) == wrd0.word_class) {
			wrd_i1 = v[i];
			word_transfer.Add(wrd_i0, wrd_i1);
			return wrd_i1;
		}
	}
	if (v.GetCount()) {
		wrd_i1 = v[0];
		word_transfer.Add(wrd_i0, wrd_i1);
	}
	else {
		int wrd_i1 = d1.words_.GetCount();
		WordData& wrd1 = d1.words_.Add();
		wrd1.clr = wrd0.clr;
		wrd1.count = wrd0.count;
		wrd1.word_class = wrd0.word_class;
		wrd1.lang = wrd0.lang;
		wrd1.text = wrd0.text;
		wrd1.spelling = wrd0.spelling;
		wrd1.phonetic = wrd0.phonetic;
		v.Add(wrd0.word_class, wrd_i1);
	}
	
	return wrd_i1;
}

int MergeProcess::TransferVirtualPhrase(int vp_i0) {
	if (vp_i0 < 0) return -1;
	int i = vp_transfer.Find(vp_i0);
	if (i >= 0) return vp_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	const VirtualPhrase& vp0 = d0.virtual_phrases[vp_i0];
	Vector<int> word_classes;
	for (int wc_i0 : vp0.word_classes) {
		word_classes << TransferWordClass(wc_i0);
	}
	hash_t key1 = VirtualPhrase::GetHash(word_classes);
	int vp_i1 = -1;
	VirtualPhrase& vp1 = d1.virtual_phrases.GetAddPos(key1, vp_i1);
	vp_transfer.Add(vp_i0, vp_i1);
	
	vp1.word_classes <<= word_classes;
	d1.virtual_phrases[vp_i1].virtual_phrase_struct = TransferVirtualPhraseStruct(vp0.virtual_phrase_struct);
	
	return vp_i1;
}

int MergeProcess::TransferVirtualPhraseStruct(int vps_i0) {
	if (vps_i0 < 0) return -1;
	int i = vps_transfer.Find(vps_i0);
	if (i >= 0) return vps_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	const auto& vps0 = d0.virtual_phrase_structs[vps_i0];
	
	Vector<int> vpp_is;
	for (int vpp_i0 : vps0.virtual_phrase_parts) {
		int vpp_i1 = TransferVirtualPhrasePart(vpp_i0);
		vpp_is << vpp_i1;
	}
	hash_t key1 = VirtualPhraseStruct::GetHash(vpp_is);
	int vps_i1 = -1;
	auto& vps1 = d1.virtual_phrase_structs.GetAddPos(key1, vps_i1);
	vps_transfer.Add(vps_i0, vps_i1);
	
	vps1.virtual_phrase_parts <<= vpp_is;
	vps1.struct_type = TransferStructType(vps0.struct_type);
	
	return vps_i1;
}

int MergeProcess::TransferWordClass(int wc_i0) {
	if (wc_i0 < 0) return -1;
	int i = wordclass_transfer.Find(wc_i0);
	if (i >= 0) return wordclass_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	String key = d0.word_classes[wc_i0];
	int wc_i1 = d1.word_classes.FindAdd(key);
	wordclass_transfer.Add(wc_i0, wc_i1);
	return wc_i1;
}

int MergeProcess::TransferStructType(int st_i0) {
	if (st_i0 < 0) return -1;
	int i = structtype_transfer.Find(st_i0);
	if (i >= 0) return structtype_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	String key = d0.struct_types[st_i0];
	int st_i1 = d1.struct_types.FindAdd(key);
	structtype_transfer.Add(st_i0, st_i1);
	return st_i1;
}

int MergeProcess::TransferVirtualPhrasePart(int vpp_i0) {
	if (vpp_i0 < 0) return -1;
	int i = vpp_transfer.Find(vpp_i0);
	if (i >= 0) return vpp_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	const auto& vpp0 = d0.virtual_phrase_parts[vpp_i0];
	Vector<int> wcs;
	for (int wc_i0 : vpp0.word_classes) {
		int wc_i1 = TransferWordClass(wc_i0);
		wcs << wc_i1;
	}
	hash_t key1 = VirtualPhrasePart::GetHash(wcs);
	
	int vpp_i1 = -1;
	auto& vpp1 = d1.virtual_phrase_parts.GetAddPos(key1, vpp_i1);
	vpp_transfer.Add(vpp_i0, vpp_i1);
	
	vpp1.word_classes <<= wcs;
	vpp1.count = 0;
	vpp1.struct_part_type = TransferStructPartType(vpp0.struct_part_type);
	
	return vpp_i1;
}

int MergeProcess::TransferStructPartType(int spt_i0) {
	if (spt_i0 < 0) return -1;
	int i = spt_transfer.Find(spt_i0);
	if (i >= 0) return spt_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	String key = d0.struct_part_types[spt_i0];
	int spt_i1 = d1.struct_part_types.FindAdd(key);
	spt_transfer.Add(spt_i0, spt_i1);
	return spt_i1;
}

int MergeProcess::TransferPhrasePart(int pp_i0, const TokenText* tt0) {
	if (pp_i0 < 0) return -1;
	int i = pp_transfer.Find(pp_i0);
	if (i >= 0) return pp_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	const auto& pp0 = d0.phrase_parts[pp_i0];
	if (tt0 && pp0.words.GetCount() != tt0->tokens.GetCount())
		return -1;
	
	Vector<int> w_is;
	for(int w_i0 : pp0.words)
		w_is << TransferWord(w_i0);
	hash_t key1 = PhrasePart::GetHash(w_is);
	int pp_i1 = d1.phrase_parts.Find(key1);
	if (pp_i1 >= 0) {
		pp_transfer.Add(pp_i0, pp_i1);
		return pp_i1;
	}
	
	auto& pp1 = d1.phrase_parts.GetAddPos(key1, pp_i1);
	pp_transfer.Add(pp_i0, pp_i1);
	
	ASSERT(pp1.words.IsEmpty());
	pp1.clr = pp0.clr;
	pp1.typecasts <<= pp0.typecasts;
	pp1.contrasts <<= pp0.contrasts;
	for(int i = 0; i < SCORE_COUNT; i++)
		pp1.scores[i] = pp0.scores[i];
	pp1.lang = current_language;
	ASSERT(current_language != 0xFF);
	pp1.words <<= w_is;
	pp1.ctx = this->current_ctx.value;
	
	d1.phrase_parts[pp_i1].tt_i = TransferTokenText(pp0.tt_i);
	d1.phrase_parts[pp_i1].virtual_phrase_part = TransferVirtualPhrasePart(pp0.virtual_phrase_part);
	d1.phrase_parts[pp_i1].attr = TransferAttribute(pp0.attr);
	d1.phrase_parts[pp_i1].el_i = TransferElement(pp0.el_i);
	
	Vector<int> acts;
	for(int act_i0 : pp0.actions)
		acts << TransferAction(act_i0);
	d1.phrase_parts[pp_i1].actions <<= acts;
	
	
	return pp_i1;
}

int MergeProcess::TransferAttribute(int attr_i0) {
	if (attr_i0 < 0) return -1;
	int i = attr_transfer.Find(attr_i0);
	if (i >= 0) return attr_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	const auto& key = d0.attrs.GetKey(attr_i0);
	const auto& attr0 = d0.attrs[attr_i0];
	int attr_i1 = -1;
	auto& attr1 = d1.attrs.GetAddPos(key, attr_i1);
	attr_transfer.Add(attr_i0, attr_i1);
	
	attr1.count = 0;
	attr1.positive = attr0.positive;
	d1.attrs[attr_i1].simple_attr = TransferSimpleAttr(attr0.simple_attr);
	
	return attr_i1;
}

int MergeProcess::TransferSimpleAttr(int sa_i0) {
	if (sa_i0 < 0) return -1;
	int i = sa_transfer.Find(sa_i0);
	if (i >= 0) return sa_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	const auto& key = d0.simple_attrs.GetKey(sa_i0);
	const auto& sa0 = d0.simple_attrs[sa_i0];
	int sa_i1 = -1;
	auto& sa1 = d1.simple_attrs.GetAddPos(key, sa_i1);
	sa_transfer.Add(sa_i0, sa_i1);
	
	d1.simple_attrs[sa_i1].attr_i0 = TransferAttribute(sa0.attr_i0);
	d1.simple_attrs[sa_i1].attr_i1 = TransferAttribute(sa0.attr_i1);
	
	return sa_i1;
}

int MergeProcess::TransferAction(int act_i0) {
	if (act_i0 < 0) return -1;
	int i = act_transfer.Find(act_i0);
	if (i >= 0) return act_transfer[i];
	
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	const auto& key = d0.actions.GetKey(act_i0);
	const auto& act0 = d0.actions[act_i0];
	int act_i1 = -1;
	auto& act1 = d1.actions.GetAddPos(key, act_i1);
	act_transfer.Add(act_i0, act_i1);
	
	act1.count = 0;
	act1.clr = act0.clr;
	d1.actions[act_i1].attr = TransferAttribute(act0.attr);
	
	return act_i1;
}

void MergeProcess::TransferAmbiguous() {
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	for(int i = 0; i < d0.ambiguous_word_pairs.GetCount(); i++) {
		const auto& awp0 = d0.ambiguous_word_pairs[i];
		WordPairType awp1;
		awp1.from = TransferWord(awp0.from);
		if (awp1.from < 0) continue;
		awp1.to = TransferWord(awp0.to);
		if (awp1.to < 0) continue;
		awp1.from_type = TransferWordClass(awp0.from_type);
		if (awp1.from_type < 0) continue;
		awp1.to_type = TransferWordClass(awp0.to_type);
		if (awp1.to_type < 0) continue;
		hash_t key1 = awp1.GetHashValue();
		Swap(awp1, d1.ambiguous_word_pairs.GetAdd(key1));
	}
	
	NextPhase();
}

void MergeProcess::TransferContext() {
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	if (append) {
		skip_typeclass_content = true;
		NextPhase();
		return;
	}
	
	for(int i = 0; i < d0.ctxs.GetCount(); i++) {
		ContextType ct0 = d0.ctxs.GetKey(i);
		const ContextData& cd0 = d0.ctxs[i];
		
		int j = d1.ctxs.Find(ct0);
		if (j >= 0)
			continue;
		
		ContextData& cd1 = d1.ctxs.GetAdd(ct0);
		VisitCopy(cd0, cd1);
	}
	
	NextPhase();
}

void MergeProcess::TransferPhraseParts() {
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	if (pp_transfer.GetCount() == d0.phrase_parts.GetCount()) {
		LOG("MergeProcess::TransferPhraseParts: no orphaned phrase parts");
		NextPhase();
		return;
	}
	
	int orphaned_count = 0;
	for(int i = 0; i < d0.phrase_parts.GetCount(); i++) {
		int j = pp_transfer.Find(i);
		if (j >= 0)
			continue;
		
		TransferPhrasePart(i, 0);
		orphaned_count++;
	}
	double perc = (double)orphaned_count / d0.phrase_parts.GetCount() * 100.0;
	LOG("MergeProcess::TransferPhraseParts: orphaned phrase parts: " << orphaned_count << "/" << d0.phrase_parts.GetCount() << ", " << perc << "%");
	NextPhase();
}

void MergeProcess::TransferWordnets() {
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	for(int i = 0; i < d0.wordnets.GetCount(); i++) {
		const ExportWordnet& wn0 = d0.wordnets[i];
		ASSERT(wn0.word_count >= 0 && wn0.word_count <= ExportWordnet::MAX_WORDS);
		ASSERT(wn0.word_clr_count >= 0 && wn0.word_clr_count <= ExportWordnet::MAX_WORDS);
		
		Vector<int> w_is;
		for(int j = 0; j < wn0.word_count; j++)
			w_is << TransferWord(wn0.words[j]);
		Sort(w_is, StdLess<int>());
		hash_t key1 = ExportWordnet::GetHash(w_is);
		
		if (d1.wordnets.Find(key1) >= 0)
			continue;
		ExportWordnet& wn1 = d1.wordnets.Add(key1);
		memset(wn1.words, 0xFF, sizeof(wn1.words));
		memset(wn1.word_clrs, 0, sizeof(wn1.word_clrs));
		memset(wn1.scores, 0, sizeof(wn1.scores));
		
		wn1.word_count = w_is.GetCount();
		for(int j = 0; j < w_is.GetCount(); j++)
			wn1.words[j] = w_is[j];
		
		wn1.word_clr_count = wn0.word_clr_count;
		for(int j = 0; j < wn0.word_clr_count; j++)
			wn1.word_clrs[j] = wn0.word_clrs[j];
		
		wn1.main_class = TransferWordClass(wn0.main_class);
		wn1.attr = TransferAttribute(wn0.attr);
		wn1.clr = wn0.clr;
		for(int j = 0; j < SCORE_COUNT; j++)
			wn1.scores[j]= wn0.scores[j];
	}
	
	NextPhase();
}

void MergeProcess::CountValues() {
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	// Zero
	for (auto it : d1.words_)
		it.count = 0;
	for (auto it : ~d1.virtual_phrase_parts)
		it.value.count = 0;
	
	for (auto tt_it : ~d1.token_texts)
		for (int w_i : tt_it.value.words)
			d1.words_[w_i].count++;
	for (auto it : ~d1.virtual_phrase_structs)
		for (auto vpp_i : it.value.virtual_phrase_parts)
			if (vpp_i >= 0)
				d1.virtual_phrase_parts[vpp_i].count++;
	
	
	/// COMPARE
	#define CMP2(a,b) LOG("MergeProcess::CountValues: compare " #a ": " << d0.a.GetCount() << " vs. " << d1.b.GetCount() << ": " << (double)d0.a.GetCount() / (double)d1.b.GetCount());
	#define CMP(x) CMP2(x,x)
	CMP(authors)
	CMP(scripts)
	CMP(tokens)
	CMP(word_classes)
	CMP(words_)
	CMP(ambiguous_word_pairs)
	CMP(token_texts)
	CMP(virtual_phrases)
	CMP(virtual_phrase_parts)
	CMP(virtual_phrase_structs)
	CMP(phrase_parts)
	CMP(struct_part_types)
	CMP(struct_types)
	CMP(simple_attrs)
	CMP(element_keys)
	CMP(attrs)
	CMP(actions)
	CMP(wordnets)
	#undef CMP
	#undef CMP2
	
	NextPhase();
}

void MergeProcess::TransferActionPhrases() {
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	d1.action_phrases.Reserve(d0.action_phrases.GetCount());
	
	for(auto it : ~d0.action_phrases) {
		int i = d1.action_phrases.Find(it.key);
		if (i >= 0)
			continue;
		const auto& dae0 = it.value;
		auto& dae1 = d1.action_phrases.Add(it.key);
		
		dae1.actions.SetCount(dae0.actions.GetCount());
		for(int i = 0; i < dae0.actions.GetCount(); i++)
			dae1.actions[i] = TransferAction(dae0.actions[i]);
		
		dae1.next_phrases.SetCount(dae0.next_phrases.GetCount());
		for(int i = 0; i < dae0.next_phrases.GetCount(); i++)
			dae1.next_phrases[i] = TransferPhrasePart(dae0.next_phrases[i], 0);
		
		dae1.next_scores <<= dae0.next_scores;
		dae1.first_lines = dae0.first_lines;
		dae1.attr = TransferAttribute(dae0.attr);
		dae1.clr = dae0.clr;
	}
	
	NextPhase();
}

void MergeProcess::TransferActionTransitions() {
	const auto& d0 = p.src->Data();
	auto& d1 = *target;
	
	d1.trans.Reserve(d0.trans.GetCount());
	
	for(auto it : ~d0.trans) {
		const auto& to_map0 = it.value;
		int from_act0 = it.key;
		int from_act1 = TransferAction(from_act0);
		auto& to_map1 = d1.trans.GetAdd(from_act1);
		
		for (auto it_to0 : ~to_map0) {
			int to_act0 = it_to0.key;
			int to_act1 = TransferAction(to_act0);
			
			int i = to_map1.Find(to_act1);
			if (i >= 0)
				continue;
			
			const auto& to0 = it_to0.value;
			auto& to1 = to_map1.Add(to_act1);
			
			to1.count = to0.count;
			to1.score_sum = to0.score_sum;
		}
	}
	
	for(auto it : ~d0.parallel) {
		const auto& to_map0 = it.value;
		int from_act0 = it.key;
		int from_act1 = TransferAction(from_act0);
		auto& to_map1 = d1.parallel.GetAdd(from_act1);
		
		for (auto it_to0 : ~to_map0) {
			int to_act0 = it_to0.key;
			int to_act1 = TransferAction(to_act0);
			
			int i = to_map1.Find(to_act1);
			if (i >= 0)
				continue;
			
			const auto& to0 = it_to0.value;
			auto& to1 = to_map1.Add(to_act1);
			
			to1.count = to0.count;
			to1.score_sum = to0.score_sum;
		}
	}
	
	NextPhase();
}

void MergeProcess::LoadForAppending() {
	if (!append) {
		NextPhase();
		return;
	}
	
	if (!FileExists(path_str)) {
		SetError("file doesn't exist: " + path_str);
		SetNotRunning();
		return;
	}
	auto& d1 = *target;
	Value val = ParseJSON(LoadFile(path_str));
	
	int ver = val("version");
	if (ver != 2) {
		SetError("unexpected version number: " + IntStr(ver));
		SetNotRunning();
		return;
	}
	
	int size = val("size");
	if (size == 0) {
		SetError("size is 0");
		SetNotRunning();
		return;
	}
	String sha1_val = val("sha1");
	if (sha1_val.IsEmpty()) {
		SetError("sha1 is empty");
		SetNotRunning();
		return;
	}
	
	Vector<String> files;
	ValueArray files_val = val("files");
	for(int i = 0; i < files_val.GetCount(); i++)
		files << files_val[i].ToString();
	if (files.IsEmpty()) {
		SetError("files empty");
		SetNotRunning();
		return;
	}
	
	String filepath = path_str;
	String compressed;
	String dir = GetFileDirectory(filepath);
	for(int i = 0; i < files.GetCount(); i++) {
		String path = AppendFileName(dir, files[i]);
		String data = LoadFile(path);
		
		compressed.Cat(data);
		
		int per_file = 1024 * 1024 * 25;
		LOG("SrcTxtHeader::LoadData" << data.GetCount() << " vs expected " << per_file << ": " << (data.GetCount() == per_file ? "True" : "False"));
	}
	String decompressed = BZ2Decompress(compressed);
	if (decompressed.GetCount() != size) {
		SetError("SrcTxtHeader::LoadData: error: size mismatch when loading: " + filepath);
		SetNotRunning();
		return;
	}
	String sha1 = SHA1String(decompressed);
	if (sha1 != sha1_val) {
		SetError("SrcTxtHeader::LoadData: error: sha1 mismatch when loading: " + filepath);
		SetNotRunning();
		return;
	}
	StringStream decomp_stream(decompressed);
	Vis vis(decomp_stream);
	d1.Visit(vis);
	
	#define CMP(x) LOG("MergeProcess::LoadForAppending: " #x ": " << d1.x.GetCount());
	CMP(authors)
	CMP(scripts)
	CMP(tokens)
	CMP(word_classes)
	CMP(words_)
	CMP(ambiguous_word_pairs)
	CMP(token_texts)
	CMP(virtual_phrases)
	CMP(virtual_phrase_parts)
	CMP(virtual_phrase_structs)
	CMP(phrase_parts)
	CMP(struct_part_types)
	CMP(struct_types)
	CMP(simple_attrs)
	CMP(element_keys)
	CMP(attrs)
	CMP(actions)
	CMP(wordnets)
	#undef CMP
	
	NextPhase();
}

void MergeProcess::Write() {
	auto& d1 = *target;
	String filepath = path_str;
	
	Value out = ValueMap();
	out("written") = StoreAsJsonValue(GetUtcTime());
	out("version") = 2;
	
	String dir = GetFileDirectory(filepath);
	String filename = GetFileName(filepath);
	StringStream decomp_stream;
	Vis vis(decomp_stream);
	d1.Visit(vis);
	String decompressed = decomp_stream.GetResult();
	
	int per_file = 1024 * 1024 * 25;
	out("sha1") = SHA1String(decompressed);
	out("size") = decompressed.GetCount();
	out("per_file") = per_file;
	
	String compressed = BZ2Compress(decompressed);
	StringStream comp_stream;
	comp_stream % compressed;
	
	ValueArray files;
	int parts = 1 + (compressed.GetCount() + 1) / per_file;
	for(int i = 0; i < parts; i++) {
		int begin = i * per_file;
		int end = min(begin+per_file, compressed.GetCount());
		String part = compressed.Mid(begin,end-begin);
		String part_path = filepath + "." + IntStr(i);
		FileOut fout(part_path);
		fout.Put(part);
		files.Add(filename + "." + IntStr(i));
	}
	for (int i = parts;;) {
		String part_path = filepath + "." + IntStr(i);
		if (FileExists(part_path))
			DeleteFile(part_path);
		else
			break;
	}
	out("files") = files;
	
	String json = AsJSON(out, true);
	LOG(json);
	
	FileOut fout(filepath);
	fout << json;
	
	NextPhase();
}

MergeProcess& MergeProcess::Get(DatasetPtrs p, String path, String language, String ctx, bool append) {
	static ArrayMap<String, MergeProcess> arr;
	ASSERT(p.src);
	language = ToLower(language);
	String key = p.src->filepath;
	ASSERT(key.GetCount());
	auto& ts = arr.GetAdd(key);
	ts.p = pick(p);
	ts.path_str = path;
	ts.language_str = language;
	ts.context_str = ctx;
	ts.append = append;
	return ts;
}


END_UPP_NAMESPACE
