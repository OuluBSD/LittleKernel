#include "DataModel.h"
#include <AI/Core/Content/Content.h>

NAMESPACE_UPP


int ContextData::FindAddEntityGroup(String s) {
	int i = entity_groups.FindAdd(s);
	for (auto& tc : typeclasses) {
		if (i >= tc.entities.GetCount())
			tc.entities.SetCount(i+1);
	}
	return i;
}

ScriptDataset& AuthorDataset::GetAddScript(String title) {
	for (ScriptDataset& s : scripts)
		if (s.title == title)
			return s;
	ScriptDataset& s = scripts.Add();
	s.title = title;
	return s;
}

// see SRC_TXT_HEADER_ENABLE

//int EditorPtrs::GetActiveEntityIndex() const {return VectorFindPtr(entity, TextDatabase::Single().entities);}
//int EditorPtrs::GetActiveComponentIndex() const {if (!entity || !component) return -1; return VectorFindPtr(component, entity->comps);}
//int EditorPtrs::GetActiveScriptIndex() const {if (!entity || !script) return -1; return VectorFindPtr(static_cast<Component*>(script), entity->comps);}


SrcTextData::SrcTextData() {
	current_language = "english";
}

void SrcTextData::Visit(Vis& s) {
	s.Ver(2);
	if (s.file_ver == 1) {
		Panic("Not loading version 1 anymore. Use commit 00809ced5a09e2293b2a9aa36b54a18251b14fb3");
	}
	else {
		s(2)("authors", authors, VISIT_VECTOR)
			("scripts", scripts, VISIT_MAP)
			("tokens", tokens, VISIT_MAP)
			("word_classes", word_classes)
			("ambiguous_word_pairs", ambiguous_word_pairs, VISIT_MAP)
			("token_texts", token_texts, VISIT_MAP)
			("virtual_phrases", virtual_phrases, VISIT_MAP)
			("virtual_phrase_parts", virtual_phrase_parts, VISIT_MAP)
			("virtual_phrase_structs", virtual_phrase_structs, VISIT_MAP)
			("phrase_parts", phrase_parts, VISIT_MAP)
			("simple_attrs", simple_attrs, VISIT_MAP)
			("element_keys", element_keys)
			("attrs", attrs, VISIT_MAP_KV)
			("actions", actions, VISIT_MAP_KV)
			("wordnets", wordnets, VISIT_MAP)
			("action_phrases", action_phrases, VISIT_MAP)
			("trans", trans, VISIT_MAPMAP)
			("parallel", parallel, VISIT_MAPMAP)
			("diagnostics", diagnostics)
			("ctxs", ctxs, VISIT_MAP_KV)
			("words", words_, VISIT_VECTOR)
			("langwords", langwords)
			("translations", translations, VISIT_VECTOR);
		// Moved here for debugging
		s
			("struct_part_types", struct_part_types)
			("struct_types", struct_types);
	}
}

int SrcTextData::FindAnyWord(const String& s) const {
	if (s.IsEmpty())
		return -1;
	int lang = langwords.Find(current_language);
	ASSERT(lang >= 0 && lang < 256);
	return FindAnyWord(s, (byte)lang);
}

int SrcTextData::FindAnyWord(const String& s, byte lang) const {
	if (s.IsEmpty())
		return -1;
	const auto& words_idx = langwords[lang];
	hash_t h = s.GetHashValue();
	int i = words_idx.Find(h);
	if (i < 0)
		return -1;
	const auto& v = words_idx[i];
	if (v.IsEmpty())
		return -1;
	return v[0];
}

String SrcTextData::GetTokenTextString(const TokenText& txt) const {
	return GetTokenTextString(txt.tokens);
}

String SrcTextData::GetTokenTextString(const Vector<int>& tokens) const {
	String o;
	for(int tk_i : tokens) {
		//const Token& tk = this->tokens[tk_i];
		const String& key = this->tokens.GetKey(tk_i);
		
		if (key.GetCount() == 1 && NaturalTokenizer::IsToken(key[0])) {
			o << key;
		}
		else {
			if (!o.IsEmpty())
				o << " ";
			o << key;
		}
	}
	return o;
}

String SrcTextData::GetTokenTypeString(const TokenText& txt) const {
	String o;
	for(int w_i : txt.words) {
		if (w_i < 0) {
			o << "{error}";
		}
		else {
			const auto& ew = this->words_[w_i];
			o << "{";
			if (ew.word_class >= 0)
				o << this->word_classes[ew.word_class];
			else
				o << "error";
			o << "}";
		}
	}
	return o;
}

