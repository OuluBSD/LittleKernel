#ifndef _AI_Core_DataModel_Data_h_
#define _AI_Core_DataModel_Data_h_



typedef int ContentIdx;
inline int GetContentMain(ContentIdx i) {return i / 3;}
inline int GetContentSub(ContentIdx i) {return i % 3;}

typedef enum : int {
	TXT_INVALID = -1,
	TXT_NORMAL,
	TXT_PRE_REPEAT,
	TXT_REPEAT,
	TXT_TWIST,
	TXT_NULL,

	TXT_COUNT
} TextPartType;

#define CONTEXTLIST \
	CTX(0,CREATIVITY, Creativity, "Is the context focused on originality and expression?") \
	CTX(1,TECHNICALITY, Technicality, "Is there an emphasis on technical precision or functionality?") \
	CTX(2,EMOTIONALITY, Emotionality, " Does the context evoke emotions or aim to connect on an emotional level?") \
	CTX(3,EFFICIENCY, Efficiency, "Is the context concerned with optimization and performance?") \
	CTX(4,COLLABORATIVE, Collaborative, "Does the context involve teamwork or community involvement?") \
	CTX(5,STABILITY, Stability, "Is there a focus on reliability and consistency?") \
	CTX(6,INNOVATIVE, Innovative, "Is there an intention to introduce new ideas or techniques?") \
	CTX(7,EXPERIMENTAL, Experimental, "Is the context about trial and error or trying unproven methods?")

struct ContextType : Moveable<ContextType> {
	byte value = 0;
	
	typedef enum {
		#define CTX(a,b,c,d) b,
		CONTEXTLIST
		#undef CTX
		CTX_COUNT
	} Feature;
	
	typedef enum {
		#define CTX(a,b,c,d) b##_BIT = 1 << a,
		CONTEXTLIST
		#undef CTX
	} Bit;
	
	static ContextType Lyrical();
	static ContextType Programming();
	static ContextType PublicShortMessage();
	static ContextType PersonalBlog();
	static ContextType CorporateBlog();
	static ContextType Dialog();
	static ContextType Storyboard();
	static ContextType GetFromString(const String& s);
	static String GetName(const ContextType& t);
	static String GetBitName(int i);
	
	bool operator==(const ContextType& t) const {return value == t.value;}
	hash_t GetHashValue() const {return value;}
	void Visit(Vis& v) {v("value", value);}
};

struct ContextData : Moveable<ContextData> {
	struct Typeclass : Moveable<Typeclass> {
		String name;
		Vector<Vector<String>> entities;
		void Visit(Vis& v) {v.Ver(1)(1)("n",name)("e",entities);}
	};
	struct Content : Moveable<Content> {
		String name;
		Vector<String> parts;
		void Visit(Vis& v) {v.Ver(1)(1)("n",name)("p",parts);}
	};
	String name;
	Vector<Typeclass> typeclasses;
	Vector<Content> contents;
	Vector<String> part_names;
	Index<String> entity_groups;
	
	int FindAddEntityGroup(String s);
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("name", name)
			("typeclasses", typeclasses, VISIT_VECTOR)
			("contents", contents, VISIT_VECTOR)
			("part_names", part_names)
			("entity_groups", entity_groups)
			;
	}
};

struct TokenIdx : Moveable<TokenIdx> {
	byte pad;

	void Visit(Vis& v) {
		v.Ver(3);
		if (v.file_ver < 3) {
			int word_;
			v(1)("w", word_);
		}
	}
};

struct TokenText : Moveable<TokenText> {
	Vector<int> tokens;
	Vector<int> words;
	int virtual_phrase = -1;
	int phrase_part = -1;

	void Visit(Vis& v) {
		v.Ver(3)
		(1)("t",tokens)("vp", virtual_phrase)
		(2)("pp",phrase_part)
		(3)("w",words);
	}
	static hash_t GetHash(const Vector<int>& tokens) {CombineHash c; for (int tk : tokens) c.Do(tk); return c;}
	hash_t GetHashValue() const {return GetHash(tokens);}
};

