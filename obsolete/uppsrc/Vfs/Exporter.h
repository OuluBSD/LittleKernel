#ifndef _Vfs_Exporter_h_
#define _Vfs_Exporter_h_



struct PkgStubFile {
	Vector<VfsValue*>				refs;
	String							name;
	
};

struct PkgStub {
	ArrayMap<String,PkgStubFile>	files;
	Index<String>					deps;
	String							name;
	
	void Set(String s) {name = s; deps.Clear();}
	String ToString() const {return name;}
};


class AssemblyExporter {
	
private:
	struct ScopeHolder {
		AssemblyExporter& e;
		ScopeHolder(AssemblyExporter* ae, VfsValue& n) : e(*ae) {e.Push(n);}
		~ScopeHolder() {e.Pop();}
	};
	
	ArrayMap<String,PkgStub>		pkgs;
	Vector<VfsValue*>				scopes;
	AstNode&						root;
	String							export_dir;
	bool							protect_pkg = false;
	
	
	bool Visit(AstNode& n);
	void Push(VfsValue& n);
	void Pop();
	
	
	bool ExportComplete(String dir);
	bool ExportPackage(PkgStub& pkg);
	bool ExportHeader(PkgStub& pkg, PkgStubFile& file, String path);
	bool ExportImplementation(PkgStub& pkg, PkgStubFile& file, String path);
	
public:
	typedef AssemblyExporter CLASSNAME;
	AssemblyExporter(AstNode& root);
	
	bool Export(String dir);
	void Dump();
	void ProtectPkgStubFile(bool b=true) {protect_pkg = b;}
	
};
	



#endif