ContextData* SrcTextData::FindContext(byte ctx) {
	for (auto it : ~ctxs)
		if (it.key.value == ctx)
			return &it.value;
	return 0;
}

const ContextData* SrcTextData::FindContext(byte ctx) const {
	for (auto it : ~ctxs)
		if (it.key.value == ctx)
			return &it.value;
	return 0;
}

AuthorDataset& SrcTextData::GetAddAuthor(String name) {
	for (AuthorDataset& a : authors)
		if (a.name == name)
			return a;
	AuthorDataset& a = authors.Add();
	a.name = name;
	return a;
}

String SrcTextData::GetWordString(const Vector<int>& words) const {
	String o;
	for(int w_i : words) {
		if (w_i < 0) continue;
		const String& key = this->words_[w_i].text;
		
		if (key.GetCount() == 1 && NaturalTokenizer::IsToken(key[0])) {
			o << key;
		}
		else {
			if (!o.IsEmpty())
				o << " ";
			o << key;
		}
	}
	return o;
}

WString SrcTextData::GetWordPronounciation(const Vector<int>& words) const {
	WString o;
	for(int w_i : words) {
		if (w_i < 0) continue;
		const auto& ew = this->words_[w_i];
		const WString& key = ew.phonetic;
		
		if (key.GetCount() == 1 && NaturalTokenizer::IsToken(key[0])) {
			o << key;
		}
		else {
			if (!o.IsEmpty())
				o << " ";
			o << key;
		}
	}
	return o;
}

String SrcTextData::GetTypeString(const Vector<int>& word_classes) const {
	String o;
	for(int wc_i : word_classes) {
		if (wc_i < 0)
			o << "{error}";
		else {
			const String& wc = this->word_classes[wc_i];
			o << "{" << wc << "}";
		}
	}
	return o;
}

String SrcTextData::GetActionString(const Vector<int>& actions) const {
	String o;
	for(int act_i : actions) {
		if (!o.IsEmpty()) o << ", ";
		if (act_i < 0)
			o << "error";
		else {
			const ActionHeader& ah = this->actions.GetKey(act_i);
			o << ah.action;
			if (!ah.arg.IsEmpty())
				o << "(" << ah.arg << ")";
		}
	}
	return o;
}

Value SrcTextData::GetScriptValue(int i) const {
	ValueArray arr0;
	const ScriptStruct& ss = this->scripts[i];
	for(int i = 0; i < ss.parts.GetCount(); i++) {
		const auto& part = ss.parts[i];
		ValueArray arr1;
		for(int j = 0; j < part.sub.GetCount(); j++) {
			const auto& sub = part.sub[j];
			ValueArray arr2;
			bool show_subsub = sub.sub.GetCount() > 1;
			for(int k = 0; k < sub.sub.GetCount(); k++) {
				const auto& ssub = sub.sub[k];
				if (show_subsub) {
					ValueArray arr3;
					for(int l = 0; l < ssub.token_texts.GetCount(); l++) {
						int tt_i = ssub.token_texts[l];
						if (tt_i < 0) continue;
						const TokenText& tt = this->token_texts[tt_i];
						arr3.Add(this->GetTokenTextString(tt));
					}
					arr2.Add(arr3);
				}
				else {
					for(int l = 0; l < ssub.token_texts.GetCount(); l++) {
						int tt_i = ssub.token_texts[l];
						if (tt_i < 0) continue;
						const TokenText& tt = this->token_texts[tt_i];
						arr2.Add(this->GetTokenTextString(tt));
					}
				}
			}
			arr1.Add(arr2);
		}
		arr0.Add(arr1);
	}
	return arr0;
}

String SrcTextData::GetScriptDump(int i) const {
	String s;
	const ScriptStruct& ss = this->scripts[i];
	for(int i = 0; i < ss.parts.GetCount(); i++) {
		const auto& part = ss.parts[i];
		//if (s.GetCount()) s << "\n";
		s << Format("[%d: %s]\n", i, GetTextTypeString(part.type) + " " + IntStr(part.num+1));
		
		for(int j = 0; j < part.sub.GetCount(); j++) {
			const auto& sub = part.sub[j];
			//if (s.GetCount()) s << "\n";
			s << Format("\t[%d.%d: repeat %.2!m]\n", i,j, sub.repeat);
			
			bool show_subsub = sub.sub.GetCount() > 1;
			for(int k = 0; k < sub.sub.GetCount(); k++) {
				const auto& ssub = sub.sub[k];
				if (show_subsub)
					s << Format("\t\t[%d.%d.%d]\n", i,j,k);
				for(int l = 0; l < ssub.token_texts.GetCount(); l++) {
					int tt_i = ssub.token_texts[l];
					if (tt_i < 0) continue;
					const TokenText& tt = this->token_texts[tt_i];
					if (show_subsub) s.Cat('\t');
					s << "\t\t" << this->GetTokenTextString(tt) << "\n";
				}
			}
		}
	}
	return s;
}

