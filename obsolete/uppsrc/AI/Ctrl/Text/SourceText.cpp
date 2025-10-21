#include "Text.h"

NAMESPACE_UPP

AuthorDataCtrl::AuthorDataCtrl(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());
	
	hsplit.Horz() << vsplit << scripts << analysis;
	hsplit.SetPos(2500);
	
	vsplit.Vert() << entities << components;
	vsplit.SetPos(1000,0);
	vsplit.SetPos(5500,1);
	
	entities.AddColumn(t_("File"));
	entities.WhenCursor << THISBACK(DataEntity);
	
	components.AddColumn(t_("Entry"));
	components.WhenCursor << THISBACK(DataExtension);
	
}

void AuthorDataCtrl::SetFont(Font fnt) {
	scripts.SetFont(fnt);
	analysis.SetFont(fnt);
}

void AuthorDataCtrl::Data() {
	DatasetPtrs p;
	o.GetDataset(p);
	if (!p.srctxt) {
		entities.Clear();
		components.Clear();
		analysis.Clear();
		return;
	}
	auto& src = *p.srctxt;
	const auto& data = src.authors;
	
	//DUMP(GetDatabase().a.dataset.scripts.GetCount());
	
	entities.SetCount(data.GetCount());
	for(int i = 0; i < data.GetCount(); i++) {
		const auto& ea = data[i];
		String s = ea.name;
		if (GetDefaultCharset() != CHARSET_UTF8)
			s = ToCharset(CHARSET_DEFAULT, s, CHARSET_UTF8);
		
		entities.Set(i, 0, s);
		entities.Set(i, 1, Join(ea.genres, ", "));
	}
	
	if (!entities.IsCursor() && entities.GetCount())
		entities.SetCursor(0);
	
	/*if (0) {
		int scripts_total = 0;
		for (const auto& d : data)
			scripts_total += d.scripts.GetCount();
		DUMP(scripts_total);
	}*/
	DataEntity();
}

