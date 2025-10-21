#ifndef _AI_Core_Content_Lyrical_h_
#define _AI_Core_Content_Lyrical_h_




String GetTextTypeString(int i);
void ParseTextPartType(String part_name, TextPartType& text_type, int& text_num);

typedef enum : int {
	VOICE_SINGING,
	VOICE_RAPPING,
	VOICE_POETRY,
	VOICE_DIALOG,
	VOICE_SKIP,

	VOICE_TYPE_COUNT
} VoiceType;

struct LineScore : Moveable<LineScore> {
	Vector<String> lines;
	Vector<int> scores;
	int line_n = 0;

	void Serialize(Stream& s) {
		int v = 1;
		s % v;
		if (v >= 1)
			s % lines % scores % line_n;
	}
	String Get(int i, int j) const;
	int GetScore(int i, int j) const;
	void SetCount(int i, int line_n);
	void Set(int i, int j, const String& s);
	void SetScore(int i, int j, int value);
	int GetCount() const;

	void Jsonize(JsonIO& json) { json("lines", lines)("scores", scores)("line_n", line_n); }
};

struct LineElement {
	String element;
	AttrHeader attr;
	ActionHeader act;
	int clr_i = -1;
	int typeclass_i = -1;
	int con_i = -1;
	hash_t sorter = 0;

	void Serialize(Stream& s) {
		int v = 1;
		s % v;
		if (v >= 1)
			s % element % attr % act % clr_i % typeclass_i % con_i % sorter;
	}
	void Jsonize(JsonIO& json)
	{
		json("e", element)("at", attr)("clr", clr_i)("a", act)("t", typeclass_i)("c", con_i)(
			"s", (int64&)sorter);
	}
	void Overlay(const LineElement& le);
};

struct DynLine : Moveable<DynLine> {
	String text;
	String user_text;
	/*String alt_text;
	String edit_text;
	String user_text;*/
	String expanded;
	Vector<String> suggs;
	int pp_i = -1;
	int end_pp_i = -1;
	LineElement el;

	// Suggestion variables
	int style_type = 0;
	int style_entity = 0;
	int safety = 0;
	int line_len = 0;
	int connector = 0;
	String line_begin;

	void Serialize(Stream& s) {
		int v = 1;
		s % v;
		if (v >= 1)
			s % text % user_text % expanded % suggs % pp_i % end_pp_i % el % style_type % style_entity % safety % line_len % connector % line_begin;
	}
	void Jsonize(JsonIO& json)
	{
		json("text", text)("user_text", user_text)/*("alt_text", alt_text)("edit_text", edit_text)("user_text", user_text)*/
			("ex", expanded)("el", el)("suggs", suggs)("pp_i", pp_i)(
			"end_pp_i", end_pp_i)("style_type", style_type)("style_entity", style_entity)(
			"safety", safety)("line_len", line_len)("connector", connector)("line_begin",
		                                                                    line_begin);
		if(json.IsLoading()) {
			String element;
			json("element", element);
			if(!element.IsEmpty() && el.element.IsEmpty())
				el.element = element;
		}
	}
	void CopySuggestionVars(const DynLine& dl)
	{
		style_type = dl.style_type;
		style_entity = dl.style_entity;
		safety = dl.safety;
		line_len = dl.line_len;
		connector = dl.connector;
		line_begin = dl.line_begin;
	}
};

struct DynSub : Moveable<DynSub> {
	LineElement el;
	Vector<DynLine> lines;
	String story;

	void Serialize(Stream& s) {
		int v = 1;
		s % v;
		if (v >= 1)
			s % el % lines % story;
	}
	void Jsonize(JsonIO& json)
	{
		json("lines", lines)("el", el)("story", story);
		if(json.IsLoading()) {
			String e0, e1;
			json("element0", e0);
			json("element1", e1);
			if(!e0.IsEmpty() && el.element.IsEmpty())
				el.element = e0;
			if(!e1.IsEmpty() && el.element.IsEmpty())
				el.element = e1;
		}
	}
};

struct DynPart {
	VoiceType voice_type = VOICE_SINGING;
	TextPartType text_type = TXT_NORMAL;
	int text_num = -1;
	int text_lines = 0;
	int text_lines_per_sub = 0;
	String person;
	LineElement el;
	Vector<DynSub> sub;
	Vector<int> phrase_parts;
	String story;

	void Serialize(Stream& s) {
		int v = 1;
		s % v;
		if (v >= 1)
			s	% (int&)voice_type
				% (int&)text_type
				% text_num
				% text_lines
				% text_lines_per_sub
				% person
				% el
				% sub
				% phrase_parts
				% story
				;
	}
	void Jsonize(JsonIO& json)
	{
		json("voice_type", (int&)voice_type)("text_type", (int&)text_type)(
			"text_num", text_num)("text_lines", text_lines)(
			"text_lines_per_sub", text_lines_per_sub)("person", person)("el", el)("sub", sub)(
			"phrase_parts", phrase_parts)("story", story);
		if(json.IsLoading()) {
			String element;
			json("element", element);
			if(!element.IsEmpty() && el.element.IsEmpty())
				el.element = element;
		}
	}
	String GetName() const;
	int GetExpectedLineCount() const;
	int GetContrastIndex() const;
	String GetLineElementString(int line) const;
};

