#include "Prompting.h"

struct Ide;

NAMESPACE_UPP

struct TaskMgrConfig {
	::Ide* ide = 0;
	bool running = false, stopped = true;
	int max_tries = 3;
#if HAVE_OPENAI
	openai::OpenAI* instance = 0;
#endif

	typedef TaskMgrConfig CLASSNAME;
	void Realize()
	{
		if (!running) Start();
	}
	void Start()
	{
		running = true;
		stopped = false;
		Thread::Start(THISBACK(Process));
	}
	void Stop()
	{
		running = false;
		while(!stopped)
			Sleep(100);
	}
	void Process();

	void SetupInstance(::Ide* ide);

	static TaskMgrConfig& Single()
	{
		static TaskMgrConfig m;
		return m;
	}
};

void TaskMgrConfig::Process()
{
	while(running && !Thread::IsShutdownThreads()) {
		AiTaskManager().Process();

		Sleep(10);
	}

	stopped = true;
}

void TaskMgr::Setup(::Ide* ide) { TaskMgrConfig::Single().SetupInstance(ide); }

template <class T>
String TaskMgr::MakeName(T& o, const char* name)
{
	String s;
	s << name;
	s << " #" << (int)o.fn;
	return s;
}

AiTask& TaskMgr::AddTask()
{
	task_lock.Enter();
	AiTask& t = tasks.Add();
	task_lock.Leave();
	return t;
}

void TaskMgr::Process()
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();

	iters++;

	int ready = 0, got_ready = 0;

	this->total = tasks.GetCount();

	for(int i = 0; i < tasks.GetCount() && mgr.running && !Thread::IsShutdownThreads(); i++) {
		AiTask& t = tasks[i];

		if(!t.ready) {
			ProcessSingle(i);

			if(t.ready) {
				actual++;
				ready++;
				got_ready++;
				task_i++;
			}
		}
		else
			ready++;
	}
	this->actual = ready;

	if(!got_ready) {
		int tried_retry = 0;
		for(AiTask& t : tasks) {
			if(t.fatal_error)
				continue;
			if(t.failed && !t.ready && t.tries < mgr.max_tries) {
				t.tries++;
				t.Retry(false, false);
				tried_retry++;
			}
		}
	}
}