struct WordPairType : Moveable<WordPairType> {
	int from = -1, to = -1;           // word index
	int from_type = -1, to_type = -1; // word class index

	void Visit(Vis& v) {v.Ver(1)(1)("f",from)("t",to)("ft",from_type)("tt",to_type);}
	
	hash_t GetHashValue() const
	{
		CombineHash c;
		c.Do(from).Put(1).Do(to);
		return c;
	}
};

struct VirtualPhrase : Moveable<VirtualPhrase> {
	Vector<int> word_classes;
	int virtual_phrase_struct = -1;

	void Visit(Vis& v) {v.Ver(1)(1)("wc",word_classes)("vps",virtual_phrase_struct);}
	
	static hash_t GetHash(const Vector<int>& word_classes)
	{
		CombineHash c;
		for(int wc_i : word_classes)
			c.Do(wc_i);
		return c;
	}
	hash_t GetHashValue() const {return GetHash(word_classes);}
};

struct VirtualPhrasePart : Moveable<VirtualPhrasePart> {
	Vector<int> word_classes;
	int struct_part_type = -1;
	int count = 0;

	void Visit(Vis& v) {v.Ver(1)(1)("wc",word_classes)("spt",struct_part_type)("c",count);}

	static hash_t GetHash(const Vector<int>& word_classes)
	{
		CombineHash c;
		for(int wc_i : word_classes)
			c.Do(wc_i).Put(1);
		return c;
	}
	hash_t GetHashValue() const {return GetHash(word_classes);}
};

struct VirtualPhraseStruct : Moveable<VirtualPhraseStruct> {
	Vector<int> virtual_phrase_parts;
	int struct_type = -1;

	void Visit(Vis& v) {v.Ver(1)(1)("vps",virtual_phrase_parts)("st",struct_type);}

	static hash_t GetHash(const Vector<int>& virtual_phrase_parts)
	{
		CombineHash c;
		for(int part : virtual_phrase_parts)
			c.Do(part).Put(1);
		return c;
	}
	hash_t GetHashValue() const
	{
		return GetHash(virtual_phrase_parts);
	}
};

struct PhrasePart : Moveable<PhrasePart> {
	Vector<int> words;
	int tt_i = -1;
	int virtual_phrase_part = -1;
	int attr = -1;
	int el_i = -1;
	Color clr = Black();
	Vector<int> actions;
	Vector<int> typecasts;
	Vector<int> contrasts;
	int scores[SCORE_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	byte lang = 0xFF;
	byte ctx = 0;

	bool HasScores() const
	{
		for(int i = 0; i < SCORE_COUNT; i++)
			if(scores[i] != 0)
				return true;
		return false;
	}
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("words", words)
			("tt_i", tt_i)
			("virtual_phrase_part", virtual_phrase_part)
			("attr", attr)
			("el_i", el_i)
			("clr", clr)
			("actions", actions)
			("typecasts", typecasts)
			("contrasts", contrasts);
		for(int i = 0; i < SCORE_COUNT; i++)
			v("s" + IntStr(i), scores[i]);
		v(2)("lang", lang)("ctx", ctx);
	}
	static hash_t GetHash(const Vector<int>& words)
	{
		CombineHash c;
		for(int w_i : words)
			c.Do(w_i).Put(1);
		return c;
	}
	hash_t GetHashValue() const {return GetHash(words);}
};

struct ScriptSuggestion : Moveable<ScriptSuggestion> {
	struct Part : Moveable<Part> {
		String name;
		Vector<String> lines;
		
	};
	Vector<Part> parts;
	int rank = -1;
	Vector<Vector<Vector<int>>> transfers;
	int scores[2] = {0,0};
	
	String GetText() const;
};

struct PhraseComb : Moveable<PhraseComb> {
	Vector<int> phrase_parts;
	
	void Visit(Vis& json) {json.Ver(1)(1)("phrase_parts", phrase_parts);}
};

struct TranslatedPhrasePart : Moveable<TranslatedPhrasePart> {
	String phrase;
	int scores[SCORE_COUNT] = {0,0,0,0,0,0,0,0,0,0};
	
