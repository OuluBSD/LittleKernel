#ifndef _clang_clang_h
#define _clang_clang_h


#include <ide/Common/Common.h>

#include <Core/Core.h>

#define UBUNTU2204_WORKAROUND // there appears to be a bug in Ubuntu 22.04.1LTS libclang 14

// Configuration

extern bool LibClangEnabled;
extern bool AssistDiagnostics;
extern bool AutoIndexer;
extern bool RelaxedIndexerDependencies; // basically: Do not rescan all .cpps on .h change
extern int  IndexerThreads;
extern int  ParsedFiles;
extern int  LibClangCppVersion;

void ClangConfigSerialize(Stream& s);
void ClangConfigSetDefaults();

void PutAssist(const char *s);

#ifndef flagLCLANG
	#ifdef PLATFORM_POSIX
	#define DYNAMIC_LIBCLANG // dynamic loading of clang experiment (does not seem to work in Win32)
	#endif
#endif

#ifdef DYNAMIC_LIBCLANG

#include "libclang.h"

bool LoadLibClang(const String& dir);
bool LoadLibClangAutomatically();

inline bool HasLibClang()
{
	extern bool hasLibClang;
	return hasLibClang && LibClangEnabled;
}

#else

#include <clang-c/Index.h>

inline bool HasLibClang()
{
	return LibClangEnabled;
}

#endif

using namespace Upp;

class CoEvent {
	Mutex             lock;
	ConditionVariable cv;

public:
	void Wait(int timeout_ms)   { lock.Enter(); cv.Wait(lock, timeout_ms); lock.Leave(); }
	void Wait()                 { lock.Enter(); cv.Wait(lock); lock.Leave(); }
	void Broadcast()            { cv.Broadcast(); }
};

String FetchString(CXString cs);
String GetCursorKindName(CXCursorKind cursorKind);
String GetCursorSpelling(CXCursor cursor);
String GetTypeSpelling(CXCursor cursor);
String GetTypeDeclarationSpelling(CXCursor cursor);

struct SourceLocation : Moveable<SourceLocation> {
	String path;
	Point  pos, begin, end;

	bool operator==(const SourceLocation& b) const { return path == b.path && pos == b.pos; }
	bool operator!=(const SourceLocation& b) const { return !operator==(b); }
	void Serialize(Stream& s)                      { s % path % pos % begin % end; }
	hash_t GetHashValue() const                    { return CombineHash(path, pos); }
	String ToString() const                        { return path + ": " + AsString(pos) + " (" + AsString(begin) + "-" + AsString(end) + ")"; }
};

String RedefineMacros();
String GetClangInternalIncludes();

enum AdditionalKinds {
	KIND_INCLUDEFILE = -1000,
	KIND_INCLUDEFILE_ANY,
	KIND_INCLUDEFOLDER,
	KIND_COMPLETE,
	KIND_ERROR,
};

Image  CxxIcon(int kind); // TODO: Move here
int    PaintCpp(Draw& w, const Rect& r, int kind, const String& name, const String& pretty, Color ink, bool focuscursor, bool retval_last = false);
String SignatureQtf(const String& name, const String& pretty, int pari = INT_MAX);
String CppText(const String& name, const String& pretty);

bool IsStruct(int kind);
bool IsTemplate(int kind);
bool IsFunction(int kind);
bool IsVariable(int kind);
bool IsDecl(int kind);
bool IsTypeDecl(int kind);
bool IsTypeRef(int kind);
bool IsErrorKind(int kind);
int  FindId(const String& s, const String& id);

struct AutoCompleteItem : Moveable<AutoCompleteItem> {
	String parent;
	String name;
	String pretty;
	int    kind;
	int    priority;
};

struct AnnotationItem : Moveable<AnnotationItem> {
	String id; // Upp::Class::Method(Upp::Point p)
	String name; // Method
	String type; // for String x, Upp::String, surely valid for variables only
	String pretty; // void Class::Method(Point p), cleaned up for Navigator (instead of class Foo just Foo)
	String pretty0; // original
	String nspace; // Upp
	String uname; // METHOD
	String nest; // Upp::Class
	String unest; // UPP::CLASS
	String bases; // base classes of struct/class
	String parent_type; // Upp::Class
	Point  pos = Null;
	Point  begin = Null;
	Point  end = Null;
	int    kind = Null;
	bool   definition = false;
	bool   isvirtual = false;
	bool   isstatic = false;