void TaskMgr::ProcessSingle(int task_i)
{
	task_lock.Enter();
	AiTask& t = tasks[task_i];
	task_lock.Leave();

	Index<AiTask*> seen;

	// Skip ready, failed and those with non-ready dependencies
	if(t.ready || t.failed)
		;
	else {
		status = "Processing #" + IntStr(task_i);
		t.Process();
		status = "";
	}

	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::Translate(String orig_lang, String orig_txt, String trans_lang,
                        Event<String> WhenResult, bool slightly_dialect)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	AiTask& t = AddTask();

	t.SetRule("translate")
		.Input(&AiTask::CreateInput_Translate)
		.Process(&AiTask::Process_Default);

	t.args << orig_lang << orig_txt << trans_lang << IntStr(slightly_dialect);
	t.WhenResult << WhenResult;
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetModels(const ModelArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	AiTask& t = AddTask();
	t.ModelTask();
	t.model.Create();
	t.model->Put(args.Get());
	t.SetRule("models").Process(&AiTask::Process_Default);
	t.args << args.Get();
	t.WhenResult << WhenResult;
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::RawCompletion(String prompt, Event<String> WhenResult)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	AiTask& t = AddTask();

	t.SetRule("raw prompt completion").Process(&AiTask::Process_Default);
	
	t.completion.Create();
	t.completion->prompt = prompt;
	t.WhenResult << WhenResult;
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetCompletion(CompletionArgs& args, Event<String> WhenResult)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	AiTask& t = AddTask();
	t.completion.Create();
	t.completion->Put(args.Get());
	t.SetRule("completion").Process(&AiTask::Process_Default);
	t.args << args.Get();
	t.WhenResult << WhenResult;
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetChat(ChatArgs& args, Event<String> WhenResult)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	AiTask& t = AddTask();
	t.type = AiTask::TYPE_CHAT;
	t.chat.Create();
	t.chat->Put(args.Get());
	t.SetRule("chat").Process(&AiTask::Process_Default);
	t.args << args.Get();
	t.WhenResult << WhenResult;
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::CreateImage(String prompt, int count, Event<Array<Image>&> WhenResult,
                          int reduce_size_mode, Event<> WhenError)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	AiTask& t = AddTask();

	t.SetRule("create image")
		.ImageTask()
		.Input(&AiTask::CreateInput_CreateImage)
		.Process(&AiTask::Process_CreateImage);

	t.args << prompt << IntStr(count) << IntStr(reduce_size_mode);
	t.WhenResultImages << WhenResult;
	t.WhenError << WhenError;
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetEditImage(Image orig, Image mask, String prompt, int count,
                           Event<Array<Image>&> WhenResult, Event<> WhenError)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	if(orig.GetSize() != mask.GetSize()) {
		WhenError();
		return;
	}

	{
		Size sz = orig.GetSize();
		ImageBuffer ib(sz);
		RGBA* dst = ib.Begin();
		const RGBA* orig_it = orig.Begin();
		const RGBA* mask_it = mask.Begin();
		const RGBA* mask_end = mask.End();
		RGBA dark;
		dark.r = 0;
		dark.g = 0;
		dark.b = 0;
		dark.a = 0;
		while(mask_it != mask_end) {
			if(mask_it->a == 0)
				*dst = *orig_it;
			else
				*dst = dark;
			mask_it++;
			orig_it++;
			dst++;
		}
		orig = ib;
	}

	AiTask& t = AddTask();

	t.SetRule("edit image")
		.ImageTask()
		.ImageEditTask()
		.Input(&AiTask::CreateInput_EditImage)
		.Process(&AiTask::Process_EditImage);

	t.send_images << orig;
	t.args << prompt << IntStr(count);
	t.WhenResultImages << WhenResult;
	t.WhenError << WhenError;
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::VariateImage(Image orig, int count, Event<Array<Image>&> WhenResult,
                           Event<> WhenError)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	AiTask& t = AddTask();

	t.SetRule("variate image")
		.ImageTask()
		.ImageVariateTask()
		.Input(&AiTask::CreateInput_VariateImage)
		.Process(&AiTask::Process_VariateImage);

	t.send_images << orig;
	t.args << IntStr(count);
	t.WhenResultImages << WhenResult;
	t.WhenError << WhenError;
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetGenericPrompt(const GenericPromptArgs& args, Event<String> WhenResult, String title)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	if (title.IsEmpty()) title = "generic prompt";
	String s = args.Get();

	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, title))
		.Input(&AiTask::CreateInput_GenericPrompt)
		.Process(&AiTask::Process_Default);

	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetVision(const String& jpeg, const VisionArgs& args, Event<String> WhenResult)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	String s = args.Get();

	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule("vision")
		.Input(&AiTask::CreateInput_Vision)
		.Process(&AiTask::Process_Default);

	t.args << s;
	t.vision.Create();
	t.vision->jpeg = jpeg;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetTranscription(const TranscriptionArgs& args, Event<String> WhenResult)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	String s = args.Get();

	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule("transcription")
		.Input(&AiTask::CreateInput_Transcription)
		.Process(&AiTask::Process_Default);

	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetBasic(const TaskArgs& args, Event<String> WhenResult, String title, bool keep_going) {
	Get(false, args, WhenResult, title, keep_going);
}

void TaskMgr::GetJson(const TaskArgs& args, Event<String> WhenResult, String title, bool keep_going) {
	Get(true, args, WhenResult, title, keep_going);
}

