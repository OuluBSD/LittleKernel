#ifndef _Meta_CodeGenerator_h_
#define _Meta_CodeGenerator_h_


class MetaCodeGenerator {
	
public:
	struct File {
		String code;
		VectorMap<TextRange,Ptr<VfsValue>> range_nodes;
		VectorMap<TextRange,Ptr<VfsValue>> code_nodes;
		Vector<int> editor_to_line;
		Vector<VfsValue*> comment_to_node;
	};
	
private:
	ArrayMap<PkgFile, File> files;
	
	
	
	void FindFiles(const VfsValueSubset& n, VectorMap<hash_t, VectorMap<hash_t,int>>& pkgfiles);
	void FindValues(const VfsValueSubset& n, const PkgFile& key, Vector<VfsValue*>& nodes);
	
public:
	typedef MetaCodeGenerator CLASSNAME;
	MetaCodeGenerator();
	
	bool Process(const VfsValueSubset& n);
	String GetResult(int pkg, int file) const;
	const File* GetResultFile(int pkg, int file) const;
	
};


#endif
