#include "Prompting.h"

NAMESPACE_UPP

void AiTask::CreateInput_GetTokenData(BasicPrompt& input) {}
void AiTask::CreateInput_GetPhraseData(BasicPrompt& input) {}
void AiTask::CreateInput_GetAttributes(BasicPrompt& input) {}

void AiTask::CreateInput_Translate(BasicPrompt& input)
{
	String orig_lng = args[0];
	String orig_txt = args[1];
	String trans_lng = args[2];
	bool slightly_dialect = StrInt(args[3]);

	Vector<String> lines = Split(orig_txt, "\n", false);

	TaskTitledList& in_orig = input.AddSub().Title("Text 1 in " + orig_lng);
	in_orig.NoListChar();
	for(const String& line : lines)
		in_orig.Add(line);

	String t = "Text 1 in " + trans_lng;
	if(slightly_dialect)
		t += " and in slightly dialect";

	TaskTitledList& results = input.PreAnswer();
	results.Title(t);

	SetMaxLength(2048);
}

void AiTask::CreateInput_CreateImage(BasicPrompt& input)
{
	int count = StrInt(args[1]);
	int reduce_size_mode = StrInt(args[2]);
	int size = 0;
	switch(reduce_size_mode) {
	case 0:
		size = 1024;
		break;
	case 1:
		size = 512;
		break;
	case 2:
		size = 256;
		break;
	default:
		SetError("invalid 'reduce_size_mode'");
		return;
	}
	image_sz = IntStr(size) + "x" + IntStr(size);
	image_n = IntStr(count);

	input.PreAnswer().NoColon().Title(args[0]);
}

void AiTask::CreateInput_EditImage(BasicPrompt& input)
{
	int count = StrInt(args[1]);
	Image orig = send_images[0];
	int size = 0;
	Size sz = orig.GetSize();
	if(sz.cx != sz.cy) {
		SetError("Image must be square");
		return;
	}
	switch(sz.cx) {
	case 1024:
		size = 1024;
		break;
	case 512:
		size = 512;
		break;
	case 256:
		size = 256;
		break;
	default:
		SetError("invalid 'size'");
		return;
	}
	image_sz = IntStr(size) + "x" + IntStr(size);
	image_n = IntStr(count);

	input.PreAnswer().NoColon().Title(args[0]);

	skip_load = true;
}

void AiTask::CreateInput_VariateImage(BasicPrompt& input)
{
	int count = StrInt(args[0]);
	Image orig = send_images[0];
	int size = 0;
	Size sz = orig.GetSize();
	if(sz.cx != sz.cy) {
		SetFatalError("Image must be square");
		return;
	}
	switch(sz.cx) {
	case 1024:
		size = 1024;
		break;
	case 512:
		size = 512;
		break;
	case 256:
		size = 256;
		break;
	default:
		SetFatalError("invalid 'size'");
		return;
	}
	image_sz = IntStr(size) + "x" + IntStr(size);
	image_n = IntStr(count);

	input.PreAnswer().NoColon().Title(
		"DUMMY PROMPT! NEVER SENT IN VARIATE MODE! PREVENTS FAILING FOR 'NO-INPUT'");

	// skip_load = true;
}

void AiTask::CreateInput_Vision(BasicPrompt& input)
{
	this->type = TYPE_VISION;

	if(args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}

	VisionArgs args;
	args.Put(this->args[0]);
	{
		{
			input.AddSub()
				.Title("Task: describe content of the image in a detailed way, which enables "
			           "the regeneration of the image using generative AI")
				.NoColon();
		}
	}
}

void AiTask::CreateInput_Transcription(BasicPrompt& input)
{
	this->type = TYPE_TRANSCRIPTION;
	TODO //raw_input = this->args[0]; // hash is made of raw_input == arguments
}

void AiTask::CreateInput_DefaultBasic(BasicPrompt& input)
{
	if(args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}

	TaskArgs args;
	args.Put(this->args[0]);

	if(args.fn == FN_VOICEOVER_1_FIND_NATURAL_PARTS) {
		ValueMap params = args.params;
		ValueArray lines = params("lines");
		{
			auto& list = input.AddSub()
				.Title("Transcript")
				.NoListChar();
			for(int i = 0; i < lines.GetCount(); i++)
				list.Add("line #" + IntStr(i) + " " + lines[i].ToString());
		}{
			auto& results = input.PreAnswer()
				.Title("List of 1-4 best lines  to split transcript");
			results.Add("line #");
		}
		SetMaxLength(50);
		tmp_str = "line #";
	}
	else if(args.fn == FN_VOICEOVER_2A_SUMMARIZE) {
		ValueMap params = args.params;
		String scene = params("scene");
		String people = params("people");
		String language = params("language");
		if (language.IsEmpty()) language = "English";
		String transcript = params("transcription_part");
		input.AddSub().Title("Scene").NoListChar().Add(scene);
		input.AddSub().Title("People").NoListChar().Add(people);
		if (params.Find("total_summarization") >= 0)
			input.AddSub().Title("Summarization of all parts").NoListChar().Add(params("total_summarization"));
		if (params.Find("prev_summarization") >= 0)
			input.AddSub().Title("Summarization of previous part").NoListChar().Add(params("prev_summarization"));
		input.AddSub().Title("Transcript of the current part").NoListChar().Add(transcript);
		{
			auto& results = input.PreAnswer()
				.Title("Make voiceover summarization of transcription of the current part in " + language + " (from 1st person perspective)")
				.NoListChar();
			results.Add("");
		}
		SetMaxLength(2048);
	}
	else if(args.fn == FN_VOICEOVER_2B_SUMMARIZE_TOTAL) {
		ValueMap params = args.params;
		String language = params("language");
		if (language.IsEmpty()) language = "English";
		input.AddSub().Title("Summarization of all previous parts").NoListChar().Add(params("total_summarization"));
		input.AddSub().Title("Summarization of current part").NoListChar().Add(params("summarization"));
		{
			auto& results = input.PreAnswer()
				.Title("Merge summarization of all previous parts and summarization of current part in " + language)
				.NoListChar();
			results.Add("");
		}
		SetMaxLength(2048);
	}
	else if(args.fn == FN_TRANSCRIPT_PROOFREAD_1) {
		String text = args.params("text");
		TranscriptResponse r;
		LoadFromJson(r, text);
		String misspelled = args.params("misspelled");
		if (!misspelled.IsEmpty()) {
			auto& l = input.AddSub()
				.Title("Misspelled words");
			misspelled.Replace(",", " ");
			Vector<String> words = Split(misspelled, " ");
			for (auto& w : words)
				l.Add(w);
		}{
			auto& l = input.AddSub()
				.Title("List of dialog segments in " + r.language)
				.NoListChar();
			for(int i = 0; i < r.segments.GetCount(); i++) {
				l.Add("#" + IntStr(i) + ": " + r.segments[i].text);
			}
		}{
			auto& results = input.PreAnswer()
				.Title("List of proofread of previous text in English, and with unique lines only, and with corrected grammar. Line format (- #original line: translation). Skip original lines with duplicate text or low content value")
				;
			results.Add("#0:");
		}
		SetMaxLength(2048);
	}
	else if(args.fn == FN_PROOFREAD_STORYLINE_1 ||
			args.fn == FN_STORYLINE_DIALOG_1) {
		ValueArray arr = args.params("proofread");
		String scene = args.params("scene");
		String people = args.params("people");
		String storyline = args.params("storyline");
		{
			auto& l = input.AddSub()
				.Title("List of dialog segments")
				.NumberedLines();
			for(int i = 0; i < arr.GetCount(); i++) {
				l.Add(arr[i]);
			}
		}
		if (!scene.IsEmpty()) {
			auto& l = input.AddSub()
				.Title("Scene")
				.NoListChar();
			l.Add(scene);
		}
		if (!people.IsEmpty()) {
			auto& l = input.AddSub()
				.Title("People")
				.NoListChar();
			l.Add(people);
		}
		if (!storyline.IsEmpty()) {
			auto& l = input.AddSub()
				.Title("Storyline")
				.NoListChar();
			l.Add(storyline);
		}
		if (args.fn == FN_PROOFREAD_STORYLINE_1) {
			auto& results = input.PreAnswer()
				.Title("Storyline of previous list")
				.NoListChar();
			results.Add("");
		}
		else if (args.fn == FN_STORYLINE_DIALOG_1) {
			auto& results = input.PreAnswer()
				.Title("New dialog for a play based on the original dialog and the storyline")
				.NoListChar();
			results.Add("");
		}
		SetMaxLength(2048);
	}
	else if(args.fn == FN_STORYLINE_DIALOG_1) {
		
		
		
	}
	else TODO
}

