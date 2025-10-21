#include "Script.h"

#ifdef flagMAIN

NAMESPACE_UPP

bool SingleMachine::Open(void(*arg_fn)()) {return false;}
void SingleMachine::Close() {}
	

bool TestEonTest(String s, ProgLang tgt) {
	DUMP(s);
	
	Compiler c;
	String code;
	bool succ = c.CompileEonFile(s, tgt, code);
	
	c.t.Dump();
	if (!succ)
		return false;
	
	LOG(c.sp.root.GetTreeString(0));
	LOG(code);
	
	return true;
}

void TestEonTests(String dir_title, String prefix="", int single=-1) {
	String dir = ShareDirFile("eon" DIR_SEPS + dir_title);
	Index<String> files;
	
	FindFile ff;
	if (ff.Search(dir + DIR_SEPS + "*.eon")) do {
		files.Add(ff.GetPath());
	}
	while (ff.Next());
	
	SortIndex(files, StdLess<String>());
	DUMPC(files);
	
	int i = -1;
	for (String file : files) {
		i++;
		String fname = GetFileName(file);
		
		if (prefix.GetCount() && fname.Find(prefix) != 0) continue;
		
		LOG("Testing " << i << ": " << fname);
		if (single >= 0 && i != single) continue;
		
		if (!TestEonTest(file, LANG_CPP)) {
			LOG("Failed: " << fname);
			break;
		}
		
	}
}

END_UPP_NAMESPACE


CONSOLE_APP_MAIN {
	//TestEonTests("lang", "meta", 13);
	TestEonTests("lang", "meta");
	//TestEonTests("lang", "test");
	//TestEonTests("tests", "", 65);
	TestEonTests("tests");
}


#endif
