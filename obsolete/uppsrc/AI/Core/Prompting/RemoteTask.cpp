#include "Prompting.h"

NAMESPACE_UPP

void AiTask::Store(bool force)
{
	if(output.IsEmpty())
		return;
	if(!changed)
		return;
	String dir = ConfigFile("ai-results");
	RealizeDirectory(dir);
	String filename = GetInputHash() + ".txt";
	String file = dir + filename;
	SaveFileBOMUtf8(file, output);
	changed = false;
}

void AiTask::Load()
{
	if(skip_load)
		return;
	String dir = ConfigFile("ai-results");
	RealizeDirectory(dir);
	String filename = GetInputHash() + ".txt";
	String file = dir + filename;
	if(FileExists(file))
		output = LoadFileBOM(file);
	if(IsAnyImageTask()) {
		String dir = ConfigFile("images");
		Vector<String> rel_paths = Split(output, "\n");
		recv_images.Clear();
		bool fail = false;
		for(String& rel_path : rel_paths) {
			String path = AppendFileName(dir, rel_path);
			Image& img = recv_images.Add();
			img = StreamRaster::LoadFileAny(path);
			if(img.IsEmpty())
				fail = true;
		}
		if(fail) {
			recv_images.Clear();
			output.Clear();
		}
	}
}

void AiTask::SetError(String s)
{
	failed = true;
	error = s;
	WhenError();
}

String AiTask::GetInputHash() const
{
	String input = MakeInputString();
	hash_t h = input.GetHashValue();
	if (vision) {
		CombineHash c;
		c.Put(h);
		vision->AppendHash(c);
		h = c;
	}
	return HexString((void*)&h, sizeof(h));
}

String AiTask::GetOutputHash() const
{
	hash_t h = output.GetHashValue();
	return HexString((void*)&h, sizeof(h));
}

bool AiTask::HasJsonInput() const {
	return !completion && !input_json.IsEmpty();
}

bool AiTask::HasAnyInput() const {
	if (model)
		return true;
	else if (completion)
		return true;
	else if (chat)
		return true;
	else if (vision)
		return true;
	else if (transcription)
		return true;
	else if (image)
		return true;
	else
		return false;
}

void AiTask::SetMaxLength(int tokens) {
	if (model)
		;
	else if (completion)
		completion->max_length = tokens;
	else if (chat)
		chat->max_completion_tokens = tokens;
	else if (vision)
		vision->max_length = tokens;
	else if (transcription)
		transcription->max_length = tokens;
	else if (image)
		;
	else
		TODO
}

void AiTask::SetPrompt(String s)
{
	if (model)
		;
	else if (completion)
		completion->prompt = s;
	else if (chat) {
		ASSERT_(0, "can't set prompt to chat args");
	}
	else if (vision)
		vision->prompt = s;
	else if (transcription)
		transcription->prompt = s;
	else if (image)
		;
	else
		TODO
}

String AiTask::MakeInputString(bool pretty) const {
	if (model)
		return String();
	if (completion)
		return completion->prompt;
	if (chat)
		return StoreAsJson(chat->messages);
	if (vision)
		return vision->prompt;
	if (transcription)
		return transcription->prompt;
	if (image)
		return image->prompt;
	//if (stage)
	//	return stage->body + "\n" + stage->funcs[fn_i].body + "\n" + AsJSON(vargs, true);
	TODO
	return String();
	/*else if (!input_json.IsEmpty())
		return input_json.AsJSON(pretty);
	else
		return input_basic.AsString();*/
}

String AiTask::GetDescription() const
{
	String s;
	s << GetTypeString();
	return s;
}

String AiTask::GetTypeString() const { return TaskRule::name; }