void TaskMgr::Get(bool json, const TaskArgs& args, Event<String> WhenResult, String title, bool keep_going)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	if (title.IsEmpty())
		title = "unnamed";
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	if (json)
		t.SetRule(MakeName(args, title))
			.Input(&AiTask::CreateInput_DefaultJson)
			.Process(&AiTask::Process_Default);
	else
		t.SetRule(MakeName(args, title))
			.Input(&AiTask::CreateInput_DefaultBasic)
			.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	t.keep_going = keep_going;
	
	t.type = AiTask::TYPE_CHAT;
	t.chat.Create();
	
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetCode(const CodeArgs& args, Event<String> WhenResult)
{
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	String s = args.Get();

	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, "generic prompt"))
		.Input(&AiTask::CreateInput_Code)
		.Process(&AiTask::Process_Default);

	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetTokenData(const TokenArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	
	t.SetRule(MakeName(args, "get token data"))
		.Input(&AiTask::CreateInput_GetTokenData)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetSourceDataAnalysis(const SourceDataAnalysisArgs& args, Event<String> WhenResult, bool keep_going) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	
	t.SetRule(MakeName(args, "get song data analysis"))
		.Input(&AiTask::CreateInput_GetSourceDataAnalysis)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	t.keep_going = keep_going;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetPhraseData(const PhraseArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	
	t.SetRule(MakeName(args, "get phrase data"))
		.Input(&AiTask::CreateInput_GetPhraseData)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetActionAnalysis(const ActionAnalysisArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;

	String s = args.Get();

	task_lock.Enter();
	AiTask& t = tasks.Add();
	
	t.SetRule(MakeName(args, "get action analysis"))
		.Input(&AiTask::CreateInput_GetActionAnalysis)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetAttributes(const AttrArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	
	t.SetRule(MakeName(args, "get attributes"))
		.Input(&AiTask::CreateInput_GetAttributes)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetScriptSolver(const ScriptSolverArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, "scripts solver"))
		.Input(&AiTask::CreateInput_ScriptSolver)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	
	if (args.ret_fail)
		t.SetAutoReturnFail();
	
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetSocial(const SocialArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, "social"))
		.Input(&AiTask::CreateInput_Social)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetBiographySummary(const BiographySummaryProcessArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, "biography summary process"))
		.Input(&AiTask::CreateInput_BiographySummaryProcess)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetLeadSolver(const LeadSolverArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, "lead solver"))
		.Input(&AiTask::CreateInput_LeadSolver)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetPerspectiveProcess(const BeliefArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, "belief solver"))
		.Input(&AiTask::CreateInput_SocialBeliefsProcess)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetMarketplace(const MarketplaceArgs& args, Event<String> WhenResult) {
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	String s = args.Get();
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.SetRule(MakeName(args, "marketplace"))
		.Input(&AiTask::CreateInput_Marketplace)
		.Process(&AiTask::Process_Default);
	
	t.args << s;
	t.WhenResult << WhenResult;
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

void TaskMgr::GetFarStage(Ptr<FarStage> stage, int fn_i, Value args, Event<String> WhenResult, Event<> WhenDone) {
	if (!stage)
		return;
	const TaskMgrConfig& mgr = TaskMgrConfig::Single();
	TaskMgr& p = *this;
	
	task_lock.Enter();
	AiTask& t = tasks.Add();
	t.type = AiTask::TYPE_CHAT;
	t.chat.Create();
	t.SetRule("far stage")
		.Input(&AiTask::CreateInput_FarStage)
		.Process(&AiTask::Process_Default);
	
	t.stage = stage;
	t.fn_i = fn_i;
	t.vargs = args;
	t.WhenResult << WhenResult;
	t.WhenDone << WhenDone;
	
	if (stage->model_name.GetCount())
		t.chat->model_name = stage->model_name;
	
	task_lock.Leave();
	
	TaskMgrConfig().Single().Realize();
}

TaskRule& TaskRule::SetRule(const String& name)
{
	this->name = name;
	return *this;
}

TaskRule& TaskRule::Input(void (AiTask::*fn)(BasicPrompt&))
{
	this->input_basic = fn;
	return *this;
}

TaskRule& TaskRule::Input(void (AiTask::*fn)(JsonPrompt&))
{
	this->input_json = fn;
	return *this;
}

TaskRule& TaskRule::Process(void (AiTask::*fn)())
{
	process = fn;
	return *this;
}

TaskRule& TaskRule::Spawnable(bool b)
{
	spawnable = b;
	return *this;
}

TaskRule& TaskRule::MultiSpawnable(bool b)
{
	spawnable = b;
	multi_spawnable = b;
	return *this;
}

TaskRule& TaskRule::CrossMode(bool b)
{
	allow_cross_mode = b;
	return *this;
}

TaskRule& TaskRule::SeparateItems(bool b)
{
	separate_items = b;
	return *this;
}

TaskRule& TaskRule::DebugInput(bool b)
{
	debug_input = b;
	return *this;
}

TaskRule& TaskRule::ModelTask()
{
	type = TYPE_MODEL;
	return *this;
}

TaskRule& TaskRule::ImageTask()
{
	type = TYPE_IMAGE_GENERATION;
	return *this;
}

TaskRule& TaskRule::ImageEditTask()
{
	type = TYPE_IMAGE_EDIT;
	return *this;
}

TaskRule& TaskRule::ImageVariateTask()
{
	type = TYPE_IMAGE_VARIATE;
	return *this;
}

void TaskMgrConfig::SetupInstance(::Ide* ide)
{
	this->ide = ide;
	
#if HAVE_OPENAI
	if(!instance) {
		auto& mgr = AiManager();
		for(int i = 0; i < mgr.GetCount(); i++) {
			auto& prov = mgr[i];
			if (prov.type == AiServiceProvider::OPENAI) {
				if (!prov.token.IsEmpty()) {
					instance = &openai::start(prov.token.Begin());
					if (instance) {
						String proxy = prov.proxy;
						if (proxy.IsEmpty())
							proxy = GlobalProxy();
						instance->setProxy(proxy.Begin());
					}
					break;
				}
			}
		}
	}
#endif
}

TaskMgr& AiTaskManager()
{
	static TaskMgr tm;
	return tm;
}

END_UPP_NAMESPACE