	void Serialize(Stream& s);
	void Jsonize(JsonIO& json);
	void operator=(const AnnotationItem& b);
	bool operator==(const AnnotationItem& b) const;
	bool IsSameContent(const AnnotationItem& b) const;
	bool IsLineAreaPartialMatch(const AnnotationItem& b) const;
	String ToString() const;
	bool operator()(const AnnotationItem& a, const AnnotationItem& b) const {return a.pos.y != b.pos.y ? a.pos.y < b.pos.y : a.pos.x < b.pos.x;}
};

String GetClass(const AnnotationItem& m);
String GetNameFromId(const String& id);
String MakeDefinition(const AnnotationItem& m, const String& klass);
String MakeDefinition(const AnnotationItem& m);

struct ReferenceItem : Moveable<ReferenceItem> {
	int		kind = Null;
	String	id;
	Point	pos;
	Point	ref_pos;

	bool operator==(const ReferenceItem& b) const { return id == b.id && pos == b.pos; }
	hash_t GetHashValue() const                   { return CombineHash(id, pos); }
	String ToString() const;
	String MakeLocalString(const String& filepath) const;
	String MakeTargetString(const String& filepath) const;
	void Serialize(Stream& s);
	void Jsonize(JsonIO& json);
};

struct StatementItem : Moveable<StatementItem> {
	int		kind = Null;
	Point	begin;
	Point	end;

	bool operator==(const StatementItem& b) const { return begin == b.begin && end == b.end; }
	hash_t GetHashValue() const                   { return CombineHash(kind, begin, end); }
	void Serialize(Stream& s);
	void Jsonize(JsonIO& json);
};

struct CurrentFileContext {
	String                   filename;
	String                   real_filename; // in case we need to present .h as .cpp
	int                      line_delta = 0; // in case we need to present .h as .cpp
	String                   includes;
	String                   defines;
	String                   content;
	bool                     is_source_file = true;
};

struct CppFileInfo : Moveable<CppFileInfo> {
	Vector<AnnotationItem> items;
	Vector<AnnotationItem> locals;
	Vector<ReferenceItem>  refs;
	Vector<StatementItem>  stmts;
	String ToString() const;
};

enum { PARSE_FILE = 0x80000000 };

struct Clang {
	CXIndex           index = nullptr;
	CXTranslationUnit tu = nullptr;
	String            iquote;

	void Dispose();
	bool Parse(const String& filename, const String& content,
	           const String& includes, const String& defines,
	           dword options,
	           const String& filename2 = Null, const String& content2 = Null);
	bool ReParse(const String& filename, const String& content,
	             const String& filename2 = Null, const String& content2 = Null);

	Clang();
	~Clang();
};

void   Diagnostics(CXTranslationUnit tu, Event<const String&, Point, const String&, bool, int> out);
void   Diagnostics(CXTranslationUnit tu, Stream& out);

inline bool IsWarning(int q) { return q == CXDiagnostic_Warning; }
inline bool IsError(int q) { return findarg(q, CXDiagnostic_Error, CXDiagnostic_Fatal) >= 0; }

bool   IsCppSourceFile(const String& path);
bool   IsSourceFile(const String& path);
bool   IsHeaderFile(const String& path);

struct ClangNode {
	Array<ClangNode> sub;
	int kind = -1;
	String id, type;
	Point begin = Null;
	Point end = Null;
	hash_t filepos_hash = 0;
	hash_t type_hash = 0;
	bool is_ref = false;
	bool is_definition = false;
	bool is_type_builtin = false;

	void Clear() {sub.Clear(); kind = -1; id.Clear(); type.Clear(); begin = Null; end = Null; filepos_hash = 0; is_ref = false; is_definition = false; type_hash = 0; is_type_builtin = false;}
	String GetTreeString(int depth=0) const;
	hash_t GetCommonHash() const;
	bool TranslateTypeHash(const VectorMap<hash_t,hash_t>& translation);
};

class ClangVisitor {
	bool initialized = false;
	CXPrintingPolicy pp_id, pp_pretty;

	bool           ProcessNode(CXCursor c);

	friend CXChildVisitResult clang_visitor(CXCursor cursor, CXCursor p, CXClientData clientData);

	VectorMap<CXFile, String>                 cxfile; // accelerate CXFile (CXFile has to be valid across tree as there is no Dispose)
	VectorMap<CXFile, bool>                   do_file; // accelerate WhenFile
	VectorMap<String, Index<ReferenceItem>>   ref_done; // avoid self-references, multiple references

	struct MCXCursor : Moveable<MCXCursor> {
		CXCursor cursor;
	};

	ArrayMap<SourceLocation, MCXCursor>  tfn; // to convert e.g. Index<String>::Find(String) to Index::Find(T)

	struct CXLocation {
		Point  pos;
		CXFile file;
	};

	struct CXRange {
		CXLocation p0, p1;
	};