bool AiTask::ProcessInput()
{
	bool ok = true;
	
	bool premade_prompt =
		(model) ||
		(completion && completion->prompt.GetCount()) ||
		(chat && chat->messages.GetCount()) ||
		(vision && vision->prompt.GetCount()) ||
		(transcription && transcription->prompt.GetCount()) ||
		(image && image->prompt.GetCount());
	
	if (premade_prompt) {
		Load();
	}
	else if (TaskRule::input_basic) {
		input_basic.Clear();
		input_basic.Create();
		(this->*TaskRule::input_basic)(*input_basic);
		if(fast_exit)
			return true;

		if(failed)
			return false;
		
		SetPrompt(input_basic->AsString());

		Load();
	}
	else if (TaskRule::input_json) {
		input_json.Clear();
		input_json.Create();
		if (!chat)
			chat.Create();
		
		(this->*TaskRule::input_json)(*input_json);
		if(fast_exit)
			return true;

		if(failed)
			return false;
		
		if (chat->model_name.IsEmpty())
			chat->model_name = "o4-mini";
		for (const auto& from : input_json->messages) {
			auto& to = chat->messages.Add();
			switch (from.type) {
				case JsonPrompt::ASSIST: to.type = AiMsgType::MSG_DEVELOPER; break;
				case JsonPrompt::SYSTEM: to.type = AiMsgType::MSG_SYSTEM; break;
				case JsonPrompt::USER:   to.type = AiMsgType::MSG_USER; break;
				default: to.type = AiMsgType::MSG_NULL; break;
			}
			to.content = from.GetContentString();
		}
		
		Load();
	}
	else {
		// Return if this task won't have input function
		return ok;
	}

	// Remove Win32 uselessness (\r in newline)
	output = TrimBoth(output);

	// Request output from completion-mode AI
	if(HasAnyInput() && output.IsEmpty()) {
		ok = RunOpenAI();
	}

	// Remove Win32 uselessness (\r in newline)
	output.Replace("\r", "");

	if(output.IsEmpty())
		return false;

	return ok;
}

void AiTask::Process()
{

	if(ret_fail) {
		output = "fail";
		try {
			(this->*TaskRule::process)();
		}
		catch(Exc e) {
			LOG("error: " << e);
		}
		ready = true;
		fast_exit = false;
		skip_load = false;
		processing = false;
		WhenDone();
		return;
	}

	// LOG("AiTask::Process: begin of " << TaskRule::name);
	processing = true;

	bool ok = true;

	ok = ok && ProcessInput();

	if(fast_exit) {
		ready = true;
	}
	else if(ok) {
		if(TaskRule::process) {
			try {
				(this->*TaskRule::process)();
			}
			catch(Exc e) {
				LOG("error: " << e);
			}
		}

		if(wait_task) {
			wait_task = false;
		}
		else {
			if(!failed) {
				ready = true;
			}
			changed = true;
		}
	}

	fast_exit = false;
	skip_load = false;
	processing = false;

	if(ready)
		WhenDone();

	// LOG("AiTask::Process: end of " << rule->name);
}

bool AiTask::RunOpenAI()
{
	switch (type) {
		case TYPE_MODEL:			return RunOpenAI_Model();
		case TYPE_CHAT:				return RunOpenAI_Chat();
		case TYPE_COMPLETION:		return RunOpenAI_Completion();
		case TYPE_IMAGE_GENERATION:	return RunOpenAI_Image();
		case TYPE_IMAGE_EDIT:		return RunOpenAI_Image();
		case TYPE_IMAGE_VARIATE:	return RunOpenAI_Image();
		case TYPE_VISION:			return RunOpenAI_Vision();
		case TYPE_TRANSCRIPTION:	return RunOpenAI_Transcription();
		default: TODO; return false;
	}
}

String AiTask::FixInvalidChars(const String& s)
{
	WString ws = s.ToWString();
	WString out;
	for(int i = 0; i < ws.GetCount(); i++) {
		int chr = ws[i];

		// ascii
		if(chr < 32 && chr != '\n') {
			// pass
		}
		else {
			out.Cat(chr);
		}
	}
	return out.ToString();
}

void AiTask::EscapeString(String& s)
{
	s = ToCharset(CHARSET_ISO8859_15, s, CHARSET_UTF8);
	s = ToCharset(CHARSET_UTF8, s, CHARSET_ISO8859_15);
	s = StoreAsJson(s);
	RemoveQuotes(s);
}

void AiTask::RemoveQuotes(String& s)
{
	if(s.GetCount() > 0 && s[0] == '\"')
		s = s.Mid(1);
	int c = s.GetCount();
	if(c > 0 && s[c - 1] == '\"')
		s = s.Left(c - 1);
}

