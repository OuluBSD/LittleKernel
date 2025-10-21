#include "Eon00.h"

using namespace Upp;

namespace {

struct EngineGuard {
	Engine* eng = nullptr;
	~EngineGuard() {
		if (eng)
			Engine::Uninstall(true, eng);
	}
};

struct TestCase {
	void (*runner)(Engine&, int);
	const char* label;
};

void ConfigureEngine(Engine& eng, void (*runner)(Engine&, int), int method) {
	eng.ClearCallbacks();
	eng.WhenInitialize << callback(MachineEcsInit);
	eng.WhenPreFirstUpdate << callback(DefaultStartup);
	eng.WhenBoot << callback(DefaultSerialInitializerInternalEon);
	eng.WhenUserInitialize << callback1(runner, method);
}

void RunScenario(void (*runner)(Engine&, int), int method, const char* label) {
	EngineGuard guard;
	guard.eng = &ShellMainEngine();
	Engine& eng = *guard.eng;
	
	ConfigureEngine(eng, runner, method);
	
	ValueMap args;
	args.Add("MACHINE_TIME_LIMIT", 3);
	
	if (!eng.StartLoad("Shell", String(), args))
		throw Exc(String().Cat() << label << ": engine failed to start");
	
	eng.MainLoop();
	
	Engine::Uninstall(true, guard.eng);
	guard.eng = nullptr;
}

const TestCase kTests[] = {
	{ Run00aAudioGen, "Run00aAudioGen" },
	{ Run00bAudioGen, "Run00bAudioGen" },
	{ Run00cAudioGen, "Run00cAudioGen" },
};

constexpr int kTestCount = int(sizeof(kTests) / sizeof(kTests[0]));

void PrintTestCatalog() {
	Cout() << "Tests:\n";
	Cout() << "  -1 runs all tests.\n";
	for(int i = 0; i < kTestCount; i++)
		Cout() << "  " << i << ": " << kTests[i].label << '\n';
}

void RunSingleTest(int index, int method) {
	if (index < 0 || index >= kTestCount)
		throw Exc(String().Cat() << "invalid test index " << index);
	RunScenario(kTests[index].runner, method, kTests[index].label);
}

void RunAllTests(int method) {
	for(int i = 0; i < kTestCount; i++)
		RunScenario(kTests[i].runner, method, kTests[i].label);
}

} // namespace

CONSOLE_APP_MAIN {
	CommandLineArguments cmd;
	cmd.AddPositional("test number", INT_V, -1);
	cmd.AddPositional("method number", INT_V, 0);
	cmd.AddArg('h', "Show usage information", false);
	if (!cmd.Parse() || cmd.IsArg('h')) {
		cmd.PrintHelp();
		PrintTestCatalog();
		return;
	}
	
	int test_number = (int)cmd.GetPositional(0);
	int method_number = (int)cmd.GetPositional(1);
	
	if (test_number < -1 || test_number >= kTestCount) {
		Cerr() << "Test number out of range: " << test_number << '\n';
		cmd.PrintHelp();
		PrintTestCatalog();
		return;
	}
	
	try {
		if (test_number == -1)
			RunAllTests(method_number);
		else
			RunSingleTest(test_number, method_number);
	}
	catch (Exc e) {
		Cout() << "error: " << e << '\n';
		throw;
	}
}
