#ifndef _AI_Common_h_
#define _AI_Common_h_

// TODO convert *Args classes to use Value based args file only (too many classes, when 1 is enough)





struct ModelArgs : Moveable<ModelArgs> {
	enum {
		FN_LIST,
		FN_RETRIEVE,
		FN_DELETE
	};
	int fn = FN_LIST;
	String model;
	
	void Jsonize(JsonIO& json) {
		json("fn", fn)
			("model", model)
			;
	}
	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};

struct CompletionArgs : Moveable<CompletionArgs> {
	typedef enum : int {
		NO_PROBABILITIES,
		MOST_LIKELY,
		LEAST_LIKELY,
		FULL_SPECTRUM
	} ShowProbs;
	
	String prompt;
	String model_name;
	double temperature = 1;
	int max_length = 2048;
	String stop_seq;
	double top_prob = 1;
	double frequency_penalty = 0; // between -2 and 2
	double presence_penalty = 0; // between -2 and 2
	int best_of = 1;
	String inject_start_text;
	String inject_restart_text;
	ShowProbs show_probabilities = NO_PROBABILITIES;
	
	void Jsonize(JsonIO& json) {
		json("prompt", prompt)
			("model_name", model_name)
			("temperature", temperature)
			("max_length", max_length)
			("stop_seq", stop_seq)
			("top_prob", top_prob)
			("frequency_penalty", frequency_penalty)
			("presence_penalty", presence_penalty)
			("best_of", best_of)
			("inject_start_text", inject_start_text)
			("inject_restart_text", inject_restart_text)
			("show_probabilities", (int&)show_probabilities)
			;
	}
	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};

typedef enum : int {
	MSG_NULL,
	MSG_USER,
	MSG_DEVELOPER,
	MSG_SYSTEM = MSG_DEVELOPER,
	MSG_ASSISTANT,
	MSG_TOOL,
	MSG_FUNCTION,
} AiMsgType;

inline String GetMessageTypeString(AiMsgType t) {
	switch(t) {
		case MSG_NULL: return "";
		case MSG_USER: return "User";
		case MSG_DEVELOPER: return "Developer";
		case MSG_ASSISTANT: return "Assistant";
		case MSG_TOOL: return "Tool";
		case MSG_FUNCTION: return "Function";
		default: return "<error>";
	}
}

typedef enum : int {
	REASONING_NULL = 0,
	REASONING_LOW,
	REASONING_MEDIUM,
	REASONING_HIGH,
} ReasoningEffort;

inline String GetReasoningEffortString(ReasoningEffort reasoning_effort) {
	switch (reasoning_effort) {
		case REASONING_LOW:    return "low";
		case REASONING_MEDIUM: return "medium";
		case REASONING_HIGH:   return "high";
		default: return "";
	}
}

struct ChatArgs : Moveable<ChatArgs> {
	
	struct ToolFunction : Moveable<ToolFunction> {
		String arguments, name;
		void Jsonize(JsonIO& json) {json("arguments",arguments)("name",name);}
	};
	struct ToolCall : Moveable<ToolCall> {
		ToolFunction function;
		String id, type;
		bool IsEmpty() const {return type.IsEmpty();}
		void Jsonize(JsonIO& json) {json("function",function)("id",id)("type",type);}
	};
	struct ToolDeclaration : Moveable<ToolDeclaration> {
		ToolFunction function;
		String type;
		bool IsEmpty() const {return type.IsEmpty();}
		void Jsonize(JsonIO& json) {json("function",function)("type",type);}
	};
	struct UrlCitation : Moveable<UrlCitation> {
		int end_index = -1, start_index = -1;
		String title, url;
		void Jsonize(JsonIO& json) {
			json("end_index",end_index)
				("start_index",start_index)
				("title",title)
				("url",url)
				;
		}
	};
	struct Annotation : Moveable<Annotation> {
		String type;
		UrlCitation url_citation;
		void Jsonize(JsonIO& json) {
			json("type",type)
				("url_citation",url_citation)
				;
		}
	};
	struct Message : Moveable<Message> {
		AiMsgType type = MSG_NULL;
		String content, name, refusal;
		Vector<ToolCall> tool_calls;
		Vector<Annotation> annotations;
		