void AiTask::RemoveQuotes2(String& s_)
{
	WString ws = s_.ToWString();
	if(ws.GetCount() > 0 && (ws[0] == '\"' || ws[0] == L"“"[0]))
		ws = ws.Mid(1);
	int c = ws.GetCount();
	if(c > 0 && (ws[c - 1] == '\"' || ws[c - 1] == L"”"[0]))
		ws = ws.Left(c - 1);
	s_ = ws.ToString();
}

void AiTask::RemoveParenthesis(String& s)
{
	if(s.GetCount() > 0 && s[0] == '(')
		s = s.Mid(1);
	int c = s.GetCount();
	if(c > 0 && s[c - 1] == ')')
		s = s.Left(c - 1);
}

bool AiTask::RunOpenAI_Image()
{
	if (!image)
		return false;
	ImageArgs& args = *image;
	output.Clear();

	String prompt = args.prompt;

	prompt.Replace("\\", "\\\\");
	prompt.Replace("\n", " ");
	prompt.Replace("\t", " ");
	prompt.Replace("\"", "\\\"");
	prompt = TrimBoth(prompt);

	prompt = FixInvalidChars(prompt); // NOTE: warning: might break something

	// Don't even try offensive language
	prompt.Replace(" fuck ", " f**k ");
	prompt.Replace(" bitch ", " b***h ");
	prompt.Replace(" whore ", " w***e ");
	prompt.Replace(" nigga ", " n***a ");

	if(image_n.IsEmpty() || image_sz.IsEmpty()) {
		SetError("No image arguments set");
		return false;
	}

	String recv;
#if HAVE_OPENAI
	try {
		if(TaskRule::type == TYPE_IMAGE_EDIT) {
			if(send_images.GetCount() != 1) {
				SetError("expected sendable images");
				return false;
			}
			String file_path0 = ConfigFile("tmp0.png");
			PNGEncoder().SaveFile(file_path0, send_images[0]);

			openai::Json json({{"image", file_path0.Begin()},
			                   {"prompt", prompt.Begin()},
			                   {"n", StrInt(image_n)},
			                   {"size", image_sz},
			                   {"response_format", "b64_json"}});
			auto img = openai::image().edit(json);
			recv = String(img.dump(2));
		}
		else if(TaskRule::type == TYPE_IMAGE_VARIATE) {
			if(send_images.GetCount() != 1) {
				SetError("expected sendable images");
				return false;
			}
			String file_path0 = ConfigFile("tmp0.png");
			PNGEncoder().SaveFile(file_path0, send_images[0]);

			openai::Json json({{"image", file_path0.Begin()},
			                   {"n", StrInt(image_n)},
			                   {"size", image_sz},
			                   {"response_format", "b64_json"}});
			auto img = openai::image().variation(json);
			recv = String(img.dump(2));
		}
		else {
			openai::Json json({{"prompt", prompt.Begin()},
			                   {"n", StrInt(image_n)},
			                   {"size", image_sz},
			                   {"response_format", "b64_json"}});
			auto img = openai::image().create(json);
			recv = String(img.dump(2));
		}
	}
	catch(std::runtime_error e) {
		return OnImageException(e.what());
	}
	catch(std::string e) {
		return OnImageException(e.c_str());
	}
	catch(NLOHMANN_JSON_NAMESPACE::detail::parse_error e) {
		return OnImageException(e.what());
	}
	catch(std::exception e) {
		return OnImageException(e.what());
	}
#endif
	
	DalleResponse response;
	LoadFromJson(response, recv);

	output.Clear();
	recv_images.Clear();

	for(int i = 0; i < response.data.GetCount(); i++) {
		String img_str = Base64Decode(response.data[i].b64_json);

		Image& in = recv_images.Add();
		in = StreamRaster::LoadStringAny(img_str);

		// Get file path
		String part_str = " " + IntStr(i + 1) + "/" + IntStr(response.data.GetCount());
		if(TaskRule::type == TYPE_IMAGE_EDIT || TaskRule::type == TYPE_IMAGE_VARIATE)
			part_str << " "
					 << IntStr64(Random64()); // add never-matching random number to name for
			                                  // editing and variation creation purposes
		String dir = ConfigFile("images");
		String filename = Base64Encode(prompt + part_str) + ".png";
		String rel_path = AppendFileName(image_sz, filename);
		String path = AppendFileName(dir, rel_path);
		RealizeDirectory(GetFileDirectory(path));

		// Store to file
		PNGEncoder enc;
		enc.SaveFile(path, in);

		// Add file path to output
		output << rel_path << "\n";
	}

	changed = true;
	Store();
	return output.GetCount() > 0;
}