// TODO remove ScriptPostFix class or fix the post-analysis
struct ScriptPostFix {
	struct Weak : Moveable<Weak> {
		int line_i = -1;
		int altline_i = -1;
		String description;
		void Jsonize(JsonIO& json) { json("l0", line_i)("l1", altline_i)("d", description); }
	};
	struct Improvement : Moveable<Improvement> {
		int line_i = -1;
		String text;
		void Jsonize(JsonIO& json) { json("l", line_i)("txt", text); }
	};
	struct Variation : Moveable<Variation> {
		String text;
		Vector<int> scores;
		int ScoreSum() const
		{
			int s = 0;
			for(int i : scores)
				s += i;
			return s;
		}
		double ScoreAv() const
		{
			int s = 0;
			for(int i : scores)
				s += i;
			return (double)s / scores.GetCount();
		}
		void Jsonize(JsonIO& json)
		{
			json("txt", text)("scores", scores);
			if(json.IsLoading())
				scores.SetCount(10, 0);
		}
	};
	Vector<Vector<Weak>> weaks;
	Vector<String> src_lines;
	VectorMap<int, String> src_line_parts;
	Vector<Vector<Improvement>> improvements;
	Vector<Variation> variations;

	void Jsonize(JsonIO& json)
	{
		json("weaks", weaks)("src_lines", src_lines)("src_line_parts", src_line_parts)(
			"improvements", improvements)("variations", variations);
	}
};

String GetStructText(const Array<DynPart>& parts, bool src_text);

struct LyricalStructure : Component {
	Array<DynPart>			parts;
	
	CLASSTYPE(LyricalStructure)
	LyricalStructure(VfsValue& owner) : Component(owner) {}
	~LyricalStructure() {}
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("parts", parts);
	}
	DynPart* FindPartByName(const String& name);
	void LoadStructuredText(const String& s);
	void LoadStructuredTextExt(const String& s, bool user_text);
	void SetText(const String& s, bool user_text);
	String GetStructText(bool user_text) const;
	
	
};

INITIALIZE(LyricalStructure);

// TODO rename to LyricsDraft
struct Script : Component {
	Vector<bool>			simple_attrs;
	Vector<int>				clr_list;
	Vector<bool>			actions_enabled;
	Vector<int>				phrase_parts[PART_COUNT];
	
	CLASSTYPE(Script)
	Script(VfsValue& owner) : Component(owner) {}
	~Script();
	void Store(Entity& a);
	void LoadTitle(Entity& a, String title);
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("simple_attrs", simple_attrs)
			("clr_list", clr_list)
			("actions_enabled", actions_enabled)
			;
		for(int i = 0; i < PART_COUNT; i++)
			v("phrase_parts["+IntStr(i)+"]", phrase_parts[i]);
	}
	
	
};

INITIALIZE(Script);

void ReplaceWord(String& s, const String& orig_word, const String& replace_word);
void HotfixReplaceWord(WString& ws);
void HotfixReplaceWord(String& s);

// Lyrics <-- previously ComponentAnalysis
struct Lyrics : Component {
	String					name;
	String					content_vision;
	String					copyright;
	String					description;
	String					lang;
	
	String					__text;
	VectorMap<int, String>	__suggestions;
	bool					is_unsafe = false;
	bool					is_story = false;
	bool					is_self_centered = false;
	
	/*
	VectorMap<hash_t,PhrasePart> phrase_parts[PART_COUNT];
	Index<int> source_pool[PART_COUNT];
	VectorMap<hash_t,PhraseComb> phrase_combs[PART_COUNT];
	VectorMap<hash_t,ScriptSuggestion> script_suggs;
	*/
	
	
	CLASSTYPE(Lyrics)
	Lyrics(VfsValue& owner) : Component(owner) {}
	~Lyrics() {}
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("name", name)
			("content_vision", content_vision)
			("copyright", copyright)
			("description", description)
			("lang", lang)
			("text", __text)
			("suggestions", __suggestions)
			("is_unsafe", is_unsafe)
			("is_story", is_story)
			("is_self_centered", is_self_centered)
			;
	}
	String GetText() const;
	
};

INITIALIZE(Lyrics);

/*struct Song : Component {
	VectorMap<String,String> data;
	
	Song(VfsValue& owner) : Component(owner) {}
	~Song(){}
	void Serialize(Stream& s) override {
		int v = 1; s % v;
		if (v >= 1) s % data;
	}
	void Jsonize(JsonIO& json) override {
		json("data", data) ;
	}
	hash_t GetHashValue() const override {
		CombineHash c;
		c.Do(data) ;
		return c;
	}
	
};

INITIALIZE(Song);*/



#endif