VectorMap<int,int> SrcTextData::GetSortedElements() {
	VectorMap<int,int> vmap;
	for(const ScriptStruct& ss : this->scripts.GetValues()) {
		for(const auto& part : ss.parts) {
			if (part.el_i >= 0)
				vmap.GetAdd(part.el_i,0)++;
			for(const auto& sub : part.sub) {
				if (sub.el_i >= 0)
					vmap.GetAdd(sub.el_i,0)++;
				for(const auto& ssub : sub.sub) {
					if (ssub.el_i >= 0)
						vmap.GetAdd(ssub.el_i,0)++;
				}
			}
		}
	}
	SortByValue(vmap, StdGreater<int>());
	return vmap;
}

VectorMap<int,int> SrcTextData::GetSortedElementsOfPhraseParts() {
	VectorMap<int,int> vmap;
	for (const auto& pp : this->phrase_parts.GetValues()) {
		if (pp.el_i >= 0)
			vmap.GetAdd(pp.el_i,0)++;
	}
	SortByValue(vmap, StdGreater<int>());
	return vmap;
}

ArrayMap<String, Ptr<VfsValueExt>>& DatasetIndex() {
	static ArrayMap<String, Ptr<VfsValueExt>> map;
	return map;
}



SrcTxtHeader::~SrcTxtHeader() {
	
}

void SrcTxtHeader::Visit(Vis& v) {
	v.Ver(2)
	(1)	("written",written)
		("size",size)
		("sha1",sha1)
		("files",files)
	(2)	("version",version)
		;
	
	if (v.IsStoring()) {
		LOG("SrcTxtHeader::Visit: error: storing data not implemented yet");
		v.SetError("SrcTxtHeader::Visit: error: storing data not implemented yet");
	}
}

void SrcTxtHeader::RealizeData() {
	if (!data)
		LoadData();
}

bool SrcTxtHeader::LoadData() {
	ASSERT(!filepath.IsEmpty());
	// if (files.IsEmpty()) {
	//	if (!LoadFromJsonFile(*this, filepath))
	//		return false;
	//}
	if (files.GetCount() == 0) {
		RLOG("SrcTxtHeader::LoadData: warning: no files in SrcTxtHeader");
		return false;
	}
	
	String compressed;
	String dir = GetFileDirectory(filepath);
	for(int i = 0; i < this->files.GetCount(); i++) {
		String path = AppendFileName(dir, this->files[i]);
		String data = LoadFile(path);
		
		compressed.Cat(data);
		
		int per_file = 1024 * 1024 * 25;
		LOG("SrcTxtHeader::LoadData" << data.GetCount() << " vs expected " << per_file << ": " << (data.GetCount() == per_file ? "True" : "False"));
	}
	String decompressed = BZ2Decompress(compressed);
	if (decompressed.GetCount() != this->size) {
		LOG("SrcTxtHeader::LoadData: error: size mismatch when loading: " << filepath);
		return false;
	}
	String sha1 = SHA1String(decompressed);
	if (sha1 != this->sha1) {
		LOG("SrcTxtHeader::LoadData: error: sha1 mismatch when loading: " << filepath);
		return false;
	}
	StringStream decomp_stream(decompressed);
	
	this->data.Create();
	
	if (version == 1) {
		Panic("SrcTxtHeader::LoadData: error: version 1 is not supported anymore");
		return false;
	}
	else {
		Vis vis(decomp_stream);
		this->data->Visit(vis);
	}
	
	return true;
}

String SrcTxtHeader::SaveData() {
	ASSERT(!filepath.IsEmpty());
	
	String dir = GetFileDirectory(filepath);
	String filename = GetFileName(filepath);
	StringStream decomp_stream;
	this->Serialize(decomp_stream);
	String decompressed = decomp_stream.GetResult();
	
	this->written = GetUtcTime();
	this->sha1 = SHA1String(decompressed);
	this->size = decompressed.GetCount();
	
	String compressed = BZ2Compress(decompressed);
	StringStream comp_stream;
	comp_stream % compressed;
	
	this->files.Clear();
	int per_file = 1024 * 1024 * 25; // todo read from ecs-db file
	int parts = 1 + (compressed.GetCount() + 1) / per_file;
	for(int i = 0; i < parts; i++) {
		int begin = i * per_file;
		int end = min(begin+per_file, compressed.GetCount());
		String part = compressed.Mid(begin,end-begin);
		String part_path = filepath + "." + IntStr(i);
		FileOut fout(part_path);
		fout.Put(part);
		this->files.Add(filename + "." + IntStr(i));
	}
	for (int i = parts;;) {
		String part_path = filepath + "." + IntStr(i);
		if (FileExists(part_path))
			DeleteFile(part_path);
		else
			break;
	}
	
	return StoreAsJson(*this, true);
}