bool AiTask::OnImageException(String msg) {
	if(keep_going) {
		output = " ";
		GetTaskMgr().keep_going_counter++;
		return true;
	}
	fatal_error = true;
	SetError(msg);
	Array<Image> res;
	WhenResultImages(res);
	if(auto_ret_fail)
		ReturnFail();
	return false;
}

bool AiTask::RunOpenAI_Model()
{
	output.Clear();
	
	ASSERT(model);
	if (!model)
		return false;
	
#if HAVE_OPENAI
	return TryOpenAI("", "", [this]{
		ModelArgs& args = *model;
		
		if (args.fn == ModelArgs::FN_LIST) {
			OpenAiModelResponse response;
			auto model_list = openai::model().list();
			LOG("Response is:\n" << model_list.dump(2));
			LoadFromJson(response, String(model_list.dump(2)));
			// LOG(response.ToString());
			
			Vector<String> models;
			for (auto it : response.data)
				models.Add(it.id);
			output = StoreAsJson(models);
			WhenResult(output);
		}
		else TODO
	});
#else
	return false;
#endif
}

bool AiTask::RunOpenAI_Completion()
{
	output.Clear();
	
	ASSERT(completion);
	if (!completion)
		return false;

	CompletionArgs& args = *completion;
	if(!args.max_length) {
		LOG("warning: no response length set");
		args.max_length = 1024;
	}
	String prompt = args.prompt;

	prompt = FixInvalidChars(prompt); // NOTE: warning: might break something

	// Cache prompts too (for crash debugging)
	{
		String prompt_cache_dir = ConfigFile("prompt-cache");
		String fname = IntStr64(prompt.GetHashValue()) + ".txt";
		// DUMP(fname);
		String path = prompt_cache_dir + DIR_SEPS + fname;
		if(!FileExists(path)) {
			RealizeDirectory(prompt_cache_dir);
			FileOut fout(path);
			fout << prompt;
			fout.Flush();
			fout.Close();
		}
	}
	
	{
		EscapeString(prompt);

		if(GetDefaultCharset() != CHARSET_UTF8)
			prompt = ToCharset(CHARSET_UTF8, prompt, CHARSET_DEFAULT);

		ASSERT(!(input_json && input_json->force_completion));
		
		String txt = R"_({
		    "model": )_" + AsJSON(args.model_name) + R"_(,
		    "prompt": )_" + AsJSON(args.prompt) + R"_(,
		    "best_of": )_" + IntStr(args.best_of) + R"_(,
		    "frequency_penalty": )_" + DblStr(args.frequency_penalty) + R"_(,
		    "max_tokens": )_" + IntStr(args.max_length) + R"_(,
		    "presence_penalty": )_" + DblStr(args.presence_penalty) + R"_(,
		    "stop": )_" + AsJSON(args.stop_seq) + R"_(,
		    "temperature": )_" + DblStr(args.temperature) + R"_(,
		    "top_p": )_" + DblStr(args.top_prob) + R"_(
		})_";
		
#if HAVE_OPENAI
		return TryOpenAI(prompt, txt, [this,txt]{
			nlohmann::json json = nlohmann::json::parse(txt.Begin(), txt.End());
			OpenAiResponse response;
			
			auto completion = openai::completion().create(json);
			// LOG("Response is:\n" << completion.dump(2));
			LoadFromJson(response, String(completion.dump(2)));
			// LOG(response.ToString());
			
			if(response.choices.GetCount())
				output = response.choices[0].GetText();
			else {
				SetError("invalid output");
				output.Clear();
			}
		});
#else
		return false;
#endif
	}
	return false;
}