void AuthorDataCtrl::DataEntity() {
	DatasetPtrs p;
	o.GetDataset(p);
	if (!p.srctxt) {
		components.Clear();
		analysis.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	if (!entities.IsCursor()) return;
	int acur = entities.GetCursor();
	const auto& data = src.authors;
	const auto& artist = data[acur];
	
	components.SetCount(artist.scripts.GetCount());
	for(int i = 0; i < artist.scripts.GetCount(); i++) {
		String s = artist.scripts[i].title;
		if (GetDefaultCharset() != CHARSET_UTF8)
			s = ToCharset(CHARSET_DEFAULT, s, CHARSET_UTF8);
		
		components.Set(i, 0, s);
	}
	
	if (!components.IsCursor() && components.GetCount())
		components.SetCursor(0);
	
	DataExtension();
}

void AuthorDataCtrl::DataExtension() {
	DatasetPtrs p;
	o.GetDataset(p);
	if (!p.srctxt) {
		analysis.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	if (!entities.IsCursor() || !components.IsCursor()) return;
	int acur = entities.GetCursor();
	int scur = components.GetCursor();
	const auto& data = src.authors;
	const auto& artist = data[acur];
	const auto& song = artist.scripts[scur];
	
	if (data_type == TEXT) {
		String s = song.text;
		if (GetDefaultCharset() != CHARSET_UTF8)
			s = ToCharset(CHARSET_DEFAULT, s, CHARSET_UTF8);
		this->scripts.SetData(s);
		analysis.Clear();
		
		if (s.Left(1) == "[") {
			ValueArray v = ParseJSON(s);
			String s;
			for(int i = 0; i < v.GetCount(); i++)
				s += v[i].ToString() + "\n";
			analysis.SetData(s);
		}
		else if (s.Left(1) == "{") {
			analysis.SetData("TODO");
		}
		else {
			TryNo5tStructureSolver solver;
			solver.Process(s);
			analysis.SetData(solver.GetResult());
			//analysis.SetData(solver.GetDebugLines());
		}
	}
	else if (data_type == STRUCTURED) {
		String key = artist.name + " - " + song.title;
		int ss_i = src.scripts.Find(key.GetHashValue());
		if (ss_i < 0) {
			scripts.Clear();
			analysis.Clear();
			return;
		}
		
		ScriptStruct& ss = src.scripts[ss_i];
		String txt = src.GetScriptDump(ss_i);
		scripts.SetData(txt);
		analysis.Clear();
	}
	else if (data_type == MIXED) {
		String s = song.text;
		if (GetDefaultCharset() != CHARSET_UTF8)
			s = ToCharset(CHARSET_DEFAULT, s, CHARSET_UTF8);
		this->scripts.SetData(s);
		
		String key = artist.name + " - " + song.title;
		int ss_i = src.scripts.Find(key.GetHashValue());
		if (ss_i < 0) {
			analysis.Clear();
			return;
		}
		ScriptStruct& ss = src.scripts[ss_i];
		String txt = src.GetScriptDump(ss_i);
		analysis.SetData(txt);
	}
}

















TokensPage::TokensPage(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());
	
	hsplit.Horz() << tokens;
	hsplit.SetPos(2000);
	
	tokens.AddColumn(t_("Token"));
	tokens.AddIndex("IDX");
	tokens.ColumnWidths("3 1");
	tokens.WhenBar << [this](Bar& bar){
		bar.Add("Copy", [this]() {
			int i = tokens.GetCursor();
			String text = tokens.Get(i, 0);
			WriteClipboardText(text);
		});
	};
}

void TokensPage::Data() {
	DatasetPtrs p;
	o.GetDataset(p);
	if (!p.srctxt) {
		tokens.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	for(int j = 0; j < src.tokens.GetCount(); j++) {
		const String& txt = src.tokens.GetKey(j);
		const TokenIdx& tk = src.tokens[j];
		tokens.Set(j, 0, txt);
	}
	tokens.SetCount(src.tokens.GetCount());
	
}













ScriptTextDebuggerPage::ScriptTextDebuggerPage(DatasetProvider& o) : o(o) {
	Add(tabs.SizePos());
	#define LIST \
		ITEM(tokens) ITEM(token_texts) \
		ITEM(word_classes) ITEM(words) \
		ITEM(virtual_phrases) ITEM(virtual_phrase_parts) ITEM(virtual_phrase_structs) \
		ITEM(phrase_parts) ITEM(struct_part_types) ITEM(struct_types) ITEM(simple_attrs) \
		ITEM(element_keys) ITEM(attrs) ITEM(actions) \
		ITEM(wordnets) ITEM(action_phrases) ITEM(trans) ITEM(parallel)
	#define ITEM(x) tabs.Add(x.SizePos(), #x);
	LIST
	#undef ITEM
	
	tabs.WhenSet = THISBACK(Data);
	
	tokens.AddColumn("#");
	tokens.AddColumn("String");
	
	word_classes.AddColumn("#");
	word_classes.AddColumn("Word class");
	
	words.AddColumn("#");
	words.AddColumn("Word");
	words.AddColumn("Language");
	words.AddColumn("Spelling");
	words.AddColumn("Phonetic");
	words.AddColumn("Count");
	words.AddColumn("Clr");
	words.AddColumn("Classes");
	
	token_texts.AddColumn("#");
	token_texts.AddColumn("Hash");
	token_texts.AddColumn("Tokens");
	token_texts.AddColumn("Words");
	token_texts.AddColumn("Virtual phrase");
	token_texts.ColumnWidths("1 2 8 4 3");
	
	virtual_phrases.AddColumn("#");
	virtual_phrases.AddColumn("Hash");
	virtual_phrases.AddColumn("Word classes");
	virtual_phrases.AddColumn("Virtual phrase struct");
	
	virtual_phrase_parts.AddColumn("#");
	virtual_phrase_parts.AddColumn("Hash");
	virtual_phrase_parts.AddColumn("Word classes");
	virtual_phrase_parts.AddColumn("Struct part type");
	virtual_phrase_parts.AddColumn("Count");
	
	virtual_phrase_structs.AddColumn("#");
	virtual_phrase_structs.AddColumn("Hash");
	virtual_phrase_structs.AddColumn("Virtual phrase parts");
	virtual_phrase_structs.AddColumn("Struct type");
	
	phrase_parts.AddColumn("#");
	phrase_parts.AddColumn("Hash");
	phrase_parts.AddColumn("Words");
	phrase_parts.AddColumn("Token text");
	phrase_parts.AddColumn("Virtual phrase part");
	phrase_parts.AddColumn("Attr");
	phrase_parts.AddColumn("Element");
	phrase_parts.AddColumn("Color");
	phrase_parts.AddColumn("Actions");
	phrase_parts.AddColumn("Typecasts");
	phrase_parts.AddColumn("Contrasts");
	phrase_parts.AddColumn("Scores");
	
	struct_part_types.AddColumn("#");
	struct_part_types.AddColumn("Struct part type");
	
	struct_types.AddColumn("#");
	struct_types.AddColumn("Struct type");
	
	simple_attrs.AddColumn("#");
	simple_attrs.AddColumn("Key");
	simple_attrs.AddColumn("Attr 0");
	simple_attrs.AddColumn("Attr 1");
	
	element_keys.AddColumn("#");
	element_keys.AddColumn("Element key");
	
	attrs.AddColumn("#");
	attrs.AddColumn("Attr Action");
	attrs.AddColumn("Attr Arg");
	attrs.AddColumn("Simple attr");
	attrs.AddColumn("Positive");
	attrs.AddColumn("Link");
	attrs.AddColumn("Count");
	
	actions.AddColumn("#");
	actions.AddColumn("Action");
	actions.AddColumn("Action Arg");
	actions.AddColumn("Attr");
	actions.AddColumn("Color");
	actions.AddColumn("Count");
	
}

void ScriptTextDebuggerPage::Data() {
	int tab = tabs.Get();
	int i = 0;
	enum {
	#define ITEM(x) PAGE_##x,
	LIST
	#undef ITEM
	};
	
	DatasetPtrs p;
	o.GetDataset(p);
	if (!p.srctxt) {
		#define ITEM(x) x.Clear();
		LIST
		#undef ITEM
	}
	const auto& src = *p.srctxt;
	
	if (tab == PAGE_tokens) {
		int i = 0;
		for(auto it : ~src.tokens) {
			tokens.Set(i, 0, i);
			tokens.Set(i, 1, it.key);
			i++;
		}
		tokens.SetCount(src.tokens.GetCount());
	}
	if (tab == PAGE_word_classes) {
		int i = 0;
		for(auto it : src.word_classes) {
			word_classes.Set(i, 0, i);
			word_classes.Set(i, 1, it);
			i++;
		}
		word_classes.SetCount(src.word_classes.GetCount());
	}
	if (tab == PAGE_words) {
		int i = 0;
		for(auto it : src.words_) {
			words.Set(i, 0, i);
			words.Set(i, 1, it.text);
			if (it.lang != 0xFF && it.lang < src.langwords.GetCount())
				words.Set(i, 2, src.langwords.GetKey(it.lang));
			else if (it.lang != 0xFF)
				words.Set(i, 2, "<error>");
			else
				words.Set(i, 2, Value());
			words.Set(i, 3, it.spelling);
			words.Set(i, 4, it.phonetic);
			words.Set(i, 5, it.count);
			words.Set(i, 6, AttrText(" ").NormalPaper(it.clr).Paper(it.clr));
			String s;
			if (it.word_class >= 0 && it.word_class < src.word_classes.GetCount())
				s = src.word_classes[it.word_class];
			words.Set(i, 7, s);
			i++;
		}
		words.SetCount(i);
	}
	#if 0
	if (tab == PAGE_ambiguous_word_pairs) {
		int i = 0;
		for(auto it : ~src.ambiguous_word_pairs) {
			ambiguous_word_pairs.Set(i, 0, i);
			ambiguous_word_pairs.Set(i, 1, HexStr64(it.key));
			
			if (it.value.from >= 0)
				ambiguous_word_pairs.Set(i, 2, src.words_[it.value.from].text);
			else
				ambiguous_word_pairs.Set(i, 2, Value());
			
			if (it.value.from_type >= 0)
				ambiguous_word_pairs.Set(i, 3, src.word_classes[it.value.from_type]);
			else
				ambiguous_word_pairs.Set(i, 3, Value());
			
			if (it.value.to >= 0)
				ambiguous_word_pairs.Set(i, 4, src.words_[it.value.to].text);
			else
				ambiguous_word_pairs.Set(i, 4, Value());
			
			if (it.value.to_type >= 0)
				ambiguous_word_pairs.Set(i, 5, src.word_classes[it.value.to_type]);
			else
				ambiguous_word_pairs.Set(i, 5, Value());
			i++;
		}
		ambiguous_word_pairs.SetCount(src.ambiguous_word_pairs.GetCount());
	}
	#endif
	if (tab == PAGE_token_texts) {
		int i = 0;
		for(auto it : ~src.token_texts) {
			token_texts.Set(i, 0, i);
			token_texts.Set(i, 1, HexStr64(it.key));
			String s;
			for (int tk_i : it.value.tokens) {
				if (!s.IsEmpty()) s << " ";
				if (tk_i >= 0 && tk_i < src.tokens.GetCount())
					s << src.tokens.GetKey(tk_i);
				else
					s << "<" << tk_i << ">";
			}
			token_texts.Set(i, 2, s);
			
			s.Clear();
			for (int wrd_i : it.value.words) {
				if (!s.IsEmpty()) s << " ";
				if (wrd_i >= 0 && wrd_i < src.words_.GetCount())
					s << src.words_[wrd_i].text;
				else
					s << "<" << wrd_i << ">";
			}
			token_texts.Set(i, 3, s);
			
			int vp_i = it.value.virtual_phrase;
			if (vp_i >= 0 && vp_i < src.virtual_phrases.GetCount())
				token_texts.Set(i, 4, HexStr64(src.virtual_phrases.GetKey(vp_i)));
			else if (vp_i >= 0)
				token_texts.Set(i, 4, "Error: " + IntStr(vp_i));
			else
				token_texts.Set(i, 4, Value());
			i++;
		}
		token_texts.SetCount(src.token_texts.GetCount());
	}
	if (tab == PAGE_virtual_phrases) {
		int i = 0;
		for(auto it : ~src.virtual_phrases) {
			virtual_phrases.Set(i, 0, i);
			virtual_phrases.Set(i, 1, HexStr64(it.key));
			String s;
			for (int wc_i : it.value.word_classes) {
				if (!s.IsEmpty()) s << " ";
				if (wc_i >= 0 && wc_i < src.word_classes.GetCount())
					s << src.word_classes[wc_i];
				else
					s << "<" << wc_i << ">";
			}
			virtual_phrases.Set(i, 2, s);
			
			int vps_i = it.value.virtual_phrase_struct;
			if (vps_i >= 0 && vps_i < src.virtual_phrase_structs.GetCount())
				virtual_phrases.Set(i, 3, HexStr64(src.virtual_phrase_structs.GetKey(vps_i)));
			else if (vps_i >= 0)
				virtual_phrases.Set(i, 3, "Error: " + IntStr(vps_i));
			else
				virtual_phrases.Set(i, 3, Value());
			i++;
		}
		virtual_phrases.SetCount(src.virtual_phrases.GetCount());
	}
	if (tab == PAGE_virtual_phrase_parts) {
		int i = 0;
		for(auto it : ~src.virtual_phrase_parts) {
			virtual_phrase_parts.Set(i, 0, i);
			virtual_phrase_parts.Set(i, 1, HexStr64(it.key));
			String s;
			for (int wc_i : it.value.word_classes) {
				if (!s.IsEmpty()) s << " ";
				if (wc_i >= 0 && wc_i < src.word_classes.GetCount())
					s << src.word_classes[wc_i];
				else
					s << "<" << wc_i << ">";
			}
			virtual_phrase_parts.Set(i, 2, s);
			
			int spt_i = it.value.struct_part_type;
			if (spt_i >= 0 && spt_i < src.struct_part_types.GetCount())
				virtual_phrase_parts.Set(i, 3, src.struct_part_types[spt_i]);
			else if (spt_i >= 0)
				virtual_phrase_parts.Set(i, 3, "Error: " + IntStr(spt_i));
			else
				virtual_phrase_parts.Set(i, 3, Value());
			i++;
		}
		virtual_phrase_parts.SetCount(src.virtual_phrase_parts.GetCount());
	}
	if (tab == PAGE_virtual_phrase_structs) {
		int i = 0;
		for(auto it : ~src.virtual_phrase_structs) {
			virtual_phrase_structs.Set(i, 0, i);
			virtual_phrase_structs.Set(i, 1, HexStr64(it.key));
			String s;
			for (int vpp_i : it.value.virtual_phrase_parts) {
				if (!s.IsEmpty()) s << " ";
				if (vpp_i >= 0 && vpp_i < src.virtual_phrase_parts.GetCount())
					s << "<" << i << ":" << HexStr64(src.virtual_phrase_parts.GetKey(vpp_i)) << ">";
				else
					s << "<" << vpp_i << ">";
			}
			virtual_phrase_structs.Set(i, 2, s);
			
			int st_i = it.value.struct_type;
			if (st_i >= 0 && st_i < src.struct_types.GetCount())
				virtual_phrase_structs.Set(i, 3, src.struct_types[st_i]);
			else if (st_i >= 0)
				virtual_phrase_structs.Set(i, 3, "Error: " + IntStr(st_i));
			else
				virtual_phrase_structs.Set(i, 3, Value());
			i++;
		}
		virtual_phrase_structs.SetCount(src.virtual_phrase_structs.GetCount());
	}
	if (tab == PAGE_phrase_parts) {
		int i = 0;
		for(auto it : ~src.phrase_parts) {
			if (!it.value.ctx)
				continue;
			const ContextData* cd = src.FindContext(it.value.ctx);
			ASSERT(cd);
			if (!cd) continue;
			phrase_parts.Set(i, 0, i);
			phrase_parts.Set(i, 1, HexStr64(it.key));
			String s;
			for(int wrd_i : it.value.words) {
				if (!s.IsEmpty()) s << " ";
				s << src.words_[wrd_i].text;
			}
			phrase_parts.Set(i, 2, s);
			phrase_parts.Set(i, 3, it.value.tt_i);
			phrase_parts.Set(i, 4, it.value.virtual_phrase_part);
			phrase_parts.Set(i, 5, it.value.attr);
			
			if (it.value.el_i >= 0 && it.value.el_i < src.element_keys.GetCount())
				phrase_parts.Set(i, 6, src.element_keys[it.value.el_i]);
			else
				phrase_parts.Set(i, 6, Value());
			
			phrase_parts.Set(i, 7, AttrText("").NormalPaper(it.value.clr).Paper(it.value.clr));
			
			s = "";
			for (int act_i : it.value.actions) {
				if (!s.IsEmpty()) s << " ";
				const auto& ah = src.actions.GetKey(act_i);
				s << ah.action << "(" << ah.arg << ")";
			}
			phrase_parts.Set(i, 8, s);
			
			s = "";
			for (int tc_i : it.value.typecasts) {
				if (!s.IsEmpty()) s << " ";
				ASSERT(cd);
				const auto& tc = cd->typeclasses[tc_i].name;
				s << tc;
			}
			phrase_parts.Set(i, 9, s);
			
			s = "";
			for (int con_i : it.value.contrasts) {
				if (!s.IsEmpty()) s << " ";
				const auto& con = cd->contents[con_i];
				s << con.name;
			}
			phrase_parts.Set(i, 10, s);
			i++;
		}
		phrase_parts.SetCount(src.phrase_parts.GetCount());
	}
	if (tab == PAGE_struct_part_types) {
		int i = 0;
		for(auto it : src.struct_part_types) {
			struct_part_types.Set(i, 0, i);
			struct_part_types.Set(i, 1, it);
			i++;
		}
		struct_part_types.SetCount(src.struct_part_types.GetCount());
	}
	if (tab == PAGE_struct_types) {
		int i = 0;
		for(auto it : src.struct_types) {
			struct_types.Set(i, 0, i);
			struct_types.Set(i, 1, it);
			i++;
		}
		struct_types.SetCount(src.struct_types.GetCount());
	}
	if (tab == PAGE_simple_attrs) {
		int i = 0;
		for(auto it : ~src.simple_attrs) {
			simple_attrs.Set(i, 0, i);
			simple_attrs.Set(i, 1, it.key);
			if (it.value.attr_i0 >= 0)
				simple_attrs.Set(i, 2, src.attrs.GetKey(it.value.attr_i0).ToString());
			else
				simple_attrs.Set(i, 2, Value());
			if (it.value.attr_i1 >= 0)
				simple_attrs.Set(i, 3, src.attrs.GetKey(it.value.attr_i1).ToString());
			else
				simple_attrs.Set(i, 3, Value());
			i++;
		}
		simple_attrs.SetCount(src.simple_attrs.GetCount());
	}
	if (tab == PAGE_element_keys) {
		int i = 0;
		for(auto it : src.element_keys) {
			element_keys.Set(i, 0, i);
			element_keys.Set(i, 1, it);
			i++;
		}
		element_keys.SetCount(src.element_keys.GetCount());
	}
	if (tab == PAGE_attrs) {
		int i = 0;
		for(auto it : ~src.attrs) {
			attrs.Set(i, 0, i);
			attrs.Set(i, 1, it.key.group);
			attrs.Set(i, 2, it.key.value);
			if (it.value.simple_attr >= 0)
				attrs.Set(i, 3, src.simple_attrs.GetKey(it.value.simple_attr));
			else
				attrs.Set(i, 3, Value());
			attrs.Set(i, 4, it.value.positive ? "True" : "False");
			attrs.Set(i, 5, it.value.link);
			attrs.Set(i, 6, it.value.count);
			i++;
		}
		attrs.SetCount(src.attrs.GetCount());
	}
	if (tab == PAGE_actions) {
		int i = 0;
		for(auto it : ~src.actions) {
			actions.Set(i, 0, i);
			actions.Set(i, 1, it.key.action);
			actions.Set(i, 2, it.key.arg);
			if (it.value.attr >= 0)
				actions.Set(i, 3, src.attrs.GetKey(it.value.attr).ToString());
			else
				actions.Set(i, 3, Value());
			actions.Set(i, 4, AttrText("").NormalPaper(it.value.clr).Paper(it.value.clr));
			actions.Set(i, 5, it.value.count);
			i++;
		}
		actions.SetCount(src.actions.GetCount());
	}
}







TextElements::TextElements(DatasetProvider& o) : o(o) {
	Add(list.SizePos());
	
	list.AddColumn("Text");
	list.AddColumn("Element");
	list.ColumnWidths("3 1");
	
}

void TextElements::Data() {
	DatasetPtrs p;
	o.GetDataset(p);
	if (!p.srctxt) {
		list.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	if (src.scripts.GetCount() == 1) {
		int line = 0;
		const auto& s = src.scripts[0];
		for(const auto& part : s.parts) {
			for(const auto& sub1 : part.sub) {
				for(const auto& sub2 : sub1.sub) {
					for (int tt_i : sub2.token_texts) {
						const TokenText& tt = src.token_texts[tt_i];
						String s = src.GetTokenTextString(tt);
						list.Set(line, 0, s);
						if (sub2.el_i >= 0)
							list.Set(line, 1, src.element_keys[sub2.el_i]);
						else
							list.Set(line, 1, Value());
						line++;
					}
				}
			}
		}
		list.SetCount(line);
	}
	else list.Clear();
	
}



















VirtualPhrases::VirtualPhrases(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());
	
	hsplit.Horz() << vsplit;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << texts << parts;
	
	texts.AddColumn(t_("Phrase"));
	texts.AddColumn(t_("Classes"));
	texts.AddColumn(t_("Parts"));
	texts.AddColumn(t_("Category"));
	texts.AddIndex("IDX");
	texts.ColumnWidths("1 1 1 1");
	texts.WhenBar << [this](Bar& bar){
		bar.Add("Copy", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 0);
			WriteClipboardText(text);
		});
		bar.Add("Copy virtual text", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 1);
			WriteClipboardText(text);
		});
	};
	
	parts.AddColumn(t_("From"));
	parts.AddIndex("IDX");
	parts.WhenBar << [this](Bar& bar){
		bar.Add("Copy virtual text", [this]() {
			int i = parts.GetCursor();
			String text = parts.Get(i, 0);
			WriteClipboardText(text);
		});
	};
	
}

