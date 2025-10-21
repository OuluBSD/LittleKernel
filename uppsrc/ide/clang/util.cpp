#include "clang.h"

bool LibClangEnabled = true;
bool AssistDiagnostics;
bool AutoIndexer;
bool RelaxedIndexerDependencies = true;
int  IndexerThreads;
int  ParsedFiles;
int  LibClangCppVersion = 17;

void ClangConfigSerialize(Stream& s)
{
	int version = 1;
	s % version
	  % LibClangEnabled
	  % AssistDiagnostics
	  % AutoIndexer
	  % IndexerThreads
	  % ParsedFiles
	;
	
	if(version >= 1)
		s % LibClangCppVersion;
}

void ClangConfigSetDefaults()
{
	uint64 total, avail;
	GetSystemMemoryStatus(total, avail);
	int mem_mb = int(total >> 20);
	
	ParsedFiles = clamp((mem_mb - 4000) / 500, 1, 12);

	IndexerThreads = max(min(CPU_Cores() - 2, (mem_mb - 4000) / 1000), 1);

//	AutoIndexer = CPU_Cores() >= 8 && mem_mb > 8000;
	AutoIndexer = true;

	LibClangCppVersion = 17;
}

INITBLOCK {
	ClangConfigSetDefaults();
}

void PutAssist(const char *s)
{
	GuiLock __;
	if(AssistDiagnostics)
		PutConsole(s);
}

bool IsCppSourceFile(const String& path)
{
	String ext = ToLower(GetFileExt(path));
	return findarg(ext, ".cpp", ".cc", ".cxx", ".icpp") >= 0;
}

bool IsSourceFile(const String& path)
{
	String ext = ToLower(GetFileExt(path));
	return findarg(ext, ".cpp", ".cc", ".cxx", ".icpp", ".c") >= 0;
}

bool IsHeaderFile(const String& path)
{
	String ext = ToLower(GetFileExt(path));
	return findarg(ext, ".h", ".hxx", ".hpp", ".hh") >= 0;
}

bool IsStruct(int kind)
{
	return findarg(kind, CXCursor_StructDecl, CXCursor_UnionDecl, CXCursor_ClassDecl,
	                     CXCursor_ClassTemplate) >= 0;
}

bool IsTemplate(int kind)
{
	return findarg(kind, CXCursor_FunctionTemplate, CXCursor_ClassTemplate) >= 0;
}

bool IsFunction(int kind)
{
	return findarg(kind, CXCursor_FunctionTemplate, CXCursor_FunctionDecl, CXCursor_Constructor,
	                     CXCursor_Destructor, CXCursor_ConversionFunction, CXCursor_CXXMethod) >= 0;
}

bool IsVariable(int kind)
{
	return findarg(kind, CXCursor_VarDecl, CXCursor_FieldDecl, CXCursor_ParmDecl) >= 0;
}

bool IsDecl(int kind) {
	return kind >= (int)CXCursor_FirstDecl && kind <= (int)CXCursor_LastDecl;
}

bool IsTypeDecl(int kind)
{
	return findarg(kind,
		CXCursor_UnexposedDecl,
		CXCursor_StructDecl, CXCursor_UnionDecl, CXCursor_ClassDecl, CXCursor_EnumDecl,
		CXCursor_TypedefDecl, CXCursor_Namespace, CXCursor_TemplateTypeParameter,
		CXCursor_ClassTemplate, CXCursor_ClassTemplatePartialSpecialization,
		CXCursor_NamespaceAlias, CXCursor_UsingDeclaration,
		CXCursor_TypeAliasDecl,
		CXCursor_TypeAliasTemplateDecl,
		CXCursor_OverloadedDeclRef,
		CXCursor_TemplateTemplateParameter
		) >= 0;
}

bool IsErrorKind(int kind)
{
	return findarg(kind,
	  CXCursor_FirstInvalid,
	  CXCursor_InvalidFile,
	  CXCursor_NoDeclFound,
	  CXCursor_NotImplemented,
	  CXCursor_InvalidCode
		) >= 0;
}

bool IsTypeRef(int kind)
{
	return kind >= (int)CXCursor_FirstRef && kind <= (int)CXCursor_LastRef;
}

int FindId(const String& s, const String& id) {
	if(id.GetCount() == 0)
		return -1;
	int q = 0;
	for(;;) {
		q = s.Find(id, q);
		if(q < 0)
			return -1;
		if((q == 0 || !iscid(s[q - 1])) && // character before id
		   (q + id.GetCount() >= s.GetCount() || !iscid(s[q + id.GetCount()]))) // and after..
			return q;
		q++;
	}
};

String AnnotationItem::ToString() const {
	if (type.GetCount()) return type + " " + id;
	else return id;
}

String ReferenceItem::ToString() const {
	return Format("%s (%d:%d -> %d:%d)", id, pos.x,pos.y, ref_pos.x,ref_pos.y);
}

String ReferenceItem::MakeLocalString(const String& filepath) const {
	String s;
	s << filepath << ":" << pos.y << ":" << pos.x << ":" << id;
	return s;
}


String ReferenceItem::MakeTargetString(const String& filepath) const {
	String s;
	s << filepath << ":" << ref_pos.y << ":" << ref_pos.x << ":" << id;
	return s;
}



String ClangNode::GetTreeString(int depth) const {
	String s;
	s.Cat('\t', depth);
	s << FetchString(clang_getCursorKindSpelling((CXCursorKind)kind));
	if (!id.IsEmpty()) s << ": " << id;
	s << "\n";
	for (auto& n : sub)
		s << n.GetTreeString(depth+1);
	return s;
}

hash_t ClangNode::GetCommonHash() const {
	CombineHash ch;
	ch.Do(kind).Do(id).Do(type);
	for (const auto& s : sub)
		ch.Put(s.GetCommonHash());
	return ch;
}

bool ClangNode::TranslateTypeHash(const VectorMap<hash_t,hash_t>& translation) {
	if (type_hash) {
		int i = translation.Find(type_hash);
		if (i < 0) {
			LOG("ClangNode::TranslateTypeHash: error: translation type not found: " + id + "(" + type + ")");
			return false;
		}
		type_hash = translation[i];
	}
	for (auto& s : sub)
		if (!s.TranslateTypeHash(translation))
			return false;
	return true;
}

bool IsPreprocessingCursor(CXCursor c)
{
	return c.kind >= 500 && c.kind < 600;
}