		void Jsonize(JsonIO& json) {
			json("type",(int&)type)
				("content",content)
				("name",name)
				("refusal",refusal)
				("tool_calls",tool_calls)
				("annotations",annotations)
				;
		}
	};
	struct Prediction : Moveable<Prediction> {
		struct Part : Moveable<Part> {
			String text, type;
			void Jsonize(JsonIO& json) {json("text",text)("type",type);}
		};
		String content_txt;
		Vector<Part> content_parts;
		bool IsEmpty() const {return content_txt.IsEmpty() && content_parts.IsEmpty();}
		void Jsonize(JsonIO& json) {json("content_txt",content_txt)("content_parts",content_parts);}
	};
	struct ApproximateLocation : Moveable<ApproximateLocation> {
		String country, city, region;
		void Jsonize(JsonIO& json) {json("country",country)("city",city)("region",region);}
	};
	struct UserLocation : Moveable<UserLocation> {
		String type;
		ApproximateLocation approximate;
		void Jsonize(JsonIO& json) {json("type",type)("approximate",approximate);}
	};
	struct WebSearchOptions : Moveable<WebSearchOptions> {
		UserLocation user_location;
		ReasoningEffort search_context_size = REASONING_NULL;
		bool IsEmpty() const {return user_location.type.IsEmpty();}
		void Jsonize(JsonIO& json) {json("user_location",user_location)("search_context_size",(int&)search_context_size);}
	};
	Vector<Message> messages;
	String model_name;
	double frequency_penalty = 0; // between -2 and 2
	int max_completion_tokens = 0;
	Vector<String> modalities;
	int count = 1;
	Prediction prediction;
	double presence_penalty = 0; // between -2 and 2
	ReasoningEffort reasoning_effort = REASONING_NULL;
	double temperature = 1.0;
	ToolCall tool_choice;
	Vector<ToolDeclaration> tools;
	double top_prob = 1;
	String stop_seq;
	WebSearchOptions web_search_options;
	
	void Jsonize(JsonIO& json) {
		json("messages", messages)
			("model_name", model_name)
			("frequency_penalty", frequency_penalty)
			("max_completion_tokens", max_completion_tokens)
			("modalities", modalities)
			("count", count)
			("presence_penalty", presence_penalty)
			("prediction", prediction)
			("reasoning_effort", (int&)reasoning_effort)
			("temperature", temperature)
			("tool_choice", tool_choice)
			("tools", tools)
			("top_prob", top_prob)
			("stop_seq", stop_seq)
			("web_search_options", web_search_options)
			;
	}
	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};

struct VisionArgs : Moveable<VisionArgs> {
	String prompt;
	String jpeg;
	int max_length = 2048;
	
	void AppendHash(CombineHash& c) const {c.Do(jpeg);}
	void Jsonize(JsonIO& json) {
		json("prompt", prompt)
			("jpeg", jpeg)
			("max_length", max_length)
			;
	}
	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};

struct TranscriptionArgs {
	String prompt;
	int max_length = 2048;
	
	String file;
	String language;
	String misspelled;
	int ai_provider_idx = -1;

	void Jsonize(JsonIO& json) {
		json("prompt", prompt)
			("max_length", max_length)
			("ai_provider_idx", ai_provider_idx)
			("file", file)
			("language", language)
			("misspelled", misspelled);
	}
	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};

struct ImageArgs {
	String prompt;
	
	void Jsonize(JsonIO& json) {
		json("prompt", prompt)
			;
	}
	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};