	void Visit(Vis& json) {json.Ver(1)(1)("phrase", phrase); for(int i = 0; i < SCORE_COUNT; i++) json("score[" + IntStr(i) + "]", scores[i]);}
};

struct ExportAttr : Moveable<ExportAttr> {
	int simple_attr = -1, unused = -1;
	int positive = -1, link = -1;
	int count = 0;

	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("s", simple_attr)
			("u", unused)
			("p", positive)
			("l", link)
			("c", count);
	}
};

struct ExportAction : Moveable<ExportAction> {
	int attr = -1;
	Color clr;
	int count = 0;

	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("a", attr)
			("clr", clr)
			("c", count);
	}
};

struct ExportParallel : Moveable<ExportParallel> {
	int count = 0, score_sum = 0;

	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("c", count)
			("ss", score_sum);
	}
};

struct ExportTransition : Moveable<ExportTransition> {
	int count = 0, score_sum = 0;

	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("c", count)
			("ss", score_sum);
	}
};

struct ExportDepActionPhrase : Moveable<ExportDepActionPhrase> {
	Vector<int> actions;
	Vector<int> next_phrases;
	Vector<Vector<int>> next_scores;
	int first_lines = 0;
	int attr = -1;
	Color clr = Black();

	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("ac", actions)
			("np", next_phrases)
			("ns", next_scores)
			("fl", first_lines)
			("at", attr)
			("cl", clr);
	}
};

struct ExportWordnet : Moveable<ExportWordnet> {
	static const int MAX_WORDS = 64;
	int word_count = 0;
	int words[MAX_WORDS];
	int word_clr_count = 0;
	Color word_clrs[MAX_WORDS];
	int main_class = -1;
	int attr = -1;
	Color clr;
	int scores[SCORE_COUNT];
	
	ExportWordnet() {
		memset(words, 0xFF, sizeof(words));
		memset(word_clrs, 0, sizeof(word_clrs));
		memset(scores, 0, sizeof(scores));
	}
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("wc", word_count);
		for(int i = 0; i < MAX_WORDS; i++)
			v	("w" + IntStr(i), words[i])
				("wc" + IntStr(i), word_clrs[i]);
		v	("wcc", word_clr_count)
			("mc", main_class)
			("at", attr)
			("clr", clr);
		for(int i = 0; i < SCORE_COUNT; i++)
			v("sc" + IntStr(i), scores[i]);
	}
	static hash_t GetHash(const Vector<int>& words)
	{
		CombineHash c;
		for(int w_i : words)
			c.Do(w_i).Put(1);
		return c;
	}
	hash_t GetHashValue() const {
		CombineHash c;
		for(int i = 0; i < word_count; i++)
			c.Do(words[i]).Put(1);
		return c;
	}
};

struct ExportSimpleAttr : Moveable<ExportSimpleAttr> {
	int attr_i0 = -1, attr_i1 = -1;

	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("ai0", attr_i0)
			("ai1", attr_i1);
	}
};

ArrayMap<String, Ptr<VfsValueExt>>& DatasetIndex();

struct ScriptDataset : Moveable<ScriptDataset> {
	String title;
	String text;
	ContextType ctx;
	void Visit(Vis& v) {
		v.Ver(2)
		(1)	("name", title)
			("txt", text)
		(2)	("ctx", ctx, VISIT_NODE);
	}
};

struct AuthorDataset : Moveable<AuthorDataset> {
	String name;
	Vector<ScriptDataset> scripts;
	Vector<String> genres;
	ContextType ctx;
	
	ScriptDataset& GetAddScript(String title);
	void Visit(Vis& v) {
		v.Ver(2)
		(1)	("name", name)
			("scripts", scripts, VISIT_VECTOR)
			("genres", genres)
		(2)	("ctx", ctx, VISIT_NODE);
	}
};

struct ScriptStruct : Moveable<ScriptStruct> {
	ScriptStruct() {}
	ScriptStruct(const ScriptStruct& s) { *this = s; }
	ScriptStruct(ScriptStruct&& s) { *this = s; }
	void operator=(const ScriptStruct& s) { parts <<= s.parts; }