String GetTypePhraseString(const Vector<int>& word_classes, const SrcTextData& src) {
	String o;
	for (int wc_i : word_classes) {
		if (wc_i >= src.word_classes.GetCount()) {
			o << "{error}";
			continue;
		}
		String wc = src.word_classes[wc_i];
		
		int a = wc.Find(",");
		if (a >= 0)
			wc = TrimBoth(wc.Left(a));
		
		a = wc.Find("(");
		if (a >= 0)
			wc = TrimBoth(wc.Left(a));
		
		o << "{" << wc << "}";
	}
	return o;
}

void VirtualPhrases::Data() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		texts.Clear();
		return;
	}
	auto& src = *p.srctxt;
	int row = 0;
	for(int i = 0; i < src.token_texts.GetCount(); i++) {
		const TokenText& txt = src.token_texts[i];
		if (txt.virtual_phrase < 0)
			continue;
		
		VirtualPhrase& vp = src.virtual_phrases[txt.virtual_phrase];
		
		String txt_str = src.GetTokenTextString(txt);
		String type_str = GetTypePhraseString(vp.word_classes, src);
		String parts_str;
		String struct_str;
		if (vp.virtual_phrase_struct >= 0) {
			VirtualPhraseStruct& vps = src.virtual_phrase_structs[vp.virtual_phrase_struct];
			int struct_type = vps.struct_type;
			if (struct_type >= 0)
				struct_str = src.struct_types[struct_type];
			for (int vpp_i : vps.virtual_phrase_parts) {
				if (!parts_str.IsEmpty()) parts_str += " + ";
				const VirtualPhrasePart& vpp = src.virtual_phrase_parts[vpp_i];
				parts_str += vpp.struct_part_type >= 0 ? src.struct_part_types[vpp.struct_part_type] : String("error");
			}
		}
		texts.Set(row, 0, txt_str);
		texts.Set(row, 1, type_str);
		texts.Set(row, 2, parts_str);
		texts.Set(row, 3, struct_str);
		row++;
		
		if (row >= 10000)
			break;
	}
	texts.SetCount(row);
	
	row = 0;
	for(int i = 0; i < src.virtual_phrase_parts.GetCount(); i++) {
		const VirtualPhrasePart& vpp = src.virtual_phrase_parts[i];
		if (vpp.word_classes.IsEmpty())
			continue;
		
		String type_str = GetTypePhraseString(vpp.word_classes, src);
		parts.Set(row, 0, type_str);
		row++;
	}
	parts.SetCount(row);
	
	
}












VirtualPhraseParts::VirtualPhraseParts(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());
	
	hsplit.Horz() << texts;
	hsplit.SetPos(2000);
	
	//vsplit.Vert() << texts << parts;
	
	texts.AddColumn(t_("Types"));
	texts.AddColumn(t_("Structural name"));
	texts.AddColumn(t_("Count"));
	texts.AddIndex("IDX");
	texts.ColumnWidths("3 2 1");
	texts.WhenBar << [this](Bar& bar){
		bar.Add("Copy virtual type", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 0);
			WriteClipboardText(text);
		});
		bar.Add("Copy type", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 1);
			WriteClipboardText(text);
		});
	};
	
}

void VirtualPhraseParts::Data() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		texts.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	int row = 0;
	for(int i = 0; i < src.virtual_phrase_parts.GetCount(); i++) {
		const VirtualPhrasePart& vpp = src.virtual_phrase_parts[i];
		String type_name;
		if (vpp.struct_part_type >= 0) {
			type_name = src.struct_part_types[vpp.struct_part_type];
		}
		
		String type_str = GetTypePhraseString(vpp.word_classes, src);
		texts.Set(row, 0, type_str);
		texts.Set(row, 1, type_name);
		texts.Set(row, 2, vpp.count);
		texts.Set(row, "IDX", i);
		row++;
		
		if (row >= 10000) break;
	}
	texts.SetCount(row);
	texts.SetSortColumn(2, true);
	
}







VirtualPhraseStructs::VirtualPhraseStructs(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());
	
	hsplit.Horz() << texts;
	hsplit.SetPos(2000);
	
	//vsplit.Vert() << texts << parts;
	
	texts.AddColumn(t_("Structural name"));
	texts.AddColumn(t_("Types"));
	texts.AddColumn("");
	texts.AddIndex("IDX");
	texts.ColumnWidths("2 3 3");
	texts.WhenBar << [this](Bar& bar){
		bar.Add("Copy virtual type", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 0);
			WriteClipboardText(text);
		});
		bar.Add("Copy type", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 1);
			WriteClipboardText(text);
		});
	};
	
}

void VirtualPhraseStructs::Data() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		texts.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	int row = 0;
	for(int i = 0; i < src.virtual_phrase_structs.GetCount(); i++) {
		const VirtualPhraseStruct& vps = src.virtual_phrase_structs[i];
		
		String type_name;
		if (vps.struct_type >= 0) {
			type_name = src.struct_types[vps.struct_type];
		}
		
		String type_str;
		for(int j = 0; j < vps.virtual_phrase_parts.GetCount(); j++) {
			if (j) type_str += " + ";
			int vpp_i = vps.virtual_phrase_parts[j];
			VirtualPhrasePart& vpp = src.virtual_phrase_parts[vpp_i];
			if (vpp.struct_part_type < 0) {
				type_str += "error";
			}
			else {
				const String& part_type_name = src.struct_part_types[vpp.struct_part_type];
				type_str += part_type_name;
			}
		}
		texts.Set(row, 0, type_name);
		texts.Set(row, 1, type_str);
		texts.Set(row, "IDX", i);
		row++;
		
		if (row >= 10000) break;
	}
	texts.SetCount(row);
	texts.SetSortColumn(2, true);
	
}









void ScoreDisplay::Paint(Draw& d, const Rect& r, const Value& q,
                         Color ink, Color paper, dword style) const {
	d.DrawRect(r, Blend(paper, White()));
	ValueArray va = q;
	if (va.GetCount() == SCORE_COUNT) {
		int w = r.Width();
		int h = r.Height();
		double cx = (double)w / (double)SCORE_COUNT;
		Color clr = LtBlue();
		for(int i = 0; i < va.GetCount(); i++) {
			int sc = va[i];
			int h0 = h * sc / 10;
			int x0 = (int)(i * cx);
			int x1 = (int)((i+1) * cx);
			int y0 = h - h0;
			d.DrawRect(Rect(x0,y0,x1,h), clr);
		}
		for(int i = 1; i < SCORE_COUNT; i++) {
			int x0 = (int)(i * cx);
			d.DrawLine(x0,0,x0,h,1,GrayColor());
		}
	}
}



PhrasePartAnalysis::PhrasePartAnalysis(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());

	hsplit.Horz() << vsplit << parts;
	hsplit.SetPos(2000);

	vsplit.Vert() << attrs << colors << actions << action_args;

	attrs.AddColumn(t_("Group"));
	attrs.AddColumn(t_("Value"));
	attrs.ColumnWidths("1 1");
	attrs.WhenCursor << THISBACK(DataAttribute);

	colors.AddColumn(t_("Colors"));
	colors.WhenCursor << THISBACK(DataColor);

	actions.AddColumn(t_("Action"));
	actions.AddColumn(t_("Count"));
	actions.ColumnWidths("3 1");
	actions.WhenCursor << THISBACK(DataAction);

	action_args.AddColumn(t_("Action args"));
	action_args.AddColumn(t_("Count"));
	action_args.ColumnWidths("3 1");
	action_args.WhenCursor << THISBACK(DataActionHeader);

	parts.AddColumn(t_("Phrase"));
	parts.AddColumn(t_("Actions"));
	parts.AddColumn(t_("Group"));
	parts.AddColumn(t_("Value"));
	parts.AddColumn(t_("Scores")).SetDisplay(Single<ScoreDisplay>());
	parts.AddColumn(t_("Score-sum"));
	parts.AddIndex("IDX");
	parts.ColumnWidths("8 16 6 6 3 1");
	parts.WhenBar << [this](Bar& bar){
		bar.Add("Copy", [this]() {
			int i = parts.GetCursor();
			AttrText text = parts.Get(i, 3);
			String s = text.text.ToString();
			WriteClipboardText(s);
		});
	};

}

void PhrasePartAnalysis::Data() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		attrs.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	// Set attributes
	attrs.Set(0,0, "All");
	for(int i = 0; i < src.attrs.GetCount(); i++) {
		const AttrHeader& ah = src.attrs.GetKey(i);
		attrs.Set(1+i, 0, ah.group);
		attrs.Set(1+i, 1, ah.value);
	}
	INHIBIT_CURSOR(attrs);
	attrs.SetCount(src.attrs.GetCount());
	if (!attrs.IsCursor() && attrs.GetCount())
		attrs.SetCursor(0);


	DataAttribute();
}