	CXLocation      GetLocation(CXSourceLocation cxlocation);
	CXRange         GetRange(CXSourceRange cxrange);
	SourceLocation  GetSourceLocation(const CXLocation& p, const CXRange& r);

	bool locals = false;

public:
	VectorMap<String, CppFileInfo> info;
	ClangNode ast;
	Vector<ClangNode*> scope;

	Gate<const String&> WhenFile;

	void Do(CXTranslationUnit tu);
	~ClangVisitor();
};

struct Diagnostic : Moveable<Diagnostic> {
	int    kind;
	bool   detail;
	String path;
	Point  pos;
	String text;
};

void SetCurrentFile(const CurrentFileContext& ctx, Event<const CppFileInfo&, const Vector<Diagnostic>&> done);
bool IsCurrentFileParsing();
void CancelCurrentFile();
bool IsCurrentFileDirty();

void StartAutoComplete(const CurrentFileContext& ctx, int line, int column, bool macros,
                       Event<const Vector<AutoCompleteItem>&> done);
bool IsAutocompleteParsing();
void CancelAutoComplete();

String FindMasterSource(PPInfo& ppi, const Workspace& wspc, const String& header_file);

struct MasterSourceCacheRecord : Moveable<MasterSourceCacheRecord> {
	String master;
	VectorMap<String, Time> chain;

	bool CheckTimes(PPInfo& ppi) const;
	void Serialize(Stream& s);
};

const VectorMap<String, Time>& FindMasterSourceCached(PPInfo& ppi, const Workspace& wspc, const String& header_file,
                                                      VectorMap<String, MasterSourceCacheRecord>& cache);

struct FileAnnotation0 {
	String defines = "<not_loaded>";
	String includes;
	String master_file;
	Time   time = Time::Low();
};

struct FileAnnotation : FileAnnotation0, CppFileInfo {
	String path;
	void Serialize(Stream& s);
};

ArrayMap<String, FileAnnotation>& CodeIndex();

struct IndexerJob : Moveable<IndexerJob> {
	String                                  path;
	String                                  blitz;
	String                                  includes;
	String                                  defines;
	WithDeepCopy<VectorMap<String, Time>>   file_times;
	WithDeepCopy<VectorMap<String, String>> master_files;
	int                                     ext = -1;
};

struct IndexerExtension {
	virtual ~IndexerExtension() {}
	virtual void RunJob(IndexerJob& job) = 0;
	virtual bool RunCurrentFile() = 0;
	virtual bool IsDirty(const String& s) = 0;
};

class Indexer {
	static CoEvent            event, scheduler;
	static Mutex              mutex;
	static Vector<IndexerJob> jobs;
	static std::atomic<int>   jobi;
	static std::atomic<int>   jobs_done;
	static std::atomic<int>   jobs_count; // way to get jobs.GetCount without mutex
	static std::atomic<int>   running_indexers;
	static bool               running_scheduler;
	static String             main, includes, defines;
	static bool               relaxed;

	static void IndexerThread();
	static void SchedulerThread();
	static void BuildingPause();

public:
	static void   Start(const String& main, const String& includes, const String& defines);
	static bool   IsRunning();
	static int    GetJobsCount()       { return jobs_count; }
	static bool   IsSchedulerRunning() { return running_scheduler; }
	static double Progress();

public:
	typedef IndexerExtension* (*NewExt)();
	typedef bool (*AcceptExt)(String);
	template <class T> static IndexerExtension* New() {return new T;}
	template <class T> static bool Accept(String ext) {return T::AcceptExt(ext);}
	struct Extension {
		String name;
		NewExt new_fn = 0;
		AcceptExt accept_fn = 0;
		One<IndexerExtension> obj;
		IndexerExtension& Get() {if (obj.IsEmpty()) obj = new_fn(); return *obj;}
	};
	static Array<Extension>& Extensions() {return Single<Array<Extension>>();}
	template <class T> static void RegisterExtension(String name) {
		auto& e = Extensions().Add();
		e.name = name;
		e.new_fn = &New<T>;
		e.accept_fn = &Accept<T>;
	}
	static int FindExtensionByExt(String ext) {
		for(int i = 0; i < Extensions().GetCount(); i++)
			if (Extensions()[i].accept_fn(ext))
				return i;
		return -1;
	}
	
	static const Vector<IndexerJob>& Jobs() {return jobs;}
	
};

#define INITIALIZER_INDEXER_EXTENSION(x) INITIALIZER(x) {Indexer::RegisterExtension<x>(#x);}

void DumpIndex(const char *file, const String& what = Null);

void CurrentFileDeleteCache();

bool IsPreprocessingCursor(CXCursor);

#endif