	struct SubSubPart : Moveable<SubSubPart> {
		Vector<int> token_texts;
		int el_i = -1;

		SubSubPart() {}
		SubSubPart(const SubSubPart& s) { *this = s; }
		void Visit(Vis& json) { json.Ver(1)(1)("token_texts", token_texts)("cls", el_i); }
		void operator=(const SubSubPart& s)
		{
			token_texts <<= s.token_texts;
			el_i = s.el_i;
		}
	};
	struct SubPart : Moveable<SubPart> {
		Vector<SubSubPart> sub;
		int el_i = -1;
		int repeat = 0; // TODO remove (see below)
		double repeat_ = 0;

		SubPart() {}
		SubPart(const SubPart& s) { *this = s; }
		void Visit(Vis& v) {
			v.Ver(2);
			if (v.file_ver == 1) {
				int repeat;
				v(1)("sub", sub, VISIT_VECTOR)("cls", el_i)("repeat", repeat);
				repeat_ = repeat;
			}
			else {
				v(2)("sub", sub, VISIT_VECTOR)("el_i", el_i)("repeat", repeat_);
				if (v.IsLoading()) repeat = (int)repeat_;
			}
		}
		void operator=(const SubPart& s)
		{
			sub <<= s.sub;
			el_i = s.el_i;
			repeat = s.repeat;
			repeat_ = s.repeat_;
		}
	};
	struct Part : Moveable<Part> {
		Vector<SubPart> sub;
		TextPartType type = TXT_INVALID;
		int num = -1;
		int el_i = -1, typeclass = -1;
		ContentIdx content = -1;

		Part() {}
		Part(const Part& p) { *this = p; }
		void Visit(Vis& v)
		{
			v.Ver(1)
			(1)	("sub", sub, VISIT_VECTOR)
				("type", (int&)type)
				("num", num)
				("cls", el_i)
				("tc", typeclass)
				("c", (int&)content);
		}
		void operator=(const Part& s)
		{
			sub <<= s.sub;
			type = s.type;
			num = s.num;
			el_i = s.el_i;
			typeclass = s.typeclass;
			content = s.content;
		}
	};
	Vector<Part> parts;
	String author, title;
	ContextType ctx;

	void Visit(Vis& json) {
		json.Ver(3)
		(1)("parts", parts, VISIT_VECTOR)
		(2)("author", author)("title", title)
		(3)("ctx", ctx, VISIT_NODE);
	}
	
	bool HasAnyClasses() const
	{
		for(const auto& p : parts) {
			if(p.el_i >= 0)
				return true;
			for(const auto& s : p.sub) {
				if(s.el_i >= 0)
					return true;
				for(const auto& ss : s.sub) {
					if(ss.el_i >= 0)
						return true;
				}
			}
		}
		return false;
	}
	double GetNormalScore() const;
};

struct WordData : Moveable<WordData> {
	byte lang = 0xFF;
	String text;
	String spelling;
	WString phonetic;
	int count = 0;
	Color clr;
	int word_class = -1;
	int translation = -1;
	
	static hash_t GetHash(byte lang, const String& text, int word_class) {
		CombineHash ch;
		ch.Put(lang).Do(text).Do(word_class);
		return ch;
	}
	hash_t GetHashValue() const {return GetHash(lang, text, word_class);}
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("lang", lang)
			("text", text)
			("spelling", spelling)
			("phonetic", phonetic)
			("count", count)
			("clr", clr)
			("word_class", word_class)
			("translation", translation)
			;
	}
};

struct WordTranslation : Moveable<WordTranslation> {
	static const int MAX_CLASS_COUNT = 8;
	
	Vector<int> translations;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("t", translations);
	}
};

struct TextKeypoint : Moveable<TextKeypoint> {
	struct Register : Moveable<Register> {
		int type = -1; // e.g. pronouns (I, you, he, they);
		int attr = -1, action = -1;
		int element = -1, typecast = -1, content = -1;
		Color clr;
	};
	Vector<int> token_texts;
	Vector<Register> registers;
	Vector<double> descriptor;
	int descriptor_cluster = -1;
	
};