void PhrasePartAnalysis::DataAttribute() {
	colors.SetCount(1+GetColorGroupCount());
	colors.Set(0, 0, t_("All words"));
	for(int i = 0; i < GetColorGroupCount(); i++) {
		colors.Set(1+i, 0,
			AttrText("#" + IntStr(i))
				.NormalPaper(GetGroupColor(i)).NormalInk(Black())
				.Paper(Blend(GrayColor(), GetGroupColor(i))).Ink(White()));
	}
	if (colors.GetCount() && !colors.IsCursor())
		colors.SetCursor(0);


	DataColor();
}

void PhrasePartAnalysis::DataColor() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		actions.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	uniq_acts.Clear();
	for (const ActionHeader& ah : src.actions.GetKeys()) {
		uniq_acts.GetAdd(ah.action).GetAdd(ah.arg, 0)++;
	}
	struct Sorter {
		bool operator()(const VectorMap<String, int>& a, const VectorMap<String, int>& b) const {
			return a.GetCount() > b.GetCount();
		}
	};
	SortByValue(uniq_acts, Sorter());
	for (auto& v : uniq_acts.GetValues())
		SortByValue(v, StdGreater<int>());

	actions.Set(0, 0, "All");
	actions.Set(0, 1, src.actions.GetCount());
	for(int i = 0; i < uniq_acts.GetCount(); i++) {
		actions.Set(1+i, 0, uniq_acts.GetKey(i));
		actions.Set(1+i, 1, uniq_acts[i].GetCount());
	}
	actions.SetCount(1+uniq_acts.GetCount());
	if (!actions.IsCursor() && actions.GetCount())
		actions.SetCursor(0);

	DataAction();
}

void PhrasePartAnalysis::DataAction() {
	if (!actions.IsCursor())
		return;

	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		action_args.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	if (actions.IsCursor()) {
		String action = actions.Get(0);
		int i = uniq_acts.Find(action);
		if (i < 0) {
			action_args.SetCount(1);
			action_args.Set(0, 0, "All");
			action_args.Set(0, 1, src.actions.GetCount());
		}
		else {
			auto& args = uniq_acts[i];
			action_args.Set(0, 0, "All");
			action_args.Set(0, 1, args.GetCount());
			for(int i = 0; i < args.GetCount(); i++) {
				action_args.Set(1+i, 0, args.GetKey(i));
				action_args.Set(1+i, 1, args[i]);
			}
			action_args.SetCount(1+args.GetCount());
		}
		if (!action_args.IsCursor() && action_args.GetCount())
			action_args.SetCursor(0);
	}

	DataActionHeader();
}

void PhrasePartAnalysis::DataActionHeader() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		parts.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	int clr_i = colors.IsCursor() ? colors.GetCursor() : -1;
	int act_i = actions.IsCursor() ? actions.GetCursor() : -1;
	int arg_i = action_args.IsCursor() ? action_args.GetCursor() : -1;
	int attr_i = attrs.IsCursor() ? attrs.GetCursor() : -1;

	bool clr_filter = clr_i > 0;
	bool attr_filter = attr_i > 0;
	bool action_filter = act_i > 0;
	bool arg_filter = arg_i > 0;

	int match_attr = -1;
	if (attr_filter)
		match_attr = attr_i - 1;

	clr_i--;

	String action_str, arg_str;
	if (action_filter) {
		action_str = actions.Get(0);
		if (arg_filter)
			arg_str = action_args.Get(0);
	}

	int row = 0, max_rows = 10000;
	for(int i = 0; i < src.phrase_parts.GetCount(); i++) {
		PhrasePart& pp = src.phrase_parts[i];

		parts.Set(row, "IDX", i);

		String phrase = src.GetWordString(pp.words);
		parts.Set(row, 0,
			AttrText(phrase)
				.NormalPaper(Blend(pp.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(pp.clr, GrayColor())).Ink(White())
			);

		parts.Set(row, 1, src.GetActionString(pp.actions));


		// Filter by attribute
		if (attr_filter) {
			if (pp.attr >= 0) {
				if (match_attr != pp.attr)
					continue;
			}
			else continue;
		}
		if (pp.attr >= 0) {
			const AttrHeader& ah = src.attrs.GetKey(pp.attr);
			parts.Set(row, 2, ah.group);
			parts.Set(row, 3, ah.value);
		}
		else {
			parts.Set(row, 2, Value());
			parts.Set(row, 3, Value());
		}
		
		
		// Filter by color group
		if (clr_filter && GetColorGroup(pp.clr) != clr_i)
			continue;

		// Filter by action
		if (action_filter) {
			bool found = false;
			for (int ah_i : pp.actions) {
				const ActionHeader& ah = src.actions.GetKey(ah_i);
				if (ah.action == action_str) {
					if (arg_filter) {
						if (ah.arg == arg_str)
							found = true;
					}
					else found = true;
				}
			}
			if (!found)
				continue;
		}

		ValueArray va;
		int sum = 0;
		for(int i = 0; i < SCORE_COUNT; i++) {
			va.Add(pp.scores[i]);
			sum += pp.scores[i];
		}
		parts.Set(row, 4, va);
		parts.Set(row, 5, sum);

		/*parts.Set(row, 2,
			AttrText(ah.action)
				.NormalPaper(Blend(pp.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(pp.clr, GrayColor())).Ink(White())
			);
		parts.Set(row, 3,
			AttrText(ah.arg)
				.NormalPaper(Blend(pp.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(pp.clr, GrayColor())).Ink(White())
			);*/
		row++;

		if (row >= max_rows)
			break;
	}
	parts.SetCount(row);
	parts.SetSortColumn(5, true);

}










PhrasePartAnalysis2::PhrasePartAnalysis2(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());
	
	hsplit.Horz() << vsplit << parts;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << elements << typecasts << contrasts << colors;
	
	elements.AddColumn(t_("Element"));
	elements.AddColumn(t_("Count"));
	elements.AddIndex("IDX");
	elements.ColumnWidths("3 1");
	elements.WhenCursor << [this]() {
		DataElement();
	};
	
	typecasts.AddColumn(t_("Typeclass"));
	typecasts.AddColumn(t_("Count"));
	typecasts.ColumnWidths("3 1");
	typecasts.WhenCursor << [this]() {
		//DatabaseBrowser::Single().SetAttr(typecasts.GetCursor());
		DataTypeclass();
	};
	
	contrasts.AddColumn(t_("Profile"));
	contrasts.AddColumn(t_("Count"));
	contrasts.ColumnWidths("3 1");
	contrasts.WhenCursor << [this]() {
		//DatabaseBrowser::Single().SetColor(contrasts.GetCursor());
		DataContrast();
	};
	
	colors.AddColumn(t_("Colors"));
	colors.WhenCursor << THISBACK(DataColor);
	
	parts.AddColumn(t_("Phrase"));
	parts.AddColumn(t_("Typeclass"));
	parts.AddColumn(t_("Contrast"));
	/*parts.AddColumn(t_("Profile"));
	parts.AddColumn(t_("Content"));
	parts.AddColumn(t_("Primary"));
	parts.AddColumn(t_("Secondary"));*/
	parts.AddColumn(t_("Score-sum"));
	parts.AddIndex("IDX");
	parts.ColumnWidths("12 12 12 1");
	parts.WhenBar << [this](Bar& bar){
		bar.Add("Copy", [this]() {
			int i = parts.GetCursor();
			AttrText text = parts.Get(i, 3);
			String s = text.text.ToString();
			WriteClipboardText(s);
		});
	};
	
}

void PhrasePartAnalysis2::ClearAll() {
	if (!PromptYesNo(DeQtf("Do you really want to remove all typecasts and contrasts?")))
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt)
		return;
	auto& src = *p.srctxt;
	
	for(int i = 0; i < src.phrase_parts.GetCount(); i++) {
		PhrasePart& pp = src.phrase_parts[i];
		pp.typecasts.Clear();
		pp.contrasts.Clear();
	}
	
	PostCallback(THISBACK(Data));
}

void PhrasePartAnalysis2::Data() {
	DataMain();
}

void PhrasePartAnalysis2::DataMain() {
	DatabaseBrowser& b = DatabaseBrowser::Single();
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		elements.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	// Set elements
	VectorMap<int,int> el_map = src.GetSortedElementsOfPhraseParts();
	
	elements.Set(0, 0, "All");
	elements.Set(0, "IDX", -1);
	for(int i = 0; i < el_map.GetCount(); i++) {
		int el_i = el_map.GetKey(i);
		elements.Set(1+i, 0, src.element_keys[el_i]);
		elements.Set(1+i, 1, el_map[i]);
		elements.Set(1+i, "IDX", el_map.GetKey(i));
	}
	INHIBIT_CURSOR(elements);
	elements.SetCount(1+el_map.GetCount());
	if (!elements.IsCursor() && elements.GetCount())
		elements.SetCursor(0);
	
	DataElement();
}