bool AiTask::RunOpenAI_Chat()
{
	output.Clear();
	
	ASSERT(chat);
	if (!chat)
		return false;

	ChatArgs& args = *chat;
	if(!args.max_completion_tokens) {
		LOG("warning: no response length set");
		args.max_completion_tokens = 1024;
	}
	String prompt = StoreAsJson(args);
	
	// Cache prompts too (for crash debugging)
	{
		String prompt_cache_dir = ConfigFile("prompt-cache");
		String fname = IntStr64(prompt.GetHashValue()) + ".txt";
		// DUMP(fname);
		String path = prompt_cache_dir + DIR_SEPS + fname;
		if(!FileExists(path)) {
			RealizeDirectory(prompt_cache_dir);
			FileOut fout(path);
			fout << prompt;
			fout.Flush();
			fout.Close();
		}
	}
	
	{
		String sys_key;
		if (args.model_name.GetCount() >= 2 &&
			args.model_name[0] == 'o' &&
			IsDigit(args.model_name[1]))
			sys_key = "developer";
		else
			sys_key = "system";
		
		String messages_txt = "[";
		for(int i = 0; i < args.messages.GetCount(); i++) {
			if (i) messages_txt.Cat(',');
			const auto& msg = args.messages[i];
			
			if (msg.type == MSG_DEVELOPER) {
				messages_txt <<
					"{" <<
						"\"content\":" << AsJSON(msg.content) << "," <<
						"\"role\":\"" << sys_key << "\"";
				if (msg.name.GetCount())
					messages_txt << ",\"name\": " << AsJSON(msg.name);
				messages_txt << "}";
			}
			else if (msg.type == MSG_USER) {
				messages_txt <<
					"{" <<
						"\"content\":" << AsJSON(msg.content) << "," <<
						"\"role\":\"user\"";
				if (msg.name.GetCount())
					messages_txt << ",\"name\": " << AsJSON(msg.name);
				messages_txt << "}";
			}
			else if (msg.type == MSG_ASSISTANT) {
				messages_txt <<
					"{" <<
						"\"content\":" << AsJSON(msg.content) << "," <<
						"\"role\":\"assistant\"";
				if (msg.name.GetCount())
					messages_txt << ",\"name\": " << AsJSON(msg.name);
				if (msg.refusal.GetCount())
					messages_txt << ",\"refusal\": " << AsJSON(msg.refusal);
				if (msg.tool_calls.GetCount())
					messages_txt = ",\"tool_calls\":" << AsJSON(msg.tool_calls);
				messages_txt << "}";
			}
			else if (msg.type == MSG_FUNCTION) {
				messages_txt <<
					"{" <<
						"\"content\":" << AsJSON(msg.content) << ","
						"\"role\":\"function\","
						"\"name\": " << AsJSON(msg.name) <<
					"}";
			}
			else TODO
		}
		messages_txt << "]";
		
		// Modalities breaks at least o4-mini
		//if (args.modalities.IsEmpty())
		//	args.modalities << "text";
		
		String txt = R"_({
		    "messages": )_" + messages_txt + R"_(,
		    "model": )_" + AsJSON(args.model_name) + R"_(,
		    "frequency_penalty": )_" + DblStr(args.frequency_penalty) + R"_(,
		    "max_completion_tokens": )_" + IntStr(args.max_completion_tokens) +
			R"_(,"top_p": )_" + AsJSON(args.top_prob) + R"_(,
		    "n": )_" + IntStr(args.count) + R"_(,
			"presence_penalty": )_" + DblStr(args.presence_penalty) + R"_(,
		    "temperature": )_" + DblStr(args.temperature);
		if (!args.prediction.IsEmpty()) {
			txt += R"_(,"prediction":)_";
			if (!args.prediction.content_txt.IsEmpty())
				txt += AsJSON(args.prediction.content_txt);
			else
				txt += AsJSON(args.prediction.content_parts);
		}
		if (args.modalities.GetCount())
			txt += R"_(,"modalities": )_" + StoreAsJson(args.modalities);
		if (args.reasoning_effort)
			txt += ",\"reasoning_effort\":" + AsJSON(GetReasoningEffortString(args.reasoning_effort));
		if (!args.tool_choice.IsEmpty())
			txt += ",\"tool_choice\":" + StoreAsJson(args.tool_choice);
		if (!args.tools.IsEmpty())
			txt += ",\"tools\":" + StoreAsJson(args.tools);
		if (!args.stop_seq.IsEmpty())
			txt += ",\"stop\":" + AsJSON(args.stop_seq);
		if (!args.web_search_options.IsEmpty())
			txt += ",\"web_search_options\":" + StoreAsJson(args.web_search_options);
		txt += "\n}";
		
		LOG("AiTask::RunOpenAI_Chat:\n" << txt);
		
