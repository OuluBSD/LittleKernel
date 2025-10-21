#include "Vfs.h"

NAMESPACE_UPP


Compiler::Compiler(VfsValue& v) :
	VfsValueExt(v)
{
	t.WhenMessage << THISBACK(OnProcMsg);
	GetTokenStructure().WhenMessage << THISBACK(OnProcMsg);
	GetSemanticParser().WhenMessage << THISBACK(OnProcMsg);
	ex.WhenMessage << THISBACK(OnProcMsg);
	
}

bool Compiler::CompileEonFile(String filepath, ProgLang lang, String& output) {
	output.Clear();
	
	ProcMsg m;
	if (!FileExists(filepath)) {
		m.msg = "file does not exist: " + filepath;
		m.severity = PROCMSG_ERROR;
		OnProcMsg(m);
		return false;
	}
	
	String content = LoadFile(filepath);
	if (content.IsEmpty()) {
		m.msg = "got empty file from: " + filepath;
		m.severity = PROCMSG_ERROR;
		OnProcMsg(m);
		return false;
	}
	
	return CompileEon(content, filepath, lang, output);
}

#define TEST(x) if (!(x)) {return 0;}

AstNode* Compiler::CompileAst(String content, String path, bool verbose) {
	
	// Tokenize eon
	TEST(Tokenize(path, content, true))
	if (verbose) t.Dump();
	
	// Parse error handling structure (for multiple error messages per single try)
	TEST(ParseStructure())
	
	// Semantically parse
	TEST(Parse())
	if (verbose) {LOG(GetSemanticParser().GetRoot().GetTreeString(0));}
	
	// Run meta
	TEST(RunMeta())
	if (verbose) {LOG(GetAstRunner().GetRoot().GetTreeString(0));}
	
	return &GetAstRunner().GetRoot();
}

bool Compiler::CompileEon(String content, String path, ProgLang lang, String& output, bool verbose) {
	
	if (!CompileAst(content, path))
		return false;
	
	if (lang == LANG_HIGH) {
		// Export High script
		TEST(ExportHigh())
	}
	else if (lang == LANG_CPP) {
		TEST(ExportCpp());
	}
	else {
		TODO
	}
	
	output = ex.GetResult();
	if (verbose) {LOG(output);}
	
	return true;
}

#undef TEST

bool Compiler::Tokenize(String filepath, String content, bool pythonic) {
	this->filepath = filepath;
	this->content = content;
	
	t.SkipNewLines(!pythonic);
	t.SkipComments();
	if (!t.Process(content, filepath))
		return false;
	t.CombineTokens();
	if (pythonic)
		t.NewlineToEndStatement();
	
	return true;
}

bool Compiler::ParseStructure() {
	auto& ts = GetTokenStructure();
	ts.WhenMessage = THISBACK(OnProcMsg);
	if (!ts.ProcessEon(t)) {
		return false;
	}
	//LOG(val.GetTreeString());
	return true;
}

bool Compiler::Parse() {
	auto& ts = GetTokenStructure();
	auto& sp = GetSemanticParser();
	
	sp.WhenMessage = THISBACK(OnProcMsg);
	if (!sp.ProcessEon(ts)) {
		return false;
	}
	
	return true;
}

bool Compiler::RunMeta() {
	auto& ar = GetAstRunner();
	auto& sp = GetSemanticParser();
	ar.WhenMessage = THISBACK(OnProcMsg);
	if (!ar.Execute(sp.GetRoot()))
		return false;
	
	return true;
}

bool Compiler::ExportHigh() {
	auto& ar = GetAstRunner();
	InitHighExporter(ex.lang);
	
	ex.WhenMessage = THISBACK(OnProcMsg);
	if (!ex.Process(ar.GetRoot()))
		return false;
	
	return true;
}

bool Compiler::ExportCpp() {
	auto& ar = GetAstRunner();
	InitCppExporter(ex.lang);
	
	ex.WhenMessage = THISBACK(OnProcMsg);
	if (!ex.Process(ar.GetRoot()))
		return false;
	
	return true;
}

void Compiler::OnProcMsg(ProcMsg msg) {
	LOG(msg.ToString());
}


END_UPP_NAMESPACE