void PhrasePartAnalysis2::DataElement() {
	if (!elements.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		typecasts.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	// Set typeclasses
	if (src.ctxs.IsEmpty())
		return;
	auto& ctx = src.ctxs[0];
	const auto& tc = ctx.typeclasses;
	typecasts.Set(0, 0, "All");
	for(int i = 0; i < tc.GetCount(); i++) {
		typecasts.Set(1+i, 0, "DEPRECATED: " + tc[i].name);
		typecasts.Set(1+i, 1, 0);
	}
	INHIBIT_CURSOR(typecasts);
	typecasts.SetCount(1+tc.GetCount());
	//typecasts.SetSortColumn(2, true);
	if (!typecasts.IsCursor() && typecasts.GetCount())
		typecasts.SetCursor(0);
	
	DataTypeclass();
}

void PhrasePartAnalysis2::DataTypeclass() {
	if (!typecasts.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		contrasts.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	auto& ctx = src.ctxs[0];
	const auto& vec = ctx.contents;
	contrasts.Set(0, 0, "All");
	for(int i = 0; i < vec.GetCount(); i++) {
		/*DatabaseBrowser::ActionGroup& a = b.groups[i];
		contrasts.Set(i, 0, a.group);
		contrasts.Set(i, 1, a.count);*/
		contrasts.Set(1+i, 0, vec[i].name);
		contrasts.Set(1+i, 1, 0);
	}
	INHIBIT_CURSOR(contrasts);
	contrasts.SetCount(1+vec.GetCount());
	//contrasts.SetSortColumn(1, true);
	if (!contrasts.IsCursor() && contrasts.GetCount())
		contrasts.SetCursor(0);
	
	DataContrast();
}


void PhrasePartAnalysis2::DataContrast() {
	colors.SetCount(1+GetColorGroupCount());
	colors.Set(0, 0, t_("All words"));
	for(int i = 0; i < GetColorGroupCount(); i++) {
		colors.Set(1+i, 0,
			AttrText("#" + IntStr(i))
				.NormalPaper(GetGroupColor(i)).NormalInk(Black())
				.Paper(Blend(GrayColor(), GetGroupColor(i))).Ink(White()));
	}
	if (colors.GetCount() && !colors.IsCursor())
		colors.SetCursor(0);


	DataColor();
}

void PhrasePartAnalysis2::DataColor() {
	if (!typecasts.IsCursor() || !colors.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		parts.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	//DatabaseBrowser& b = DatabaseBrowser::Single();
	int el_i = elements.Get("IDX");
	int tc_i = typecasts.GetCursor() - 1;
	int con_i = contrasts.GetCursor() - 1;
	int clr_i = colors.GetCursor() - 1;
	bool clr_filter = clr_i >= 0;
	
	auto& ctx = src.ctxs[0];
	const auto& tc_v = ctx.typeclasses;
	const auto& con_v = ctx.contents;
	
	//int count = min(b.data.GetCount(), 10000);
	int count = src.phrase_parts.GetCount();
	int row = 0;
	for(int i = 0; i < count; i++) {
		/*int pp_i = b.data[i];
		int row = i;
		PhrasePart& pp = src.phrase_parts[pp_i];*/
		int pp_i = i;
		PhrasePart& pp = src.phrase_parts[i];
		
		if (el_i >= 0) {
			bool found = pp.el_i == el_i;
			if (!found) continue;
		}
		if (tc_i >= 0) {
			bool found = false;
			for (int j : pp.typecasts)
				if (j == tc_i)
					{found = true; break;}
			if (!found) continue;
		}
		if (con_i >= 0) {
			bool found = false;
			for (int j : pp.contrasts)
				if (j == con_i)
					{found = true; break;}
			if (!found) continue;
		}
		
		// Filter by color group
		if (clr_filter && GetColorGroup(pp.clr) != clr_i)
			continue;
		
		{
			String s;
			for (int j : pp.typecasts)
				if (j >= 0 && j < tc_v.GetCount())
					s << tc_v[j].name << ", ";
			parts.Set(row, 1, s);
		}
		{
			String s;
			for (int j : pp.contrasts) {
				int con_i = j / PART_COUNT;
				int con_j = j % PART_COUNT;
				if (con_i < con_v.GetCount())
					s << con_v[con_i].name << " #" << (con_j+1) << ", ";
			}
			parts.Set(row, 2, s);
		}
		
		parts.Set(row, "IDX", pp_i);
		
		String phrase = src.GetWordString(pp.words);
		parts.Set(row, 0,
			AttrText(phrase)
				.NormalPaper(Blend(pp.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(pp.clr, GrayColor())).Ink(White())
			);
		
		int sum = 0;
		for(int i = 0; i < SCORE_COUNT; i++) {
			sum += pp.scores[i];
		}
		
		parts.Set(row, 3, sum);
		
		row++;
		
		
		if (row >= 10000)
			break;
	}
	parts.SetCount(row);
	parts.SetSortColumn(3, true);
	
}

void PhrasePartAnalysis2::UpdateCounts() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		return;
	}
	auto& src = *p.srctxt;
	
	TODO
	/*int count = src.phrase_parts.GetCount();
	int row = 0;
	for(int i = 0; i < count; i++) {
		PhrasePart& pp = src.phrase_parts[i];
		pp.profiles.Clear();
		pp.primary.Clear();
		pp.secondary.Clear();
	}*/
	/*for (ExportAction& ea : src.primaries.GetValues())
		ea.count = 0;
	for (ExportAttr& ea : src.typecasts.GetValues())
		ea.count = 0;
	
	for(int i = 0; i < src.phrase_parts.GetCount(); i++) {
		PhrasePart& pp = src.phrase_parts[i];
		
		for (int ah_i : pp.primaries) {
			ExportAction& ea = src.primaries[ah_i];
			ea.count++;
		}
		
		if (pp.attr >= 0) {
			ExportAttr& ea = src.typecasts[pp.attr];
			ea.count++;
		}
	}*/
}






ActionAttrsPage::ActionAttrsPage(DatasetProvider& o) : o(o) {
	Add(hsplit.VSizePos(0,30).HSizePos());
	
	hsplit.Horz() << vsplit << actions;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << attrs << colors;
	
	attrs.AddColumn(t_("Group"));
	attrs.AddColumn(t_("Value"));
	attrs.AddIndex("GROUP");
	attrs.AddIndex("VALUE");
	attrs.ColumnWidths("1 1");
	attrs.WhenCursor << THISBACK(DataAttribute);
	
	colors.AddColumn(t_("Colors"));
	colors.WhenCursor << THISBACK(DataColor);
	
	actions.AddColumn(t_("Group"));
	actions.AddColumn(t_("Value"));
	actions.AddColumn(t_("Action"));
	actions.AddColumn(t_("Action arg"));
	actions.AddIndex("IDX");
	actions.ColumnWidths("2 2 4 4");
}

void ActionAttrsPage::Data() {
	int gi = 0;
	int i = 0;
	
	attrs.Set(i, 0, "All");
	attrs.Set(i, 1, "All");
	attrs.Set(i, "GROUP", -1);
	attrs.Set(i, "VALUE", -1);
	i++;
	
	#define ATTR_ITEM(e, g, i0, i1) \
		attrs.Set(i, 0, g); \
		attrs.Set(i, 1, i0); \
		attrs.Set(i, "GROUP", gi); \
		attrs.Set(i, "VALUE", 0); \
		i++; \
		attrs.Set(i, 0, g); \
		attrs.Set(i, 1, i1); \
		attrs.Set(i, "GROUP", gi); \
		attrs.Set(i, "VALUE", 1); \
		i++, gi++;
	ATTR_LIST
	#undef ATTR_ITEM

	if (!attrs.IsCursor() && attrs.GetCount())
		attrs.SetCursor(0);
	
	
	DataAttribute();
}

void ActionAttrsPage::DataAttribute() {
	if (!attrs.IsCursor())
		return;
	
	
	
	colors.SetCount(1+GetColorGroupCount());
	colors.Set(0, 0, t_("All words"));
	for(int i = 0; i < GetColorGroupCount(); i++) {
		colors.Set(1+i, 0,
			AttrText("#" + IntStr(i))
				.NormalPaper(GetGroupColor(i)).NormalInk(Black())
				.Paper(Blend(GrayColor(), GetGroupColor(i))).Ink(White()));
	}
	if (colors.GetCount() && !colors.IsCursor())
		colors.SetCursor(0);
	
	
	DataColor();
}

void ActionAttrsPage::DataColor() {
	if (!colors.IsCursor() || !attrs.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		actions.Clear();
		return;
	}
	auto& src = *p.srctxt;
	int clr_i = colors.GetCursor();
	int attr_group_i = attrs.Get("GROUP");
	int attr_value_i = attrs.Get("VALUE");
	String group_str = attr_group_i >= 0 ? ToLower(AttrKeys[attr_group_i][1]) : String();
	String value_str = attr_group_i >= 0 ? ToLower(AttrKeys[attr_group_i][2 + attr_value_i]) : String();
	
	bool clr_filter = clr_i > 0;
	bool attr_filter = attr_group_i >= 0;
	clr_i--;
	
	int row = 0, max_rows = 10000;
	for(int i = 0; i < src.actions.GetCount(); i++) {
		const ActionHeader& ah = src.actions.GetKey(i);
		ExportAction& aa = src.actions[i];
		
		// Filter by color group
		if (clr_filter && GetColorGroup(aa.clr) != clr_i)
			continue;
		
		// Filter by attribute
		String g, v;
		if (attr_filter) {
			if (aa.attr < 0)
				continue;
			const AttrHeader& ath = src.attrs.GetKey(aa.attr);
			if (ath.group != group_str || ath.value != value_str)
				continue;
		}
		
		
		actions.Set(row, "IDX", i);
		if (aa.attr >= 0) {
			const AttrHeader& ath = src.attrs.GetKey(aa.attr);
			actions.Set(row, 0, ath.group);
			actions.Set(row, 1, ath.value);
		}
		else {
			actions.Set(row, 0, Value());
			actions.Set(row, 1, Value());
		}
		actions.Set(row, 2,
			AttrText(ah.action)
				.NormalPaper(Blend(aa.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(aa.clr, GrayColor())).Ink(White())
			);
		actions.Set(row, 3,
			AttrText(ah.arg)
				.NormalPaper(Blend(aa.clr, White(), 128+64)).NormalInk(Black())
				.Paper(Blend(aa.clr, GrayColor())).Ink(White())
			);
		row++;
		if (row >= max_rows)
			break;
	}
	actions.SetCount(row);
	actions.SetSortColumn(2);
	
}







Attributes::Attributes(DatasetProvider& o) : o(o) {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << groups << values << vsplit;
	hsplit.SetPos(1000,0);
	
	vsplit.Vert() << pos_values << neg_values;
	
	groups.AddColumn(t_("Group"));
	groups.AddColumn(t_("Value count"));
	groups.AddColumn(t_("Link"));
	groups.AddIndex("IDX");
	groups.ColumnWidths("3 1 1");
	groups.WhenCursor << THISBACK(DataGroup);
	
	values.AddColumn(t_("Value"));
	
	pos_values.AddColumn(t_("Value"));
	pos_values.AddColumn(t_("Most popular"));
	
	neg_values.AddColumn(t_("Value"));
	neg_values.AddColumn(t_("Most popular"));
	
	
}

void Attributes::RealizeTemp() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt)
		return;
	auto& src = *p.srctxt;
	
	uniq_attrs.Clear();
	uniq_attrs_i.Clear();
	
	{
		for(int i = 0; i < src.attrs.GetCount(); i++) {
			const AttrHeader& ah = src.attrs.GetKey(i);
			uniq_attrs.GetAdd(ah.group).FindAdd(ah.value);
			uniq_attrs_i.GetAdd(ah.group).FindAdd(i);
		}
		
		SortByKey(uniq_attrs, StdLess<String>());
		for(int i = 0; i < uniq_attrs.GetCount(); i++) {
			SortIndex(uniq_attrs[i], StdLess<String>());
		}
	}
}

void Attributes::Data() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		groups.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	RealizeTemp();
	
	for(int i = 0; i < uniq_attrs.GetCount(); i++) {
		String group = uniq_attrs.GetKey(i);
		const auto& v = uniq_attrs[i];
		groups.Set(i, 0, group);
		groups.Set(i, 1, v.GetCount());
		groups.Set(i, "IDX", i);
		
		if (v.GetCount() == 1) {
			AttrHeader ah;
			ah.group = group;
			ah.value = v[0];
			int j = src.attrs.Find(ah);
			if (j >= 0) {
				const ExportAttr& ea = src.attrs[j];
				if (ea.link >= 0) {
					const AttrHeader& link_ah = src.attrs.GetKey(ea.link);
					groups.Set(i, 2, link_ah.group + ": " + link_ah.value);
				}
			}
			else
				groups.Set(i, 2, Value());
		}
		else {
			groups.Set(i, 2, Value());
		}
	}
	INHIBIT_CURSOR(groups);
	groups.SetCount(uniq_attrs.GetCount());
	groups.SetSortColumn(1, true);
	if (!groups.IsCursor() && groups.GetCount())
		groups.SetCursor(0);
	
	DataGroup();
}

