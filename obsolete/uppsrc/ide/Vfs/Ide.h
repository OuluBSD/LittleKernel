#ifndef _ide_Vfs_Ide_h_
#define _ide_Vfs_Ide_h_

struct VfsSrcFile : Moveable<VfsSrcFile> {
	//int id = -1;
	String saved_hash;
	hash_t highest_seen_serial = 0;
	VectorMap<hash_t,String> seen_types;
	One<VfsValue> temp;
	int temp_id = -1;
	bool loaded = false;
	
	VfsSrcPkg* pkg = 0;
	String full_path, rel_path;
	mutable Mutex lock;
	bool managed_file = false; // if file only exists as VfsValue json
	bool keep_file_ids = false;
	bool env_file = false;
	
	VfsSrcFile() {}
	void Visit(Vis& v);
	bool IsDirTree() const {return GetFileExt(full_path) == ".d";}
	bool IsBinary() const {return GetFileExt(full_path) == ".bin";}
	bool IsECS() const {return GetFileExt(full_path) == ".ecs";}
	bool IsEnv() const {return GetFileExt(full_path) == ".env";}
	bool IsExt(String s) const {return GetFileExt(full_path) == s;}
	bool Store(bool forced=false);
	bool StoreSimpleReferences(const String& ref_file);
	String StoreJson();
	bool Load();
	bool IsLoaded() const;
	bool LoadJson(String json);
	void RefreshSeenTypes();
	VfsValue& GetTemp();
	VfsValue& CreateTemp(int dbg_src);
	void ClearTemp(int dbg_src);
	void FixAstValue();
	void OnSeenTypes();
	void OnSerialCounter();
	String GetTitle() const {return GetFileTitle(full_path);}
	void KeepFileIndices(bool b=true) {keep_file_ids = b;}
	void ManageFile(bool b=true) {managed_file = b;}
	void MakeTempFromEnv(bool all_files=false);
	String UpdateStoring();
	void UpdateLoading();
	hash_t GetPackageHash() const;
	hash_t GetFileHash() const;
	
	#if 0
	VfsSrcFile(const VfsSrcFile& f) {*this = f;}
	VfsSrcFile(VfsSrcFile&& f) /*: ai_items(pick(f.ai_items))*/ {}
	void operator=(const VfsSrcFile& s);
	void UpdateLinks(FileAnnotation& ann);
	#endif
};

struct VfsSrcPkg {
	typedef VfsSrcPkg CLASSNAME;
	
	Array<VfsSrcFile> src_files;
	Index<String> rel_files;
	
	String assembly_dir;
	String dir;
	mutable hash_t cached_hash = 0;
	
	VfsSrcPkg() {}
	VfsSrcPkg(VfsSrcPkg&& f) {*this = f;}
	VfsSrcPkg(const VfsSrcPkg& f) {*this = f;}
	void operator=(const VfsSrcPkg& f);
	void Init();
	void RefreshCachedPkgHash() const;
	bool Load();
	bool Store(bool forced);
	int GetAddRelPath(const String& rel_path);
	VfsSrcFile& GetAddFile(const String& full_path);
	VfsSrcFile& GetMetaFile();
	//void SetPath(String full_path, String upp_dir);
	String GetTitle() const;
	String GetRelativePath() const;
	String GetRelativePath(const String& path) const;
	String GetFullPath(const String& rel_path) const;
	String GetFullPath(hash_t file_hash) const;
	hash_t GetPackageHash() const;
	hash_t GetPackageHashCached() const;
	hash_t GetFileHash(const String& path) const;
	void Visit(Vis& v);
	int FindFile(String path) const;
	String GetDirectory() const {return dir;}
	int GetFileId(const String& path) const;
	//int GetAddFileId(const String& path);
private:
	bool post_saving = false;
	Mutex lock;
	mutable String title;
};

struct IdeMetaEnvironment {
	MetaEnvironment& env;
	
private:
	Array<VfsSrcPkg> pkgs;
	
public:
	IdeMetaEnvironment();
	VfsSrcPkg& GetAddPackage(String path);
	VfsSrcPkg& GetAddPackage(hash_t pkg);
	String ResolveVfsSrcPkgPath(const String& includes, String path, String& ret_upp_dir);
	VfsSrcFile& ResolveFile(const String& includes, const String& path);
	//VfsSrcFile& ResolveFileInfo(const String& includes, String path);
	VfsSrcFile& Load(const String& includes, const String& path);
	void OnLoadFile(VfsSrcFile& file);
	VfsValue* LoadDatabaseSourceVisit(VfsSrcFile& file, String path, Vis& v);
	VfsValue& RealizeFileNodeHash(hash_t pkg, hash_t file, hash_t type_hash);
	void SplitValueHash(VfsValue& root, VfsValueSubset& other, hash_t pkg);
	void SplitValueHash(VfsValue& root, VfsValueSubset& other, hash_t pkg, hash_t file);
	void SplitValueHash(const VfsValue& root, VfsValue& other, hash_t pkg);
	void SplitValueHash(const VfsValue& root, VfsValue& other, hash_t pkg, hash_t file);
	int FindPkg(String dir) const;
	Vector<VfsValue*> FindAllEnvs();
	VfsValue* FindNodeEnv(Entity& n);
	bool LoadFileRoot(const String& includes, const String& path, bool manage_file);
	bool LoadFileRootVisit(const String& includes, const String& path, Vis& v, bool manage_file, VfsValue*& file_node);
	String GetHashFilepath(hash_t pkg, hash_t file) const;
	VfsValue* FindDeclaration(const VfsValue& n);
	Vector<VfsValue*> FindDeclarationsDeep(const VfsValue& n);
	static VfsValue* FindDeclarationStatic(const VfsValue& n);
	static Vector<VfsValue*> FindDeclarationsDeepStatic(const VfsValue& n);
	Array<VfsSrcPkg>& GetPackages() {return pkgs;}
};

void Assign(VfsValue& mn, VfsValue* owner, const ClangNode& n);
void Store(IdeMetaEnvironment& env, String& includes, const String& path, ClangNode& n);
void UpdateWorkspace(IdeMetaEnvironment& env, Workspace& wspc);
IdeMetaEnvironment& IdeMetaEnv();
bool IsStructKind(const VfsValue& n);
bool IsMergeable(CXCursorKind kind);
VfsValue* FindNodeEnv(Entity& n);
VfsValue* IdeVfsFillDatasetPtrs(DatasetPtrs& p, hash_t type_hash);


// MCP-facing adapters over IdeMetaEnvironment.
// These forward to Env* functions once implemented, but provide IDE-local context if needed.

struct IdeEnvStatus {
    bool initialized = false;
    String builder;
    int64 last_update_ts = 0;
    int   stale_files = 0;
};

IdeEnvStatus IdeEnvStatusQuery();

#endif
