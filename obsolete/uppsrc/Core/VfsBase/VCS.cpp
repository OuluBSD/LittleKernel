#include "VfsBase.h"


NAMESPACE_UPP


VersionControlSystem::VersionControlSystem() {
	
	
}

VersionControlSystem::~VersionControlSystem() {
	Close();
}

void VersionControlSystem::Initialize(String path, bool storing) {
	ASSERT(!FileExists(path) || DirectoryExists(path));
	scopes.Clear();
	this->storing = storing;
	
	if (path.Right(1) == DIR_SEPS)
		path = path.Left(path.GetCount()-1);
	dir = GetFileDirectory(path);
	
	BeginObject(GetFileName(path));
	
	RealizeDirectory(path);
}

void VersionControlSystem::Close() {
	ASSERT(scopes.GetCount() <= 1);
	while (scopes.GetCount())
		End();
}

bool VersionControlSystem::IsStoring() const {
	return storing;
}

bool VersionControlSystem::IsLoading() const {
	return !storing;
}

VersionControlSystem::Scope& VersionControlSystem::Push(ScopeType t, String key) {
	Scope& s = scopes.Add();
	s.type = t;
	s.key = key;
	RealizeDirectory(GetCurrentDirectory());
	return s;
}

void VersionControlSystem::Pop() {
	scopes.SetCount(scopes.GetCount()-1);
}

void VersionControlSystem::RemoveIdxFolders(int begin) {
	for (int i = begin;; i++) {
		String last_path = GetCurrentPath(IntStr(i));
		if (!DirectoryExists(last_path))
			break;
		DeleteFolderDeep(last_path);
	}
}

void VersionControlSystem::RealizeScopeJson(const String& key) {
	if (!storing) {
		Scope& s = scopes.Top();
		s.value = ParseJSON(LoadFile(GetCurrentPath(key + ".json")));
		s.json = new JsonIO(s.value);
	}
}

void VersionControlSystem::BeginObject(String key) {
	Push(ST_OBJECT, key);
	RealizeScopeJson(key);
}
void VersionControlSystem::BeginAt(int i) {
	String key = IntStr(i);
	Push(ST_OBJECT, key);
	RealizeScopeJson(key);
}

void VersionControlSystem::BeginKV(int i) {
	String key = IntStr(i);
	Push(ST_AT_KV, key);
	RealizeScopeJson(key);
}

void VersionControlSystem::BeginKeyVisit() {
	Push(ST_OBJECT, "key");
	RealizeScopeJson("key");
}

void VersionControlSystem::BeginValueVisit() {
	Push(ST_OBJECT, "value");
	RealizeScopeJson("value");
}

void VersionControlSystem::End() {
	if (storing) {
		Scope& s = scopes.Top();
		if (s.json) {
			Value jv = s.json->GetResult();
			String json = AsJSON(jv, pretty_json);
			FileOut fout(GetCurrentPath(s.key + ".json"));
			fout << json;
		}
	}
	
	Pop();
}

String VersionControlSystem::GetCurrentDirectory() const {
	String path = this->dir;
	for (const auto& scope : scopes) {
		path = AppendFileName(path, scope.key);
	}
	return path;
}

String VersionControlSystem::GetCurrentPath(String name) const {
	return AppendFileName(GetCurrentDirectory(), name);
}

int VersionControlSystem::ResolveCount() const {
	FindFile ff;
	int max_count = 0;
	if (ff.Search(AppendFileName(GetCurrentDirectory(), "*"))) do {
		if (!ff.IsDirectory()) continue;
		String name = ff.GetName();
		if (IsAllDigit(name))
			max_count = max(max_count, ScanInt(name)+1);
	} while (ff.Next());
	return max_count;
}


END_UPP_NAMESPACE