void Attributes::DataGroup() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		values.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	RealizeTemp();
	
	if (!groups.IsCursor()) {
		pos_values.Clear();
		neg_values.Clear();
		return;
	}
	
	int g_i = groups.Get("IDX");
	const String& group = uniq_attrs.GetKey(g_i);
	const Index<String>& gvalues = uniq_attrs[g_i];
	
	int attr_i0 = -1;
	int attr_i1 = -1;
	String attr_s0, attr_s1;
	int i = src.simple_attrs.Find(group);
	if (i >= 0) {
		const ExportSimpleAttr& t = src.simple_attrs[i];
		attr_i0 = t.attr_i0;
		attr_i1 = t.attr_i1;
		attr_s0 = src.attrs.GetKey(attr_i0).value;
		attr_s1 = src.attrs.GetKey(attr_i1).value;
	}
	for(int i = 0; i < gvalues.GetCount(); i++) {
		String attr_s = gvalues[i];
		if (attr_s == attr_s0)
			SetColoredListValue(values, i, 0, attr_s, Green());
		else if (attr_s == attr_s1)
			SetColoredListValue(values, i, 0, attr_s, Red());
		else
			values.Set(i, 0, attr_s);
	}
	INHIBIT_CURSOR(values);
	values.SetCount(gvalues.GetCount());
	if (!values.IsCursor() && values.GetCount())
		values.SetCursor(0);
	
	i = uniq_attrs_i.Find(group);
	if (i < 0) {
		pos_values.SetCount(0);
		neg_values.SetCount(0);
	}
	else {
		const auto& indices = uniq_attrs_i[i];
		int prow = 0, nrow = 0;
		for(int i = 0; i < indices.GetCount(); i++) {
			const AttrHeader& ah = src.attrs.GetKey(indices[i]);
			const ExportAttr& ea = src.attrs[indices[i]];
			if (ea.positive) {
				pos_values.Set(prow, 0, ah.value);
				prow++;
			}
			else {
				neg_values.Set(nrow, 0, ah.value);
				nrow++;
			}
			pos_values.SetCount(prow);
			neg_values.SetCount(nrow);
		}
	}
	
}







TextDataDiagnostics::TextDataDiagnostics(DatasetProvider& o) : o(o) {
	Add(hsplit.HSizePos().VSizePos(0,30));
	
	hsplit.Horz() << values;
	
	values.AddColumn(t_("Key"));
	values.AddColumn(t_("Value"));
}

void TextDataDiagnostics::Data() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) {
		values.Clear();
		return;
	}
	auto& src = *p.srctxt;
	
	for(int i = 0; i < src.diagnostics.GetCount(); i++) {
		const String& key = src.diagnostics.GetKey(i);
		const String& value = src.diagnostics[i];
		
		values.Set(i, 0, key);
		
		if (key.Find("percentage") >= 0 && key.Find("fail percentage") < 0) {
			double perc = StrDbl(value);
			if (perc < 50)
				SetColoredListValue(values, i, 1, Format("%.3m", perc), Red());
			else
				values.Set(i, 1, value);
		}
		else values.Set(i, 1, value);
	}
	values.SetCount(src.diagnostics.GetCount());
	
}







TextDataWordnet::TextDataWordnet(DatasetProvider& o) : o(o) {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << vsplit << wordnets;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << attrs << colors;
	vsplit.SetPos(1000,0);
	
	attrs.AddColumn(t_("Group"));
	attrs.AddColumn(t_("Value"));
	attrs.AddIndex("GROUP");
	attrs.AddIndex("VALUE");
	attrs.ColumnWidths("1 1");
	attrs.WhenCursor << THISBACK(DataAttribute);
	
	colors.AddColumn(t_("Colors"));
	colors.WhenCursor << THISBACK(DataColor);
	
	wordnets.AddColumn(t_("Group"));
	wordnets.AddColumn(t_("Value"));
	wordnets.AddColumn(t_("Main class"));
	wordnets.AddColumn(t_("Anchor word"));
	wordnets.AddColumn(t_("#1 alternative"));
	wordnets.AddColumn(t_("#2 alternative"));
	wordnets.AddColumn(t_("#3 alternative"));
	wordnets.AddColumn(t_("#4 alternative"));
	wordnets.AddColumn(t_("#5 alternative"));
	wordnets.AddColumn(t_("#6 alternative"));
	wordnets.AddColumn(t_("#7 alternative"));
	wordnets.AddIndex("IDX");
	wordnets.ColumnWidths("1 2 1 1 1 1 1 1 1 1 1");
	
	
}

void TextDataWordnet::EnableAll() {
	
}

void TextDataWordnet::DisableAll() {
	
}

void TextDataWordnet::Data() {
	DataMain();
}

void TextDataWordnet::DataMain() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	
	
	int gi = 0;
	int i = 0;
	
	attrs.Set(i, 0, "All");
	attrs.Set(i, 1, "All");
	attrs.Set(i, "GROUP", -1);
	attrs.Set(i, "VALUE", -1);
	i++;
	
	#define ATTR_ITEM(e, g, i0, i1) \
		attrs.Set(i, 0, g); \
		attrs.Set(i, 1, i0); \
		attrs.Set(i, "GROUP", gi); \
		attrs.Set(i, "VALUE", 0); \
		i++; \
		attrs.Set(i, 0, g); \
		attrs.Set(i, 1, i1); \
		attrs.Set(i, "GROUP", gi); \
		attrs.Set(i, "VALUE", 1); \
		i++, gi++;
	ATTR_LIST
	#undef ATTR_ITEM

	if (!attrs.IsCursor() && attrs.GetCount())
		attrs.SetCursor(0);
	
	
	DataAttribute();
}

void TextDataWordnet::DataAttribute() {
	if (!attrs.IsCursor())
		return;
	
	
	
	colors.SetCount(1+GetColorGroupCount());
	colors.Set(0, 0, t_("All words"));
	for(int i = 0; i < GetColorGroupCount(); i++) {
		colors.Set(1+i, 0,
			AttrText("#" + IntStr(i))
				.NormalPaper(GetGroupColor(i)).NormalInk(Black())
				.Paper(Blend(GrayColor(), GetGroupColor(i))).Ink(White()));
	}
	if (colors.GetCount() && !colors.IsCursor())
		colors.SetCursor(0);
	
	
	DataColor();
}

void TextDataWordnet::DataColor() {
	if (!colors.IsCursor() || !attrs.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	
	int clr_i = colors.GetCursor();
	int attr_group_i = attrs.Get("GROUP");
	int attr_value_i = attrs.Get("VALUE");
	String group_str = attr_group_i >= 0 ? ToLower(AttrKeys[attr_group_i][1]) : String();
	String value_str = attr_group_i >= 0 ? ToLower(AttrKeys[attr_group_i][2 + attr_value_i]) : String();
	
	bool clr_filter = clr_i > 0;
	bool attr_filter = attr_group_i >= 0;
	clr_i--;
	
	lock.Enter();
	
	int row = 0;
	for(int i = 0; i < src.wordnets.GetCount(); i++) {
		ExportWordnet& wn = src.wordnets[i];
		
		// Filter by color group
		if (clr_filter && GetColorGroup(wn.clr) != clr_i)
			continue;
		
		// Filter by attribute
		if (attr_filter) {
			if (wn.attr < 0)
				continue;
			const AttrHeader& ah = src.attrs.GetKey(wn.attr);
			if (ah.group != group_str || ah.value != value_str)
				continue;
		}
		
		wordnets.Set(row, "IDX", i);
		
		if (wn.attr >= 0) {
			const AttrHeader& ah = src.attrs.GetKey(wn.attr);
			wordnets.Set(row, 0, ah.group);
			wordnets.Set(row, 1, ah.value);
		}
		else {
			wordnets.Set(row, 0, Value());
			wordnets.Set(row, 1, Value());
		}
		
		// Colored main class
		/*bool has_main_class_clr = false;
		Color main_class_clr;
		{
			int j = GetWordgroupColors().Find(wn.main_class);
			if (j >= 0) {
				main_class_clr = GetWordgroupColors()[j];
				has_main_class_clr = true;
			}
		}
		if (has_main_class_clr) {
			wordnets.Set(row, 2,
				AttrText(wn.main_class)
					.NormalPaper(Blend(main_class_clr, White(), 128+64)).NormalInk(Black())
					.Paper(Blend(main_class_clr, GrayColor())).Ink(White())
			);
		}
		else {*/
		if (wn.main_class >= 0)
			wordnets.Set(row, 2, src.word_classes[wn.main_class]);
		else
			wordnets.Set(row, 2, Value());
		//}
		
		
		// Anchor word
		if (wn.word_count > 0) {
			int w_i = wn.words[0];
			String w;
			if (w_i >= 0 && w_i < src.words_.GetCount())
				w = src.words_[w_i].text;
			else
				w = "<error>";
			wordnets.Set(row, 3,
				AttrText(w)
					.NormalPaper(Blend(wn.clr, White(), 128+64)).NormalInk(Black())
					.Paper(Blend(GrayColor(), wn.clr)).Ink(White()));
		}
		
		
		// Alternative words
		int c = min(8, wn.word_count);
		for(int j = 1; j < c; j++) {
			int w_i = wn.words[j];
			String w;
			if (w_i >= 0 && w_i < src.words_.GetCount())
				w = src.words_[w_i].text;
			else
				w = "<error>";
			SetColoredListValue(
				wordnets,
				row, 3+j, w,
				wn.word_clrs[j]);
		}
		for(int j = c; j < 8; j++)
			wordnets.Set(row, 3+j, Value());
		
		row++;
	}
	wordnets.SetCount(row);
	wordnets.SetSortColumn(1, false);
	
	
	lock.Leave();
}

void TextDataWordnet::ToolMenu(Bar& bar) {
	bar.Add(t_("Update data"), MetaImgs::BlueRing(), THISBACK(DataMain)).Key(K_CTRL_Q);
	bar.Separator();
	//bar.Add(t_("Make wordnets from template phrases"), MetaImgs::RedRing(), THISBACK1(DoWordnet, 0)).Key(K_F5);
	bar.Add(t_("Get color alternatives"), MetaImgs::RedRing(), THISBACK1(DoWordnet, 1)).Key(K_F6);
}

void TextDataWordnet::DoWordnet(int fn) {
	//TextLib::TaskManager& tm = GetTaskManager();
	//tm.DoWordnet(fn);
}













PhraseParts::PhraseParts(DatasetProvider& o) : o(o) {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << vsplit;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << texts << parts;
	
	texts.AddColumn(t_("Phrase"));
	texts.AddColumn(t_("Types"));
	texts.AddColumn(t_("Structural name"));
	texts.AddIndex("IDX");
	texts.ColumnWidths("1 1 1");
	texts.WhenBar << [this](Bar& bar){
		bar.Add("Copy virtual type", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 0);
			WriteClipboardText(text);
		});
		bar.Add("Copy type", [this]() {
			int i = texts.GetCursor();
			String text = texts.Get(i, 1);
			WriteClipboardText(text);
		});
	};
	
	parts.AddColumn(t_("Phrase"));
	parts.AddColumn(t_("Next scores"));
	parts.AddColumn(t_("Score sum"));
	parts.AddIndex("IDX");
	parts.ColumnWidths("6 3 1");
	
}