#if HAVE_OPENAI
		return TryOpenAI(prompt, txt, [this,txt]{
			nlohmann::json json = nlohmann::json::parse(txt.Begin(), txt.End());
			OpenAiResponse response;
			
			auto chat = openai::chat().create(json);
			LOG("Response is:\n" << chat.dump(2));
			LoadFromJson(response, String(chat.dump(2)));
			// LOG(response.ToString());
			
			if(response.choices.GetCount())
				output = response.choices[0].GetText();
			else {
				SetError("invalid output");
				output.Clear();
			}
		});
#else
		return false;
#endif
	}
	return false;
}

bool AiTask::RunOpenAI_Vision()
{
	if (!vision)
		return false;
	output.Clear();

	VisionArgs& args = *vision;
	if(!args.max_length) {
		LOG("warning: no response length set");
		args.max_length = 1024;
	}
	String prompt = args.prompt;
	
	if (args.jpeg.IsEmpty()) {
		SetError("no jpeg set");
		return false;
	}
	String jpeg_base64 = Base64Encode(args.jpeg);

	{
		EscapeString(prompt);

		if(GetDefaultCharset() != CHARSET_UTF8)
			prompt = ToCharset(CHARSET_UTF8, prompt, CHARSET_DEFAULT);

		String txt;
		if(quality == 1) {
			txt = R"_({
    "model": "gpt-4o",
    "messages":[
		{"role":"user", "content": [
			{
				"type":"text",
				"text":")_" +
			      prompt + R"_("
			},
			{
				"type":"image_url",
				"image_url": {
					"url": "data:image/jpeg;base64,)_" +
			      jpeg_base64 + R"_("
				}
			}
		]}
	],
    "max_tokens": )_" +
			      IntStr(args.max_length) + R"_(,
    "temperature": 1
})_";
		}
		else {
			txt = R"_({
    "model": "gpt-4-turbo",
    "messages":[
		{"role":"user", "content": [
			{
				"type":"text",
				"text":")_" +
			      prompt + R"_("
			},
			{
				"type":"image_url",
				"image_url": {
					"url": "data:image/jpeg;base64,)_" +
			      jpeg_base64 + R"_("
				}
			}
		]}
	],
    "max_tokens": )_" +
			      IntStr(args.max_length) + R"_(,
    "temperature": 1
})_";
		}
		LOG(txt);
		
#if HAVE_OPENAI
		return TryOpenAI(prompt, txt, [this,txt]{
			nlohmann::json json = nlohmann::json::parse(txt.Begin(), txt.End());
			OpenAiResponse response;

			auto completion = openai::completion().create_chat(json);
			LoadFromJson(response, String(completion.dump(2)));

			if(response.choices.GetCount())
				output = response.choices[0].GetText();
			else {
				SetError("invalid output");
				output.Clear();
			}
		});
#else
		return false;
#endif
	}
}

