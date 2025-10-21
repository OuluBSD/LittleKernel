#ifndef _ide_Vfs_EcsLang_h_
#define _ide_Vfs_EcsLang_h_


class EcsIndexer : public IndexerExtension {
	int pkg_i = -1;
	int file_i = -1;
	VectorMap<String,FileTime> last_checks;
public:
	typedef EcsIndexer CLASSNAME;
	EcsIndexer();
	
	void RunJob(IndexerJob& job) override;
	bool RunCurrentFile() override;
	bool IsDirty(const String& s) override;
	//bool LoadEcsSpace(String path);
	static bool AcceptExt(String ext);
	static void RunPath(String path);
};

INITIALIZE(EcsIndexer)


#endif