void PhraseParts::Data() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	
	int row = 0;
	for(int i = 0; i < src.phrase_parts.GetCount(); i++) {
		const PhrasePart& pp = src.phrase_parts[i];
		
		String struct_part_type, type_str;
		if (pp.virtual_phrase_part >= 0) {
			const VirtualPhrasePart& vpp = src.virtual_phrase_parts[pp.virtual_phrase_part];
			type_str = src.GetTypeString(vpp.word_classes);
			
			if (vpp.struct_part_type >= 0)
				const String& struct_part_type = src.struct_part_types[vpp.struct_part_type];
		}
		
		String phrase = src.GetWordString(pp.words);
		
		texts.Set(row, 0, phrase);
		texts.Set(row, 1, type_str);
		texts.Set(row, 2, struct_part_type);
		texts.Set(row, "IDX", i);
		row++;
		
		if (row >= 10000) break;
	}
	texts.SetCount(row);
	
	
	row = 0;
	for(int i = 0; i < src.action_phrases.GetCount(); i++) {
		const String& phrase = src.action_phrases.GetKey(i);
		const ExportDepActionPhrase& ap = src.action_phrases[i];
		
		parts.Set(row, 0, phrase);
		
		String ns;
		int score_sum = 0;
		for(int j = 0; j < ap.next_scores.GetCount(); j++) {
			if (j) ns << ", ";
			int row_sum = 0;
			for (int s : ap.next_scores[j])
				row_sum += s;
			ns << row_sum;
			score_sum += row_sum;
		}
		parts.Set(row, 1, ns);
		parts.Set(row, 2, score_sum);
		parts.Set(row, "IDX", i);
		row++;
		
		if (row >= 10000) break;
	}
	parts.SetCount(row);
	parts.SetSortColumn(2, true);
}