#define TASKFN_TYPE int
typedef enum : TASKFN_TYPE {
	FN_INVALID = -1,
	
	FN_VOICEOVER_1_FIND_NATURAL_PARTS,
	FN_VOICEOVER_2A_SUMMARIZE,
	FN_VOICEOVER_2B_SUMMARIZE_TOTAL,
	
	FN_TRANSCRIPT_PROOFREAD_1,
	FN_PROOFREAD_STORYLINE_1,
	FN_STORYLINE_DIALOG_1,
	
	FN_ANALYZE_CONTEXT_TYPECLASSES,
	FN_ANALYZE_CONTEXT_CONTENTS,
	FN_ANALYZE_CONTEXT_PARTS,
	FN_ANALYZE_PUBLIC_FIGURE,
	FN_ANALYZE_ELEMENTS,
	FN_TOKENS_TO_LANGUAGES,
	FN_TOKENS_TO_WORDS,
	FN_WORD_CLASSES, // TODO Remove
	FN_WORD_PAIR_CLASSES,
	FN_CLASSIFY_SENTENCE,
	FN_CLASSIFY_SENTENCE_STRUCTURES,
	FN_CLASSIFY_PHRASE_ELEMENTS,
	FN_CLASSIFY_PHRASE_COLOR,
	FN_CLASSIFY_PHRASE_ATTR,
	FN_CLASSIFY_PHRASE_ACTIONS,
	FN_CLASSIFY_PHRASE_SCORES,
	FN_CLASSIFY_PHRASE_TYPECLASS,
	FN_CLASSIFY_PHRASE_CONTENT,
	FN_CLASSIFY_ACTION_COLOR,
	FN_CLASSIFY_PHRASE_ACTION_ATTR,
	FN_SORT_ATTRS,
	FN_ATTR_POLAR_OPPOSITES,
	FN_MATCHING_ATTR,
	FN_SORA_PRESETS,
	FN_SLIDESHOW_PROMPTS,
	FN_VIDEO_WEBSITE_DESCRIPTIONS,
	FN_VIDEO_ADS,
	FN_VIDEO_COVER_IMAGE,
	FN_ENGLISH_LYRICS,
	
} TaskFn;

struct TaskArgs : Moveable<TaskArgs> {
	TaskFn fn = FN_INVALID;
	Value params;
	
	void Jsonize(JsonIO& json) {json("fn", (TASKFN_TYPE&)fn)("params", params);}
	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};

struct GenericPromptArgs {
	int fn = 0;
	VectorMap<String, Vector<String>> lists;
	String response_path;
	String response_title;
	bool is_numbered_lines = false;

	void Jsonize(JsonIO& json) {
		json("lists", lists)("rp", response_path)("t", response_title)("nl", is_numbered_lines);
	}

	String Get() const { return StoreAsJson(*this); }
	void Put(const String& s) { LoadFromJson(*this, s); }
};

struct VfsSrcFile;

String GetStringRange(String content, Point begin, Point end);
bool UpdateVfsSrcFile(VfsSrcFile& f, const String& path);


// TextTool classes
// TODO optimize & merge