bool AiTask::RunOpenAI_Transcription()
{
	if (!transcription)
		return false;
	TranscriptionArgs& args = *transcription;
	
	output.Clear();

	if(!args.max_length) {
		LOG("warning: no response length set");
		args.max_length = 1024;
	}
	String prompt = args.prompt;
	
	{
		EscapeString(prompt);

		if(GetDefaultCharset() != CHARSET_UTF8)
			prompt = ToCharset(CHARSET_UTF8, prompt, CHARSET_DEFAULT);

		auto& ai_mgr = AiManager();
		ASSERT(args.ai_provider_idx >= 0 && args.ai_provider_idx < ai_mgr.GetCount());
		const auto& prov = ai_mgr[args.ai_provider_idx];
		
#if HAVE_OPENAI
		if (prov.type == AiServiceProvider::OPENAI) {
			String txt =
			R"_({
			    "model": "whisper-1",
			    "response_format": "verbose_json",
			    "language": ")_" + args.language + R"_("
			})_";
			return TryOpenAI(prompt, txt, [this,txt,&args]{
				nlohmann::json json = nlohmann::json::parse(txt.Begin(), txt.End());
				
				json.push_back({"file", args.file});
				
				//OpenAiResponse response;
	
				auto transcription = openai::audio().transcribe(json);
				String str = String(transcription.dump(2));
				//LOG(str);
				//LoadFromJson(response, str);
	
				if (!str.IsEmpty())
					output = str;
				else {
					SetError("invalid output");
					output.Clear();
				}
			});
		}
		else if (prov.type == AiServiceProvider::API_WHISPERFILE_TRANSCRIPT) {
			#ifdef flagWIN32
			TODO
			#else
			String output = ConfigFile("curl-output.txt");
			// TODO use api
			String cmd =
			"curl " + prov.url + "/inference"
			   " -H \"Content-Type: multipart/form-data\""
			   " -F file=\"@" + args.file + "\""
			   " -F language=\"" + args.language + "\""
			   " -F temperature=\"0.0\""
			   " -F temperature_inc=\"0.2\""
			   " -F response_format=\"verbose_json\""
			   " -o \"" + output + "\"";
			LOG(cmd);
			String out;
			if (!Sys(cmd, out)) {
				this->output = LoadFile(output);
				return true;
			}
			else {
				SetError("executing curl failed");
				output.Clear();
				return false;
			}
			#endif
		}
#else
		return false;
#endif
	}
	return false;
}

bool AiTask::TryOpenAI(String prompt, String txt, Event<> cb) {
#if HAVE_OPENAI
	{
		try {
			cb();
		}
		catch(std::runtime_error e) {
			if(keep_going) {
				output = " ";
				return true;
			}
			LOG(prompt);
			LOG(txt);
			fatal_error = true;
			SetError(e.what());
			if(auto_ret_fail)
				ReturnFail();
			return false;
		}
		catch(std::string e) {
			if(keep_going) {
				output = " ";
				return true;
			}
			LOG(prompt);
			LOG(txt);
			fatal_error = true;
			SetError(e.c_str());
			if(auto_ret_fail)
				ReturnFail();
			return false;
		}
		catch(NLOHMANN_JSON_NAMESPACE::detail::parse_error e) {
			if(keep_going) {
				output = " ";
				return true;
			}
			LOG(prompt);
			LOG(txt);
			LOG(e.what());
			fatal_error = true;
			SetError(e.what());
			if(auto_ret_fail)
				ReturnFail();
			return false;
		}
		catch(std::exception e) {
			if(keep_going) {
				output = " ";
				return true;
			}
			LOG(prompt);
			LOG(txt);
			SetError(e.what());
			fatal_error = true;
			if(auto_ret_fail)
				ReturnFail();
			return false;
		}
		/*catch (...) {
		    SetError("unknown error");
		    return false;
		}*/

		// LOG(IntStr64(input.AsString().GetHashValue()));

		// Fix unicode formatting
		output = ToUnicode(output, CHARSET_UTF8).ToString();
	}
#else
	return false;
#endif
	changed = true;
	Store();
	return output.GetCount() > 0;
}

void AiTask::Retry(bool skip_prompt, bool skip_cache)
{
	if(!skip_prompt) {
		input_json.Clear();
		input_basic.Clear();
		output.Clear();
	}
	skip_load = skip_cache;
	failed = false;
	fatal_error = false;
	ready = false;
	error.Clear();
	changed = true;
	tries = 0;
}

void AiTask::ReturnFail()
{
	output = "fail";
	tmp_str = "";

	ret_fail = true;
	failed = false;
	fatal_error = false;
	ready = false;
	error.Clear();
	changed = true;
	tries = 1;
}

TaskMgr& AiTask::GetTaskMgr() { return AiTaskManager(); }

void AiTask::Process_CreateImage() { WhenResultImages(recv_images); }

void AiTask::Process_EditImage() { WhenResultImages(recv_images); }

void AiTask::Process_VariateImage() { WhenResultImages(recv_images); }

void AiTask::Process_Default() { WhenResult(tmp_str + output); }

END_UPP_NAMESPACE