void PhraseParts::ToolMenu(Bar& bar) {
	bar.Add(t_("Update Data"), MetaImgs::BlueRing(), THISBACK(Data)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Get all line actions"), MetaImgs::RedRing(), THISBACK1(DoWords, 3)).Key(K_F5);
	bar.Separator();
	// Won't ever work--> bar.Add(t_("Get line change scores using existing"), MetaImgs::VioletRing(), THISBACK(DoWordsUsingExisting)).Key(K_F6);
	bar.Add(t_("Get line change scores"), MetaImgs::RedRing(), THISBACK1(DoWords, 4)).Key(K_F6);
	
}

void PhraseParts::DoWordsUsingExisting(int fn) {
	/*int lng_i = MetaDatabase::Single().GetOtherLanguageIndex();
	TextLib::TaskManager& tm = GetTaskManager();
	tm.DoWordsUsingExisting(lng_i, fn);*/
}

void PhraseParts::DoWords(int fn) {
	/*int lng_i = MetaDatabase::Single().GetOtherLanguageIndex();
	TextLib::TaskManager& tm = GetTaskManager();
	tm.DoWords(lng_i, fn);*/
}














ActionParallelsPage::ActionParallelsPage(DatasetProvider& o) : o(o) {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << vsplit << parallels;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << actions << action_args;
	
	actions.AddColumn(t_("Action"));
	actions.AddColumn(t_("Count"));
	actions.ColumnWidths("3 1");
	actions.WhenCursor << THISBACK(DataAction);
	
	action_args.AddColumn(t_("Action args"));
	action_args.AddColumn(t_("Count"));
	action_args.ColumnWidths("3 1");
	action_args.WhenCursor << THISBACK(DataActionHeader);
	
	parallels.AddColumn(t_("From action"));
	parallels.AddColumn(t_("From action arg"));
	parallels.AddColumn(t_("To action"));
	parallels.AddColumn(t_("To action arg"));
	parallels.AddColumn(t_("Score"));
	parallels.AddColumn(t_("Count"));
	parallels.AddColumn(t_("Weight"));
	parallels.AddIndex("IDX0");
	parallels.AddIndex("IDX1");
	parallels.ColumnWidths("2 2 2 2 1 1 1");
	
}

void ActionParallelsPage::Data() {
	
}

void ActionParallelsPage::ToolMenu(Bar& bar) {
	bar.Add(t_("Update data"), MetaImgs::BlueRing(), THISBACK(DataMain)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Update parallels"), MetaImgs::RedRing(), THISBACK(UpdateParallels)).Key(K_F6);
	
}

void ActionParallelsPage::DataMain() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	
	uniq_acts.Clear();
	for (const ActionHeader& ah : src.actions.GetKeys()) {
		uniq_acts.GetAdd(ah.action).GetAdd(ah.arg, 0)++;
	}
	struct Sorter {
		bool operator()(const VectorMap<String, int>& a, const VectorMap<String, int>& b) const {
			return a.GetCount() > b.GetCount();
		}
	};
	SortByValue(uniq_acts, Sorter());
	for (auto& v : uniq_acts.GetValues())
		SortByValue(v, StdGreater<int>());
	
	actions.Set(0, 0, "All");
	actions.Set(0, 1, src.actions.GetCount());
	for(int i = 0; i < uniq_acts.GetCount(); i++) {
		actions.Set(1+i, 0, uniq_acts.GetKey(i));
		actions.Set(1+i, 1, uniq_acts[i].GetCount());
	}
	actions.SetCount(1+uniq_acts.GetCount());
	if (!actions.IsCursor() && actions.GetCount())
		actions.SetCursor(0);
	
	DataAction();
}

void ActionParallelsPage::DataAction() {
	if (!actions.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	
	String action = actions.Get(0);
	int i = uniq_acts.Find(action);
	if (i < 0) {
		action_args.SetCount(1);
		action_args.Set(0, 0, "All");
		action_args.Set(0, 1, src.actions.GetCount());
	}
	else {
		auto& args = uniq_acts[i];
		action_args.Set(0, 0, "All");
		action_args.Set(0, 1, args.GetCount());
		for(int i = 0; i < args.GetCount(); i++) {
			action_args.Set(1+i, 0, args.GetKey(i));
			action_args.Set(1+i, 1, args[i]);
		}
		action_args.SetCount(1+args.GetCount());
	}
	if (!action_args.IsCursor() && action_args.GetCount())
		action_args.SetCursor(0);
	
	DataActionHeader();
}

void ActionParallelsPage::DataActionHeader() {
	if (!actions.IsCursor() || !action_args.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	
	String action = actions.Get(0);
	String action_arg = action_args.Get(0);
	bool filter_action = action != "All";
	bool filter_action_arg = action_arg != "All";
	
	int idx = -1;
	int row = 0;
	for(int i = 0; i < src.parallel.GetCount(); i++) {
		int from_a = src.parallel.GetKey(i);
		const ActionHeader& at0 = src.actions.GetKey(from_a);
		const auto& to_v = src.parallel[i];
		
		bool at0_skipped = false;
		if (filter_action) {
			if (at0.action != action || (filter_action_arg && at0.arg != action_arg))
				at0_skipped = true;
		}
		
		for(int j = 0; j < to_v.GetCount(); j++) {
			int to_a = to_v.GetKey(j);
			const ActionHeader& at1 = src.actions.GetKey(to_a);
			const ExportParallel& at = to_v[j];
			
			bool at1_skipped = false;
			if (filter_action && at0_skipped) {
				if (at1.action != action || (filter_action_arg && at1.arg != action_arg))
					at1_skipped = true;
			}
			if (at1_skipped && at1_skipped)
				continue;
			
			parallels.Set(row, "IDX0", i);
			parallels.Set(row, "IDX1", j);
			parallels.Set(row, 0, at0.action);
			parallels.Set(row, 1, at0.arg);
			parallels.Set(row, 2, at1.action);
			parallels.Set(row, 3, at1.arg);
			
			double score = at.count > 0 ? at.score_sum / at.count : 0;
			
			parallels.Set(row, 4, score);
			parallels.Set(row, 5, at.count);
			parallels.Set(row, 6, at.score_sum);
			
			row++;
			if (row >= 10000) break;
		}
		if (row >= 10000) break;
	}
	parallels.SetCount(row);
	parallels.SetSortColumn(6, true);
	if (!parallels.IsCursor() && parallels.GetCount())
		parallels.SetCursor(0);
}

void ActionParallelsPage::UpdateParallels() {
	//TextLib::TaskManager& tm = GetTaskManager();
	//tm.DoActionParallel(0);
}












ActionTransitionsPage::ActionTransitionsPage(DatasetProvider& o) : o(o) {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << vsplit << transitions;
	hsplit.SetPos(2000);
	
	vsplit.Vert() << actions << action_args;
	
	actions.AddColumn(t_("Action"));
	actions.AddColumn(t_("Count"));
	actions.ColumnWidths("3 1");
	actions.WhenCursor << THISBACK(DataAction);
	
	action_args.AddColumn(t_("Action args"));
	action_args.AddColumn(t_("Count"));
	action_args.ColumnWidths("3 1");
	action_args.WhenCursor << THISBACK(DataActionHeader);
	
	transitions.AddColumn(t_("From action"));
	transitions.AddColumn(t_("From action arg"));
	transitions.AddColumn(t_("To action"));
	transitions.AddColumn(t_("To action arg"));
	transitions.AddColumn(t_("Score"));
	transitions.AddColumn(t_("Count"));
	transitions.AddColumn(t_("Weight"));
	transitions.AddIndex("IDX0");
	transitions.AddIndex("IDX1");
	transitions.ColumnWidths("2 2 2 2 1 1 1");
	
}

void ActionTransitionsPage::Data() {
	
}

void ActionTransitionsPage::ToolMenu(Bar& bar) {
	bar.Add(t_("Update data"), MetaImgs::BlueRing(), THISBACK(DataMain)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Update transitions"), MetaImgs::RedRing(), THISBACK(UpdateTransitions)).Key(K_F6);
	
}

void ActionTransitionsPage::DataMain() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	#if 0
	SourceData& sd = db.src_data;
	SourceDataAnalysis& sda = db.src_data.a;
	DatasetAnalysis& da = sda.dataset;
	
	uniq_acts.Clear();
	for (const ActionHeader& ah : da.actions.GetKeys()) {
		uniq_acts.GetAdd(ah.action).GetAdd(ah.arg, 0)++;
	}
	struct Sorter {
		bool operator()(const VectorMap<String, int>& a, const VectorMap<String, int>& b) const {
			return a.GetCount() > b.GetCount();
		}
	};
	SortByValue(uniq_acts, Sorter());
	for (auto& v : uniq_acts.GetValues())
		SortByValue(v, StdGreater<int>());
	
	actions.Set(0, 0, "All");
	actions.Set(0, 1, da.actions.GetCount());
	for(int i = 0; i < uniq_acts.GetCount(); i++) {
		actions.Set(1+i, 0, uniq_acts.GetKey(i));
		actions.Set(1+i, 1, uniq_acts[i].GetCount());
	}
	actions.SetCount(1+uniq_acts.GetCount());
	if (!actions.IsCursor() && actions.GetCount())
		actions.SetCursor(0);
	
	DataAction();
	#endif
}

void ActionTransitionsPage::DataAction() {
	if (!actions.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	#if 0
	SourceData& sd = db.src_data;
	SourceDataAnalysis& sda = db.src_data.a;
	DatasetAnalysis& da = sda.dataset;
	
	
	String action = actions.Get(0);
	int i = uniq_acts.Find(action);
	if (i < 0) {
		action_args.SetCount(1);
		action_args.Set(0, 0, "All");
		action_args.Set(0, 1, da.actions.GetCount());
	}
	else {
		auto& args = uniq_acts[i];
		action_args.Set(0, 0, "All");
		action_args.Set(0, 1, args.GetCount());
		for(int i = 0; i < args.GetCount(); i++) {
			action_args.Set(1+i, 0, args.GetKey(i));
			action_args.Set(1+i, 1, args[i]);
		}
		action_args.SetCount(1+args.GetCount());
	}
	if (!action_args.IsCursor() && action_args.GetCount())
		action_args.SetCursor(0);
	
	DataActionHeader();
	#endif
}

void ActionTransitionsPage::DataActionHeader() {
	if (!actions.IsCursor() || !action_args.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	if (!p.srctxt) return;
	auto& src = *p.srctxt;
	#if 0
	SourceData& sd = db.src_data;
	SourceDataAnalysis& sda = db.src_data.a;
	DatasetAnalysis& da = sda.dataset;
	
	String action = actions.Get(0);
	String action_arg = action_args.Get(0);
	bool filter_action = action != "All";
	bool filter_action_arg = action_arg != "All";
	
	int idx = -1;
	int row = 0;
	for(int i = 0; i < da.trans.GetCount(); i++) {
		int from_a = da.trans.GetKey(i);
		const ActionHeader& at0 = da.actions.GetKey(from_a);
		const Vector<ExportTransition>& to_v = da.trans.GetValues(i);
		
		bool at0_skipped = false;
		if (filter_action) {
			if (at0.action != action || (filter_action_arg && at0.arg != action_arg))
				at0_skipped = true;
		}
		
		for(int j = 0; j < to_v.GetCount(); j++) {
			int to_a = da.trans.GetKey(i, j);
			const ActionHeader& at1 = da.actions.GetKey(to_a);
			const ExportTransition& at = to_v[j];
			
			bool at1_skipped = false;
			if (filter_action && at0_skipped) {
				if (at1.action != action || (filter_action_arg && at1.arg != action_arg))
					at1_skipped = true;
			}
			if (at1_skipped && at1_skipped)
				continue;
			
			transitions.Set(row, "IDX0", i);
			transitions.Set(row, "IDX1", j);
			transitions.Set(row, 0, at0.action);
			transitions.Set(row, 1, at0.arg);
			if (at0.action != at1.action || at0.arg != at1.arg) {
				transitions.Set(row, 2, at1.action);
				transitions.Set(row, 3, at1.arg);
			}
			else {
				// no change
				transitions.Set(row, 2, Value());
				transitions.Set(row, 3, Value());
			}
			
			double score = at.count > 0 ? at.score_sum / at.count : 0;
			
			transitions.Set(row, 4, score);
			transitions.Set(row, 5, at.count);
			transitions.Set(row, 6, at.score_sum);
			
			row++;
			if (row >= 10000) break;
		}
		if (row >= 10000) break;
	}
	transitions.SetCount(row);
	transitions.SetSortColumn(6, true);
	if (!transitions.IsCursor() && transitions.GetCount())
		transitions.SetCursor(0);
	#endif
}

void ActionTransitionsPage::UpdateTransitions() {
	//TextLib::TaskManager& tm = GetTaskManager();
	//tm.DoActionTransition(0);
}








SourceTextMergerCtrl::SourceTextMergerCtrl(DatasetProvider& o) : o(o) {
	CtrlLayout(conf);
	TabCtrl::Add(conf.SizePos(), "Conf");
	NoTransparent();
	
	conf.path.SetData("/common/sblo/Dev/local-upp/txtsrc/Database/NaturalLanguage.db-src");
	conf.context.SetData("");
	conf.language.SetData("english");
	conf.process <<= THISBACK1(Do, 0);
	
}

void SourceTextMergerCtrl::Data() {
	
}

void SourceTextMergerCtrl::Do(int fn) {
	ToolAppCtrl& app = dynamic_cast<ToolAppCtrl&>(o);
	DatasetPtrs p; o.GetDataset(p);
	String path = conf.path.GetData();
	String ctx = conf.context.GetData();
	String lang = conf.language.GetData();
	bool append = conf.append.Get();
	if (path.IsEmpty() || ctx.IsEmpty() || lang.IsEmpty()) {
		PromptOK("All parameters are required");
		return;
	}
	MergeProcess& sdi = MergeProcess::Get(p, path, lang, ctx, append);
	// causes crash
	/*app.prog.Attach(sdi); 
	sdi.WhenRemaining <<
		[this](String s) { PostCallback([this, s]() {
		dynamic_cast<ToolAppCtrl&>(o).remaining.SetLabel(s); }); };*/
	if(fn == 0)
		sdi.Start();
	else
		sdi.Stop();
}







SourceTextCtrl::SourceTextCtrl() :
	tk(*this),
	src(*this),
	vp(*this),
	vpp(*this),
	vps(*this),
	vpa(*this),
	vpa2(*this),
	aap(*this),
	att(*this),
	diag(*this),
	wn(*this),
	pp(*this),
	apar(*this),
	atra(*this),
	merger(*this)
{
	data_type.Add("Source");
	data_type.Add("Analyzed");
	data_type.Add("Tokens");
	data_type.Add("Ambiguous Word Pairs");
	data_type.Add("Virtual Phrases");
	data_type.Add("Virtual Phrase Parts");
	data_type.Add("Virtual Phrase Structs");
	data_type.Add("Phrase Part Analysis 1");
	data_type.Add("Phrase Part Analysis 2");
	data_type.Add("Action Attrs Page");
	data_type.Add("Attributes");
	data_type.Add("Text Data Diagnostics");
	data_type.Add("Wordnet");
	data_type.Add("Phrase-actions");
	data_type.Add("Parallels");
	data_type.Add("Transition");
	data_type.Add("Merger");
	data_type.SetIndex(0);
	data_type.WhenAction << THISBACK(SetDataCtrl);
	PostCallback(THISBACK(SetDataCtrl));
	
	Add(prog.BottomPos(0,30).HSizePos(300));
	Add(remaining.BottomPos(0,30).LeftPos(0,300));
	AddMenu();
	
}

void SourceTextCtrl::SetFont(Font fnt) {
	src.SetFont(fnt);
	//tk.SetFont(fnt);
}

void SourceTextCtrl::SetDataCtrl() {
	RemoveChild(&src);
	RemoveChild(&tk);
	RemoveChild(&vp);
	RemoveChild(&vpp);
	RemoveChild(&vps);
	RemoveChild(&vpa);
	RemoveChild(&vpa2);
	RemoveChild(&aap);
	RemoveChild(&att);
	RemoveChild(&diag);
	RemoveChild(&wn);
	RemoveChild(&pp);
	RemoveChild(&apar);
	RemoveChild(&atra);
	RemoveChild(&merger);
	
	int idx = data_type.GetIndex();
	switch (idx) {
		case 0:
		case 1: Add(src.SizePos()); break;
		case 2: Add(tk.SizePos()); break;
		case 3: Add(vp.SizePos()); break;
		case 4: Add(vpp.SizePos()); break;
		case 5: Add(vps.SizePos()); break;
		case 6: Add(vpa.SizePos()); break;
		case 7: Add(vpa2.SizePos()); break;
		case 8: Add(aap.SizePos()); break;
		case 9: Add(att.SizePos()); break;
		case 10: Add(diag.SizePos()); break;
		case 11: Add(wn.SizePos()); break;
		case 12: Add(pp.SizePos()); break;
		case 13: Add(apar.SizePos()); break;
		case 14: Add(atra.SizePos()); break;
		case 15: Add(merger.SizePos()); break;
	}
	Data();
}

void SourceTextCtrl::Data() {
	int idx = data_type.GetIndex();
	switch (idx) {
		case 0:
		case 1: src.SetDataType((AuthorDataCtrl::Type)idx); src.Data(); break;
		case 2: tk.Data(); break;
		case 3: vp.Data(); break;
		case 4: vpp.Data(); break;
		case 5: vps.Data(); break;
		case 6: vpa.Data(); break;
		case 7: vpa2.Data(); break;
		case 8: aap.Data(); break;
		case 9: att.Data(); break;
		case 10: diag.Data(); break;
		case 11: wn.Data(); break;
		case 12: pp.Data(); break;
		case 13: apar.Data(); break;
		case 14: atra.Data(); break;
		case 15: merger.Data(); break;
	}
}

void SourceTextCtrl::Visit(Vis& v) {
	if (v.IsLoading()) {
		VfsValue* n = 0;
		TODO // IdeMetaEnv().LoadFileRootVisit(GetFileIncludes(), GetFilePath(), v, true, n);
		//TODO register IdeMetaEnv().LoadFileRootVisit to a function pointer
		if (n)
			SetFileNode(n);
	}
	else {
		LOG("SourceTextCtrl::Visit(Vis& v): error: storing SourceTextCtrl is not yet implemented");
		v.SetError("not implemented");
		/*VfsSrcFile& file = RealizeFileRoot();
		file.MakeTempFromEnv(false);
		file.Visit(vis);
		file.ClearTemp();*/
	}
}

bool SourceTextCtrl::Load(const String& includes, const String& filename, Stream& in, byte charset) {
	Realize(includes, filename);
	String json = in.Get((int)in.GetSize());
	if (json.IsEmpty())
		return false;
	Value jv = ParseJSON(json);
	if(jv.IsError())
		return false;
	JsonIO io(jv);
	Vis vis(io);
	Visit(vis);
	return !vis.IsError();
}

void SourceTextCtrl::ToolMenu(Bar& bar) {
	// TODO improve gui look and user experience
	bar.Add(data_type, Size(200,24));
	bar.Separator();
	bar.Add(t_("Refresh"), MetaImgs::BlueRing(), THISBACK(Data)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
}

void SourceTextCtrl::Do(int fn) {
	int page = data_type.GetIndex();
	switch (page) {
		#if 0
		case 0: DoT<SourceDataImporter>(fn - 0); break;
		case 1: DoT<SourceAnalysisProcess>(fn - 2); break;
		case 2: DoT<TokenDataProcess>(fn - 4); break;
		case 3: DoT<AmbiguousWordPairsProcess>(fn); break; // awp
		case 4: DoT<VirtualPhrasesProcess>(fn); break; // vp
		case 5: DoT<VirtualPhrasePartsProcess>(fn); break; // vpp
		case 6: DoT<VirtualPhraseStructsProcess>(fn); break; // vps
		case 7: DoT<PhrasePartAnalysisProcess>(fn); break; // vpa
		case 8: DoT<PhrasePartAnalysisProcess>(fn); break; // vpa2
		case 9: DoT<ActionAttrsProcess>(fn); break; // aap
		case 10: DoT<AttributesProcess>(fn); break; // att
		case 11: break; // diag
		#endif
		case 16: merger.Do(fn); break;
		default: break;
	}
}


END_UPP_NAMESPACE