struct SrcTextData : EntityData {
	String filepath;
	
	// INPUT
	Vector<AuthorDataset> authors;
	VectorMap<hash_t, ScriptStruct> scripts;
	
	// CONTEXT
	ArrayMap<ContextType, ContextData> ctxs;
	
	// WORDS
	VectorMap<String, TokenIdx> tokens;
	Vector<WordData> words_;
	VectorMap<String, VectorMap<hash_t,VectorMap<int,int>>> langwords;
	Vector<WordTranslation> translations;
	Index<String> word_classes;
	VectorMap<hash_t, WordPairType> ambiguous_word_pairs; // todo remove
	
	// PHRASES
	VectorMap<hash_t, TokenText> token_texts;
	VectorMap<hash_t, VirtualPhrase> virtual_phrases;
	VectorMap<hash_t, VirtualPhrasePart> virtual_phrase_parts;
	VectorMap<hash_t, VirtualPhraseStruct> virtual_phrase_structs;
	VectorMap<hash_t, PhrasePart> phrase_parts;
	Index<String> struct_part_types;
	Index<String> struct_types;
	VectorMap<String, ExportSimpleAttr> simple_attrs;
	
	// CLASSIFIERS
	Index<String> element_keys;
	VectorMap<AttrHeader, ExportAttr> attrs;
	VectorMap<ActionHeader, ExportAction> actions;
	
	// TRANSITION VALUES
	VectorMap<hash_t, ExportWordnet> wordnets;
	VectorMap<String, ExportDepActionPhrase> action_phrases;
	VectorMap<int, VectorMap<int, ExportTransition>> trans;
	VectorMap<int, VectorMap<int, ExportParallel>> parallel;
	
	// TEXT CLASSIFIER
	Vector<TextKeypoint> text_keypoints;
	
	// DIAGNOSTICS
	VectorMap<String, String> diagnostics;
	
	
	
	// TEMP
	String current_language;
	
	#if 0
	const Vector<ContentType>& GetContents() {return ctx.content.labels;}
	const Vector<String>& GetContentParts() {return ctx.content.parts;}
	//const Index<String>& GetTypeclasses() {return typeclasses;}
	int GetContentCount() {return ctx.content.labels.GetCount();}
	//int GetTypeclassCount() {return typeclasses.GetCount();}
	int GetLanguage() const {return lang;}
	#endif
	
	CLASSTYPE(SrcTextData)
	SrcTextData();
	int FindAnyWord(const String& s) const; // deprecated: avoid using
	int FindAnyWord(const String& s, byte lang) const;
	ContextData* FindContext(byte ctx);
	const ContextData* FindContext(byte ctx) const;
	AuthorDataset& GetAddAuthor(String name);
	String GetTokenTypeString(const TokenText& txt) const;
	String GetWordString(const Vector<int>& words) const;
	WString GetWordPronounciation(const Vector<int>& words) const;
	String GetTypeString(const Vector<int>& word_classes) const;
	String GetActionString(const Vector<int>& actions) const;
	String GetScriptDump(int i) const;
	Value GetScriptValue(int i) const;
	VectorMap<int, int> GetSortedElements();
	VectorMap<int, int> GetSortedElementsOfPhraseParts();
	
	String GetTokenTextString(const TokenText& txt) const;
	String GetTokenTextString(const Vector<int>& tokens) const;
	void Visit(Vis& s) override;
};

// see SRC_TXT_HEADER_ENABLE

struct SrcTxtHeader : Component {
	Time written;
	int64 size = 0;
	String sha1;
	Vector<String> files;
	int version = 1;
	void Visit(Vis& v) override;
	String GetName() const override {return "Source Database";}
	
	String filepath;
	One<SrcTextData> data;
	
	CLASSTYPE(SrcTxtHeader)
	SrcTxtHeader(VfsValue& owner) : Component(owner) {}
	~SrcTxtHeader();
	SrcTextData& Data() {if (data.IsEmpty()) data.Create(); return *data;}
	void RealizeData();
	bool LoadData();
	String SaveData();
	
	
};

INITIALIZE(SrcTxtHeader);



#endif
