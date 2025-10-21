#ifndef _ide_Vfs_ClangTypeResolver_h_
#define _ide_Vfs_ClangTypeResolver_h_


class ClangTypeResolver {
	String last_error;
	Vector<ClangNode*> scope;
	VectorMap<hash_t,Vector<ClangNode*>> type_nodes;
	VectorMap<hash_t,Index<String>> type_scopepaths;
	VectorMap<hash_t,hash_t> type_translation;
	
	bool Visit(ClangNode& cn);
	bool VisitSub(ClangNode& cn);
	
	void Push(ClangNode& cn) {scope << &cn;}
	void Pop() {scope.Remove(scope.GetCount()-1);}
	String GetScopePath(ClangNode& cn) const;
	String GetTemplateScopePath(ClangNode& cn);
public:
	typedef ClangTypeResolver CLASSNAME;
	ClangTypeResolver();
	
	bool Process(ClangNode& cn);
	void DumpFoundTypes();
	void DumpFoundScopePaths();
	void DumpUnresolvedHashes();
	
	void SetError(String msg) {last_error = msg;}
	String GetError() const {return last_error;}
	const VectorMap<hash_t,Index<String>>& GetScopePaths() const {return type_scopepaths;}
	VectorMap<hash_t,hash_t>& GetTypeTranslation() {return type_translation;}
};


#endif