INITIALIZER_COMPONENT(SrcTxtHeader, "ecs.disposable.srctxt", "Ecs|Disposable")








ContextType ContextType::Lyrical() {
	ContextType t;
	t.value =	CREATIVITY_BIT    |
				EMOTIONALITY_BIT  |
				EFFICIENCY_BIT    |
				COLLABORATIVE_BIT |
				STABILITY_BIT     |
				INNOVATIVE_BIT    |
				EXPERIMENTAL_BIT;
	return t;
}

ContextType ContextType::Programming() {
	ContextType t;
	t.value =	TECHNICALITY_BIT  |
				EFFICIENCY_BIT    |
				COLLABORATIVE_BIT |
				STABILITY_BIT     |
				INNOVATIVE_BIT    |
				EXPERIMENTAL_BIT;
	return t;
}
/*
CTX(0,CREATIVITY, Creativity, "Is the context focused on originality and expression?") \
CTX(1,TECHNICALITY, Technicality, "Is there an emphasis on technical precision or functionality?") \
CTX(2,EMOTIONALITY, Emotionality, " Does the context evoke emotions or aim to connect on an emotional level?") \
CTX(3,EFFICIENCY, Efficiency, "Is the context concerned with optimization and performance?") \
CTX(4,COLLABORATIVE, Collaborative, "Does the context involve teamwork or community involvement?") \
CTX(5,STABILITY, Stability, "Is there a focus on reliability and consistency?") \
CTX(6,INNOVATIVE, Innovative, "Is there an intention to introduce new ideas or techniques?") \
CTX(7,EXPERIMENTAL, Experimental, "Is the context about trial and error or trying unproven methods?")
*/

ContextType ContextType::PublicShortMessage() {
	ContextType t;
	t.value =	CREATIVITY_BIT    |
				EFFICIENCY_BIT    |
				COLLABORATIVE_BIT;
	return t;
}

ContextType ContextType::PersonalBlog() {
	ContextType t;
	t.value =	CREATIVITY_BIT    |
				EMOTIONALITY_BIT  |
				EFFICIENCY_BIT    |
				COLLABORATIVE_BIT |
				EXPERIMENTAL_BIT
				;
	return t;
}
ContextType ContextType::CorporateBlog() {
	ContextType t;
	t.value =	CREATIVITY_BIT    |
				TECHNICALITY_BIT  |
				COLLABORATIVE_BIT |
				STABILITY_BIT |
				INNOVATIVE_BIT
				;
	return t;
}

ContextType ContextType::Dialog() {
	ContextType t;
	t.value =	CREATIVITY_BIT    |
				EMOTIONALITY_BIT  |
				EFFICIENCY_BIT    |
				COLLABORATIVE_BIT |
				INNOVATIVE_BIT    |
				EXPERIMENTAL_BIT;
	return t;
}

ContextType ContextType::Storyboard() {
	ContextType t;
	t.value =	CREATIVITY_BIT    |
				TECHNICALITY_BIT  |
				EMOTIONALITY_BIT  |
				EFFICIENCY_BIT    |
				COLLABORATIVE_BIT |
				INNOVATIVE_BIT;
	return t;
}

ContextType ContextType::GetFromString(const String& context_str) {
	if (context_str == "song" || context_str == "lyrical")
		return ContextType::Lyrical();
	else if (context_str == "twitter")
		return ContextType::PublicShortMessage();
	else if (context_str == "blog")
		return ContextType::PersonalBlog();
	else if (context_str == "dialog")
		return ContextType::Dialog();
	else if (context_str == "storyboard")
		return ContextType::Storyboard();
	else
		return ContextType();
}

String ContextType::GetName(const ContextType& t) {
	if (t == ContextType::Lyrical())
		return "lyrical";
	if (t == ContextType::PublicShortMessage())
		return "twitter";
	if (t == ContextType::PersonalBlog())
		return "blog";
	if (t == ContextType::Dialog())
		return "dialog";
	if (t == ContextType::Storyboard())
		return "storyboard";
	
	return "unnamed";
}

String ContextType::GetBitName(int i) {
	#define CTX(idx, e, n, str) if (i == idx) return #n;
	CONTEXTLIST
	#undef CTX
	return String();
}

END_UPP_NAMESPACE