// TODO remove
struct SourceDataAnalysisArgs {
	int fn;
	String artist, song, text;
	Vector<String> words;
	Vector<String> phrases;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("artist", artist)
				("song", song)
				("text", text)
				("words", words)
				("phrases", phrases)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct TokenArgs {
	int fn;
	Vector<String> words;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("words", words)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct PhraseArgs {
	int fn;
	Vector<String> phrases;
	Vector<String> elements;
	Vector<String> typeclasses;
	Vector<String> contents;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("phrases", phrases)
				("elements", elements)
				("typeclasses", typeclasses)
				("contents", contents)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct ActionAnalysisArgs {
	int fn;
	Vector<String> actions;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("actions", actions)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct AttrArgs {
	int fn;
	String group;
	Vector<String> groups, values;
	String attr0, attr1;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("group", group)
				("groups", groups)
				("values", values)
				("attr0", attr0)
				("attr1", attr1)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct ScriptSolverArgs {
	int fn;
	int sub_fn = -1;
	String lng;
	VectorMap<String,String> artist, release, song;
	Vector<String> parts, attrs, phrases, scores, phrases2, styles;
	Vector<int> counts, offsets;
	String part, vision, ref;
	bool is_story = false;
	bool is_unsafe = false;
	bool is_self_centered = false;
	bool ret_fail = false;
	double factor = 0;
	Vector<String> elements;
	String rhyme_element;
	String previously;
	String peek;
	
	
	struct State {
		String			element;
		String			attr_group;
		String			attr_value;
		int				clr_i = -1;
		String			act_action;
		String			act_arg;
		String			typeclass;
		String			content, content_mod;
		String			style_type;
		String			style_entity;
		int				safety = 0;
		int				line_len = 0;
		int				connector = 0;
		String			line_begin;
		
		void Jsonize(JsonIO& json) {
			json	("element", element)
					("attr_group", attr_group)
					("attr_value", attr_value)
					("clr_i", clr_i)
					("act_action", act_action)
					("act_arg", act_arg)
					("typeclass", typeclass)
					("content", content)
					("content_mod", content_mod)
					("style_type", style_type)
					("style_entity", style_entity)
					("safety", safety)
					("line_len", line_len)
					("connector", connector)
					("line_begin", line_begin)
					;
		}
	};
	
	State state;
	Array<State> line_states;

	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("sub_fn", sub_fn)
				("lng", lng)
				("artist", artist)
				("release", release)
				("song", song)
				("parts", parts)
				("attrs", attrs)
				("phrases", phrases)
				("phrases2", phrases2)
				("styles", styles)
				("scores", scores)
				("counts", counts)
				("offsets", offsets)
				("part", part)
				("vision", vision)
				("ref", ref)
				("is_story", is_story)
				("is_unsafe", is_unsafe)
				("is_self_centered", is_self_centered)
				("ret_fail", ret_fail)
				("factor", factor)
				("elements", elements)
				("rhyme_element", rhyme_element)
				("state", state)
				("line_states", line_states)
				("previously", previously)
				("peek", peek)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct ConceptualFrameworkArgs {
	int fn = 0;
	VectorMap<String,String> elements;
	Vector<String> scores;
	String lyrics, genre;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("elements", elements)
				("scores", scores)
				("lyrics", lyrics)
				("genre", genre)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
};

struct SocialArgs {
	int fn = 0;
	String text, description, profile, photo_description;
	VectorMap<String,String> parts;
	int len = 0;
	
	void Jsonize(JsonIO& json) {
		json	("text", text)
				("desc", description)
				("fn", fn)
				("parts", parts)
				("len", len)
				("profile", profile)
				("photo_description", photo_description)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct BiographySummaryProcessArgs {
	int fn = 0;
	VectorMap<String,String> parts;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("parts", parts)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
};

struct SongHeaderArgs {
	int fn = 0;
	int tc_i = -1;
	int con_i = -1;
	String lyrics_idea;
	String music_style;
	
	void Jsonize(JsonIO& json) {
		json	("tc_i", tc_i)
				("con_i", con_i)
				("lyrics_idea", lyrics_idea)
				("music_style", music_style)
				("fn", fn)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct LeadSolverArgs {
	int fn = 0;
	int opp_i = 0;
	
	void Jsonize(JsonIO& json) {
		json	("opp_i", opp_i)
				("fn", fn)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

//TODO: rename to PerspectiveArgs
struct BeliefArgs {
	int fn = 0;
	Vector<String> user;
	Vector<String> pos, neg;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("user", user)
				("pos", pos)
				("neg", neg)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
	
};

struct MarketplaceArgs {
	int fn = 0;
	VectorMap<String,String> map;
	
	void Jsonize(JsonIO& json) {
		json	("fn", fn)
				("map", map)
				;
	}
	String Get() const {return StoreAsJson(*this);}
	void Put(const String& s) {LoadFromJson(*this, s);}
};

String AppendUnixFileName(String a, String b);
ValueMap& ValueToMap(Value& val);
ValueArray& ValueToArray(Value& val);
void RemoveColonTrail(String& s);
void RemoveCommentTrail(String& s);
double FractionDbl(const String& s);
String GetDurationString(double seconds);
String GetSizeString(uint64 bytes);
Size GetAspectRatio(Size sz);



#endif
