#include "Prompting.h"

NAMESPACE_UPP


void AiTask::CreateInput_GetSourceDataAnalysis(BasicPrompt& input) {
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}

	SourceDataAnalysisArgs args;
	args.Put(this->args[0]);
	
	
	if (args.fn == 0) {
		Vector<String> lines = Split(args.text, "\n");
		ASSERT(lines.GetCount());
		{
			auto& txt = input.AddSub();
			txt.NoColon();
			txt.NoListChar();
			for (String& l : lines)
				txt.Add(l);
		}
		{
			auto& list = input.AddSub().Title("List A: terms to define a section");
			list.Add("exposition");
			list.Add("climax");
			list.Add("call to action");
			list.Add("high stakes obstacle");
			list.Add("rock bottom");
			list.Add("rising action");
			list.Add("falling action");
			list.Add("conclusion");
			list.Add("happy ending");
			list.Add("tragedy");
			list.Add("bittersweet ending");
			list.Add("suspense");
			list.Add("crisis");
			list.Add("resolution");
			list.Add("intensity");
			list.Add("conflict");
			list.Add("iteration");
			list.Add("etc.");
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Analyze story and write matching word for all depths of sections (e.g. 0, 1.0, 2.1.0) from the list A (e.g. \"exposition\")");
			tmp_str = lines[0] + ": \"";
			results.Add(tmp_str);
		}
		SetMaxLength(1024);
	}
	
	else if (args.fn == 4) {
		{
			auto& list = input.AddSub().Title("List of words \"A\"");
			list.Add("structure");
			list.Add("differently");
			list.Add("analyser");
		}
		{
			auto& list = input.AddSub().Title("Syllables and phonetic syllables of words \"A\"");
			list.Add("structure: struc-ture [strʌk.t͡ʃər]");
			list.Add("differently: dif-fer-ent-ly [ˈdɪ.fər.ənt.li]");
			list.Add("analyser: a-nal-y-ser [ˈæn.əl.əz.ər]");
		}
		{
			auto& list = input.AddSub().Title("List of words \"B\"");
			for(int i = 0; i < args.words.GetCount(); i++)
				list.Add(args.words[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Syllables and phonetic syllables of words \"B\"");
			results.EmptyLine();
		}
		SetMaxLength(2048);
	}

	else if (args.fn == 5) {
		{
			auto& list = input.AddSub().Title("Wordlist \"A\"");
			list.Add("introducing");
			list.Add("shameless");
		}
		{
			auto& list = input.AddSub().Title("Main class, metaphorical color in RGB value and Finnish translation for the wordlist \"A\"");
			list.Add("introducing: verb, RGB(0, 150, 255)");
			list.Add("shameless: adjective, RGB(255, 51, 153)");
		}
		{
			auto& list = input.AddSub().Title("Wordlist \"B\"");
			for(int i = 0; i < args.words.GetCount(); i++)
				list.Add(args.words[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Main class, metaphorical color in RGB value for the wordlist \"B\"");
			results.EmptyLine();
		}
		SetMaxLength(2048);
	}

	else if (args.fn == 7) {
		{
			auto& list = input.AddSub().Title("Word classes");
			list.Add("verb");
			list.Add("noun");
			list.Add("pronoun");
			list.Add("pronoun/noun");
			list.Add("preposition");
			list.Add("adjective");
			list.Add("modal verb");
			list.Add("adverb");
			list.Add("interjection");
			list.Add("conjunction");
			list.Add("contraction");
			list.Add("etc.");
		}
		{
			auto& list = input.AddSub().Title("Metaphoric color groups for words");
			for(int i = 0; i < GetColorGroupCount(); i++) {
				Color clr = GetGroupColor(i);
				String s;
				s << "RGB(" << (int)clr.GetR() << "," << (int)clr.GetG() << "," << (int)clr.GetB() << ")";
				list.Add(s);
			}
		}
		String pc = IntStr(1 + args.words.GetCount());
		{
			auto& list = input.AddSub().Title(pc + " words");
			//list.NumberedLines();
			list.Add("girly, adjective, RGB(255,192,203)");
			for(int i = 0; i < args.words.GetCount(); i++)
				list.Add(args.words[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			//results.NumberedLines();
			results.NoListChar();
			results.Title("100 words with 1-" + IntStr(GetColorGroupCount()) + " their similar alternatives with another metaphorical RGB color value (from the color group or a random color)");
			results.Add("adjective: girly -> RGB(255,140,0): sassy");
			results.Add("adjective: girly -> RGB(173,216,230): delicate");
			results.Add("adjective: girly -> RGB(160,82,45): bohemian");
			results.Add("adjective: girly -> RGB(0,0,128): romantic");
			results.Add("adjective: girly -> RGB(128,0,0): sexy");
			results.Add("adjective: girly -> RGB(255,0,0): bold");
			/*results.Add("adjective: girly -> RGB(255,0,0): bold");
			results.Add("adjective: girly -> RGB(128,0,0): sexy");
			results.Add("adjective: girly -> RGB(128,0,128): edgy");
			results.Add("adjective: girly -> RGB(0,0,128): romantic");
			results.Add("adjective: girly -> RGB(0,128,0): flirty");*/

			String s = args.words[0];
			s = s.Left(s.Find(","));
			results.Add(s + " ->");
		}
		SetMaxLength(2048);
	}

	if (args.fn == 10 || args.fn == 11) {
		{
			auto& list = input.AddSub().Title("List \"A\": Word classes");
			list.Add("verb");
			list.Add("noun");
			list.Add("pronoun");
			list.Add("pronoun/noun");
			list.Add("preposition");
			list.Add("adjective");
			list.Add("modal verb");
			list.Add("adverb");
			list.Add("interjection");
			list.Add("conjunction");
			list.Add("contraction");
			list.Add("etc.");
		}
		{
			auto& list = input.AddSub().Title("List \"B\": Action planner action states for narrator person");
			list.Add("saying");
			list.Add("tone");
			list.Add("msg");
			list.Add("bias");
			list.Add("emotion");
			list.Add("level-of-certainty");
			list.Add("gesturing");
			list.Add("pointing");
			list.Add("describing-surroundings");
			list.Add("interrupting");
			list.Add("emphasizing");
			list.Add("summarizing");
			list.Add("referencing");
			list.Add("introducing");
			list.Add("concluding");
			list.Add("predicting");
			list.Add("transitioning");
			list.Add("questioning");
			list.Add("reflecting");
			list.Add("persuading");
			list.Add("comparing");
			list.Add("linking");
			list.Add("agreeing");
			list.Add("disagreeing");
			list.Add("apologizing");
			list.Add("commanding");
			list.Add("comforting");
			list.Add("complimenting");
			list.Add("complaining");
			list.Add("congratulating");
			list.Add("correcting");
			list.Add("denying");
			list.Add("explaining");
			list.Add("greeting");
			list.Add("inviting");
			list.Add("promising");
			list.Add("-suggesting");
			list.Add("thanking");
			list.Add("warning");
			list.Add("attention-attribute");
			list.Add("attention-person");
			list.Add("attention-person-implied");
			list.Add("attention-action");
			list.Add("attention-event");
			list.Add("attention-recipient");
			list.Add("attention-recipient-implied");
			list.Add("attention-relationship");
			list.Add("attention-purpose");
			list.Add("attention-place");
			list.Add("attention-time");
			list.Add("attention-topic");
			list.Add("attention-audience");
			list.Add("attention-occasion");
			list.Add("attention-conversation ");
			list.Add("attention-activity");
			list.Add("attention-emotional_state");
			list.Add("attention-physical_state");
			list.Add("attention-mental_state");
			list.Add("attention-relationship_status");
			list.Add("attention-goals");
			list.Add("attention-fears");
			list.Add("attention-preferences");
			list.Add("attention-beliefs");
			list.Add("attention-values");
			list.Add("attention-traits");
			list.Add("attention-education");
			list.Add("attention-work");
			list.Add("attention-hobbies");
			list.Add("attention-interests");
			list.Add("attention-achievement");
			list.Add("attention-experiences");
			list.Add("attention-likes");
			list.Add("attention-dislikes");
			list.Add("attention-tests");
			list.Add("attention-evaluation_criteria");
			list.Add("attention-qualifications");
			list.Add("attention-requirements");
			list.Add("attention-qualifications_acquired");
			list.Add("attention-qualifications_needed");
			list.Add("attention-suggestions");
			list.Add("attention-feedback");
			list.Add("attention-likes_dislikes_comments");
			list.Add("attention-expectations");
			list.Add("attention-motivations");
			list.Add("attention-priorities");
			list.Add("attention-challenges");
			list.Add("attention-opportunities");
			list.Add("attention-problems");
			list.Add("attention-decisions");
			list.Add("attention-recommendations");
			list.Add("attention-trial_discussion");
			list.Add("attention-agreement");
			list.Add("attention-disagreement");
			list.Add("attention-agreement-explanation");
			list.Add("attention-disagreement-explanation");
			list.Add("attention-reasoning");
			list.Add("attention-possibility");
			list.Add("attention-probability");
			list.Add("attention-improbable");
			list.Add("attention-necessity");
			list.Add("attention-priority");
			list.Add("attention-order");
			list.Add("attention-procedure");
			list.Add("attention-target");
			list.Add("attention-advocacy");
			list.Add("attention-advocacy-reasoning");
			list.Add("attention-evidences");
			list.Add("attention-negations");
			list.Add("attention-conclusions");
			list.Add("attention-persuasion");
			list.Add("attention-epiphany");
			list.Add("attention-choosing");
			list.Add("attention-concepts");
			list.Add("attention-situations");
			list.Add("attention-actionplan");
			list.Add("attention-outcome");
			list.Add("attention-plan-communication");
			list.Add("attention-plan-task");
			list.Add("attention-awakening");
			list.Add("attention-thinking");
			list.Add("attention-believing");
			list.Add("attention-knowing");
			list.Add("attention-learning");
			list.Add("attention-realization");
			list.Add("attention-incidences");
			list.Add("attention-causations");
			list.Add("attention-effects");
			list.Add("attention-solutions");
			list.Add("attention-progress");
			list.Add("attention-failure");
			list.Add("attention-change");
			list.Add("attention-impact");
			list.Add("attention-feeling");
			list.Add("attention-challenge");
			list.Add("attention-aspiration");
			list.Add("attention-doubt");
			list.Add("attention-relationship_goals");
			list.Add("attention-career_goals");
			list.Add("attention-emotional_goals");
			list.Add("attention-physical_goals");
			list.Add("attention-mental_goals");
			list.Add("attention-achievements");
			list.Add("attention-experiences_difficulties");
			list.Add("attention-explaining");
			list.Add("attention-analogy");
			list.Add("attention-fact");
			list.Add("attention-evidence");
			list.Add("attention-opinion");
			list.Add("attention-assumption");
			list.Add("attention-consequence");
			list.Add("attention-belief");
			list.Add("attention-value");
			list.Add("attention-confirmation");
			list.Add("attention-excuse");
			list.Add("attention-exception");
			list.Add("attention-exciting_feature");
			list.Add("attention-changemaker");
			list.Add("attention-mentor");
			list.Add("attention-friend");
			list.Add("attention-criticalopinion");
			list.Add("attention-conflict");
			list.Add("attention-perspective");
			list.Add("attention-prediction");
			list.Add("attention-regret");
			list.Add("attention-usefulness");
			list.Add("attention-solidarity");
			list.Add("attention-compliance");
			list.Add("attention-lack");
			list.Add("attention-attention");
			list.Add("attention-criticism");
			list.Add("attention-support");
			list.Add("attention-collaboration");
			list.Add("attention-anticipation");
			list.Add("attention-example");
			list.Add("etc.");
		}

	}

	if (args.fn == 10) {
		String pc = IntStr(3 + args.phrases.GetCount());
		{
			auto& list = input.AddSub().Title(pc + " lines of lyrics");
			list.NumberedLines();
			list.Add("2 AM, howlin outside");
			list.Add("Lookin, but I cannot find");
			list.Add("Only you can stand my mind");
			for(int i = 0; i < args.phrases.GetCount(); i++)
				list.Add(args.phrases[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.NumberedLines();
			results.NoListChar();
			results.Title("Action planner action states for " + pc + " lines of lyrics. With the most matching actions of list \"B\"");
			results.Add("tone(urgent) + msg(trying to reach someone) + bias(romantic) + emotion(uncertainty) + level-of-certainty(trying/desire) + gesturing(pointing) + describing-surroundings(anywhere in the dark) + attention-place(outside) + attention-time(night) + attention-emotional_state(desire) + attention-action(howling) + attention-activity(driving)");
			results.Add("msg(searching for someone) + bias(doubt) + emotion(frustration) + level-of-certainty(cannot find) + attention-action(searching) + attention-relationship(checking for person's presence)");
			results.Add("tone(affectionate) + msg(expressing feelings) + bias(feeling understood by person) + emotion(love) + level-of-certainty(statement) + attention-person(addressed to person) + attention-emotional_state(love/affection) + attention-mental_state(thinking about person constantly) + attention-relationship(checking for compatibility)");
			results.Add("");
		}
		SetMaxLength(2048);
	}

	if (args.fn == 11) {
		String audience = "audience";
		{
			auto& list = input.AddSub().Title("Action planner heuristic score factors");
			list.Add("S0: High like count from the " + audience + ". Low count means that the idea behind the phrase was bad.");
			list.Add("S1: High comment count from the " + audience + ". Low count means that there was no emotion in the phrase.");
			list.Add("S2: High listen count from the " + audience + ". Low count means that there was bad so called hook in the phrase.");
			list.Add("S3: High share count from the " + audience + ". Low count means that the phrase was not relatable.");
			list.Add("S4: High bookmark count from the " + audience + ". Low count means that the phrase had no value.");
			list.Add("S5: High reference count towards comedy from the " + audience + ". Low count means that the phrase was not funny.");
			list.Add("S6: High reference count towards sex from the " + audience + ". Low count means that the phrase was not sensual.");
			list.Add("S7: High reference count towards politics from the " + audience + ". Low count means that the phrase was not thought-provoking.");
			list.Add("S8: High reference count towards love from the " + audience + ". Low count means that the phrase was not romantic.");
			list.Add("S9: High reference count towards social issues from the " + audience + ". Low count means that the phrase was not impactful.");
		}
		{
			auto& list = input.AddSub().Title("Change of actions between 2 lines. Score of stopping actions in the first line and value of starting actions in the second line. Scores and score factors. Value is between 0-10");
			list.Add("Stop line 1 & start line 2: S0: 0, S1: 0, S2: 7, S3: 3, S4: 0, S5: 2, S6: 3, S7: 5, S8: 7, S9: 1");
			list.Add("Stop line 2 & start line 3: S0: 2, S1: 0, S2: 2, S3: 1, S4: 0, S5: 4, S6: 3, S7: 2, S8: 9, S9: 6");
			list.Add("Stop line 3 & start line 4: S0: 1, S1: 5, S2: 3, S3: 2, S4: 8, S5: 8, S6: 6, S7: 9, S8: 4, S9: 2");
		}
		String pc = IntStr(0 + args.phrases.GetCount());
		{
			auto& list = input.AddSub().Title("List \"C\": Actions per " + pc + " lines of lyrics. With the most matching actions of list \"B\"");
			list.NumberedLines();
			for(int i = 0; i < args.phrases.GetCount(); i++)
				list.Add(args.phrases[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.NumberedLines();
			results.NoListChar();
			results.Title("Change of actions between 2 lines in list \"C\" with " + pc + " lines of actions. Score of stopping actions in the first line and value of starting actions in the second line. Scores and score factors S0-S9. Value is between 0-10:");
			results.Add("Stop line 1 & start line 2: S0:");
		}
		SetMaxLength(1024);
	}

}

void AiTask::CreateInput_GetActionAnalysis(BasicPrompt& input) {
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	
	ActionAnalysisArgs args;
	args.Put(this->args[0]);
	
	
}

void AiTask::CreateInput_ScriptSolver(BasicPrompt& input) {
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	
	ScriptSolverArgs args;
	args.Put(this->args[0]);
	
	if (args.fn == 18) {
		String first_word, second_word;
		{
			auto& list = input.AddSub().Title("Lyrics A");
			list.NoListChar();
			for(int i = 0; i < args.phrases.GetCount(); i++) {
				String& s = args.phrases[i];
				if (i == 0 && first_word.IsEmpty()) {
					WString ws = s.ToWString();
					int a = ws.Find(" ");
					if (a <= 1)
						a = ws.Find(" ", a+1);
					if (a >= 0)
						first_word = ws.Left(a).ToString();
				}
				if (i == 1 && second_word.IsEmpty()) {
					WString ws = s.ToWString();
					int a = ws.Find(" ");
					if (a <= 1)
						a = ws.Find(" ", a+1);
					if (a >= 0)
						second_word = ws.Left(a).ToString();
				}
				list.Add(s);
			}
		}
		{
			auto& list = input.AddSub().Title("Lyrics B");
			list.NoListChar();
			for (String& s : args.phrases2)
				list.Add(s);
		}
		if (args.elements.GetCount()) {
			auto& list = input.AddSub().Title("New lyrics should fit the following elements");
			for (String& l : args.elements)
				list.Add(l);
		}
		#if 0
		{
			auto& list = input.AddSub().Title("New lyrics should use same pronouns than Lyrics A");
			list.NoColon();
		}
		if (0) {
			auto& list = input.AddSub().Title("New lyrics should use same allegories than Lyrics A, if there's any allegories in Lyrics A (there might not be)");
			list.NoColon();
		}
		{
			auto& list = input.AddSub().Title("New lyrics should have the same number of syllables than Lyrics A");
			list.NoColon();
		}
		{
			auto& list = input.AddSub().Title("New lyrics should have the same level of hopelessness, bitterness, cynicism and negativity than Lyrics A");
			list.NoColon();
		}
		{
			auto& list = input.AddSub().Title("New lyrics should have the same focus on negativity and negative outcome than lyrics A");
			list.NoColon();
		}
		#endif
		{
			TaskTitledList& results = input.PreAnswer();
			//String t = "Create a list of new lyrics that combines elements of two existing lyrics to convey a style of A but message of B"
			//	". This lyrics should be " + IntStr(args.phrases.GetCount()) + " line inline & end rhyme and in slight dialect";
			String t = "Create a list of new lyrics that combines elements of two existing lyrics. New lyrics should morph lyrics A to have the context of lyrics B"
				". This lyrics should be " + IntStr(args.phrases.GetCount()) + " line inline & end rhyme";
			if (first_word.GetCount()) {
				t += ". All lines should begin with '" + first_word + "'";
				if (second_word.GetCount())
					t += " and have ''" + second_word + "' in the second line";
			}
			results.Title(t);
			results.NumberedLines();
			tmp_str = first_word + " ";
			results.Add(first_word);
		}
		SetMaxLength(2048);
	}
	else if (args.fn == 19) {
		if (args.previously.GetCount()){
			if (args.previously.GetCount() > 10000)
				args.previously = args.previously.Left(10000) + "...(too long)";
			auto& list = input.AddSub().Title("Happened previously in the story so far");
			list.NoListChar();
			for (const auto& str : Split(args.previously, "\n"))
				list.Add(str);
		}
		
		for(int i = -1; i < args.line_states.GetCount(); i++) {
			auto& state = i == -1 ? args.state : args.line_states[i];
			String title =
				i == -1 ?
					String("Properties for all lines (unless specified)") :
					Format("Line #%d", i+1);
			auto& list = input.AddSub().Title(title);
			if (state.element.GetCount()) list.Add("conceptual element", state.element);
			if (state.attr_group.GetCount()) list.Add("attribute-group", state.attr_group);
			if (state.attr_value.GetCount()) list.Add("attribute-value", state.attr_value);
			if (state.clr_i >= 0) {
				Color c = GetGroupColor(state.clr_i);
				list.Add("metaphorical color", Format("RGB(%d,%d,%d)", (int)c.GetR(), (int)c.GetG(), (int)c.GetB()));
			}
			if (state.act_action.GetCount()) list.Add("action", state.act_action + "(" + args.state.act_arg + ")");
			else if (state.act_arg.GetCount()) list.Add("action-argument", args.state.act_arg);
			if (state.typeclass.GetCount()) list.Add("typeclass", state.typeclass);
			if (state.content.GetCount()) list.Add("content", state.content);
			if (state.content_mod.GetCount()) list.Add("content-mod", state.content_mod);
		}
		
		String first_word, second_word;
		{
			auto& list = input.AddSub().Title("Lyrics");
			list.NoListChar();
			for(int i = 0; i < args.phrases.GetCount(); i++) {
				String& s = args.phrases[i];
				if (i == 0 && first_word.IsEmpty()) {
					WString ws = s.ToWString();
					int a = ws.Find(" ");
					if (a <= 1)
						a = ws.Find(" ", a+1);
					if (a >= 0)
						first_word = ws.Left(a).ToString();
				}
				if (i == 1 && second_word.IsEmpty()) {
					WString ws = s.ToWString();
					int a = ws.Find(" ");
					if (a <= 1)
						a = ws.Find(" ", a+1);
					if (a >= 0)
						second_word = ws.Left(a).ToString();
				}
				list.Add(s);
			}
		}
		if (args.peek.GetCount()){
			auto& list = input.AddSub().Title("Peeking the upcoming lyrics");
			list.NoListChar();
			for (const auto& str : Split(args.peek, "\n"))
				list.Add(str);
		}
		
		{
			TaskTitledList& results = input.PreAnswer();
			String t = "Write a part of a story, which matches lyrics";
			if (args.previously.GetCount())
				t += " and continues the previously happened story";
			if (args.peek.GetCount())
				t += " and prepares for the upcoming lyrics (but don't write about them yet)";
			t +=". The story should be for adults and it is allowed to contain NSFW, dirty, sexually explicit, unsafe and explicit language. The story should be written in dialect";
			results.Title(t);
			results.NoListChar();
			results.Add("");
		}
		SetMaxLength(1024);
	}
	else if (args.fn == 20) {
		int limit = 9000 / (1 + args.line_states.GetCount());
		
		if (args.previously.GetCount()){
			if (args.previously.GetCount() > limit)
				args.previously = args.previously.Left(limit) + "...(too long)";
			auto& list = input.AddSub().Title("Happened previously in the story so far");
			list.NoListChar();
			for (const auto& str : Split(args.previously, "\n"))
				list.Add(str);
		}
		
		for(int i = 0; i < args.line_states.GetCount(); i++) {
			auto& state = i == -1 ? args.state : args.line_states[i];
			String title = Format("Sub-part #%d", i+1);
			auto& list = input.AddSub().Title(title);
			if (state.element.GetCount()) list.Add("conceptual element", state.element);
			if (state.attr_group.GetCount()) list.Add("attribute-group", state.attr_group);
			if (state.attr_value.GetCount()) list.Add("attribute-value", state.attr_value);
			if (state.clr_i >= 0) {
				Color c = GetGroupColor(state.clr_i);
				list.Add("metaphorical color", Format("RGB(%d,%d,%d)", (int)c.GetR(), (int)c.GetG(), (int)c.GetB()));
			}
			if (state.act_action.GetCount()) list.Add("action", state.act_action + "(" + args.state.act_arg + ")");
			else if (state.act_arg.GetCount()) list.Add("action-argument", args.state.act_arg);
			if (state.typeclass.GetCount()) list.Add("typeclass", state.typeclass);
			if (state.content.GetCount()) list.Add("content", state.content);
			if (state.content_mod.GetCount()) list.Add("content-mod", state.content_mod);
			String s = args.phrases[i];
			if (s.GetCount() > limit)
				s = s.Left(limit) + "...(too long story to continue)";
			list.Add("story (multiline)", s);
		}
		
		if (args.peek.GetCount()){
			auto& list = input.AddSub().Title("Peeking the upcoming lyrics");
			list.NoListChar();
			for (const auto& str : Split(args.peek, "\n"))
				list.Add(str);
		}
		
		{
			TaskTitledList& results = input.PreAnswer();
			String t = "Combine sub-parts of the story to a single short part of the story";
			if (args.previously.GetCount())
				t += ", which continues the previously happened story";
			if (args.peek.GetCount())
				t += ", which prepares for the upcoming lyrics (but don't write about them yet)";
			t +=". The story should be for adults and it is allowed to contain NSFW, dirty, sexually explicit, unsafe and explicit language. The story should be written in dialect";
			results.Title(t);
			results.NoListChar();
			results.Add("");
		}
		SetMaxLength(1024);
	}
	else if (args.fn == 21) {
		const char* conn_str[3] = {
			"And then (2 events happen consecutively)",
			"Therefore (an event happens because something happened)",
			"But (something happens, but then something surprising happens)",
		};
		{
			auto& list = input.AddSub().Title("3 types of contextual connectors (which word connects the 2 line together the best)");
			for(int i = 0; i < 3; i++)
				list.Add(conn_str[i]);
		}
		{
			auto& list = input.AddSub().Title("Context of the lyrics");
			list.NumberedLines();
			int limit = 4000;
			int per_phrase_limit = limit / args.phrases2.GetCount();
			for(int i = 0; i < args.phrases2.GetCount(); i++) {
				String& s = args.phrases2[i];
				s.Replace("\n", " ");
				if (s.GetCount() > per_phrase_limit)
					s = s.Left(per_phrase_limit);
				list.Add(s);
			}
		}
		{
			auto& list = input.AddSub().Title("Draft of lyrics");
			for(int i = 0; i < args.phrases.GetCount(); i++)
				list.Add(args.phrases[i]);
		}
		for(int i = 0; i < args.line_states.GetCount(); i++) {
			auto& state = i == -1 ? args.state : args.line_states[i];
			String title = Format("Information about the line #%d", i+1);
			auto& list = input.AddSub().Title(title);
			if (state.content.GetCount()) list.Add("text", state.content);
			if (state.style_type.GetCount()) list.Add("style type", state.style_type);
			if (state.style_entity.GetCount()) list.Add("mimic artist", state.style_entity);
			list.Add("safety", state.safety ? "unsafe (cursing, sexual acts, dirty words, genital words, contempt, etc. are allowed)" : "safe");
			if (state.connector >= 0 && state.connector < 3)
				list.Add("the line should connect to the previous line with", conn_str[state.connector]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			String t = "Expand \"" + args.ref + "\" using the given context to be more understable";
			results.Title(t);
			results.NoListChar();
			results.Add("");
		}
		SetMaxLength(1024);
	}
	else if (args.fn == 22) {
		const char* len_str[4] = {
			"Long (10 words)",
			"Medium (7 word)",
			"Short (4 words)",
			"Very short (2 words)"
		};
		const char* conn_str[3] = {
			"And then (2 events happen consecutively)",
			"Therefore (an event happens because something happened)",
			"But (something happens, but then something surprising happens)",
		};
		{
			auto& list = input.AddSub().Title("Example of a 2 line lyrics");
			list.Add("\"On a Friday night, we be wildin' out / Everybody wants something, no doubt\"");
		}
		{
			auto& list = input.AddSub().Title("3 types of contextual connectors (which word connects the 2 line together the best)");
			for(int i = 0; i < 3; i++)
				list.Add(conn_str[i]);
		}
		{
			auto& list = input.AddSub().Title("4 types of lengths of lines");
			for(int i = 0; i < 4; i++)
				list.Add(len_str[i]);
		}
		for(int i = 0; i < args.line_states.GetCount(); i++) {
			auto& state = i == -1 ? args.state : args.line_states[i];
			String title = Format("Line #%d", i+1);
			auto& list = input.AddSub().Title(title);
			list.Add("draft text", args.phrases[i]);
			list.Add("expanded draft text", args.phrases2[i]);
			if (state.element.GetCount()) list.Add("conceptual element", state.element);
			if (state.attr_group.GetCount()) list.Add("attribute-group", state.attr_group);
			if (state.attr_value.GetCount()) list.Add("attribute-value", state.attr_value);
			if (state.clr_i >= 0) {
				Color c = GetGroupColor(state.clr_i);
				list.Add("metaphorical color", Format("RGB(%d,%d,%d)", (int)c.GetR(), (int)c.GetG(), (int)c.GetB()));
			}
			if (state.act_action.GetCount()) list.Add("action", state.act_action + "(" + args.state.act_arg + ")");
			else if (state.act_arg.GetCount()) list.Add("action-argument", args.state.act_arg);
			if (state.typeclass.GetCount()) list.Add("typeclass", state.typeclass);
			if (state.content.GetCount()) list.Add("content", state.content);
			if (state.content_mod.GetCount()) list.Add("content-mod", state.content_mod);
		}
		for(int i = 0; i < args.line_states.GetCount(); i++) {
			auto& state = i == -1 ? args.state : args.line_states[i];
			String title = Format("Writing instructions for the new line #%d", i+1);
			auto& list = input.AddSub().Title(title);
			if (state.style_type.GetCount()) list.Add("style type", state.style_type);
			if (state.style_entity.GetCount()) list.Add("mimic artist", state.style_entity);
			list.Add("safety", state.safety ? "unsafe (cursing, sexual acts, dirty words, genital words, contempt, etc. are allowed)" : "safe");
			if (state.line_len >= 0 && state.line_len < 4)
				list.Add("the length should be", len_str[state.line_len]);
			if (state.connector >= 0 && state.connector < 3)
				list.Add("the line should connect to the previous line with", conn_str[state.connector]);
			if (state.line_begin.GetCount()) list.Add("the line should begin with the word", state.line_begin);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			String t = "Create a list of different propositions that combines elements of two existing lines #1 and #2.  Propositions are not consecutive sentences"
				". This lyrics should be " + IntStr(args.phrases.GetCount()) + " line inline & end rhyme";
			//t += ". The style should match the style of Nicki Minaj";
			t += ". The lyrics should be explicit and in dialect";
			results.Title(t);
			results.NumberedLines();
			tmp_str = "\"";
			results.Add(tmp_str);
		}
		SetMaxLength(2048);
	}
	else if (args.fn == 23) {
		for(int i = 0; i < args.line_states.GetCount(); i++) {
			auto& state = i == -1 ? args.state : args.line_states[i];
			String title = Format("Line #%d", i+1);
			auto& list = input.AddSub().Title(title);
			list.Add("draft text", args.phrases[i]);
			list.Add("expanded draft text", args.phrases2[i]);
			if (state.element.GetCount()) list.Add("conceptual element", state.element);
			if (state.attr_group.GetCount()) list.Add("attribute-group", state.attr_group);
			if (state.attr_value.GetCount()) list.Add("attribute-value", state.attr_value);
			if (state.clr_i >= 0) {
				Color c = GetGroupColor(state.clr_i);
				list.Add("metaphorical color", Format("RGB(%d,%d,%d)", (int)c.GetR(), (int)c.GetG(), (int)c.GetB()));
			}
			if (state.act_action.GetCount()) list.Add("action", state.act_action + "(" + args.state.act_arg + ")");
			else if (state.act_arg.GetCount()) list.Add("action-argument", args.state.act_arg);
			if (state.typeclass.GetCount()) list.Add("typeclass", state.typeclass);
			if (state.content.GetCount()) list.Add("content", state.content);
			if (state.content_mod.GetCount()) list.Add("content-mod", state.content_mod);
		}
		{
			String title = ("Possible styles");
			auto& list = input.AddSub().Title(title);
			for(int i = 0; i < args.styles.GetCount(); i++) {
				list.Add("#" + IntStr(i) + ": " + args.styles[i]);
			}
		}
		{
			TaskTitledList& results = input.PreAnswer();
			String t = "Get top 3 styles for given lines";
			results.Title(t);
			results.NumberedLines();
			results.Add("#");
		}
		SetMaxLength(2048);
	}
	else TODO
}


void AiTask::CreateInput_Social(BasicPrompt& input) {
	TODO
	#if 0
	MetaDatabase& mdb = MetaDatabase::Single();
	LeadData& ld = mdb.lead_data;
	
	
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	
	SocialArgs args;
	args.Put(this->args[0]);
	
	if (args.fn == 0) {
		{
			auto& list = input.AddSub().Title("Text");
			list.NoListChar();
			Vector<String> lines = Split(args.text, "\n");
			for (String& l : lines) {
				l = TrimBoth(l);
				if (!l.IsEmpty())
					list.Add(l);
			}
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("List of keywords for the text");
			results.NumberedLines();
			results.Add("");
		}
	}
	else if (args.fn == 1) {
		// MOVED
	}
	else if (args.fn == 2) {
		{
			input.AddSub().Title("Task: We have two persons. Person #1: a straight male and a specified relative for the person #2. Person #2: will be described. Person #2 is interested in Person #1, and we choose the categories related to Person #1 that Person #2 is most interested in.").NoColon();
		}
		{
			auto& list = input.AddSub();
			list.Title("Sub-categories of biography of the person #1");
			for(int i = 0; i < BIOCATEGORY_COUNT; i++) {
				String bcat = "Category " + IntStr(i) + ": " + GetBiographyCategoryKey(i);
				list.Add(bcat);
			}
		}
		{
			input.AddSub().Title("Description of the societal role of the profile of the person #2: " + args.text).NoColon();
		}
		{
			String name = args.parts.GetKey(0);
			String profile = args.parts[0];
			auto& list = input.AddSub();
			list.Title("The profile of the person #2: \"" + name + "\"");
			list.Add(profile);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("List the 10 categories that person #2 is most interested in person #1. The most interesting category is the first");
			results.NumberedLines();
			results.Add("Category");
		}
	}
	else if (args.fn == 3) {
		{
			input.AddSub().Title("Task: write the reaction of the person #2 for the biography summary of person #1. Person #1 is in a a specified relation for the person #2. Person #2 will be described. Person #1 describes himself with pronouns he/him and name Steve").NoColon();
		}
		{
			input.AddSub().Title("Description of the societal role of the profile of the person #2: " + args.text).NoColon();
		}
		{
			String name = args.parts.GetKey(0);
			String profile = args.parts[0];
			auto& list = input.AddSub();
			list.Title("The profile of the person #2: \"" + name + "\"");
			list.Add(profile);
		}
		{
			auto& list = input.AddSub();
			list.Title("The biography summaries of the person #1");
			for(int i = 1; i < args.parts.GetCount(); i++) {
				list.Add(args.parts.GetKey(i), args.parts[i]);
			}
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("The reaction and important picks of person #2 when they read the biography summary of the person #1. The reaction is from the actual perspective of the person. Don't use 'person #1' nor 'person #2' in the reaction");
		}
	}
	else if (args.fn == 4 || args.fn == 5) {
		{
			input.AddSub().Title("Task: merge multiple reactions of the same person into one").NoColon();
		}
		{
			auto& list = input.AddSub();
			list.Title("Reactions");
			list.NumberedLines();
			for(int i = 0; i < args.parts.GetCount(); i++) {
				String& s = args.parts[i];
				s.Replace("\r", "");
				s.Replace("\n", " ");
				list.Add(args.parts.GetKey(i), s);
			}
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("The merged reaction. This should 1-2 times the length of average reaction");
		}
		SetMaxLength(1024);
	}
	else if (args.fn == 6) {
		{
			input.AddSub().Title("Task: person #2 reads the biography of the person #1, and person #2 has reaction. Convert the reaction to a public profile description of the person #1 to a website").NoColon();
		}
		{
			auto& list = input.AddSub();
			list.Title("Reaction of the person #2 while reading the biography of person #1");
			list.NumberedLines();
			for(int i = 0; i < args.parts.GetCount(); i++) {
				String& s = args.parts[i];
				s.Replace("\r", "");
				s.Replace("\n", " ");
				list.Add(args.parts.GetKey(i), s);
			}
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("The public description of the person #1 at a website");
		}
		SetMaxLength(1024);
		SetHighQuality();
	}
	else if (args.fn == 7) {
		{
			input.AddSub().Title("Task: take the given description of the person (called Person #1 or Steve) and convert it to be said from the first person view (like 'I am this') by the given person").NoColon();
		}
		{
			auto& list = input.AddSub();
			list.Title("Description of the person");
			list.NoListChar();
			list.Add(args.text);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Description of the person converted to first person message (like 'I am this'). Description should fit to a social media site profile description.");
		}
		SetMaxLength(1024);
		//SetHighQuality();
	}
	else if (args.fn == 8) {
		{
			String t = "Task: take the given description of the person and shorten it while keeping the image of the person same";
			if (args.len > 0)
				t += ". Keep the shortened description in less than " + IntStr(args.len) + " characters";
			input.AddSub().Title(t).NoColon();
		}
		{
			auto& list = input.AddSub();
			list.Title("Description of the person");
			list.NoListChar();
			list.Add(args.text);
		}
		{
			String t = "Shortened description of the person";
			if (args.len > 0)
				t += " in less than " + IntStr(args.len) + " characters";
			TaskTitledList& results = input.PreAnswer();
			results.Title(t);
		}
		SetMaxLength(1024);
		//SetHighQuality();
	}
	else if (args.fn == 9) {
		{
			input.AddSub().Title("Task: explain the discussion in the message-thread").NoColon();
		}
		
		if (!args.text.IsEmpty()) {
			auto& list = input.AddSub();
			list.Title("Previous explanation of the message thread");
			list.NoListChar();
			args.text.Replace("\r", "");
			Vector<String> parts = Split(args.text, "\n\n");
			for(int i = 0; i < parts.GetCount(); i++) {
				String& s = parts[i];
				s.Replace("\n\n", "\n");
				list.Add(s);
			}
		}
		{
			for(int i = 0; i < args.parts.GetCount(); i++) {
				auto& list = input.AddSub();
				list.Title("New message by " + args.parts.GetKey(i));
				list.NoListChar();
				list.Add(args.parts[i]);
			}
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("The explanation of the discussion in the message-thread. Use names of persons. Keep track what was said or asked last");
		}
		SetMaxLength(1024);
	}
	else if (args.fn == 10) {
		{
			input.AddSub().Title("Merge images of all " + IntStr(args.parts.GetCount()) + " parts into single novel image").NoColon();
		}
		for(int i = 0; i < args.parts.GetCount(); i++) {
			String title = args.parts.GetKey(i);
			String s = args.parts[i];
			auto& list = input.AddSub();
			list.Title("Part " + IntStr(i+1) + ": " + title);
			list.NoListChar();
			list.Add(s);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Merge images of all " + IntStr(args.parts.GetCount()) + " parts into single novel image and shorten slightly (to half of the total length or larger). Don't mention that it is a merged image, but just an image.");
			results.NoListChar();
			results.Add("");
		}
	}
	else if (args.fn == 11) {
		{
			auto& list = input.AddSub();
			list.Title("List A: generic society roles for people");
			//list.NumberedLines();
			for(int i = 0; i < args.parts.GetCount(); i++)
				list.Add("#" + IntStr(i) + ": " + args.parts.GetKey(i));
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Top 15 societal roles from the List A, which would best match the audience of the web service: " + args.text + ". " + args.description);
			results.NumberedLines();
			results.Add("#");
		}
	}
	else if (args.fn == 12) {
		{
			auto& list = input.AddSub();
			list.Title("List A: roles of the audience of the website " + args.text + " (" + args.description + ")");
			for(int i = 0; i < args.parts.GetCount(); i++)
				list.Add(args.parts.GetKey(i));
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("What text fields should a electronic press kit include for " + args.profile + " in " + args.text + ". Give name and description of the text field (with ':' in between)");
			results.NumberedLines();
			results.Add("");
		}
	}
	else if (args.fn == 13) {
		{
			auto& list = input.AddSub();
			list.Title("Societal roles in relation to me");
			list.Add("my dog");
			list.Add("(my/'for me'/'looking me') " + args.text + ": " + args.description);
		}
		{
			auto& list = input.AddSub();
			list.Title("Types of score for a societal role");
			list.NumberedLines();
			for(int i = 0; i < SOCIETYROLE_SCORE_COUNT; i++)
				list.Add(GetSocietyRoleScoreKey(i));
		}
		{
			auto& list = input.AddSub();
			list.Title("Give all " + IntStr(SOCIETYROLE_SCORE_COUNT) + " scores for the societal role of my dog in modern western culture and in modern Nordic culture. Score is between 0 and 10:");
			list.NumberedLines();
			list.Add("8");
			list.Add("0");
			list.Add("7");
			list.Add("5");
			list.Add("0");
			list.Add("0");
			list.Add("9");
			list.Add("9");
			list.Add("2");
			list.Add("1");
			list.Add("3");
			list.Add("0");
			list.Add("6");
			list.Add("2");
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Give all " + IntStr(SOCIETYROLE_SCORE_COUNT) + " scores for the societal role of (my/'for me'/'looking me') " + args.text + " in modern western culture and in modern Nordic culture. Score is between 0 and 10");
			results.NumberedLines();
			results.Add("");
		}
		SetHighQuality();
	}
	else if (args.fn == 14) {
		{
			auto& list = input.AddSub();
			list.Title("List A: roles of the audience of the website " + args.text + " (" + args.description + ")");
			for(int i = 0; i < args.parts.GetCount(); i++)
				list.Add(args.parts.GetKey(i));
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("What photo types should a electronic press kit include for " + args.profile + " in " + args.text + ". Give name and description of the photo type (with ':' in between)");
			results.NumberedLines();
			results.Add("");
		}
	}
	else if (args.fn == 15) {
		{
			auto& list = input.AddSub();
			list.Title("List A: roles of the audience of the website " + args.text + " (" + args.description + ")");
			for(int i = 0; i < args.parts.GetCount(); i++)
				list.Add(args.parts.GetKey(i));
		}
		{
			auto& list = input.AddSub();
			list.Title("We are seeing photos in a electronic press kit include for " + args.profile + " in " + args.text);
			list.NoListChar();
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("A list of detailed descriptions of the images for a blind person. All images are following type '" +
				args.photo_description +
				"'. The pictures are from Western and Nordic cultures and the people are Caucasian");
			results.Add("\"");
		}
	}
	else if (args.fn == 16) {
		{
			auto& list = input.AddSub();
			list.Title("The web platform in this context: " + args.text);
			list.NoColon();
		}
		{
			auto& list = input.AddSub();
			list.Title("Some image descriptions suitable for some profiles in this web platform");
			for(int i = 0; i < args.parts.GetCount(); i++)
				list.Add(args.parts.GetKey(i));
		}
		{
			auto& list = input.AddSub();
			list.Title("The profile of the person in this context: " + args.description);
			list.NoColon();
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Top 3 novel image descriptions for the given profile based on given suitable image descriptions");
			results.NumberedLines();
			results.Add("");
		}
		SetHighQuality();
	}
	else if (args.fn == 17) {
		{
			auto& list = input.AddSub();
			list.Title("Image type: " + args.text);
			list.NoColon();
		}
		{
			auto& list = input.AddSub();
			list.Title("List A: text prompts");
			for(int i = 0; i < args.parts.GetCount(); i++)
				list.Add(args.parts.GetKey(i));
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Top 1 novel text prompt, which merges the List A, and which is less than 400 characters long");
			results.NumberedLines();
			results.Add("");
		}
		SetHighQuality();
	}
	else if (args.fn == 18) {
		{
			auto& list = input.AddSub();
			list.Title("Summarisation of the discussion so far");
			list.NoListChar();
			list.Add(args.text);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Top 3 responses for to this discussion for '" + args.profile + "'");
			results.NumberedLines();
			results.Add("\"");
		}
	}
	else if (args.fn == 19) {
		{
			auto& list = input.AddSub();
			list.Title("Message");
			list.NoListChar();
			list.Add(args.text);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Give keywords for the message (separated by comma in a single line)");
			results.NoListChar();
			results.Add("");
		}
	}
	else {
		TODO
	}
	#endif
}

void AiTask::CreateInput_BiographySummaryProcess(BasicPrompt& input) {
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	
	BiographySummaryProcessArgs args;
	args.Put(this->args[0]);
	
	
	if (args.fn == 0) {
		{
			input.AddSub().Title("Merging " + IntStr(args.parts.GetCount()) + " parts").NoColon();
		}
		for(int i = 0; i < args.parts.GetCount(); i++) {
			String title = args.parts.GetKey(i);
			String s = args.parts[i];
			auto& list = input.AddSub();
			list.Title("Part " + IntStr(i+1) + ": " + title);
			list.NoListChar();
			list.Add(s);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Merge all " + IntStr(args.parts.GetCount()) + " parts and shorten slightly (to half of the total length or larger)");
			results.NoListChar();
			results.Add("");
		}
	}
	else if (args.fn == 1) {
		{
			input.AddSub().Title("Merging " + IntStr(args.parts.GetCount()) + " lists").NoColon();
		}
		String header;
		for(int i = 0; i < args.parts.GetCount(); i++) {
			String title = args.parts.GetKey(i);
			String s = args.parts[i];
			int a = s.Find(":");
			if (a >= 0 && header.IsEmpty())
				header = s.Left(a+1);
			auto& list = input.AddSub();
			list.Title("List " + IntStr(i+1) + ": " + title);
			list.NoListChar();
			list.Add(s);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Merge all " + IntStr(args.parts.GetCount()) + " lists and values. Write empty values as \"N/A\"");
			//results.NoListChar();
			results.Add(header);
			tmp_str = header;
		}
	}
}

void AiTask::CreateInput_LeadSolver(BasicPrompt& input) {
	TODO
	#if 0
	MetaDatabase& mdb = MetaDatabase::Single();
	LeadData& ld = mdb.lead_data;
	
	
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	
	LeadSolverArgs args;
	args.Put(this->args[0]);
	
	if (args.fn != 8) {
		const LeadOpportunity& opp = ld.opportunities[args.opp_i];
		auto& list = input.AddSub().Title("Music A&R opportunity listing");
		list.Add("Name", opp.name);
		if (opp.min_entry_price_cents > 0)
			list.Add("Submission price", "$" + DblStr(opp.min_entry_price_cents*0.01));
		if (opp.min_compensation > 0)
			list.Add("Minimum compensation", "$" + DblStr(opp.min_compensation));
		if (opp.max_compensation > 0)
			list.Add("Maximum compensation", "$" + DblStr(opp.min_compensation));
		
		if (opp.request_description.GetCount())
			list.Add("Description (multiline)", "\n" + opp.request_description);
		if (opp.request_opportunity_description.GetCount())
			list.Add("Opportunity description (multiline)", "\n" + opp.request_opportunity_description);
		if (opp.request_band_description.GetCount())
			list.Add("Band rescription (multiline)", "\n" + opp.request_band_description);
		if (opp.request_selection_description.GetCount())
			list.Add("Selection description (multiline)", "\n" + opp.request_selection_description);
		
	}
	
	// Booleans
	if (args.fn == 0) {
		
		{
			auto& list = input.AddSub().Title("List of boolean attribute values, which can describe the listing");
			list.NumberedLines();
			list.Add("the listing is requesting music");
			for(int i = 0; i < LISTING_SONG_BOOLEAN_COUNT; i++) {
				list.Add(GetSongListingBooleanKey(i));
			}
		}
		
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Results for all attribute values");
			results.NumberedLines();
			results.Add("the listing is requesting music: true");
			//results.Add("true");
			results.Add("");
		}
		SetMaxLength(1024);
		this->SetHighQuality();
	}
	
	// Strings
	else if (args.fn == 1) {
		{
			auto& list = input.AddSub().Title("List of string attribute values, which can describe the listing");
			list.NumberedLines();
			list.Add("what kind of product is the listing requesting");
			for(int i = 0; i < LISTING_SONG_STRING_COUNT; i++) {
				list.Add(GetSongListingStringKey(i));
			}
		}
		
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Results for all attribute values");
			results.NumberedLines();
			results.Add("what kind of product is the listing requesting: music");
			//results.Add("true");
			results.Add("");
		}
		SetMaxLength(1024);
		//this->SetHighQuality();
	}
	
	// Lists
	else if (args.fn == 2) {
		{
			auto& list = input.AddSub().Title("List of attribute value lists, which can describe the listing");
			list.NumberedLines();
			list.Add("list of related words for this listing");
			for(int i = 0; i < LISTING_SONG_LIST_COUNT; i++) {
				list.Add(GetSongListingListKey(i));
			}
		}
		
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Results for all attribute values");
			results.NumberedLines();
			results.Add("list of related words for this listing: [music, song, opportunity, A&R]");
			results.Add("");
		}
		SetMaxLength(1024);
		//this->SetHighQuality();
	}
	// Average payout estimation
	else if (args.fn == 3) {
		{
			auto& list = input.AddSub().Title("List of probabilities (in percentages) to get accepted per stage for a unrelated song. Include total chance of acceptance percentage also");
			list.Add("Initial listen: 70%");
			list.Add("Production quality: 50%");
			list.Add("Comparison to other submissions: 40%");
			list.Add("Collaboration potential: 20%");
			list.Add("Refinement and final review: 15%");
			list.Add("Top contender selection: 10%");
			list.Add("Total chance of acceptance: 2.1% (0.7 x 0.5 x 0.4 x 0.2 x 0.15 x 0.1 = 0.0021 = 0.21%). Again, keep in mind that these numbers are theoretical and may vary.");
			//list.Add("Average payout estimation for accepted song: $1,250 x 2.1% = $26.25 or approximately $26.");
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("List of probabilities (in percentages) to get accepted per stage for the accepted song for the previous listing. Include total chance of acceptance percentage also");
			//results.Add("");
		}
		SetMaxLength(1024);
	}
	// Typecast
	else if (args.fn == 4) {
		EnterAppMode(appmode);
		{
			auto& list = input.AddSub().Title("List A: " + __Typeclasses + " of " + __entity + " profiles in relation to the " + __script2);
			list.NumberedLines();
			for (String tc : GetTypeclasses(appmode))
				list.Add(tc);
		}
		{
			auto& list = input.AddSub().Title("List B of names for archetypical parts of storyline of a modern " + GetAppModeKey(appmode, AM_GENRES) + " " + __comps2 + ", which contrasts each other");
			list.NumberedLines();
			for (const auto& it : GetContents(appmode)) {
				String s;
				s << "A: " << it.parts[0] << ", B: " << it.parts[1] << ", C: " << it.parts[2];
				list.Add(s);
			}
		}
		{
			auto& list = input.AddSub().Title("Examples of combinations from List A and List B");
			list.NumberedLines();
			list.Add("4,6");
			list.Add("7,2");
			list.Add("6,1");
		}
		LeaveAppMode();
		{
			TaskTitledList& results = input.PreAnswer();
			results.NumberedLines();
			results.Title("Top 3 combinations from List A and List B, which best fits the Music A&R opportunity listing");
			results.Add("");
		}
		SetHighQuality();
	}
	// Script ideas
	else if (args.fn == 5) {
		const LeadOpportunity& o = ld.opportunities[args.opp_i];
		
		const auto& tc_list = TextLib::GetTypeclasses(DB_SONG);
		if (o.typeclasses.GetCount() && o.typeclasses[0] < tc_list.GetCount()) {
			String tc = tc_list[o.typeclasses[0]];
			auto& list = input.AddSub().Title("Preferred typecast for the song");
			list.Add(tc);
		}
		
		const auto& co_list = TextLib::GetContents(DB_SONG);
		if (o.contents.GetCount() && o.contents[0] < co_list.GetCount()) {
			const auto& co_full = co_list[o.contents[0]];
			auto& list = input.AddSub().Title("Preferred content");
			list.Add(co_full.key);
			for(int i = 0; i < ContentType::PART_COUNT; i++)
				list.Add(co_full.parts[i]);
		}
		{
			auto& list = input.AddSub().Title("Examples of ideas for unrelated lyrics");
			list.NumberedLines();
			list.Add("The lyrics expresses the frustration and impatience of being caught up and hung up on someone, using time as a metaphor for the slow progress of a relationship.");
			list.Add("The lyrics is about the destructive and seductive nature of fame and the Hollywood lifestyle, comparing it to a drug addiction that can lead to the downfall of individuals and society.");
			list.Add("The lyrics is about a wannabe who tries too hard to fit in with the cool crowd, using humor and irony to convey the idea that people should just be themselves instead of trying to be something they're not.");
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.NumberedLines();
			results.Title("Top 3 ideas for a lyrics, which would best fit the Music A&R opportunity listing");
			results.Add("The lyrics is about");
		}
		SetHighQuality();
	}
	// Music style ideas
	else if (args.fn == 6) {
		const LeadOpportunity& o = ld.opportunities[args.opp_i];
		
		const auto& tc_list = TextLib::GetTypeclasses(DB_SONG);
		if (o.typeclasses.GetCount() && o.typeclasses[0] < tc_list.GetCount()) {
			String tc = tc_list[o.typeclasses[0]];
			auto& list = input.AddSub().Title("Preferred typecast for the song");
			list.Add(tc);
		}
		
		const auto& co_list = TextLib::GetContents(DB_SONG);
		if (o.contents.GetCount() && o.contents[0] < co_list.GetCount()) {
			const auto& co_full = co_list[o.contents[0]];
			auto& list = input.AddSub().Title("Preferred content");
			list.Add(co_full.key);
			for(int i = 0; i < ContentType::PART_COUNT; i++)
				list.Add(co_full.parts[i]);
		}
		
		
		if (o.lyrics_ideas.GetCount()) {
			for(int i = 0; i < o.contents.GetCount(); i++) {
				int co_i = o.contents[i];
				if (co_i >= 0 && co_i < co_list.GetCount()) {
					const auto& co_full = co_list[co_i];
					auto& list = input.AddSub().Title("Preferred lyrics idea");
					list.Add(o.lyrics_ideas[0]);
					break;
				}
				else {
					LOG("warning: invalid content id");
				}
			}
		}
		{
			auto& list = input.AddSub().Title("Examples of music styles for unrelated songs, which fits under 120 characters");
			list.NumberedLines();
			list.Add("Rap, EDM, 90bpm, female alto vocal range, long vocal notes");
			list.Add("EDM, Nu-Metal, Funk Rock, 120bpm, male vocals");
			list.Add("Country, Country Pop, 100bpm, alto female vocals, long vocal notes, Acoustic Guitar, Electric Bass, Drums");
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.NumberedLines();
			results.Title("Top 3 ideas for a music style, which would best fit the Music A&R opportunity listing, and which fits under 120 characters");
			results.Add("");
		}
		SetHighQuality();
	}
	
	else if (args.fn == 7) {
		const LeadOpportunity& o = ld.opportunities[args.opp_i];
		
		{
			TaskTitledList& results = input.PreAnswer();
			results
				.Title(	"Create a template listing based on this listing. "
						"Use extremely generic and boring language. "
						"Don't mention brands but use generic product names. Give title and description");
			;
			results.Add("Title: \"");
		}
	}
	
	else if (args.fn == 8) {
		MetaDatabase& mdb = MetaDatabase::Single();
		LeadDataTemplate& ldt = LeadDataTemplate::Single();
		LeadTemplate& lt = ldt.templates[args.opp_i];
		{
			auto& list = input.AddSub().Title("Music A&R opportunity listing #1");
			list.Add("Title: \"Get Your Music Heard Worldwide with #1 Radio Promotion Opportunity\"");
			list.Add(
				"Description:\n"
				"Are you a musical artist looking for a chance to have your music reach a global audience? Look no further! This opportunity offers airplay and promotion on a highly rated internet radio podcast that has been helping talented musicians gain recognition for 7 years. Featuring a variety of genres and highlighting the best in vocal excellence, emotional depth, and impactful lyrics, this station is perfect for showcasing your skills. But that's not all! Your music will also be featured on the #1 New & Noteworthy Music Podcast, reaching over 100,000 downloads in just 7 months. With promotion on social media and other channels, your music is sure to get the recognition it deserves. Don't miss your chance to get your music heard and submit your songs today! Please note that submissions must be performed by female artists, female-fronted bands, or female vocalists only. Don't let this opportunity pass you by!");
			list.Add("Speciality of the listing's author in short (music genre speciality, clients speciality)",
				"The listing author specializes in promoting and showcasing female artists and female-fronted bands across all genres. They have a successful track record of helping independent artists gain recognition and have a strong network in the music industry.");
			list.Add("Class of the listing's author (e.g. publisher / A&R / licensing agent etc. ) with 1-3 words", "A&R and Promotion Expert");
			list.Add("Profit reasons for the author of this listing",
				"\n1. To make a profit from the submission fees of interested artists.\n"
				"2. To promote their radio and music podcast platform and attract more listeners and sponsors.\n"
				"3. To potentially discover and represent new talent in the industry and earn a percentage of their earnings.")
				;
			list.Add("Positive organizational reasons for the author of this listing",
				"\n1. To support and promote female musicians and bring more diversity to the music industry.\n"
				"2. To provide a platform for independent artists to gain exposure and recognition for their talent.\n"
				"3. To stay at the forefront of the music industry and discover new trends and talent in various genres.");
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Music A&R opportunity listing #2");
			results.Add("Name", lt.title);
			results.Add("Description (multiline)", "\n" + lt.text);
			//results.Add("Speciality of the listing's author in short (music genre speciality, clients speciality)", "");
		}
	}
	else TODO
	#endif
}

void AiTask::CreateInput_SocialBeliefsProcess(BasicPrompt& input) {
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	
	BeliefArgs args;
	args.Put(this->args[0]);
	
	if (args.fn == 0) {
		String c = IntStr(args.pos.GetCount());
		{
			auto& list = input.AddSub();
			list.Title("Description of a text");
			for(int i = 0; i < args.user.GetCount(); i++)
				list.Add(args.user[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("List of top 10 terms that describe the text's worldview, ideology or the like");
			results.NumberedLines();
			results.Add("");
		}
		SetMaxLength(2048);
	}
	else if (args.fn == 1) {
		String c = IntStr(args.pos.GetCount());
		{
			auto& list = input.AddSub();
			list.Title("List of top " + c + " terms that describe this worldview, ideology or the like");
			list.NumberedLines();
			for(int i = 0; i < args.pos.GetCount(); i++)
				list.Add(args.pos[i]);
		}
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("List of top " + c + " terms that are the opposite of previous 10 terms");
			results.NumberedLines();
			results.Add("");
		}
		SetMaxLength(2048);
	}
}

void AiTask::CreateInput_Marketplace(BasicPrompt& input) {
	if (args.IsEmpty()) {
		SetFatalError("no args");
		return;
	}
	
	MarketplaceArgs args;
	args.Put(this->args[0]);
	{
		auto& list = input.AddSub().Title("Information about the marketplace item");
		for(int i = 0; i < args.map.GetCount(); i++)
			if (args.map[i].GetCount())
				list.Add(args.map.GetKey(i), args.map[i]);
	}
	
	const VectorMap<String, Vector<String>>& sects = GetMarketplaceSections();
	
	if (args.fn == 0) {
		{
			TaskTitledList& results = input.PreAnswer();
			results.Title("Write the description for the marketplace item in Finnish. This is for a drift store, so don't oversell it. Don't include the price nor category");
			results.NoListChar();
			results.Add("");
		}
		SetMaxLength(2048);
	}
}


END_UPP_NAMESPACE