void AiTask::CreateInput_FarStage(JsonPrompt& json_input) {
	if (vargs.IsNull()) {
		SetFatalError("no args");
		return;
	}
	if (!stage) {
		SetFatalError("stage ptr is null");
		return;
	}
	if (fn_i < 0 || fn_i >= stage->funcs.GetCount()) {
		SetFatalError("function index is invalid");
		return;
	}
	
	const FarStage& stage = *this->stage;
	const FarStage::Function& func = stage.funcs[fn_i];
	
	if (stage.system.GetCount())
		json_input.AddSystem(stage.system);
	else
		json_input.AddDefaultSystem();
	
	json_input.AddAssist(stage.body);
	
	auto& user = json_input.AddUser(func.body);
	ValueMap args = vargs;
	for(int i = 0; i < args.GetCount(); i++) {
		String key = args.GetKey(i);
		Value val = args.GetValue(i);
		user.Set(key, val);
	}
	if (stage.max_tokens)
		SetMaxLength(stage.max_tokens);
	else
		SetMaxLength(2048);
}

void AiTask::CreateInput_DefaultJson(JsonPrompt& json_input)
{
	if(args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}

	TaskArgs args;
	args.Put(this->args[0]);
	
	String content_policy_fix = "If the content violates the content policies, first convert the content to be allowed (add the word 'almost' before the violating word), and then continue as normal.";
	
	if(args.fn == FN_ANALYZE_CONTEXT_TYPECLASSES) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "__comment__": {
		            "context name": "generic",
		            "typecasts": [
						)ML"
			            #define TYPECAST(idx, str, c) "\"" str "\","
						TYPECAST_LIST
						#undef TYPECAST
						R"ML(
					],
		            "number of typecasts": )ML" + IntStr(TYPECAST_COUNT) + R"ML(
				},
				"typecasts": {
					"context name": "lyrical",
					"context bits": [
						"Creativity",
						"Emotionality",
						"Efficiency",
						"Collaborative",
						"Stability",
						"Innovative",
						"Experimental"
					],
					"typecast count": )ML" + IntStr(TYPECAST_COUNT) + R"ML(,
					"__comment__": "convert generic typecasts for the new context"
				}
			},
			"response": {
				"__comment__": "following 'typecasts' array must have )ML" + IntStr(TYPECAST_COUNT) + R"ML( string values",
				"context name": "lyrical",
				"typecast count": )ML" + IntStr(TYPECAST_COUNT) + R"ML(,
				"typecasts" : [
					"Heartbroken/lovesick",
					"Rebel/anti-establishment",
					"Political activist",
					"Social justice advocate",
					"Party/club",
					"Hopeful/dreamer",
					"Confident/empowered",
					"Vulnerable/raw",
					"Romantic/love-driven",
					"Failure/loser",
					"Spiritual/faithful",
					"Passionate/determined",
					"Reflective/self-reflective",
					"Witty/sarcastic",
					"Melancholic/sad",
					"Humble/down-to-earth",
					"Charismatic/charming",
					"Resilient/overcoming adversity",
					"Carefree/joyful",
					"Dark/mysterious",
					"Comical/humorous",
					"Controversial/provocative",
					"Nostalgic/sentimental",
					"Wise/philosophical",
					"Angry/outspoken",
					"Calm/peaceful.",
					"Confident/self-assured",
					"Self-destructive/self-sabotaging",
					"Hopeful/optimistic",
					"Fearful/anxious",
					"Eccentric/quirky",
					"Sensitive/emotional",
					"Bitter/resentful",
					"Unique/nonconformist",
					"Free-spirited/nonconformist",
					"Sultry/seductive",
					"Inspirational/motivational",
					"Authentic/real",
					"Mysterious/enigmatic",
					"Carefree/bohemian",
					"Street-smart/tough",
					"Romantic/idealistic",
					"Nurturing/motherly",
					"Dark/tormented",
					"Remorseful/regretful",
					"Bold/brave",
					"Outcast/rebel",
					"Lost/disconnected",
					"Tough/badass",
					"Sincere/genuine",
					"Honest/vulnerable",
					"Innocent/naive",
					"Bold/risk-taking"
				]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "typecasts": {
					"context name": "",
					"context bits": [],
					"typecast count": )ML" + IntStr(TYPECAST_COUNT) + R"ML(,
		            "__comment__": "get response"
		        }
		    }
		})ML")
			.Set("/query/typecasts/context name", args.params("context name"))
			.Set("/query/typecasts/context bits", args.params("context bits"))
			;
		SetMaxLength(2048);
	}
	else if(args.fn == FN_ANALYZE_CONTEXT_CONTENTS) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "__comment__": {
		            "context name": "generic",
		            "contents": [
						)ML"
						#define CONTENT(idx, str) "\"" str "\","
						CONTENT_LIST
						#undef CONTENT
						R"ML(
					],
		            "number of contents": )ML" + IntStr(CONTENT_COUNT) + R"ML(
				},
				"contents": {
					"context name": "lyrical",
					"context bits": [
						"Creativity",
						"Emotionality",
						"Efficiency",
						"Collaborative",
						"Stability",
						"Innovative",
						"Experimental"
					],
					"content count": )ML" + IntStr(CONTENT_COUNT) + R"ML(,
					"__comment__": "convert generic contents for the new context"
				}
			},
			"response": {
				"__comment__": "following 'contents' array must have )ML" + IntStr(CONTENT_COUNT) + R"ML( string values",
				"context name": "lyrical",
				"content count": )ML" + IntStr(CONTENT_COUNT) + R"ML(,
				"contents" : [
					"Seductive intro",
					"Rise and fall",
					"Fun and games",
					"Love at first sight",
					"Struggle and triumph",
					"Ups and downs",
					"Escape to paradise",
					"Rebellious spirit",
					"Broken and mended",
					"Chase your dreams",
					"Dark secrets",
					"Rags to riches",
					"Lost and found",
					"Ignite the fire",
					"From the ashes",
					"Fame and fortune",
					"Healing in the darkness",
					"City lights and lonely nights",
					"Breaking the mold",
					"Haunted by the past",
					"Wild and free",
					"Clash of opinions",
					"Long distance love",
					"Finding inner strength",
					"Living a double life",
					"Caught in the spotlight",
					"Love and war",
					"The art of letting go",
					"Living in the moment",
					"Conquering fears"
				]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "contents": {
					"context name": "",
					"context bits": [],
					"content count": )ML" + IntStr(CONTENT_COUNT) + R"ML(,
		            "__comment__": "get response"
		        }
		    }
		})ML")
			.Set("/query/contents/context name", args.params("context name"))
			.Set("/query/contents/context bits", args.params("context bits"))
			;
		SetMaxLength(2048);
	}
	else if(args.fn == FN_ANALYZE_CONTEXT_PARTS) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "__comment__": {
		            "context name": "generic",
		            "component names": ["begin","middle","end"],
		            "part count": )ML" + IntStr(CONTENT_COUNT) + R"ML(,
		            "number of components in a part": 3
				},
				"parts": {
					"context name": "lyrical",
					"context bits": [
						"Creativity",
						"Emotionality",
						"Efficiency",
						"Collaborative",
						"Stability",
						"Innovative",
						"Experimental"
					],
					"part count": )ML" + IntStr(CONTENT_COUNT) + R"ML(,
					"__comment__": "generate generic parts for the new context"
				}
			},
			"response": {
				"__comment__": "following 'parts' array must have )ML" + IntStr(CONTENT_COUNT) + R"ML( string values",
				"context name": "lyrical",
				"part count": )ML" + IntStr(CONTENT_COUNT) + R"ML(,
		        "number of components in a part": 3,
				"parts" : [["a seductive and sultry melody draws the listener in","the scripts talk about a passionate and intense relationship","the mood shifts as the singer realizes they are not truly in love"],["the beat builds and intensifies, creating a sense of excitement and anticipation","the scripts tell a story of overcoming obstacles and achieving success","the energy drops suddenly and the singer reflects on the sacrifices and struggles that came with their success"],["a carefree and lively melody sets the tone for a carefree party anthem","the scripts are about enjoying life and living in the moment","the party comes to an end and the reality of responsibilities and consequences sink in"],["a romantic and dreamy melody introduces the concept of falling in love at first sight","the scripts describe the intense feelings and desires that come with falling for someone instantly","the singer wakes up from the fantasy and realizes"],["a slower and melancholic melody sets the scene for a character facing challenges and adversity","the scripts depict the struggles and hardships they have faced","the pace picks up and the music becomes more triumphant as the character overcomes their struggles and achieves success"],["a catchy and upbeat melody reflects the highs of a new relationship","the scripts delve into the challenges and conflicts that arise within the relationship","the music slows down as the couple try to work through their problems and find a resolution"],["a tropical and laid-back beat transports the listener to a paradise destination","the scripts describe a desire to escape from reality and find solace in a beautiful location","the singer comes back to reality and faces the consequences of leaving everything behind"],["a rebellious and edgy guitar riff sets the rebellious tone of the song","the scripts speak of breaking rules and societal expectations","the song ends with the realization that rebellion can have consequences"],["a somber and melancholic melody reflects a heartbroken state","the scripts describe the pain and sadness of a broken relationship","the tone shifts as the singer begins to heal and move on from the heartbreak"],["an uplifting and motivational melody encourages listeners to chase their dreams","the scripts tell a story of overcoming obstacles and pursuing one's passions","the song concludes with a sense of fulfillment and the realization that the journey towards achieving dreams is never-ending"],["a haunting and mysterious introduction sets the tone for secrets and deceit","the scripts reveal dark secrets and hidden motives among the characters","the song ends with a sense of betrayal and the consequences of keeping secrets"],["a humble and modest melody represents the beginnings of a character's journey","the scripts describe the climb to success and wealth","the music becomes more grandiose as the character achieves their dreams and reflects on their journey"],["a haunting and melancholic melody portrays a sense of being lost and alone","the scripts depict a journey of self-discovery and finding one's place in the world","the music becomes more uplifting as the character finds a sense of belonging and purpose"],["an energetic and intense beat sparks excitement and passion","the scripts describe the power and intensity of a new love or passion","the music dies down as the flame fades and the singer is left with the memories of the passion that once consumed them"],["a slow and mournful melody sets the scene for a character who has hit rock bottom","the scripts depict the struggles and hardships they have faced","the music picks up as the character rises from the ashes and rebuilds their life"],["a flashy and upbeat melody represents the allure of fame and fortune","the scripts describe the glamorous lifestyle and perks that come with success","the song ends with a cautionary tale about the emptiness and pitfalls of a life solely focused on money and fame"],["a haunting and ethereal melody reflects a state of darkness and pain","the scripts speak of finding light and healing in the darkest times","the music builds to a triumphant and uplifting finale as the singer finds strength and hope in their struggles"],["a bustling and energetic beat represents the excitement of the city at night","the scripts tell a story of chasing dreams and living life to the fullest in the city","the song ends with a sense of loneliness and longing for something more meaningful outside of the fast-paced city life"],["a unique and unconventional melody sets the tone for breaking the norm","the scripts describe defying expectations and being true to oneself","the song ends with a sense of liberation and empowerment as the singer embraces their individuality"],["a haunting and eerie melody reflects the weight of a character's past traumas","the scripts delve into the pain and struggles of moving on from the past","the music becomes more hopeful as the character learns to let go and move forward"],["a carefree and adventurous melody embodies the thrill of living life on the edge","the scripts describe the rush and excitement of taking risks and living in the moment","the song concludes with a reminder that with freedom comes consequences and responsibilities"],["a catchy and upbeat melody sets the tone for a heated argument","the scripts depict conflicting opinions and viewpoints","the song ends with the understanding that sometimes it's best to agree to disagree and move on"],["a soft and tender melody represents the longing and distance in a relationship","the scripts tell a story of the struggles and sacrifices of maintaining a long distance love","the song ends with a sense of hope and determination to make the relationship work"],["a slow and contemplative melody represents a character facing inner struggles","the scripts speak of finding courage and strength from within to overcome challenges","the song crescendos as the singer embraces their inner strength and triumphs over their struggles"],["a mysterious and seductive beat sets the stage for a character leading a secretive life","the scripts tell the story of juggling two separate identities and the dangers that come with it","the song concludes with the realization that living a lie is destructive and unsustainable"],["a bright and flashy melody reflects the thrill of being in the spotlight","the scripts depict the pressure and challenges of fame and constantly being in the public eye","the music slows down as the singer reflects on the toll fame has taken on their personal life"],["a powerful and intense beat represents the passionate and tumultuous nature of love","the scripts depict a couple's constant battle and struggle to make their relationship work","the song ends with a bittersweet realization that love can be both beautiful and painful"],["a slow and somber melody sets the tone for learning to let go","the scripts describe the struggles of moving on and leaving the past behind","the music builds to a hopeful and empowering finale as the singer finally finds the strength to let go"],["an upbeat and carefree melody represents living life with no regrets","the scripts encourage taking chances and embracing every moment","the song ends with a reminder to cherish the present and not dwell on the past or worry about the future"],["a tense and ominous melody reflects the fear and anxiety a character faces","the scripts speak of overcoming fears and finding courage to face them","the music becomes triumphant and uplifting as the character conquers their fears and grows stronger"]]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "parts": {
					"context name": "",
					"context bits": [],
					"component names": [],
					"part count": )ML" + IntStr(CONTENT_COUNT) + R"ML(,
		            "__comment__": "get response"
		        }
		    }
		})ML")
			.Set("/query/parts/context name", args.params("context name"))
			.Set("/query/parts/context bits", args.params("context bits"))
			.Set("/query/parts/component names", args.params("part names"))
			;
		SetMaxLength(2048);
	}
	else if(args.fn == FN_ANALYZE_PUBLIC_FIGURE) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "__comment__": {
		            "incomplete list of types of public figures": [
			            "Actress",
						"Athlete",
						"Author",
						"Chef",
						"Comedian",
						"Influencer",
						"Journalist",
						"Model",
						"Musician",
						"Politician"
					],
					"incomplete list of genres for actresses": [
						"Drama",
						"Romance",
						"Comedy",
						"Thriller",
						"Science Fiction"
					]
				},
				"person": {
					"name": "Donald Trump",
					"type": "Politician",
					"description": "Donald Trump is a divisive businessman, reality TV star, and former President of the United States known for his controversial policies and rhetoric.",
					"__comment__": "Analyze this person and write genres"
				}
			},
			"response": {
				"genres" : ["Business", "Politics", "Reality Television"]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "person": {
		            "name": "",
		            "type": "",
		            "description": "",
		            "__comment__": "get response"
		        }
		    }
		})ML")
			.Set("/query/person/name", args.params("name"))
			.Set("/query/person/type", args.params("type"))
			.Set("/query/person/description", args.params("description"))
			;
		SetMaxLength(2048);
	}
	else if(args.fn == FN_ANALYZE_ELEMENTS) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "__comment__": {
		            "incomplete list of elements to define a part of a script": [
		                "exposition",
		                "climax",
		                "call to action",
		                "high stakes obstacle",
		                "rock bottom",
		                "rising action",
		                "falling action",
		                "conclusion",
		                "happy ending",
		                "tragedy",
		                "bittersweet ending",
		                "suspense",
		                "crisis",
		                "resolution",
		                "intensity",
		                "conflict",
		                "iteration"
		            ]
		        },
		        "script": {
		            "text": [
		                [
		                    "Hello"
		                ]
		            ],
		            "__comment__": "Analyze script and write matching word for all depths of script from the list A (e.g. \"exposition\")"
		        }
		    },
		    "response-full": {
		        "text & elements": [
		            [
		                [
		                    "Hello",
		                    "greeting"
		                ]
		            ]
		        ]
		    },
		    "response-short": {
		        "elements": [
		            [
		                "greeting"
		            ]
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "text": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/text", args.params("text"));
		SetMaxLength(2048);
	}
	else if(args.fn == FN_TOKENS_TO_LANGUAGES) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
			"query": {
				"script": {
					"__comment__": "get for languages for following words",
					"words": [
						"apple","is","red",
						"were","all","humans",
						"omena","on","red"
					]
				},
				"__comment__": "analyze words and give a array of words per languages. All input words must belong to some language array. Use only valid names for languages, e.g. 'english' but no 'extra' nor 'words'"
			},
			"response": {
				"__comment__": "no unrelated languages and word lists, only words that were in the query",
				"english": ["apple","is","red","were","all","humans"],
				"finnish":["omena","on"]
			}
		}
		)ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "__comment__": "get response for this query: related languages and word lists",
		            "words": []
		        }
		    }
		})ML")
			.Set("/query/script/words", args.params("words"));
		SetMaxLength(2048);
	}
	else if(args.fn == FN_TOKENS_TO_WORDS) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
			"query": {
				"__comment__": {
					"incomplete list of word classes": [
						"Nouns",
						"Verbs",
						"Adjectives",
						"Adverbs",
						"Pronouns",
						"Prepositions",
						"Conjunctions",
						"Determiners",
						"Interjections",
						"Articles",
						"Modal verbs",
						"Gerunds",
						"Infinitives",
						"Participles",
						"Definite article",
						"Indefinite article",
						"Proper nouns",
						"Collective nouns",
						"Concrete nouns",
						"Regular verbs",
						"Transitive verbs",
						"First person pronouns",
						"Second person pronouns",
						"Possessive determiners",
						"Possessive adjectives",
						"Comparative adjectives"
					]
				},
				"script": {
					"words": [
						["apple","is","red",","],
						["were","all","humans","."],
						["omena","on","red"]
					],
					"__comment__": "Analyze the words and rewrite the words and give the word class"
				}
			},
			"response": {
				"__comment__": "the number of values must be the same as in query.script.words",
				"words_and_word_classes": [
					[["apple","noun"], ["is","verb"], ["red","adjective"], [",","punctuation"]],
					[["we're","verb"], ["all","determiner"], ["humans", "noun"], [".","punctuation"]],
					[["omena","noun"], ["on","verb"], ["red","adjective"]]
				]
			}
		}
		)ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "words": [],
		            "__comment__": "get response with response.words_and_word_classes"
		        }
		    }
		})ML")
			.Set("/query/script/words", args.params("texts"));
		json_input.UseLegacyCompletion();
		SetMaxLength(2048);
	}
	else if(args.fn == FN_WORD_CLASSES) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
			"query": {
				"__comment__": {
					"incomplete list of word classes": [
						"Nouns",
						"Verbs",
						"Adjectives",
						"Adverbs",
						"Pronouns",
						"Prepositions",
						"Conjunctions",
						"Determiners",
						"Interjections",
						"Articles",
						"Modal verbs",
						"Gerunds",
						"Infinitives",
						"Participles",
						"Definite article",
						"Indefinite article",
						"Proper nouns",
						"Collective nouns",
						"Concrete nouns",
						"Abstract nouns",
						"Irregular verbs",
						"Regular verbs",
						"Transitive verbs",
						"Intransitive verbs",
						"Auxiliary verbs",
						"Reflexive verbs",
						"Imperative verbs",
						"First person pronouns",
						"Second person pronouns",
						"Third person pronouns",
						"Possessive pronouns",
						"Demonstrative pronouns",
						"Relative pronouns",
						"Intensive pronouns",
						"Indefinite pronouns",
						"Personal pronouns",
						"Subject pronouns",
						"Objective pronouns",
						"Possessive determiners",
						"Possessive adjectives",
						"Comparative adjectives",
						"Superlative adjectives",
						"Proper adjectives",
						"Positive adjectives",
						"Negative adjectives",
						"etc."
					]
				},
				"script": {
					"words": [
						"you",
						"what's",
						"smile"
					],
					"__comment__": "Analyze words and write word classes"
				}
			},
			"response-full": {
				"__comment__": "the next list must have the same number of values in array that was in query.script.words list",
				"words & word classes": [
					[
						"you",
						["pronoun"]
					],
					[
						"what's",
						["contraction (what + is)"]
					],
					[
						"smile",
						["noun", "verb"]
					]
				]
			},
			"response-short": {
				"word classes": [
					["pronoun"],
					["contraction (what + is)"],
					["noun", "verb"]
				]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "words": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/words", args.params("words"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_WORD_PAIR_CLASSES) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML({
		    "query": {
		        "__comment__": {
		            "incomplete list of word classes": [
		                "Nouns",
		                "Verbs",
		                "Adjectives",
		                "Adverbs",
		                "Pronouns",
		                "Prepositions",
		                "Conjunctions",
		                "Determiners",
		                "Interjections",
		                "Articles",
		                "Modal verbs",
		                "Gerunds",
		                "Infinitives",
		                "Participles",
		                "Definite article",
		                "Indefinite article",
		                "Proper nouns",
		                "Collective nouns",
		                "Concrete nouns",
		                "Abstract nouns",
		                "Irregular verbs",
		                "Regular verbs",
		                "Transitive verbs",
		                "Intransitive verbs",
		                "Auxiliary verbs",
		                "Reflexive verbs",
		                "Imperative verbs",
		                "First person pronouns",
		                "Second person pronouns",
		                "Third person pronouns",
		                "Possessive pronouns",
		                "Demonstrative pronouns",
		                "Relative pronouns",
		                "Intensive pronouns",
		                "Indefinite pronouns",
		                "Personal pronouns",
		                "Subject pronouns",
		                "Objective pronouns",
		                "Possessive determiners",
		                "Possessive adjectives",
		                "Comparative adjectives",
		                "Superlative adjectives",
		                "Proper adjectives",
		                "Positive adjectives",
		                "Negative adjectives"
		            ]
		        },
		        "script": {
		            "word pairs": [
		                [
		                    "automobile",
		                    "drives"
		                ]
		            ],
		            "__comment__": "Analyze word pairs and write word classes"
		        }
		    },
		    "response-full": {
		        "word pairs & word pairs classes": [
		            [
		                {
		                    "word": "automobile",
		                    "class": "noun"
		                },
		                {
		                    "word": "drives",
		                    "class": "verb"
						}
		            ]
		        ]
		    },
		    "response-short": {
		        "word pairs classes": [
		            [
	                    "noun",
	                    "verb"
		            ]
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "word pairs": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/word pairs", args.params("word pairs"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_SENTENCE) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML({
			"query": {
				"__comment__": {
					"incomplete list of sentence structures": [
						"declarative sentence",
						"conditional sentence",
						"descriptive sentence",
						"causal sentence",
						"subject-verb-object sentence",
						"subject-verb-adjective sentence",
						"etc."
					],
					"incomplete list of classes of sentences": [
						"independent clause",
						"dependent clause",
						"coordinating clause",
						"modifying clause",
						"non-coordinating clause",
						"subordinating clause",
						"etc."
					]
				},
				"script": {
					"sentence_count": 3,
					"classified_sentences": [
						["noun","verb","adjective"],
						["adjective","noun","preposition","noun"],
						["conjunction","pronoun","verb","noun"]
					],
					"__comment__": "Analyze classified sentences based on word classes"
				}
			},
			"response-full": {
				"sentence_count": 3,
				"titles_of_classified_sentences": [
					{"sentence":["noun","verb","adjective"], "title":"independent clause"},
					{"sentence":["adjective","noun","preposition","noun"], "title":"prepositional sentence"},
					{"sentence":["conjunction","pronoun","verb","noun"], "title":"complex sentence"}
				]
			},
			"response-short": {
				"sentence_count": 3,
				"titles": [
					"independent clause",
					"prepositional sentence",
					"complex sentence"
				]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
					"sentence_count": 0,
		            "classified_sentences": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/classified_sentences", args.params("classified_sentences"))
			.Set("/query/script/sentence_count", args.params("classified_sentences").GetCount());
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_SENTENCE_STRUCTURES) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML({
		    "query": {
		        "script": {
		            "classes of sentences": [
		                [
		                    "noun phrase",
		                    "independent clause"
		                ],
		                [
		                    "independent clause",
		                    "dependent clause"
		                ],
		                [
		                    "prepositional phrase",
		                    "independent clause"
		                ]
		            ],
		            "__comment__": "Identify sentence structures"
		        }
		    },
		    "response-full": {
		        "Sentence structure categorizations": [
		            [
		                [
		                    "noun phrase",
		                    "independent clause"
		                ],
		                "declarative sentence"
		            ],
		            [
		                [
		                    "independent clause",
		                    "dependent clause"
		                ],
		                "conditional sentence"
		            ],
		            [
		                [
		                    "prepositional phrase",
		                    "independent clause"
		                ],
		                "descriptive sentence"
		            ]
		        ]
		    },
		    "response-short": {
		        "categories": [
		            "declarative sentence",
		            "conditional sentence",
		            "descriptive sentence"
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "classified_sentences": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/classified_sentences", args.params("classified_sentences"));
		SetMaxLength(2048);
	}
	
	else if (args.fn == FN_CLASSIFY_PHRASE_ELEMENTS) {
		json_input.AddDefaultSystem();
		auto& in = json_input.AddAssist(R"ML(
		{
		    "query": {
	            "incomplete list of elements": [
	                "exposition",
	                "climax",
	                "call to action",
	                "high stakes obstacle",
	                "rock bottom",
	                "rising action",
	                "falling action",
	                "conclusion",
	                "happy ending",
	                "tragedy",
	                "bittersweet ending",
	                "suspense",
	                "crisis",
	                "resolution",
	                "intensity",
	                "conflict",
	                "iteration"
	            ],
		        "script": {
		            "phrases": [
		                "everyone of us loves her",
		                "they need to be silenced",
		                "you need help and we can contribute"
		            ],
		            "__comment__": "Analyze phrases and write matching element"
		        }
		    },
		    "response-full": {
		        "phrases & elements": [
		            {
		                "phrase": "everyone of us loves her",
		                "element": "exposition"
		            },
		            {
		                "phrase": "they need to be silenced",
		                "element": "high stakes obstacle"
		            },
		            {
		                "phrase": "you need help and we can contribute",
		                "element": "call to action"
		            }
		        ]
		    },
		    "response-short": {
		        "elements": [
		            "exposition",
		            "high stakes obstacle",
		            "call to action"
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "phrases": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/phrases", args.params("phrases"));
		
		ValueArray elements = args.params("elements");
		if (elements.GetCount() >= 10)
			in.Set("/query/script/incomplete list of elements", args.params("elements"));
		
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_PHRASE_COLOR) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "script": {
		            "phrases": [
						"everyone of us loves her",
						"they need to be silenced",
						"you need help and we can contribute"
					],
		            "__comment__": "write metaphorical colors for phrases"
				}
		    },
		    "response-full": {
				"__comment__": "the number of results must be the same as in query.script.phrases",
		        "results": [
					{"phrase":"everyone of us loves her", "color":"RGB(153,255,153)"},
					{"phrase":"they need to be silenced","color":"RGB(153,0,0)"},
					{"phrase":"you need help and we can contribute", "color":"RGB(255,153,204)"}
				]
			},
		    "response-short": {
		        "colors": [
					"RGB(153,255,153)",
					"RGB(153,0,0)",
					"RGB(255,153,204)"
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "phrases": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/phrases", args.params("phrases"));
		json_input.UseLegacyCompletion();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_PHRASE_ATTR) {
		json_input.AddDefaultSystem();
		json_input.AddAssist((String)R"ML({
		    "query": {
				"id": 1,
		        "script": {
		            "__comment__": {
						"info": "incomplete list of attribute groups and their opposite polarised attribute values",
						"attributes": {)ML"
						#define ATTR_ITEM(a,b,c,d) "\"" + IntStr(a) + "\":{\"name\":\"" b "\",\"+\":\"" c "\",\"-\":\"" d "\"},"
						ATTR_LIST
						#undef ATTR_ITEM
						R"ML("":""}
					},
		            "phrases": [
						"everyone of us loves her",
						"they need to be silenced",
						"you need help and we can contribute"
					]
				}
			},
		    "response-full": {
				"id": 1,
				"phrases & attributes": [
					{
						"text": "everyone of us loves her",
						"attribute": "4+",
						"attribute text": "belief communities: secular society"
					},
					{
						"text": "they need to be silenced",
						"attribute": "26+",
						"attribute text": "social: authoritarian"
					},
					{
						"text": "you need help and we can contribute",
						"attribute": "0-",
						"attribute text": "faith and reason seekers: rational thinker"
					}
				]
			},
			"response-short": {
				"id": 1,
				"__comment__": "attribute texts can be outside of the given incomplete list, but they must be in the format 'group: polarity' (so they must have exactly one ':' character)",
				"attribute texts": [
					"belief communities: secular society",
					"social: authoritarian",
					"faith and reason seekers: rational thinker"
				]
			}
		})ML");
				json_input.AddUser(R"ML(
		{
		    "query": {
				"id": 2,
		        "script": {
		            "phrases": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/phrases", args.params("phrases"));
		SetHighQuality();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_PHRASE_ACTIONS) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML({
		  "query": {
		    "__comment__": {
		      "incomplete list of action planner's action states for a narrating person. Every list item has two string values: the group and the value": [
		        ["msg", "statement"],
		        ["msg", "addressed to person"],
		        ["tone", "love"],
		        ["tone", "sadness"],
		        ["tone", "positive"],
		        ["bias", "assertion"],
		        ["bias", "defensive"],
		        ["attention-action", "showing off"],
		        ["attention-person", "the speaker"],
		        ["gesturing", "pointing"],
		        ["describing-surrounding", "near the ocean"]
		      ]
		    },
		    "script": {
		      "phrases": [
		          "2 AM, howlin outside",
		          "Lookin, but I cannot find",
		          "Only you can stand my mind"
		      ],
		      "__comment__": "analyze action planner action states for phrases"
		    }
		  },
		  "response-full": {
		    "__comment__": "the number of results must be the same as in query.script.phrases",
		    "phrases & action states": [
		      {"text":"2 AM, howlin outside", "actions":[["tone","urgent"], ["msg","trying to reach someone"], ["bias","romantic"], ["emotion","uncertainty"], ["level-of-certainty","trying/desire"], ["gesturing","pointing"], ["describing-surroundings","anywhere in the dark"], ["attention-place","outside"], ["attention-time","night"], ["attention-emotional_state","desire"], ["attention-action","howling"], ["attention-activity","driving"]]},
		      {"text":"Lookin, but I cannot find", "actions":[["msg","searching for someone"], ["bias","doubt"], ["emotion","frustration"], ["level-of-certainty","cannot find"], ["attention-action","searching"], ["attention-relationship","checking for person's presence"]]},
		      {"text":"Only you can stand my mind", "actions":[["tone","affectionate"], ["msg","expressing feelings"], ["bias","feeling understood by person"], ["emotion","love"], ["level-of-certainty","statement"], ["attention-person","addressed to person"], ["attention-emotional_state","love/affection"], ["attention-mental_state","thinking about person constantly"], ["attention-relationship","checking for compatibility"]]}
		    ]
		  },
		  "response-short": {
		    "action states": [
		      [["tone","urgent"], ["msg","trying to reach someone"], ["bias","romantic"], ["emotion","uncertainty"], ["level-of-certainty","trying/desire"], ["gesturing","pointing"], ["describing-surroundings","anywhere in the dark"], ["attention-place","outside"], ["attention-time","night"], ["attention-emotional_state","desire"], ["attention-action","howling"], ["attention-activity","driving"]],
		      [["msg","searching for someone"], ["bias","doubt"], ["emotion","frustration"], ["level-of-certainty","cannot find"], ["attention-action","searching"], ["attention-relationship","checking for person's presence"]],
		      [["tone","affectionate"], ["msg","expressing feelings"], ["bias","feeling understood by person"], ["emotion","love"], ["level-of-certainty","statement"], ["attention-person","addressed to person"], ["attention-emotional_state","love/affection"], ["attention-mental_state","thinking about person constantly"], ["attention-relationship","checking for compatibility"]]
		    ]
		  }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "phrases": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/phrases", args.params("phrases"));
		SetHighQuality();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_PHRASE_SCORES) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "__comment__": {
					"heuristic_score_factors": {
						"S0": "High like count from the audience. Low count means that the idea behind the phrase was bad.",
						"S1": "High comment count from the audience. Low count means that there was no emotion in the phrase.",
						"S2": "High listen count from the audience. Low count means that there was bad so-called hook in the phrase.",
						"S3": "High share count from the audience. Low count means that the phrase was not relatable.",
						"S4": "High bookmark count from the audience. Low count means that the phrase had no value.",
						"S5": "High reference count towards comedy from the audience. Low count means that the phrase was not funny.",
						"S6": "High reference count towards sex from the audience. Low count means that the phrase was not sensual.",
						"S7": "High reference count towards politics from the audience. Low count means that the phrase was not thought-provoking.",
						"S8": "High reference count towards love from the audience. Low count means that the phrase was not romantic.",
						"S9": "High reference count towards social issues from the audience. Low count means that the phrase was not impactful."
					},
					"description": [
						"Score factors are S0-S9 and can be considered as a vector of 10 integer numbers",
						"The value of a score factor is between 0-10",
						"Phrase is \"bleeding after you\"",
						"Score factors for the phrase \"bleeding after you\": S0: 9, S1: 8, S2: 8, S3: 6, S4: 7, S5: 9, S6: 4, S7: 2, S8: 3, S9: 2",
						"The score factors in shortened format: 9 8 8 6 7 9 4 2 3 2"
					]
				},
		        "script": {
		            "phrases": [
						"bleeding after you"
		            ],
		            "__comment__": "Analyze phrases and give rating (values S0-S9)"
		        }
		    },
		    "response-full": {
		        "__comment__": "the number of results must be the same as in query.script.phrases",
				"phrases & score factors": [
	                {
	                    "phrase":"bleeding after you",
	                    "score factors": [9, 8, 8, 6, 7, 9, 4, 2, 3, 2]
					}
		        ]
		    },
		    "response-short": {
		        "score factors": [
		            [9, 8, 8, 6, 7, 9, 4, 2, 3, 2]
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "phrases": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/phrases", args.params("phrases"));
		json_input.UseLegacyCompletion();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_PHRASE_TYPECLASS) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
			"query": {
				"list of possible typeclasses": {
					"0": "heartbroken/lovesick",
					"1": "rebel/anti-establishment",
					"2": "political activist",
					"3": "social justice advocate",
					"4": "party/club",
					"5": "hopeful/dreamer"
				},
		        "script": {
		            "phrases": [
						"bleeding after you"
					],
		            "__comment__": "Analyze phrases and give one or more typeclasses than could use this phrase"
				}
		    },
		    "response-full": {
		        "__comment__": "the number of results must be the same as in query.script.phrases. Only 1 typeclass per phrase",
				"phrases & typeclasses": [
	                {
	                    "phrase": "bleeding after you",
	                    "typeclasses": [
							{
			                    "typeclass text": "heartbroken/lovesick",
			                    "typeclass": 0,
			                    "__comment__": "typeclass is the index of the item in the list of possible typeclasses"
							},
							{
			                    "typeclass text": "party/club",
			                    "typeclass": 4
							}
						]
	                }
		        ]
		    },
		    "response-short": {
		        "typeclasses": [
					[0,4]
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "phrases": [],
		            "list of possible typeclasses": {},
		            "__comment__": "get response-short only"
		        }
		    }
		})ML")
			.Set("/query/script/phrases", args.params("phrases"))
			.Set("/query/script/list of possible typeclasses", args.params("typeclasses"))
			;
		json_input.UseLegacyCompletion();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_PHRASE_CONTENT) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
			"query": {
				"__comment__": {
					"incomplete list of short storylines": {
						)ML"
						#define CONTENT(idx, str) #idx ": \"" str "\","
						CONTENT_LIST
						#undef CONTENT
						R"ML(
					},
					"incomplete list of 3 phases of a storyline": {
						"generic": ["beginning", "middle", "end"],
						"encoded": ["A", "B", "C"],
						"Rise and fall": ["rise", "neutral", "fall"],
						"Escape to paradise": ["not in paradise", "going to paradise", "in paradise"]
					}
				},
		        "script": {
		            "phrases": [
						"bleeding after you"
					],
		            "__comment__": "Analyze phrases and write the matching storyline and 1 one of the 3 phases"
				}
		    },
		    "response-full": {
				"__comment__": "the phase is always in the encoded format: e.g. beginning = A",
		        "phrases & storylines": [
	                [
	                    "bleeding after you",
	                    [[10, "A"], [2, "C"], [5, "B"], [6, "A"], [26,"B"], [27,"A"]]
	                ]
		        ]
		    },
		    "response-short": {
		        "storylines": [
					[[10, "A"], [2, "C"], [5, "B"], [6, "A"], [26,"B"], [27,"A"]]
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "phrases": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/phrases", args.params("phrases"));
		json_input.UseLegacyCompletion();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_ACTION_COLOR) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
			"query": {
				"__comment__": {
					"examples of phrases and actions (group & value)": [
						["2 AM, howlin outside", [
							["attention-time","night"],
							["attention-emotional_state","desire"],
							["attention-action","howling"],
							["attention-activity","driving"],
							["tone","urgent"],
							["msg","trying to reach someone"],
							["bias","romantic"],
							["emotion","uncertainty"],
							["level-of-certainty","trying/desire"],
							["gesturing","pointing"],
							["describing-surroundings","anywhere in the dark"],
							["attention-place","outside"]
						]],
						["Lookin, but I cannot find", [
							["attention-action","looking"],
							["attention-physical state","tired"],
							["emotion","frustration"],
							["attention-emotional_state","desperation"],
							["attention-time","late at night"]
						]]
					]
				},
		        "script": {
					"__comment1__": "actions have two parts: group & value",
		            "actions": [
						["attention-event","unpleasant smell"],
						["msg","expressing physical desire"]
					],
					"__comment2__": "Analyze actions and write matching metaphorical colors"
				}
		    },
		    "response-full": {
				"__comment__": "Syntax translation: RGB(128,255,0) <-> [128,255,0]",
				"actions & colors": [
					[["attention-event","unpleasant smell"], "RGB(128,0,0)"],
					[["msg","expressing physical desire"], "RGB(255, 192, 203)"]
				]
		    },
		    "response-short": {
		        "colors": [
					[128,0,0],
					[255,192,203]
		        ]
		    }
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "actions": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/actions", args.params("actions"));
		json_input.UseLegacyCompletion();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_CLASSIFY_PHRASE_ACTION_ATTR) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "script": {
		            "__comment__": {
						"info": "List of attribute groups and their opposite polarised attribute values",
						"attributes": {)ML"
						#define ATTR_ITEM(a,b,c,d) "\"" #a "\":{\"name\":\"" b "\",\"+\":\"" c "\",\"-\":\"" d "\"},"
						ATTR_LIST
						#undef ATTR_ITEM
						R"ML("":""},
						"examples of phrases and attributes": [
							["I won't blindly follow the crowd", ["group faith", "individual spirituality"]],
							["feeling blue and green with envy", ["sexual preference", "kinky"]],
							["2 AM, howlin outside", ["faith and reason seekers", "divine worshipers"]],
							["Lookin, but I cannot find", ["truthfulness", "personal experience"]]
						],
						"examples of phrases and actions (group & value)": [
							["2 AM, howlin outside", [
								["attention-time","night"],
								["attention-emotional_state","desire"],
								["attention-action","howling"]
							]],
							["Lookin, but I cannot find", [
								["attention-action","looking"],
								["attention-physical state","tired"],
								["emotion","frustration"]
							]]
						]
					},
		            "actions": [
						["attention-event", "unpleasant smell"],
						["transition", "activities/roles"]
					]
				}
			},
		    "response-full": {
				"actions & attributes": [
					{
						"action": ["attention-event", "unpleasant smell"],
						"attribute": ["sexualization", "non-sexual"]
					},
					{
						"action": ["transition", "activities/roles"],
						"attribute": ["integrity", "twisted"]
					}
				]
			},
			"response-short": {
				"attributes": [
					["sexualization", "non-sexual"],
					["integrity", "twisted"]
				]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "actions": [],
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/actions", args.params("actions"));
		json_input.UseLegacyCompletion();
		SetMaxLength(2048);
	}
	else if (args.fn == FN_SORT_ATTRS) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "__comment1__": "attributes in the same category",
				"category": "colors",
				"attributes": [
					"Light yellow", "Grey", "Black", "White", "Dark grey", "Dark blue"
				],
		        "__comment2__": "pick 2 values from the given attribute list, which summarizes all values"
			},
			"response": {
				"attribute_summarization": [
					"Black",
					"White"
				]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
	            "category": "",
	            "attributes": [],
	            "__comment__": "get response"
		    }
		})ML")
			.Set("/query/attributes", args.params("attributes"))
			.Set("/query/category", args.params("category"))
			;
		SetMaxLength(2048);
	}
	else if (args.fn == FN_ATTR_POLAR_OPPOSITES) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
		        "script": {
		            "__comment1__": "incomplete list of values in the group and 2 polar opposite values inside the group",
		            "group": "socioeconomic status",
		            "values": ["urban", "gang affiliation", "drug dealing"],
		            "polar_opposites": {"positive": "wealth", "negative": "poverty"},
		            "__comment2__": "analyze values and write the closest polar opposite for the value"
				}
			},
			"response-full": {
				"values & polar_opposites": [
					{"value": "urban", "polar_opposite": "wealth", "polarity_key": "positive", "polarity_key_idx": 0},
					{"value": "gang affiliation", "polar_opposite": "poverty", "polarity_key": "negative", "polarity_key_idx": 1},
					{"value": "drug dealing", "polar_opposite": "poverty", "polarity_key": "negative", "polarity_key_idx": 1}
				]
			},
			"response-short": {
				"__comment__": "list of polarity_key_idx",
				"polar_opposites": [
					0,
					1,
					1
				]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
					"group": "",
		            "values": [],
		            "polar_opposites": {"positive": "", "negative": ""},
		            "__comment__": "get response-short"
		        }
		    }
		})ML")
			.Set("/query/script/group", args.params("group"))
			.Set("/query/script/values", args.params("values"))
			.Set("/query/script/polar_opposites/positive", args.params("attr0"))
			.Set("/query/script/polar_opposites/negative", args.params("attr1"))
			;
		SetMaxLength(2048);
	}
	else if (args.fn == FN_MATCHING_ATTR) {
		json_input.AddDefaultSystem();
		json_input.AddAssist(R"ML(
		{
		    "query": {
				"polar opposites": ["wealth", "poverty"],
	            "values": ["urban", "gang affiliation", "drug dealing"],
		        "__comment__": "find closest polar opposite for values"
			},
			"response-full": {
				"values & polar opposites": [
					{"value": "urban", "polar opposite": "wealth"},
					{"value": "gang affiliation", "polar opposite": "poverty"},
					{"value": "drug dealing", "polar opposite": "poverty"}
				]
			},
			"response-short": {
				"polar opposites": ["wealth", "poverty", "poverty"]
			},
			"response-shortest": {
				"polar opposites": [0, 1, 1]
			}
		})ML");
		json_input.AddUser(R"ML(
		{
		    "query": {
		        "script": {
		            "groups": [],
		            "values": [],
		            "__comment__": "get response-shortest"
		        }
		    }
		})ML")
			.Set("/query/script/groups", args.params("groups"))
			.Set("/query/script/values", args.params("values"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_SORA_PRESETS) {
		json_input.AddSystem("You help to create a style template for video generator Sora. " + content_policy_fix);
		json_input.AddAssist("German Expressionism\nTheme: German Expressionism Color: Dramatic black and white with stark contrast, deep shadows with bright highlights Camera: Vintage 35mm camera Film Stock: Kodak Panchromatic or simulating early orthochromatic filmsLighting: Intense, low-angle lighting creating sharp shadows and eerie atmospheres Vibe: Distorted, surreal, unsettling, with exaggerated emotions and stylized visuals");
		json_input.AddAssist("Archival\nShot on Eastmann 100t film, the image quality is grainy and high contrast, with shallow depth of field and cinematic look, epic and dramatic shot, very nostalgic.");
		json_input.AddAssist("Theme: Film Noir\nColor: High contrast black and white, deep shadows with selective highlights.\nCamera: Arri Alexa Mini, RED Monochrome, or vintage 35mm cameras.\nFilm Stock: Ilford HP5, Kodak Double-X, or high-contrast digital monochrome settings.\nLighting: Low key, chiaroscuro lighting with hard shadows, venetian blind effects, and strong backlighting.\nVibe: Moody, mysterious, suspenseful");
		json_input.AddAssist("Theme: Cardboard & Papercraft\nColor: Earthy tones like brown, beige, and muted pastels, with occasional pops of color to simulate colored paper.\nFilm Stock: analog film\nLighting: Soft, diffused lighting\nContent Transformation: Everything in the scene—from characters to objects and scenery—should be transformed into cardboard, paper, and glue. Elements should have visible creases, folds, and textures resembling handcrafted models.");
		json_input.AddAssist("Theme: Playful handcrafted animation.\nColor: Bright, saturated primary colors with handcrafted textures.\nFilm Stock: Smooth frame-by-frame animation with visible stop-motion quirks.\nLighting: Controlled spotlights with small shadows to highlight miniature craftsmanship.\nVibe: Quirky, charming, family-friendly");
		json_input.AddAssist("Theme: Everything is inflated like a balloon.\nColor: Glossy, bright colors—reds, yellows, blues, and metallics like gold and silver.\nFilm Stock: Clean digital with exaggerated reflections on shiny surfaces.\nLighting: High-key lighting with glossy highlights to mimic rubbery textures.\nContent Transformation: All characters, objects, and environments look inflated, with visible seams and a bouncy quality.\nVibe: Fun, surreal, viral-ready");
		json_input.AddAssist("8-bit pixel art");
		json_input.AddAssist("Theme: 1990s VHS Camcorder Recording\nColor: Warm, slightly faded colors with occasional color bleeding and chromatic distortions\nFilm Stock: VHS tape aesthetic featuring visible tracking lines, static noise, and lower resolution\nLighting: Mixed lighting with natural and artificial sources, often resulting in uneven exposure and occasional lens flare\nVibe: Nostalgic, candid, home-video feel with a raw, unpolished atmosphere");
		json_input.AddUserText(args.params("input"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_SLIDESHOW_PROMPTS) {
		json_input.AddSystem("You will write the music video screenplay for the slideshow of images. You will also help create the texts for the image generator AI.\nProvide additional options, as the decision will be made later in the video editor.\nGive answer immediately and don't ask questions." + content_policy_fix);
		json_input.AddUserText(args.params("input"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_VIDEO_WEBSITE_DESCRIPTIONS) {
		json_input.AddSystem("You help to write description text to TikTok video, which has a song and lyrics. Provide descriptions for Instagram and YouTube also. " + content_policy_fix);
		json_input.AddUserText(args.params("input"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_VIDEO_ADS) {
		json_input.AddSystem("You help to write text ads for a new music/music-video (in tiktok / youtube reels). " + content_policy_fix);
		json_input.AddUserText(args.params("input"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_VIDEO_COVER_IMAGE) {
		json_input.AddSystem("You help write a prompt for an image generator that creates a cover image for a song and its music video. " + content_policy_fix);
		json_input.AddUserText(args.params("input"));
		SetMaxLength(2048);
	}
	else if (args.fn == FN_ENGLISH_LYRICS) {
		json_input.AddSystem("You translate the lyrics into English, keeping the number of syllables and lines the same as in the original lyrics. " + content_policy_fix);
		json_input.AddUserText(args.params("input"));
		SetMaxLength(2048);
	}
	else
		SetError("Invalid function");
}

void AiTask::CreateInput_GenericPrompt(BasicPrompt& input)
{
	if(args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}

	GenericPromptArgs args;
	args.Put(this->args[0]);
	
	{
		for(int i = 0; i < args.lists.GetCount(); i++) {
			String s = args.lists.GetKey(i);
			auto& list = input.AddSub().Title(s);
			const auto& arr = args.lists[i];
			if(arr.IsEmpty())
				continue;
			for(int j = 0; j < arr.GetCount(); j++) {
				list.Add(arr[j]);
			}
		}
		if (args.response_path.GetCount()) {
			auto& list = input.AddSub();
			list.Title(args.response_path);
			list.NoColon();
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title(args.response_title);
			if(args.is_numbered_lines)
				results.NumberedLines();
			results.Add("");
		}
		SetMaxLength(2048);
	}
}

void AiTask::CreateInput_Code(BasicPrompt& input)
{
	if(args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	CodeArgs args;
	args.Put(this->args[0]);
	
	Panic("TODO: convert to json");
	
	if(args.fn == CodeArgs::SCOPE_COMMENTS) {
		{
			auto& list = input.AddSub().Title("Code");
			for(int i = 0; i < args.code.GetCount(); i++) list.Add("line #" + IntStr(i) + ": " + args.code[i]);
			list.NoListChar();
		}
		{
			TaskTitledList& results = input.PreAnswer();
			//results.Title("Add the comments which explains the purpose of the code. List of comments to insert (without code & all text in single line)");
			results.Title("Add the comments to the " + args.lang + " code. List of comments to insert (without code & all text in single line)");
			if (args.lang == "C++")
				tmp_str = "line #0: //";
			else
				tmp_str = "line #0: \"";
			results.Add(tmp_str);
		}
		SetMaxLength(2048);
	}
	if(args.fn == CodeArgs::FUNCTIONALITY) {
		#if 0
		{
			auto& list = input.AddSub().Title("List of types of functionalities");
			//list.NumberedLines();
			list.Add("generic");
			list.Add("writing data");
			list.Add("reading data");
			list.Add("referencing data for potential reading and writing");
			list.Add("referencing data for reading");
			list.Add("calling another function");
			list.Add("jumping unconditionally");
			list.Add("conditional branching");
			list.Add("synchronized (mutex, spinlock, etc.)");
			list.Add("asynchronized (callbacks, this-pointer, function/method pointers, etc.)");
			list.Add("etc.");
		}
		#endif
		{
			auto& list = input.AddSub().Title("Code");
			for(int i = 0; i < args.code.GetCount(); i++)
				list.Add(args.code[i]);
			list.NoListChar();
		}
		if (!args.data.IsEmpty()) {
			auto& list = input.AddSub().Title("Datapoints in Code");
			for(int i = 0; i < args.data.GetCount(); i++)
				list.Add(args.data.GetKey(i), args.data[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			//results.Title("List of functionalities and descriptions/explanations for the given code");
			results.Title("Explain the functionality/purpose for the given code");
			//tmp_str = "- type #";
			results.Add(tmp_str);
		}
		SetMaxLength(2048);
	}
}

END_UPP_NAMESPACE

